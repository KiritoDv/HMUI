#pragma once

#include <utility>

#include "InternalDrawable.h"

enum class Alignment {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

struct ContainerProperties {
    float width = 0;
    float height = 0;
    EdgeInsets padding = EdgeInsets();
    EdgeInsets margin = EdgeInsets();
    Alignment alignment = Alignment::TopLeft;
    bool clipToBounds = false;
    Color2D color = Color2D(1.0f, 1.0f, 1.0f, 0.0f);
    std::shared_ptr<InternalDrawable> child = nullptr;
};

class D_Container : public InternalDrawable {
public:
    explicit D_Container(
        ContainerProperties properties
    ) : properties(std::move(properties)) {};

    void init() override {
        if (properties.child) {
            properties.child->setParent(shared_from_this());
            properties.child->init();
            bounds.width = properties.width > 0 ? properties.width : properties.child->getBounds().width + properties.padding.left + properties.padding.right;
            bounds.height = properties.height > 0 ? properties.height : properties.child->getBounds().height + properties.padding.top + properties.padding.bottom;
        } else {
            bounds.width = properties.width;
            bounds.height = properties.height;
        }
    }

    void dispose() override {
        if (properties.child) {
            properties.child->dispose();
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (properties.clipToBounds) {
            ctx->setScissor(bounds);
        }

        if(properties.color.a > 0.0f){
            ctx->fillRect(Rect(x, y, bounds.width, bounds.height), properties.color);
        }

        if (properties.child) {
            float childX = x + properties.padding.left;
            float childY = y + properties.padding.top;
            Rect childBounds = properties.child->getBounds();

            switch (properties.alignment) {
                case Alignment::TopLeft:
                    break;
                case Alignment::TopCenter:
                    childX = x + (bounds.width - childBounds.width) / 2;
                    break;
                case Alignment::TopRight:
                    childX = x + bounds.width - childBounds.width - properties.padding.right;
                    break;
                case Alignment::CenterLeft:
                    childY = y + (bounds.height - childBounds.height) / 2;
                    break;
                case Alignment::Center:
                    childX = x + (bounds.width - childBounds.width) / 2;
                    childY = y + (bounds.height - childBounds.height) / 2;
                    break;
                case Alignment::CenterRight:
                    childX = x + bounds.width - childBounds.width - properties.padding.right;
                    childY = y + (bounds.height - childBounds.height) / 2;
                    break;
                case Alignment::BottomLeft:
                    childY = y + bounds.height - childBounds.height - properties.padding.bottom;
                    break;
                case Alignment::BottomCenter:
                    childX = x + (bounds.width - childBounds.width) / 2;
                    childY = y + bounds.height - childBounds.height - properties.padding.bottom;
                    break;
                case Alignment::BottomRight:
                    childX = x + bounds.width - childBounds.width - properties.padding.right;
                    childY = y + bounds.height - childBounds.height - properties.padding.bottom;
                    break;
            }

            properties.child->onDraw(ctx, childX, childY);
            properties.child->setBounds(Rect(childX, childY, childBounds.width, childBounds.height));
        }

        if (properties.clipToBounds) {
            ctx->clearScissor();
        }

        this->setBounds(Rect(bounds.x, bounds.y, bounds.width, bounds.height));
    }

    void onUpdate(float delta) override {
        if (properties.child) {
            properties.child->onUpdate(delta);
        }
    }

    Rect getBounds() const override {
        return bounds;
    }

    void setBounds(const Rect& rect) override {
        bounds = rect;
    }

    ContainerProperties properties;
protected:
    Rect bounds;
};

#define Container(...) \
    std::make_shared<D_Container>(ContainerProperties{__VA_ARGS__})