#pragma once
#define RAYLIB_IMPLEMENTATION

#include "GraphicsContext.h"
#include <raylib.h>

namespace RGGuiGC {
Texture2D loadTexture(const char* texturePath);
}

class RayGraphicsContext : public GraphicsContext {
public:
    void init() override;
    void dispose() override;
    void drawLine(float x1, float y1, float x2, float y2, const Color2D& color) override;
    void drawRect(const Rect& rect, const Color2D& color, float thickness) override;
    void fillRect(const Rect& rect, const Color2D& color) override;
    void drawText(float x, float y, const char* text, float scale, const Color2D& color) override;
    void drawImage(const Rect& rect, ImageHandle* texture, const Color2D& color, float scale = 1.0f) override;
    void drawImageEx(const Rect& rect, const Rect& srcRect, ImageHandle* texture, const Color2D& color) override;
    void setScissor(const Rect& rect) override;
    void clearScissor() override;

    Rect calculateTextBounds(std::string text) override;

    void build(GfxList* out) override;
    ~RayGraphicsContext() = default;
};