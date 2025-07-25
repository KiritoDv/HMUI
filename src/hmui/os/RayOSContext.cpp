#include "RayOSContext.h"

#include <raylib.h>

void RayOSContext::init() {}
void RayOSContext::dispose() {}

Coord RayOSContext::getMouseDelta() {
    auto delta = GetMouseDelta();
    return Coord(static_cast<float>(delta.x), static_cast<float>(delta.y));
}

Coord RayOSContext::getMousePosition() {
    auto pos = GetMousePosition();
    return Coord(static_cast<float>(pos.x), static_cast<float>(pos.y));
}

void RayOSContext::setMousePosition(Coord& pos) {
    SetMousePosition(static_cast<int>(pos.x), static_cast<int>(pos.y));
}

Coord RayOSContext::getMouseWheel() {
    auto wheel = GetMouseWheelMoveV();
    return Coord(wheel.x * 4.0f, wheel.y * 4.0f);
}

bool RayOSContext::isMouseButtonPressed(int button) {
    return IsMouseButtonPressed(button);
}

bool RayOSContext::isMouseButtonReleased(int button) {
    return IsMouseButtonReleased(button);
}

bool RayOSContext::isMouseButtonDown(int button) {
    return IsMouseButtonDown(button);
}

void RayOSContext::setMouseCursor(int cursor) {
    SetMouseCursor(cursor);
}

void RayOSContext::setClipboardText(const char* text) {
    SetClipboardText(text);
}

const char* RayOSContext::getClipboardText() {
    return GetClipboardText();
}

void RayOSContext::showCursor(bool show) {
    if(show){
        ShowCursor();
    } else {
        HideCursor();
    }
}
