#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <vector>
#include <memory>
#include <optional>
#include <algorithm>
#include <cmath>

// ==========================================
// 1. The Positioned Wrapper
// ==========================================

struct PositionedProperties {
    std::shared_ptr<InternalDrawable> child;
    std::optional<float> left;
    std::optional<float> top;
    std::optional<float> right;
    std::optional<float> bottom;
    std::optional<float> width;
    std::optional<float> height;
};

class D_Positioned : public InternalDrawable {
public:
    explicit D_Positioned(PositionedProperties props) : properties(std::move(props)) {}

    void init() override {
        if (properties.child) {
            properties.child->init();
            properties.child->setParent(shared_from_this());
        }
    }

    // Forward layout call to child
    void layout(BoxConstraints constraints) override {
        if (properties.child) {
            properties.child->layout(constraints);
            // Adopt child's size so getBounds() works expectedly
            bounds = properties.child->getBounds(); 
        } else {
            bounds.width = constraints.minWidth;
            bounds.height = constraints.minHeight;
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (properties.child) properties.child->onDraw(ctx, x, y);
    }

    void onUpdate(float delta) override {
        if (properties.child) properties.child->onUpdate(delta);
    }
    
    // Pass bounds directly to child
    void setBounds(const Rect& rect) override {
        bounds = rect;
        if (properties.child) {
            // Child is usually at (0,0) relative to this wrapper,
            // but since this wrapper is transparent in the Stack,
            // we often set the child to the same rect.
            properties.child->setBounds(Rect(0, 0, rect.width, rect.height)); 
        }
    }

    void dispose() override {
        if (properties.child) properties.child->dispose();
    }

    const PositionedProperties& getProps() const { return properties; }

protected:
    PositionedProperties properties;
    Rect bounds;
};

// ==========================================
// 2. The Stack Widget
// ==========================================

enum class StackFit {
    Loose,  // Stack wraps its non-positioned children
    Expand, // Stack fills the parent
    Passthrough
};

struct StackProperties {
    std::vector<std::shared_ptr<InternalDrawable>> children;
    Alignment alignment = Alignment::TopLeft();
    StackFit fit = StackFit::Loose;
};

class D_Stack : public InternalDrawable {
public:
    explicit D_Stack(StackProperties props) : properties(std::move(props)) {}

    void init() override {
        for (auto& child : properties.children) {
            child->init();
            child->setParent(shared_from_this());
        }
    }

