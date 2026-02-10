#pragma once

#include <utility>
#include <memory>

#include "InternalDrawable.h"

struct ImageProperties {
    std::shared_ptr<ImageProvider> provider = nullptr;
    Color2D color = Color2D(1.0f, 1.0f, 1.0f, 1.0f);

    float width = 0;
    float height = 0;
    float scale = 1.0f;

    BoxFit fit = BoxFit::Contain;
    Alignment alignment = Alignment::Center();
};

struct ContainerProperties {
    float width = 0;
    float height = 0;
    EdgeInsets padding = EdgeInsets();
    EdgeInsets margin = EdgeInsets();
    Alignment alignment = Alignment::TopLeft();
    bool clipToBounds = false;
    Color2D color = Color2D(1.0f, 1.0f, 1.0f, 0.0f);
    std::shared_ptr<InternalDrawable> child = nullptr;
    ImageProperties image = ImageProperties();
    BoxFit backgroundFit = BoxFit::Fill; 
};

class D_Container : public InternalDrawable {
public:
    explicit D_Container(
        ContainerProperties properties
    ) : properties(std::move(properties)) {};

    void init() override {
        if (properties.image.provider) {
            bgImage = properties.image.provider->load();
        }

        if (properties.child) {
            properties.child->init();
            properties.child->setParent(shared_from_this());
            bounds.width = properties.width > 0 ? properties.width : properties.child->getBounds().width + properties.padding.left + properties.padding.right;
            bounds.height = properties.height > 0 ? properties.height : properties.child->getBounds().height + properties.padding.top + properties.padding.bottom;
        } else {
            bounds.width = properties.width;
            bounds.height = properties.height;
        }
    }

    void dispose() override {
        if (properties.image.provider) {
            properties.image.provider->dispose();
        }
        if (properties.child) {
            properties.child->dispose();
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        Rect real = Rect(x + properties.padding.left, y + properties.padding.top,
                         bounds.width - properties.padding.left - properties.padding.right,
                         bounds.height - properties.padding.top - properties.padding.bottom);

        if(properties.color.a > 0.0f){
            ctx->fillRect(Rect(x, y, bounds.width, bounds.height), properties.color);
        }

        if (bgImage) {
            ctx->drawImage(Rect(x, y, bounds.width, bounds.height), bgImage, properties.image.color, properties.image.scale);
        }
    
        if (properties.clipToBounds) {
            ctx->setScissor(real);
        }

        if (properties.child) {
            float availableWidth = bounds.width - properties.padding.left - properties.padding.right;
            float availableHeight = bounds.height - properties.padding.top - properties.padding.bottom;
            Rect childBounds = properties.child->getBounds();

            float childX = x + properties.padding.left + (availableWidth - childBounds.width) * properties.alignment.x;
            float childY = y + properties.padding.top + (availableHeight - childBounds.height) * properties.alignment.y;

            properties.child->setBounds(Rect(childX, childY, childBounds.width, childBounds.height));
            properties.child->onDraw(ctx, childX, childY);
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
    ImageHandle* bgImage = nullptr;
};

#define Container(...) \
    std::make_shared<D_Container>(ContainerProperties{__VA_ARGS__})