#pragma once

#include <string>
#include <utility>
#include <algorithm>
#include <cmath>

#include "InternalDrawable.h"

enum class HorizontalAlign { Left, Center, Right };
enum class VerticalAlign   { Top, Center, Bottom };

struct TextProperties {
    std::string text;
    float scale = 1.0f;
    HorizontalAlign alignH = HorizontalAlign::Left;
    VerticalAlign alignV = VerticalAlign::Top;
    Color2D color = Color2D(1.0f, 1.0f, 1.0f, 1.0f);
};

class D_Text : public InternalDrawable {
public:
    explicit D_Text(TextProperties properties) 
        : properties(std::move(properties)) {};

    void init() override {
        // No resources to load for simple text
    }

    void layout(BoxConstraints constraints) override {
        // 1. Measure Text
        float textW = 0.0f;
        float textH = 0.0f;
        
        Rect size = hmui->getGraphicsContext()->calculateTextBounds(properties.text.c_str());
        textW = size.width * properties.scale;
        textH = size.height * properties.scale;

        // 2. Apply Constraints
        // If constraints are loose (0 to Infinity), we take the text size.
        // If constraints are tight (Width=100), we take 100.
        bounds.width = std::clamp(textW, constraints.minWidth, constraints.maxWidth);
        bounds.height = std::clamp(textH, constraints.minHeight, constraints.maxHeight);

        // Store measured content size for alignment calculation in Draw
        contentSize = Rect(0, 0, textW, textH);
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        float drawX = x;
        float drawY = y;

#ifdef DEBUG_COMPONENTS
        // Draw bounds for debugging
        ctx->drawRect(Rect(x, y, bounds.width, bounds.height), Color2D(1.0f, 0.0f, 1.0f, 1.0f));
#endif

        // If layout was skipped (shouldn't happen), measure now
        if (contentSize.width == 0 && !properties.text.empty()) {
            Rect size = ctx->calculateTextBounds(properties.text.c_str());
            contentSize.width = size.x * properties.scale;
            contentSize.height = size.y * properties.scale;
        }

        // Horizontal Alignment
        // Calculate the empty space inside the bounds and offset accordingly
        float freeSpaceW = bounds.width - contentSize.width;
        switch (properties.alignH) {
            case HorizontalAlign::Left:   
                drawX = x; 
                break;
            case HorizontalAlign::Center: 
                drawX = x + (freeSpaceW * 0.5f); 
                break;
            case HorizontalAlign::Right:  
                drawX = x + freeSpaceW; 
                break;
        }

        // Vertical Alignment
        float freeSpaceH = bounds.height - contentSize.height;
        switch (properties.alignV) {
            case VerticalAlign::Top:    
                drawY = y; 
                break;
            case VerticalAlign::Center: 
                drawY = y + (freeSpaceH * 0.5f); 
                break;
            case VerticalAlign::Bottom: 
                drawY = y + freeSpaceH; 
                break;
        }

        ctx->drawText(drawX, drawY, properties.text.c_str(), properties.scale, properties.color); 
    }

    void onUpdate(float delta) override {}

    void dispose() override {}

    Rect getBounds() const override {
        return bounds;
    }

    void setBounds(const Rect& rect) override {
        bounds = rect;
    }

    TextProperties properties;

protected:
    Rect bounds;      // The size of the widget box
    Rect contentSize; // The actual size of the text glyphs
};

#define Text(...) \
    std::make_shared<D_Text>(TextProperties{__VA_ARGS__})