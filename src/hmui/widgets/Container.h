#pragma once

#include <utility>
#include <memory>

#include "InternalDrawable.h"

struct ContainerProperties {
    float width = 0; // 0 implies "auto" / wrap content
    float height = 0;
    EdgeInsets padding = EdgeInsets();
    EdgeInsets margin = EdgeInsets();
    Alignment alignment = Alignment::TopLeft();
    bool clipToBounds = false;
    Color2D color = Color2D(0.0f, 0.0f, 0.0f, 0.0f);
    std::shared_ptr<InternalDrawable> child = nullptr;
};

// --- Container Implementation ---

class D_Container : public InternalDrawable {
public:
    explicit D_Container(ContainerProperties properties) 
        : properties(std::move(properties)) {};

    // In a real engine, init() might register events, but it shouldn't do layout.
    void init() override {
        if (properties.child) {
            properties.child->init();
            properties.child->setParent(shared_from_this());
        }
    }

    // --- The Core Flutter Logic: Layout ---
    // This function must be called by the Parent before onDraw
    void layout(BoxConstraints constraints) override {
        // 1. Account for Margin
        // Margin effectively reduces the max size available to the container itself
        // and increases the minimum space the container takes in the parent.
        // (Simplified here: we subtract margin from incoming constraints for the inner calculation)
        
        float deflatedMaxWidth = std::max(0.0f, constraints.maxWidth - properties.margin.left - properties.margin.right);
        float deflatedMaxHeight = std::max(0.0f, constraints.maxHeight - properties.margin.top - properties.margin.bottom);
        float deflatedMinWidth = std::max(0.0f, constraints.minWidth - properties.margin.left - properties.margin.right);
        float deflatedMinHeight = std::max(0.0f, constraints.minHeight - properties.margin.top - properties.margin.bottom);

        // 2. Determine Constraints for "Self" (The Container Box)
        // If properties.width is set, enforce it (Tight constraint). 
        // Otherwise, respect parent constraints (Loose constraint).
        float targetMinWidth = (properties.width > 0) ? properties.width : deflatedMinWidth;
        float targetMaxWidth = (properties.width > 0) ? properties.width : deflatedMaxWidth;
        float targetMinHeight = (properties.height > 0) ? properties.height : deflatedMinHeight;
        float targetMaxHeight = (properties.height > 0) ? properties.height : deflatedMaxHeight;

        // 3. Determine Constraints for the Child
        // Child space = Container Size - Padding
        float childAvailableMaxWidth = std::max(0.0f, targetMaxWidth - properties.padding.left - properties.padding.right);
        float childAvailableMaxHeight = std::max(0.0f, targetMaxHeight - properties.padding.top - properties.padding.bottom);
        
        BoxConstraints childConstraints(0, childAvailableMaxWidth, 0, childAvailableMaxHeight);

        float contentWidth = 0;
        float contentHeight = 0;

        // 4. Layout the Child (Recursion)
        if (properties.child) {
            properties.child->layout(childConstraints);
            Rect childRect = properties.child->getBounds(); // Assuming getBounds returns size after layout
            contentWidth = childRect.width;
            contentHeight = childRect.height;
        }

        // 5. Calculate Final Size of this Container
        // If width defined: use it.
        // If width auto: use child width + padding.
        // Finally: Clamp to parent constraints.
        
        float finalW = (properties.width > 0) 
            ? properties.width 
            : (contentWidth + properties.padding.left + properties.padding.right);
        
        float finalH = (properties.height > 0) 
            ? properties.height 
            : (contentHeight + properties.padding.top + properties.padding.bottom);

        // Ensure we respect the parent's incoming constraints
        finalW = std::clamp(finalW, deflatedMinWidth, deflatedMaxWidth);
        finalH = std::clamp(finalH, deflatedMinHeight, deflatedMaxHeight);

        // Update our bounds (Size only, position is set by parent usually, but here we store W/H)
        bounds.width = finalW;
        bounds.height = finalH;

        // 6. Alignment (Positioning the Child)
        if (properties.child) {
            // Available space for child to move around in
            float spaceForChildW = finalW - properties.padding.left - properties.padding.right;
            float spaceForChildH = finalH - properties.padding.top - properties.padding.bottom;
            
            Rect childRect = properties.child->getBounds();

            // Calculate offset based on alignment (-1.0 to 1.0 or 0.0 to 1.0 depending on your Alignment impl)
            // Assuming Alignment is 0.0(left) to 1.0(right)
            float xOffset = properties.padding.left + (spaceForChildW - childRect.width) * properties.alignment.x;
            float yOffset = properties.padding.top + (spaceForChildH - childRect.height) * properties.alignment.y;

            // Set the child's local position relative to this container
            // (Assumes InternalDrawable has a way to set local position relative to parent)
            properties.child->setBounds(Rect(xOffset, yOffset, childRect.width, childRect.height));
        }
    }

    void dispose() override {
        if (properties.child) {
            properties.child->dispose();
        }
    }

    // onDraw receives the absolute world coordinates (x, y) where this container should draw
    void onDraw(GraphicsContext* ctx, float x, float y) override {
        // Apply Margin Offset:
        // The "bounds" calculated in layout usually represent the visual box. 
        // If margin is "outer space", we usually draw at x + margin.left.
        float drawX = x + properties.margin.left;
        float drawY = y + properties.margin.top;

        Rect contentRect = Rect(drawX, drawY, bounds.width, bounds.height);

#ifdef DEBUG_COMPONENTS
        // Draw debug bounds
        ctx->drawRect(contentRect, Color2D(0.0f, 1.0f, 1.0f, 1.0f));
#endif

        // Draw Background
        if(properties.color.a > 0.0f){
            ctx->fillRect(contentRect, properties.color);
        }

        // Clip (Scissor)
        if (properties.clipToBounds) {
            ctx->setScissor(contentRect);
        }

        // Draw Child
        if (properties.child) {
            Rect childLocalInfo = properties.child->getBounds();
            // child->onDraw takes absolute coordinates
            properties.child->onDraw(ctx, drawX + childLocalInfo.x, drawY + childLocalInfo.y);
        }

        // Restore Clip
        if (properties.clipToBounds) {
            ctx->clearScissor();
        }
    
#ifdef DEBUG_COMPONENTS
        // Draw debug bounds again on top
        ctx->drawRect(contentRect, Color2D(0.0f, 1.0f, 1.0f, 1.0f));
#endif
    }

    void onUpdate(float delta) override {
        if (properties.child) {
            properties.child->onUpdate(delta);
        }
    }

    Rect getBounds() const override {
        return bounds;
    }

    // Used by the parent to force position this element
    void setBounds(const Rect& rect) override {
        bounds = rect;
    }

    ContainerProperties properties;
protected:
    Rect bounds;
};

#define Container(...) \
    std::make_shared<D_Container>(ContainerProperties{__VA_ARGS__})
