#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include "hmui/input/FocusManager.h"
#include <memory>
#include <functional>
#include <utility>

// Callback signature: Child, MouseX, MouseY
typedef std::function<void(std::shared_ptr<InternalDrawable>, float, float)> GestureCallback;

struct FocusDecorator {
    Color2D color = Color2D(0.0f, 0.0f, 0.0f, 1.0f);
    float thickness = 2.0f;
};

struct GestureDetectorProperties {
    bool focusable = false;
    FocusDecorator focusDecorator;
    GestureCallback onTap = nullptr;
    GestureCallback onTapRelease = nullptr;
    GestureCallback onHover = nullptr;
    GestureCallback onHoverEnd = nullptr;
    std::shared_ptr<InternalDrawable> child = nullptr;
};

class D_GestureDetector : public InternalDrawable {
public:
    explicit D_GestureDetector(GestureDetectorProperties properties)
        : properties(std::move(properties)) {}

    void init() override {
        InternalDrawable::init();

        if (properties.child) {
            properties.child->init();
            properties.child->setParent(shared_from_this());
        }

        if (properties.focusable) {
            focusNode = std::make_shared<FocusNode>();
            focusNode->id = "GestureDetector_" + std::to_string(reinterpret_cast<std::uintptr_t>(this));
            focusNode->widget = shared_from_this();

            // Define behaviors
            focusNode->onFocus = [this]() {
                if (properties.onHover) properties.onHover(properties.child, 0, 0);
            };

            focusNode->onBlur = [this]() {
                if (properties.onHoverEnd) properties.onHoverEnd(properties.child, 0, 0);
            };

            focusNode->onSubmit = [this]() {
                if (properties.onTap) properties.onTap(properties.child, 0, 0);
            };

            FocusManager::get()->registerNode(focusNode);
        }
    }

    void layout(BoxConstraints constraints) override {
        if (properties.child) {
            // Pass constraints through to child
            properties.child->layout(constraints);
            
            // GestureDetector adopts the size of its child
            Rect childBounds = properties.child->getBounds();
            bounds.width = childBounds.width;
            bounds.height = childBounds.height;
        } else {
            // No child: minimal size (or 0)
            bounds.width = constraints.minWidth;
            bounds.height = constraints.minHeight;
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        absoluteRect = Rect(x, y, bounds.width, bounds.height);

        if (properties.child) {
            properties.child->onDraw(ctx, x, y);
        }

        if (properties.focusable && focusNode && FocusManager::get()->isFocused(focusNode)) {
            ctx->drawRect(absoluteRect, properties.focusDecorator.color, properties.focusDecorator.thickness);
        }
    }

    void onUpdate(float delta) override {
        if (properties.child) {
            properties.child->onUpdate(delta);
        }

        auto os = hmui->getOSContext();
        auto mousePos = os->getMousePosition();

        // Hit Test using the absolute rect captured during Draw
        bool isHovering = (mousePos.x >= absoluteRect.x &&
                           mousePos.x <= absoluteRect.x + absoluteRect.width &&
                           mousePos.y >= absoluteRect.y &&
                           mousePos.y <= absoluteRect.y + absoluteRect.height);

        // 1. Hover Logic
        if (!os->isTouchDevice()) {
            if (isHovering && !isHovered) {
                isHovered = true;
                if (properties.onHover) properties.onHover(properties.child, mousePos.x, mousePos.y);
            } else if (!isHovering && isHovered) {
                isHovered = false;
                if (properties.onHoverEnd) properties.onHoverEnd(properties.child, mousePos.x, mousePos.y);
            }
        }

        // 2. Tap/Press Logic
        bool isMouseDown = os->isMouseButtonPressed(0) || os->isTouchActive();

        if (isHovering && isMouseDown) {
            if (!isPressed) {
                isPressed = true;
                if (properties.onTap) properties.onTap(properties.child, mousePos.x, mousePos.y);
            }
        } else {
            // Logic for release: 
            // If we were pressed, and now input is gone, trigger release.
            // (Note: standard UI triggers release even if mouse left bounds, as long as it started there)
            if (isPressed && !isMouseDown) {
                isPressed = false;
                if (properties.onTapRelease) properties.onTapRelease(properties.child, mousePos.x, mousePos.y);
            }
        }
    }

    Rect getBounds() const override {
        return bounds;
    }

    void setBounds(const Rect& rect) override {
        bounds = rect;
    }

    void dispose() override {
        if (focusNode) {
            FocusManager::get()->unregisterNode(focusNode);
            focusNode = nullptr;
        }

        if (properties.child) properties.child->dispose();
    }

protected:
    GestureDetectorProperties properties;
    Rect bounds;         // Local size
    Rect absoluteRect;   // Global position for hit testing
    std::shared_ptr<FocusNode> focusNode;
    bool isHovered = false;
    bool isPressed = false;
};

#define GestureDetector(...) \
    std::make_shared<D_GestureDetector>(GestureDetectorProperties{__VA_ARGS__})
