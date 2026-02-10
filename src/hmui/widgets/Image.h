#pragma once

#include <string>
#include <utility>

#include "InternalDrawable.h"

struct ImageProperties {
    std::string texture = nullptr;
    Color2D color = Color2D(1.0f, 1.0f, 1.0f, 1.0f);
    float scale = 1.0f;
    std::shared_ptr<InternalDrawable> child = nullptr;
};

class D_Image : public InternalDrawable {
public:
    explicit D_Image(
        ImageProperties properties
    ) : properties(std::move(properties)) {};

    void init() override {
    }

    void dispose() override {
        // if (properties.child) {
        //     properties.child->dispose();
        // }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (properties.texture.empty()) {
            return;
        }
        Rect rect;

        // seg2_blue_sky_backgrounD_Imageure
        // seg2_sunset_backgrounD_Imageure
        ctx->drawImage(Rect(0, 0, 200, 200), properties.texture.c_str(), properties.color, properties.scale);

        //setBounds(Rect(x, y, bounds.width, bounds.height));
    }

    void onUpdate(float delta) override {
        // if (properties.child) {
        //     properties.child->onUpdate(delta);
        // }
    }

    // Rect getBounds() const override {
    //     return bounds;
    // }

    // void setBounds(const Rect& rect) override {
    //     bounds = rect;
    // }

    ImageProperties properties;
protected:
};

#define Image(...) \
    std::make_shared<D_Image>(ImageProperties{__VA_ARGS__})