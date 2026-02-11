#pragma once

#include <memory>
#include <utility>
#include "hmui/HMUI.h"
#include "hmui/graphics/GraphicsContext.h"
#include <algorithm>
#include <cmath>

enum class CrossAxisAlignment {
    START,
    CENTER,
    END
};

enum class MainAxisAlignment {
    START,
    CENTER,
    END,
    SPACE_BETWEEN,
    SPACE_AROUND
};

struct BoxConstraints {
    float minWidth = 0.0f;
    float maxWidth = INFINITY;
    float minHeight = 0.0f;
    float maxHeight = INFINITY;

    BoxConstraints() = default;
    BoxConstraints(float minW, float maxW, float minH, float maxH)
        : minWidth(minW), maxWidth(maxW), minHeight(minH), maxHeight(maxH) {}

    // Helper to create loose constraints (0 to max)
    static BoxConstraints loose(float w, float h) {
        return BoxConstraints(0, w, 0, h);
    }
    
    // Helper to create tight constraints (exact size)
    static BoxConstraints tight(float w, float h) {
        return BoxConstraints(w, w, h, h);
    }

    // Clamp a size within these constraints
    struct Size { float width; float height; };
    Size constrain(float w, float h) const {
        return Size{
            std::clamp(w, minWidth, maxWidth),
            std::clamp(h, minHeight, maxHeight)
        };
    }
};

struct EdgeInsets {
    float left, top, right, bottom;

    explicit EdgeInsets(float l = 0, float t = 0, float r = 0, float b = 0)
            : left(l), top(t), right(r), bottom(b) {}

    static EdgeInsets all(float value) {
        return EdgeInsets(value, value, value, value);
    }

    static EdgeInsets symmetric(float horizontal, float vertical) {
        return EdgeInsets(horizontal, vertical, horizontal, vertical);
    }

    static EdgeInsets only(float left, float top, float right, float bottom) {
        return EdgeInsets(left, top, right, bottom);
    }
};

struct Alignment {
    float x;
    float y;

    static const Alignment TopLeft() { return { 0.0f, 0.0f }; }
    static const Alignment TopCenter() { return { 0.5f, 0.0f }; }
    static const Alignment TopRight() { return { 1.0f, 0.0f }; }
    static const Alignment CenterLeft() { return { 0.0f, 0.5f }; }
    static const Alignment Center() { return { 0.5f, 0.5f }; }
    static const Alignment CenterRight() { return { 1.0f, 0.5f }; }
    static const Alignment BottomLeft() { return { 0.0f, 1.0f }; }
    static const Alignment BottomCenter() { return { 0.5f, 1.0f }; }
    static const Alignment BottomRight() { return { 1.0f, 1.0f }; }
};

enum class BoxFit {
    Fill,       // Stretch to fill the box (distorts aspect ratio)
    Contain,    // Scale to fit inside (maintains aspect ratio, may leave gaps)
    Cover,      // Scale to cover the box (maintains aspect ratio, may crop/overflow)
    FitWidth,   // Scale to match the width
    FitHeight,  // Scale to match the height
    None,       // Do not scale (center original size)
    ScaleDown   // Like None, but scales down if image is too large (like Contain)
};

class InternalDrawable : public std::enable_shared_from_this<InternalDrawable> {
public:
    InternalDrawable() : bounds(Rect(0, 0, 0, 0)) {}
    virtual ~InternalDrawable() = default;

    virtual void init() {}

    virtual void dispose() {}

    virtual void layout(BoxConstraints constraints) {
        // Default implementation for non-container drawables (e.g. Text, Image)
        // calculates its size based on constraints and sets bounds.width/height
    }

    virtual void onDraw(GraphicsContext* ctx, float x, float y) {}

    virtual void onUpdate(float delta) {}

    virtual Rect getBounds() const {
        return bounds;
    }

    virtual void setBounds(const Rect& rect) {
        bounds = rect;
    }

    virtual void setParent(const std::shared_ptr<InternalDrawable>& _parent) {
        parent = _parent;
    }

    virtual std::shared_ptr<InternalDrawable> getParent() const {
        return parent;
    }

protected:
    Rect bounds;
    HMUI* hmui = HMUI::Instance;
    std::shared_ptr<InternalDrawable> parent;
};