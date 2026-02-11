#pragma once

#include "OSContext.h"

class RayOSContext : public OSContext {
public:
    void init() override;
    void dispose() override;
    Coord getMouseDelta() override;
    Coord getMousePosition() override;
    void setMousePosition(Coord& pos) override;
    Coord getMouseWheel() override;
    bool isMouseButtonPressed(int button) override;
    bool isMouseButtonReleased(int button) override;
    bool isMouseButtonDown(int button) override;
    void setMouseCursor(int cursor) override;
    bool isTouchDevice() override;
    bool isTouchActive() override;
    void setClipboardText(const char* text) override;
    const char* getClipboardText() override;
    void showCursor(bool show) override;
    bool isGamepadAvailable(int id) override;
    bool isGamepadButtonPressed(int id, ControllerButton button) override;
    float getGamepadAxis(int id, ControllerAxis axis) override;
    ~RayOSContext() override = default;
};