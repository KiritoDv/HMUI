#pragma once

#include <string>
#include <utility>

#include "InternalDrawable.h"

enum class HorizontalAlign { Left, Center, Right };
enum class VerticalAlign   { Top, Center, Bottom };

struct TextProperties {
    std::string text;

    float x = 0;
    float y = 0;
    float scale = 1.0f;
    HorizontalAlign alignH = HorizontalAlign::Center;
    VerticalAlign alignV = VerticalAlign::Center;
    //EdgeInsets margin = EdgeInsets();
    //Alignment alignment = Alignment::TopLeft;
    //bool clipToBounds = false;
    Color2D color = Color2D(1.0f, 1.0f, 1.0f, 0.0f);
};

class D_Text : public InternalDrawable {
public:
    explicit D_Text(
        TextProperties properties
    ) : properties(std::move(properties)) {};

    void init() override {
        // if (properties.child) {
        //     properties.child->init();
        //     properties.child->setParent(shared_from_this());
        //     bounds.width = properties.width > 0 ? properties.width : properties.child->getBounds().width + properties.padding.left + properties.padding.right;
        //     bounds.height = properties.height > 0 ? properties.height : properties.child->getBounds().height + properties.padding.top + properties.padding.bottom;
        // } else {
        //     bounds.width = properties.width;
        //     bounds.height = properties.height;
        // }
    }

    void dispose() override {
        // if (properties.child) {
        //     properties.child->dispose();
        // }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        float drawX = x;
        float drawY = y;
        ImVec2 textSize = ImGui::CalcTextSize(properties.text.c_str());
        switch (properties.alignH) {
            case HorizontalAlign::Left:   drawX = x + bounds.width - textSize.x; break;
            case HorizontalAlign::Center: drawX = x + (bounds.width - textSize.x) * 0.5f; break;
            case HorizontalAlign::Right:  drawX = x; break;
        }

        switch (properties.alignV) {
            case VerticalAlign::Top:    drawY = y + bounds.height - textSize.y; break;
            case VerticalAlign::Center: drawY = y + (bounds.height - textSize.y) * 0.5f; break;
            case VerticalAlign::Bottom: drawY = y; break;
        }

        ImGui::SetWindowFontScale(properties.scale);
        ctx->drawText(drawX, drawY, properties.text.c_str(), properties.color);

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

    TextProperties properties;
protected:
};

#define TextW(...) \
    std::make_shared<D_Text>(TextProperties{__VA_ARGS__})