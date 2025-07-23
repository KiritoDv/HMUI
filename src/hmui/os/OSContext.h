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
    virtual void setClipboardText(const char* text) = 0;
    virtual const char* getClipboardText() = 0;
    virtual void showCursor(bool show) = 0;
};