#pragma once

#include <memory>
#include <functional>
#include <stdexcept>
#include "Drawable.h"

typedef std::function<void(std::shared_ptr<InternalDrawable>&, float, float)> GestureDetectorFunction;

struct GestureDetectorProperties {
    GestureDetectorFunction onTap = nullptr;
    GestureDetectorFunction onHover = nullptr;
    std::shared_ptr<InternalDrawable> child = nullptr;
};

class D_GestureDetector : public Drawable {
public:
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

        if (properties.onTap != nullptr) {
            auto bounds = properties.child->getBounds();
            auto mousePos = os->getMousePosition();

            if(bounds.x <= mousePos.x && mousePos.x <= bounds.x + bounds.width &&
               bounds.y <= mousePos.y && mousePos.y <= bounds.y + bounds.height &&
               os->isMouseButtonPressed(0)) {
                properties.onTap(properties.child, mousePos.x, mousePos.y);
            }
        }

        if (properties.onHover != nullptr) {
            auto bounds = properties.child->getBounds();
            auto mousePos = os->getMousePosition();

            if(bounds.contains(mousePos.x, mousePos.y)) {
                properties.onHover(properties.child, mousePos.x, mousePos.y);
            }
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