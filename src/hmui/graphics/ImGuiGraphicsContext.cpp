#include "ImGuiGraphicsContext.h"
#include <algorithm> // For std::max

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

static ImDrawList* draw_list = nullptr;

void ImGuiGraphicsContext::init() {}
void ImGuiGraphicsContext::dispose() {}

ImVec2 normalize(const Rect& in) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    return ImVec2{ pos.x + in.x, pos.y + in.y };
}

ImVec2 normalize(float x, float y) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    return ImVec2{ pos.x + x, pos.y + y };
}

void ImGuiGraphicsContext::drawLine(float x1, float y1, float x2, float y2, const Color2D& color) {
    ImVec2 p1 = normalize(x1, y1);
    ImVec2 p2 = normalize(x2, y2);
    draw_list->AddLine(p1, p2, ImColor((int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255)));
}

void ImGuiGraphicsContext::drawRect(const Rect& rect, const Color2D& color) {
    ImVec2 pos = normalize(rect);
    draw_list->AddRect(pos, ImVec2{pos.x + rect.width, pos.y + rect.height}, 
        ImColor((int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255)));
}

void ImGuiGraphicsContext::fillRect(const Rect& rect, const Color2D& color) {
    ImVec2 pos = normalize(rect);
    draw_list->AddRectFilled(pos, ImVec2{pos.x + rect.width, pos.y + rect.height}, 
        ImColor((int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255)));
}

void ImGuiGraphicsContext::drawText(float x, float y, const char* text, float scale, const Color2D& color) {
    ImVec2 pos = normalize(x, y);
    ImGui::SetWindowFontScale(scale);
    draw_list->AddText(pos, ImColor((int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255)), text);
    ImGui::SetWindowFontScale(1.0f);
}

void ImGuiGraphicsContext::drawImage(const Rect& rect, ImageHandle* texture, const Color2D& color, float scale) {
    if (!texture) return;
    ImVec2 pos = normalize(rect);
    ImVec2 size = ImVec2{ rect.width * scale, rect.height * scale };

    draw_list->AddImage(texture->handle, pos, ImVec2{pos.x + size.x, pos.y + size.y}, 
        ImVec2{0, 0}, ImVec2{1, 1}, 
        ImColor((int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255)));
}

void ImGuiGraphicsContext::drawImageEx(const Rect& rect, const Rect& srcRect, ImageHandle* texture, const Color2D& color) {
    if (!texture) return;
    ImVec2 pos = normalize(rect);

    draw_list->AddImage(texture->handle, pos, ImVec2{pos.x + rect.width, pos.y + rect.height}, 
        ImVec2{srcRect.x, srcRect.y}, ImVec2{srcRect.x + srcRect.width, srcRect.y + srcRect.height}, 
        ImColor((int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255)));
}

void ImGuiGraphicsContext::setScissor(const Rect& rect) {
    ImVec2 pos = normalize(rect);

    float w = std::max(0.0f, rect.width);
    float h = std::max(0.0f, rect.height);

    ImVec2 min = pos;
    ImVec2 max = ImVec2(pos.x + w, pos.y + h);
    draw_list->PushClipRect(min, max, true);
}

void ImGuiGraphicsContext::clearScissor() {
    draw_list->PopClipRect();
}

Rect ImGuiGraphicsContext::calculateTextBounds(std::string text) {
    ImVec2 size = ImGui::CalcTextSize(text.c_str());
    printf("Calculated text size for '%s': %f x %f\n", text.c_str(), size.x, size.y);
    return Rect{size.x, size.y, size.x, size.y};
}

void ImGuiGraphicsContext::build(GfxList* gen) {
    draw_list = (ImDrawList*) gen->head;
}