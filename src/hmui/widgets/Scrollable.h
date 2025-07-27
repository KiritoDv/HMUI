#pragma once

#include <utility>

#include "InternalDrawable.h"
#include "Drawable.h"

enum class Direction {
    Horizontal,
    Vertical
};

struct ScrollableProperties {
    Direction direction = Direction::Vertical;
    std::shared_ptr<InternalDrawable> child = nullptr;
};

class D_Scrollable : public InternalDrawable {
public:
    explicit D_Scrollable(ScrollableProperties properties)
        : properties(std::move(properties)) {}

    void init() override {
        properties.child->init();
        properties.child->setParent(shared_from_this());
        bounds = properties.child->getBounds();
        int bp = 0;
    }

    void dispose() override {
        properties.child->dispose();
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (properties.direction == Direction::Vertical) {
            properties.child->onDraw(ctx, x, y - offset);
        } else {
            properties.child->onDraw(ctx, x - offset, y);
        }

        auto childBounds = properties.child->getBounds();
        properties.child->setBounds(Rect(x, y, childBounds.width, childBounds.height));
    }

    float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    void onUpdate(float delta) override {
        auto os = hmui->getOSContext();
        properties.child->onUpdate(delta);

        auto bounds = properties.child->getBounds();
        auto mousePos = os->getMousePosition();

        // TODO: Fix this because is broken
         if(!(bounds.x <= mousePos.x && mousePos.x <= bounds.x + bounds.width &&
             bounds.y <= mousePos.y && mousePos.y <= bounds.y + bounds.height)) {
             return;
         }

        if (properties.direction == Direction::Vertical) {
            float wheel = os->getMouseWheel().y;
            nextOffset -= wheel * 2;
        } else {
            float wheel = os->getMouseWheel().x;
            nextOffset -= wheel * 2;
        }

        // TODO: We need to clamp the offset to prevent scrolling out of bounds
        // TODO: I kinda fixed it but the clamp is still wrong
        nextOffset = std::max(0.0f, nextOffset);
        nextOffset = std::min(nextOffset, bounds.height);
        offset = lerp(offset, nextOffset, 0.1f);
    }

    Rect getBounds() const override {
        return bounds;
    }

    void setBounds(const Rect& rect) override {
        bounds = rect;
        properties.child->setBounds(Rect(rect.x, rect.y, rect.width, rect.height));
    }

protected:
    ScrollableProperties properties;
    float offset = 0.0f; // Offset for scrolling
    float nextOffset = 0.0f; // Next offset for scrolling
};

#define Scrollable(...) std::make_shared<D_Scrollable>(ScrollableProperties{__VA_ARGS__})