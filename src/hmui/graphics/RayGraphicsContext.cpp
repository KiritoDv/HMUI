#include "RayGraphicsContext.h"
#include <unordered_map>
#include <string>
#include "raylib.h"

void RayGraphicsContext::init() {

}

void RayGraphicsContext::dispose() {

}

void RayGraphicsContext::drawLine(float x1, float y1, float x2, float y2, const Color2D& color) {
    DrawLineEx(Vector2{x1, y1}, Vector2{x2, y2}, 1.0f, Color {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void RayGraphicsContext::drawRect(const Rect& rect, const Color2D& color) {
    DrawRectangleLinesEx(Rectangle{rect.x, rect.y, rect.width, rect.height}, 1.0f, Color {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void RayGraphicsContext::fillRect(const Rect& rect, const Color2D& color) {
    DrawRectangle((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, Color {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void RayGraphicsContext::drawText(float x, float y, const char* text, float scale, const Color2D& color) {
    // TODO: Implement text system later
}

void RayGraphicsContext::drawImage(const Rect& rect, ImageHandle* texture, const Color2D& color, float scale) {
    Texture2D tex = *((Texture2D*) texture->handle);
    Rectangle destRect = { rect.x, rect.y, rect.width * scale, rect.height * scale };
    DrawTexturePro(tex, Rectangle{0, 0, (float)tex.width, (float)tex.height}, destRect, Vector2{0, 0}, 0.0f, Color {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void RayGraphicsContext::drawImageEx(const Rect& rect, const Rect& srcRect, ImageHandle* texture, const Color2D& color) {
    Texture2D tex = *((Texture2D*) texture->handle);
    Rectangle destRect = { rect.x, rect.y, rect.width, rect.height };
    Rectangle sourceRect = { srcRect.x, srcRect.y, srcRect.width, srcRect.height };
    DrawTexturePro(tex, sourceRect, destRect, Vector2{0, 0}, 0.0f, Color {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void RayGraphicsContext::setScissor(const Rect& rect) {
    BeginScissorMode((int) rect.x, (int) rect.y, (int) rect.width, (int) rect.height);
}

void RayGraphicsContext::clearScissor() {
    EndScissorMode();
}

Rect RayGraphicsContext::calculateTextBounds(std::string text) {
    // TODO: Implement text bounds calculation using Raylib's text measurement functions
    return Rect{0, 0, 0, 0};
}

void RayGraphicsContext::build(GfxList* gen) {
    // Not needed for Raylib context
}