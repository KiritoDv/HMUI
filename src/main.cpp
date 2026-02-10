#include "hmui/HMUI.h"
#include "hmui/os/RayOSContext.h"
#include "raylib.h"
#include "rlImGui.h"
#include <imgui.h>
#include "hmui/demo/DemoView.h"
#include "hmui/graphics/ImGuiGraphicsContext.h"

int main() {
    std::shared_ptr<HMUI> hmui = std::make_shared<HMUI>();
    InitWindow(800, 600, "HMUI Demo");
    SetTargetFPS(60);

    hmui->initialize(std::make_shared<ImGuiGraphicsContext>(), std::make_shared<RayOSContext>());
    hmui->show(std::make_shared<DemoView>());

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    rlImGuiSetup(false);
    while (!WindowShouldClose()) {
        hmui->update(GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);

        rlImGuiBegin();
        ImGui::SetNextWindowSize(ImVec2(GetScreenWidth(), GetScreenHeight()), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("HMUI Debug Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        auto ctx = GfxList { (void*) ImGui::GetWindowDrawList() };
        hmui->draw(&ctx, GetScreenWidth(), GetScreenHeight());
        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
