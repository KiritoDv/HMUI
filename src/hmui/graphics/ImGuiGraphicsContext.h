#pragma once

#include "GraphicsContext.h"

class ImGuiGraphicsContext : public GraphicsContext {
public:
    void init() override;
    void dispose() override;
    void drawLine(float x1, float y1, float x2, float y2, const Color2D& color) override;
    void drawRect(const Rect& rect, const Color2D& color) override;
    void fillRect(const Rect& rect, const Color2D& color) override;
    void drawText(float x, float y, const char* text, const Color2D& color) override;
    void drawImage(const Rect& rect, const char* texture, const Color2D& color, float scale = 1.0f) override;
    void drawImageEx(const Rect& rect, const Rect& srcRect, const char* texture, const Color2D& color) override;
    void setScissor(const Rect& rect) override;
    void clearScissor() override;

    void build(GfxList* out) override;
    ~ImGuiGraphicsContext() = default;
};