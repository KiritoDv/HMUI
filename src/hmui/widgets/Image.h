#pragma once

#include <string>
#include <utility>
#include <algorithm>
#include <cmath>

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

class D_Image : public InternalDrawable {
public:
    explicit D_Image(ImageProperties properties) 
        : properties(std::move(properties)) {};

    void init() override {
        if (!properties.provider) {
            throw std::runtime_error("Image must have a valid ImageProvider");
        }

        image = properties.provider->load();
        if (!image) {
            throw std::runtime_error("Failed to load image");
        }

        float intrinsicW = image->width * properties.scale;
        float intrinsicH = image->height * properties.scale;

        bounds.width = (properties.width > 0) ? properties.width : intrinsicW;
        bounds.height = (properties.height > 0) ? properties.height : intrinsicH;
    }

    void dispose() override {
        if (properties.provider) {
            properties.provider->dispose();
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (!image) return;

        Rect dest = getDestinationRect(x, y, bounds.width, bounds.height, image->width, image->height);
        ctx->setScissor(Rect(x, y, bounds.width, bounds.height));

        ctx->drawImage(
            dest,
            image,
            properties.color,
            properties.scale
        );

        ctx->clearScissor();
    }

protected:
    ImageProperties properties;
    ImageHandle* image = nullptr;

private:
    Rect getDestinationRect(float bx, float by, float bw, float bh, float iw, float ih) {
        float scaleX = 1.0f;
        float scaleY = 1.0f;

        switch (properties.fit) {
            case BoxFit::Fill:
                scaleX = bw / iw;
                scaleY = bh / ih;
                break;
            case BoxFit::Contain: {
                float s = std::min(bw / iw, bh / ih);
                scaleX = scaleY = s;
                break;
            }
            case BoxFit::Cover: {
                float s = std::max(bw / iw, bh / ih);
                scaleX = scaleY = s;
                break;
            }
            case BoxFit::FitWidth:
                scaleX = scaleY = bw / iw;
                break;
            case BoxFit::FitHeight:
                scaleX = scaleY = bh / ih;
                break;
            case BoxFit::ScaleDown: {
                float s = std::min(bw / iw, bh / ih);
                if (s < 1.0f) {
                    scaleX = scaleY = s;
                } else {
                    scaleX = scaleY = 1.0f;
                }
                break;
            }
            case BoxFit::None:
                scaleX = scaleY = 1.0f;
                break;
        }

        float finalW = iw * scaleX;
        float finalH = ih * scaleY;

        float dx = (bw - finalW) * properties.alignment.x;
        float dy = (bh - finalH) * properties.alignment.y;

        return Rect(bx + dx, by + dy, finalW, finalH);
    }
};

#define Image(...) std::make_shared<D_Image>(ImageProperties{__VA_ARGS__})