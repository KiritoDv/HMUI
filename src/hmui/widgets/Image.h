#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <string>
#include <utility>
#include <algorithm>
#include <cmath>
#include <stdexcept>

struct ImageProperties {
    std::shared_ptr<ImageProvider> provider = nullptr;
    Color2D color = Color2D(1.0f, 1.0f, 1.0f, 1.0f); // Tint color

    float width = 0;   // explicit width (0 = auto)
    float height = 0;  // explicit height (0 = auto)
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
            // In a real engine, you might want a placeholder instead of crashing
            throw std::runtime_error("Image must have a valid ImageProvider");
        }

        // Load the image resource
        image = properties.provider->load();
    }

    void layout(BoxConstraints constraints) override {
        // 1. Determine Intrinsic Size
        float intrinsicW = 0.0f;
        float intrinsicH = 0.0f;

        if (image) {
            intrinsicW = image->width * properties.scale;
            intrinsicH = image->height * properties.scale;
        }

        // 2. Determine Target Size
        // If property is set, use it. Otherwise use intrinsic.
        float targetW = (properties.width > 0) ? properties.width : intrinsicW;
        float targetH = (properties.height > 0) ? properties.height : intrinsicH;

        // 3. Apply Constraints
        // Clamp the target size within the parent's min/max constraints
        bounds.width = std::clamp(targetW, constraints.minWidth, constraints.maxWidth);
        bounds.height = std::clamp(targetH, constraints.minHeight, constraints.maxHeight);

        // Handle infinite constraints fallback (rare, but good practice)
        if (bounds.width == INFINITY) bounds.width = intrinsicW > 0 ? intrinsicW : 100.0f;
        if (bounds.height == INFINITY) bounds.height = intrinsicH > 0 ? intrinsicH : 100.0f;
    }

    void dispose() override {
        if (properties.provider) {
            properties.provider->dispose();
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        // 1. Safety Check: If image is null OR bounds are invalid, do nothing.
        // This prevents pushing invalid scissors or drawing nothing.
        if (!image || bounds.width <= 0.0f || bounds.height <= 0.0f) {
            return;
        }

        // 3. Calculate Layout
        Rect dest = getDestinationRect(x, y, bounds.width, bounds.height, 
                                     image->width * properties.scale, 
                                     image->height * properties.scale);

        // 4. Draw
        ctx->drawImage(
            dest,
            image,
            properties.color
        );
    }

    Rect getBounds() const override {
        return bounds;
    }

    void setBounds(const Rect& rect) override {
        bounds = rect;
    }

protected:
    ImageProperties properties;
    ImageHandle* image = nullptr;
    Rect bounds;

private:
    // Calculates the destination rect for the image content based on BoxFit and Alignment
    Rect getDestinationRect(float bx, float by, float bw, float bh, float iw, float ih) {
        float scaleX = 1.0f;
        float scaleY = 1.0f;

        // Avoid division by zero
        if (iw == 0 || ih == 0) return Rect(bx, by, 0, 0);

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

        // Alignment logic: 0.0 (left/top) ... 0.5 (center) ... 1.0 (right/bottom)
        // Adjust based on your Alignment class implementation (assuming -1 to 1 or 0 to 1)
        // Here assuming 0.0 to 1.0 where 0.5 is center.
        float dx = (bw - finalW) * properties.alignment.x;
        float dy = (bh - finalH) * properties.alignment.y;

        return Rect(bx + dx, by + dy, finalW, finalH);
    }
};

#define Image(...) std::make_shared<D_Image>(ImageProperties{__VA_ARGS__})