    void layout(BoxConstraints constraints) override {
        float hasNonPositioned = false;
        float maxChildWidth = 0.0f;
        float maxChildHeight = 0.0f;

        // 1. Layout Non-Positioned Children
        // These determine the size of the stack if it's "Loose"
        BoxConstraints nonPosConstraints;
        if (properties.fit == StackFit::Expand) {
            nonPosConstraints = BoxConstraints::tight(constraints.maxWidth, constraints.maxHeight);
        } else if (properties.fit == StackFit::Passthrough) {
            nonPosConstraints = constraints;
        } else {
            nonPosConstraints = BoxConstraints::loose(constraints.maxWidth, constraints.maxHeight);
        }

        for (auto& child : properties.children) {
            if (std::dynamic_pointer_cast<D_Positioned>(child)) continue;

            child->layout(nonPosConstraints);
            Rect childSize = child->getBounds();
            
            maxChildWidth = std::max(maxChildWidth, childSize.width);
            maxChildHeight = std::max(maxChildHeight, childSize.height);
            hasNonPositioned = true;
        }

        // 2. Determine Stack Size
        if (properties.fit == StackFit::Expand) {
            bounds.width = constraints.maxWidth;
            bounds.height = constraints.maxHeight;
        } else {
            // Loose: Size is max child size, clamped by constraints
            float targetW = (constraints.minWidth == constraints.maxWidth) ? constraints.maxWidth : maxChildWidth;
            float targetH = (constraints.minHeight == constraints.maxHeight) ? constraints.maxHeight : maxChildHeight;
            
            bounds.width = std::clamp(targetW, constraints.minWidth, constraints.maxWidth);
            bounds.height = std::clamp(targetH, constraints.minHeight, constraints.maxHeight);
        }
        
        // Handle infinite bounds edge case (empty stack)
        if (bounds.width == INFINITY) bounds.width = 0;
        if (bounds.height == INFINITY) bounds.height = 0;

        // 3. Layout Positioned Children & Calculate Offsets
        for (auto& child : properties.children) {
            // Is it Positioned?
            auto positioned = std::dynamic_pointer_cast<D_Positioned>(child);
            
            if (positioned) {
                layoutPositionedChild(positioned, bounds.width, bounds.height);
            } else {
                // Align Non-Positioned Child
                Rect childSize = child->getBounds();
                float alignX = properties.alignment.x; // Assumes 0.0 to 1.0
                float alignY = properties.alignment.y;

                float x = (bounds.width - childSize.width) * alignX;
                float y = (bounds.height - childSize.height) * alignY;
                
                // Store relative position in the child's bounds (x,y)
                child->setBounds(Rect(x, y, childSize.width, childSize.height));
            }
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        // Draw children from bottom to top
        for (auto& child : properties.children) {
            Rect childLocal = child->getBounds();
            // Translate local coordinates to global screen coordinates
            child->onDraw(ctx, x + childLocal.x, y + childLocal.y);
        }
    }

    void onUpdate(float delta) override {
        for (auto& child : properties.children) {
            child->onUpdate(delta);
        }
    }

    void dispose() override {
        for (auto& child : properties.children) {
            child->dispose();
        }
    }

    Rect getBounds() const override { return bounds; }
    void setBounds(const Rect& rect) override { bounds = rect; }

private:
    void layoutPositionedChild(std::shared_ptr<D_Positioned> wrapper, float stackW, float stackH) {
        auto& props = wrapper->getProps();
        
        float minW = 0, maxW = stackW;
        float minH = 0, maxH = stackH;
        float x = 0, y = 0;

        // --- Horizontal Resolution ---
        if (props.left.has_value() && props.right.has_value()) {
            // Stretched: fixed width determined by parent
            float w = std::max(0.0f, stackW - *props.left - *props.right);
            minW = maxW = w;
            x = *props.left;
        } else if (props.left.has_value() && props.width.has_value()) {
            minW = maxW = *props.width;
            x = *props.left;
        } else if (props.right.has_value() && props.width.has_value()) {
            minW = maxW = *props.width;
            x = stackW - *props.right - *props.width;
        } else if (props.left.has_value()) {
            // Loose width, anchored left
            maxW = std::max(0.0f, stackW - *props.left);
            x = *props.left; // Temporary, waiting for child layout
        } else if (props.right.has_value()) {
            // Loose width, anchored right
            maxW = std::max(0.0f, stackW - *props.right);
            // X depends on child width, resolved after layout
        } else if (props.width.has_value()) {
            // Centered/Aligned with specific width
            minW = maxW = *props.width;
        }
        
        // --- Vertical Resolution ---
        if (props.top.has_value() && props.bottom.has_value()) {
            float h = std::max(0.0f, stackH - *props.top - *props.bottom);
            minH = maxH = h;
            y = *props.top;
        } else if (props.top.has_value() && props.height.has_value()) {
            minH = maxH = *props.height;
            y = *props.top;
        } else if (props.bottom.has_value() && props.height.has_value()) {
            minH = maxH = *props.height;
            y = stackH - *props.bottom - *props.height;
        } else if (props.top.has_value()) {
            maxH = std::max(0.0f, stackH - *props.top);
            y = *props.top;
        } else if (props.bottom.has_value()) {
            maxH = std::max(0.0f, stackH - *props.bottom);
        } else if (props.height.has_value()) {
            minH = maxH = *props.height;
        }

        // Layout the child with these calculated constraints
        wrapper->layout(BoxConstraints(minW, maxW, minH, maxH));
        Rect childSize = wrapper->getBounds();

        // --- Final Position Resolution ---
        // (For cases where we needed child size to determine position, e.g., right-aligned)
        
        if (!props.left.has_value() && props.right.has_value()) {
             x = stackW - *props.right - childSize.width;
        } else if (!props.left.has_value() && !props.right.has_value()) {
             // Use Alignment if no horizontal position is set
             x = (stackW - childSize.width) * properties.alignment.x;
        }

        if (!props.top.has_value() && props.bottom.has_value()) {
             y = stackH - *props.bottom - childSize.height;
        } else if (!props.top.has_value() && !props.bottom.has_value()) {
             // Use Alignment if no vertical position is set
             y = (stackH - childSize.height) * properties.alignment.y;
        }

        wrapper->setBounds(Rect(x, y, childSize.width, childSize.height));
    }

    StackProperties properties;
    Rect bounds;
};

#define Stack(...) \
    std::make_shared<D_Stack>(StackProperties{__VA_ARGS__})

#define Positioned(...) \
    std::make_shared<D_Positioned>(PositionedProperties{__VA_ARGS__})
