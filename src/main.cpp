#include "hmui/HMUI.h"
#include "hmui/demo/DemoView.h"
#include "hmui/graphics/RayGraphicsContext.h"
#include "hmui/os/RayOSContext.h"
#include "raylib.h"

int main() {
    std::shared_ptr<HMUI> hmui = std::make_shared<HMUI>();
    InitWindow(800, 600, "HMUI Demo");
    SetTargetFPS(60);

    hmui->initialize(std::make_shared<RayGraphicsContext>(), std::make_shared<RayOSContext>());
    hmui->show(std::make_shared<DemoView>());

    SetWindowState(FLAG_WINDOW_RESIZABLE);

    while (!WindowShouldClose()) {
        hmui->update(GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);
        hmui->draw(nullptr, GetScreenWidth(), GetScreenHeight());
        EndMode2D();
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
