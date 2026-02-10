#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <algorithm>
#include "InternalDrawable.h"

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
        if (properties.child) properties.child->init();
    }
    
    // Positioned widgets don't draw themselves; they just pass calls to the child.
    // The Stack (parent) handles the actual setBounds logic for this widget.
    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (properties.child) properties.child->onDraw(ctx, x, y);
    }

    void onUpdate(float delta) override {
        if (properties.child) properties.child->onUpdate(delta);
    }
    
    // Pass bounds directly to child
    void setBounds(const Rect& rect) override {
        InternalDrawable::setBounds(rect);
        if (properties.child) properties.child->setBounds(rect);
    }

    // Allow the Stack to access these properties
    const PositionedProperties& getProps() const { return properties; }

protected:
    PositionedProperties properties;
};

// ==========================================
// 2. The Stack Widget
// ==========================================

enum class StackFit {
    Loose,  // Non-positioned children can be smaller than the stack
    Expand  // Non-positioned children are forced to fill the stack
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

    void setBounds(const Rect& rect) override {
        InternalDrawable::setBounds(rect);

        for (auto& child : properties.children) {
            // 1. Check if the child is wrapped in a "Positioned" widget
            auto positioned = std::dynamic_pointer_cast<D_Positioned>(child);

            if (positioned) {
                applyPositionedLayout(positioned, rect);
            } else {
                applyAlignedLayout(child, rect);
            }
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        // Draw children from bottom (index 0) to top
        for (auto& child : properties.children) {
            // Child coordinates are usually relative to the window in this simple framework,
            // or relative to parent. Assuming 'x, y' passed here is the Stack's top-left.
            
            // We use the child's stored bounds (calculated in setBounds)
            Rect childBounds = child->getBounds();
            child->onDraw(ctx, childBounds.x, childBounds.y);
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

private:
    void applyPositionedLayout(std::shared_ptr<D_Positioned> wrapper, const Rect& stackRect) {
        auto& props = wrapper->getProps();
        
        float x = stackRect.x;
        float y = stackRect.y;
        float w = 0;
        float h = 0;

        // --- Horizontal Logic ---
        if (props.left.has_value() && props.right.has_value()) {
            // Stretched horizontally
            x += *props.left;
            w = stackRect.width - *props.left - *props.right;
        } else if (props.left.has_value()) {
            // Pinned left
            x += *props.left;
            w = props.width.value_or(wrapper->getBounds().width); // Use explicit width or current
        } else if (props.right.has_value()) {
            // Pinned right
            w = props.width.value_or(wrapper->getBounds().width);
            x += stackRect.width - *props.right - w;
        } else {
            // Default to left 0 or center? Usually behaves like normal child if no constraints
            w = props.width.value_or(wrapper->getBounds().width);
        }

        // --- Vertical Logic ---
        if (props.top.has_value() && props.bottom.has_value()) {
            // Stretched vertically
            y += *props.top;
            h = stackRect.height - *props.top - *props.bottom;
        } else if (props.top.has_value()) {
            // Pinned top
            y += *props.top;
            h = props.height.value_or(wrapper->getBounds().height);
        } else if (props.bottom.has_value()) {
            // Pinned bottom
            h = props.height.value_or(wrapper->getBounds().height);
            y += stackRect.height - *props.bottom - h;
        } else {
            h = props.height.value_or(wrapper->getBounds().height);
        }

        wrapper->setBounds(Rect(x, y, w, h));
    }

    void applyAlignedLayout(std::shared_ptr<InternalDrawable> child, const Rect& stackRect) {
        if (properties.fit == StackFit::Expand) {
            child->setBounds(stackRect);
            return;
        }

        Rect childBounds = child->getBounds(); // Get intrinsic/current size

        // Calculate alignment offsets
        // x = StackX + (StackWidth - ChildWidth) * AlignX
        float offsetX = (stackRect.width - childBounds.width) * properties.alignment.x;
        float offsetY = (stackRect.height - childBounds.height) * properties.alignment.y;

        child->setBounds(Rect(
            stackRect.x + offsetX,
            stackRect.y + offsetY,
            childBounds.width,
            childBounds.height
        ));
    }

    StackProperties properties;
};

#define Stack(...) \
    std::make_shared<D_Stack>(StackProperties{__VA_ARGS__})

#define Positioned(...) \
    std::make_shared<D_Positioned>(PositionedProperties{__VA_ARGS__})