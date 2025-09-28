#include "ImGuiGraphicsContext.h"
#include <unordered_map>
#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

void ImGuiGraphicsContext::init() {}
void ImGuiGraphicsContext::dispose() {}

void ImGuiGraphicsContext::drawLine(float x1, float y1, float x2, float y2, const Color2D& color) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImVec2{x1, y1}, ImVec2{x2, y2}, ImColor {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255), 
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void ImGuiGraphicsContext::drawRect(const Rect& rect, const Color2D& color) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(ImVec2{rect.x, rect.y}, ImVec2{rect.x + rect.width, rect.y + rect.height}, ImColor {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void ImGuiGraphicsContext::fillRect(const Rect& rect, const Color2D& color) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(ImVec2{rect.x, rect.y}, ImVec2{rect.x + rect.width, rect.y + rect.height}, ImColor {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void ImGuiGraphicsContext::drawText(float x, float y, const char* text, const Color2D& color) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddText(ImVec2{x, y}, ImColor {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    }, text);
}

void ImGuiGraphicsContext::drawImage(const Rect& rect, const char* texture, const Color2D& color, float scale) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
}

void ImGuiGraphicsContext::drawImageEx(const Rect& rect, const Rect& srcRect, const char* texture, const Color2D& color) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
}

void ImGuiGraphicsContext::setScissor(const Rect& rect) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRect(ImVec2{rect.x, rect.y}, ImVec2{rect.x + rect.width, rect.y + rect.height}, true);
}

void ImGuiGraphicsContext::clearScissor() {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->PopClipRect();
}

void ImGuiGraphicsContext::build(GfxList** gen) {
    // Not needed for ImGui context
}