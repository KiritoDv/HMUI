#pragma once

#include "InternalDrawable.h"
#include <memory>

struct ExpandedProperties {
    int flex = 1;
    std::shared_ptr<InternalDrawable> child = nullptr;
};

class D_Expanded : public InternalDrawable {
public:
    explicit D_Expanded(ExpandedProperties props) : properties(std::move(props)) {}

    void init() override {
        if (properties.child) {
            properties.child->init();
            properties.child->setParent(shared_from_this());
        }
    }

    void layout(BoxConstraints constraints) override {
        // If used outside of a Flex container (Row/Column) that recognizes it,
        // it behaves like a pass-through container.
        if (properties.child) {
            properties.child->layout(constraints);
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

    void setBounds(const Rect& rect) override {
        bounds = rect;
        if (properties.child) {
            // Expanded forces its child to fill its bounds
            properties.child->setBounds(Rect(0, 0, rect.width, rect.height));
        }
    }
    
    void dispose() override {
        if (properties.child) properties.child->dispose();
    }

    const ExpandedProperties& getProps() const { return properties; }

protected:
    ExpandedProperties properties;
};

#define Expanded(...) std::make_shared<D_Expanded>(ExpandedProperties{__VA_ARGS__})