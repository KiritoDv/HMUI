#include "ImGuiGraphicsContext.h"
#include <unordered_map>
#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

static ImDrawList* draw_list = nullptr;

void ImGuiGraphicsContext::init() {}
void ImGuiGraphicsContext::dispose() {}

ImVec2 normalize(const Rect& in) {
    ImVec2 pos = ImGui::GetWindowPos();
    return ImVec2{ pos.x + in.x, pos.y + in.y };
}

ImVec2 normalize(float x, float y) {
    ImVec2 pos = ImGui::GetWindowPos();
    return ImVec2{ pos.x + x, pos.y + y };
}

void ImGuiGraphicsContext::drawLine(float x1, float y1, float x2, float y2, const Color2D& color) {
    ImVec2 pos = normalize(x1, y1);
    draw_list->AddLine(pos, ImVec2{pos.x + (x2 - x1), pos.y + (y2 - y1) }, ImColor {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255), 
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void ImGuiGraphicsContext::drawRect(const Rect& rect, const Color2D& color) {
    ImVec2 pos = normalize(rect);
    draw_list->AddRect(pos, ImVec2{pos.x + rect.width, pos.y + rect.height}, ImColor {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void ImGuiGraphicsContext::fillRect(const Rect& rect, const Color2D& color) {
    ImVec2 pos = normalize(rect);
    draw_list->AddRectFilled(pos, ImVec2{pos.x + rect.width, pos.y + rect.height}, ImColor {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    });
}

void ImGuiGraphicsContext::drawText(float x, float y, const char* text, const Color2D& color) {
    ImVec2 pos = normalize(x, y);
    draw_list->AddText(pos, ImColor {
        (uint8_t)(color.r * 255),
        (uint8_t)(color.g * 255),
        (uint8_t)(color.b * 255),
        (uint8_t)(color.a * 255)
    }, text);
}

void ImGuiGraphicsContext::drawImage(const Rect& rect, const char* texture, const Color2D& color, float scale) {
    // auto gui = GameEngine::Instance->context->GetWindow()->GetGui();
    // if(!gui->HasTextureByName(texture)){
    //     throw std::runtime_error("Texture not found: " + std::string(texture));
    // }

    // auto tex = gui->GetTextureByName(texture);

    // ImVec2 pos = normalize(rect);
    // ImVec2 size = ImVec2{ rect.width * scale, rect.height * scale };
    // draw_list->AddImage(tex, pos, ImVec2{pos.x + size.x, pos.y + size.y}, ImVec2{0, 0}, ImVec2{1, 1}, ImColor {
    //     (uint8_t)(color.r * 255),
    //     (uint8_t)(color.g * 255),
    //     (uint8_t)(color.b * 255),
    //     (uint8_t)(color.a * 255)
    // });
}

void ImGuiGraphicsContext::drawImageEx(const Rect& rect, const Rect& srcRect, const char* texture, const Color2D& color) {
    // auto gui = GameEngine::Instance->context->GetWindow()->GetGui();
    // if(!gui->HasTextureByName(texture)){
    //     throw std::runtime_error("Texture not found: " + std::string(texture));
    // }

    // auto tex = gui->GetTextureByName(texture);
    // ImVec2 pos = normalize(rect);
    // ImVec2 size = ImVec2{ rect.width, rect.height };
    // draw_list->AddImage(tex, pos, ImVec2{pos.x + size.x, pos.y + size.y}, ImVec2{srcRect.x, srcRect.y}, ImVec2{srcRect.x + srcRect.width, srcRect.y + srcRect.height}, ImColor {
    //     (uint8_t)(color.r * 255),
    //     (uint8_t)(color.g * 255),
    //     (uint8_t)(color.b * 255),
    //     (uint8_t)(color.a * 255)
    // });
}

void ImGuiGraphicsContext::setScissor(const Rect& rect) {
    ImVec2 pos = normalize(rect);
    draw_list->PushClipRect(pos, ImVec2{pos.x + rect.width, pos.y + rect.height}, true);
}

void ImGuiGraphicsContext::clearScissor() {
    draw_list->PopClipRect();
}

void ImGuiGraphicsContext::build(GfxList* gen) {
    draw_list = (ImDrawList*) gen->head;
}