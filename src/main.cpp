#include "hmui/HMUI.h"
#include "hmui/demo/DemoView.h"
#include "hmui/graphics/RayGraphicsContext.h"
#include "hmui/os/RayOSContext.h"
#include "raylib.h"

#include "rlImGui.h"
#include "imgui.h"

int main() {
    std::shared_ptr<HMUI> hmui = std::make_shared<HMUI>();
    InitWindow(800, 600, "HMUI Demo");
    SetTargetFPS(60);

    hmui->initialize(std::make_shared<RayGraphicsContext>(), std::make_shared<RayOSContext>());
    hmui->show(std::make_shared<DemoView>());

    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    rlImGuiSetup(false);
    while (!WindowShouldClose()) {
        hmui->update(GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);

        rlImGuiBegin();
        hmui->draw(nullptr, GetScreenWidth(), GetScreenHeight());
        rlImGuiEnd();

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
