#pragma once

#include <memory>
#include <functional>
#include <stdexcept>
#include "Drawable.h"

typedef std::function<void(std::shared_ptr<InternalDrawable>&, float, float)> GestureDetectorFunction;

struct GestureDetectorProperties {
    GestureDetectorFunction onTap = nullptr;
    GestureDetectorFunction onTapRelease = nullptr;
    GestureDetectorFunction onHover = nullptr;
    GestureDetectorFunction onHoverEnd = nullptr;
    std::shared_ptr<InternalDrawable> child = nullptr;
};

class D_GestureDetector : public Drawable {
public:
    bool isHovered = false; // track hover state
    bool isPressed = false;
    explicit D_GestureDetector(GestureDetectorProperties properties)
        : properties(std::move(properties)) {}

    std::shared_ptr<InternalDrawable> build() override {
        if (!properties.child) {
            throw std::runtime_error("GestureDetector must have a child drawable");
        }
        return properties.child;
    }

    void onUpdate(float delta) override {
        Drawable::onUpdate(delta);

        auto os = hmui->getOSContext();
        auto bounds = properties.child->getBounds();
        auto mousePos = os->getMousePosition();
        bool hoveredNow = bounds.contains(mousePos.x, mousePos.y);

        // Press detection
        if (hoveredNow && os->isMouseButtonPressed(0) || os->isTouchActive()) {
            if (!isPressed) {
                isPressed = true;
                if (properties.onTap) properties.onTap(properties.child, mousePos.x, mousePos.y);
            }
        } else {
            // Release
            if (isPressed) {
                isPressed = false;
                if (properties.onTapRelease) properties.onTapRelease(properties.child, mousePos.x, mousePos.y);
            }
        }


        if (os->isTouchDevice()) {
            return; // Skip hover which is not supported for touch devices
        }

        // Hover enter
        if (hoveredNow && !isHovered) {
            isHovered = true;
            if (properties.onHover) properties.onHover(properties.child, mousePos.x, mousePos.y);
        }

        // Hover exit
        if (!hoveredNow && isHovered) {
            isHovered = false;
            if (properties.onHoverEnd) properties.onHoverEnd(properties.child, mousePos.x, mousePos.y);
        }



    }

    Rect getBounds() const override {
        return properties.child->getBounds();
    }

    void setBounds(const Rect& rect) override{
        properties.child->setBounds(rect);
        Drawable::setBounds(rect);
    }

protected:
    GestureDetectorProperties properties;
};

#define GestureDetector(...) \
    std::make_shared<D_GestureDetector>(GestureDetectorProperties{__VA_ARGS__})