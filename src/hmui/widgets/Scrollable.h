#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <algorithm>
#include <cmath>
#include <utility>

// Use the existing Direction enum or define if missing
#ifndef DIRECTION_ENUM
#define DIRECTION_ENUM
enum class Direction {
    Horizontal,
    Vertical
};
#endif

struct ScrollableProperties {
    Direction direction = Direction::Vertical;
    std::shared_ptr<InternalDrawable> child = nullptr;
    bool clipToBounds = true;
};

class D_Scrollable : public InternalDrawable {
public:
    explicit D_Scrollable(ScrollableProperties properties)
        : properties(std::move(properties)) {}

    void init() override {
        if (properties.child) {
            properties.child->init();
            properties.child->setParent(shared_from_this());
        }
    }

    void layout(BoxConstraints constraints) override {
        // 1. Determine Viewport Size (The "Window")
        // We try to fill the parent's constraints.
        float viewportWidth = (constraints.maxWidth == INFINITY) ? constraints.minWidth : constraints.maxWidth;
        float viewportHeight = (constraints.maxHeight == INFINITY) ? constraints.minHeight : constraints.maxHeight;

        // Fallback for safety if min is also infinite (rare)
        if (viewportWidth == INFINITY) viewportWidth = 300.0f;
        if (viewportHeight == INFINITY) viewportHeight = 300.0f;

        bounds = Rect(0, 0, viewportWidth, viewportHeight);

        // 2. Layout Child with Infinite Scroll Constraint
        BoxConstraints childConstraints;

        if (properties.direction == Direction::Vertical) {
            // Vertical: Child matches width, but has Infinite Height
            childConstraints = BoxConstraints(
                viewportWidth, viewportWidth, // Tight width
                0.0f, INFINITY                // Loose height
            );
        } else {
            // Horizontal: Child matches height, but has Infinite Width
            childConstraints = BoxConstraints(
                0.0f, INFINITY,               // Loose width
                viewportHeight, viewportHeight// Tight height
            );
        }

        float contentSize = 0.0f;
        float viewportSize = 0.0f;

        if (properties.child) {
            properties.child->layout(childConstraints);
            Rect childBounds = properties.child->getBounds();

            if (properties.direction == Direction::Vertical) {
                contentSize = childBounds.height;
                viewportSize = viewportHeight;
            } else {
                contentSize = childBounds.width;
                viewportSize = viewportWidth;
            }
        }

        // 3. Calculate Max Scroll Extent
        // If content is smaller than viewport, maxScroll is 0.
        maxScrollExtent = std::max(0.0f, contentSize - viewportSize);

        // Clamp offset immediately if content shrank
        nextOffset = std::clamp(nextOffset, 0.0f, maxScrollExtent);
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        // Store absolute screen position for onUpdate hit-testing
        absoluteRect = Rect(x, y, bounds.width, bounds.height);

        // 1. Clip to Viewport
        if (properties.clipToBounds) {
            ctx->setScissor(absoluteRect);
        }

        // 2. Translate Child Position
        float childX = x;
        float childY = y;

        if (properties.direction == Direction::Vertical) {
            childY -= offset;
        } else {
            childX -= offset;
        }

        // 3. Draw Child
        if (properties.child) {
            properties.child->onDraw(ctx, childX, childY);
        }

        // 4. Restore Clip
        if (properties.clipToBounds) {
            ctx->clearScissor();
        }
    }

    float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    void onUpdate(float delta) override {
        if (!properties.child) return;

        properties.child->onUpdate(delta);

        auto os = hmui->getOSContext();
        auto mousePos = os->getMousePosition();

        // Hit Test: Check if mouse is inside the Viewport (Visible Area)
        bool isHovering = (mousePos.x >= absoluteRect.x &&
                           mousePos.x <= absoluteRect.x + absoluteRect.width &&
                           mousePos.y >= absoluteRect.y &&
                           mousePos.y <= absoluteRect.y + absoluteRect.height);

        if (isHovering) {
            float wheel = 0.0f;
            if (properties.direction == Direction::Vertical) {
                wheel = os->getMouseWheel().y;
            } else {
                // Support horizontal scroll if available, or fallback to vertical wheel
                wheel = (os->getMouseWheel().x != 0) ? os->getMouseWheel().x : os->getMouseWheel().y;
            }

            if (std::abs(wheel) > 0.0f) {
                nextOffset -= wheel * 50.0f; // Scroll speed multiplier
                nextOffset = std::clamp(nextOffset, 0.0f, maxScrollExtent);
            }
        }

        // Smooth Scroll Animation
        offset = lerp(offset, nextOffset, 15.0f * delta);
    }

    void dispose() override {
        if (properties.child) {
            properties.child->dispose();
        }
    }

    Rect getBounds() const override {
        return bounds;
    }

    void setBounds(const Rect& rect) override {
        bounds = rect;
    }

protected:
    ScrollableProperties properties;
    Rect bounds;        // Local Size (Viewport)
    Rect absoluteRect;  // Global Position (for Hit Test)

    float offset = 0.0f;
    float nextOffset = 0.0f;
    float maxScrollExtent = 0.0f;
};

#define Scrollable(...) std::make_shared<D_Scrollable>(ScrollableProperties{__VA_ARGS__})