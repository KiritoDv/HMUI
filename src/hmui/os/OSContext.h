#pragma once

struct Coord {
    float x, y;

    Coord() : x(0), y(0) {}

    Coord(float xPos, float yPos) : x(xPos), y(yPos) {}

    bool operator==(const Coord& other) const {
        return (x == other.x && y == other.y);
    }

    bool operator!=(const Coord& other) const {
        return !(*this == other);
    }
};

enum class ControllerAxis {
    LEFT_X        = 0,
    LEFT_Y        = 1,
    RIGHT_X       = 2,
    RIGHT_Y       = 3,
    LEFT_TRIGGER  = 4,
    RIGHT_TRIGGER = 5 
};

enum class ControllerButton {
    UNKNOWN = 0,
    LEFT_FACE_UP,
    LEFT_FACE_RIGHT,
    LEFT_FACE_DOWN,
    LEFT_FACE_LEFT,
    RIGHT_FACE_UP,
    RIGHT_FACE_RIGHT,
    RIGHT_FACE_DOWN,
    RIGHT_FACE_LEFT,
    LEFT_TRIGGER_1, 
    RIGHT_TRIGGER_1,
    LEFT_TRIGGER_2, 
    RIGHT_TRIGGER_2,
    MIDDLE_LEFT,
    MIDDLE,
    MIDDLE_RIGHT,
    LEFT_THUMB,
    RIGHT_THUMB
};

class OSContext {
public:
    virtual ~OSContext() = default;

    virtual void init() = 0;
    virtual void dispose() = 0;

    virtual Coord getMouseDelta() = 0;
    virtual Coord getMousePosition() = 0;
    virtual void setMousePosition(Coord& pos) = 0;
    virtual Coord getMouseWheel() = 0;
    virtual bool isMouseButtonPressed(int button) = 0;
    virtual bool isMouseButtonReleased(int button) = 0;
    virtual bool isMouseButtonDown(int button) = 0;
    virtual void setMouseCursor(int cursor) = 0;
    virtual bool isTouchDevice() = 0;
    virtual bool isTouchActive() = 0;
    virtual void setClipboardText(const char* text) = 0;
    virtual const char* getClipboardText() = 0;
    virtual void showCursor(bool show) = 0;
    virtual bool isGamepadAvailable(int id) = 0;
    virtual bool isGamepadButtonPressed(int id, ControllerButton button) = 0;
    virtual float getGamepadAxis(int id, ControllerAxis axis) = 0;
};