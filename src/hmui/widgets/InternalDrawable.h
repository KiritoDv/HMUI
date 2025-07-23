#pragma once

#include <memory>
#include <utility>
#include "hmui/HMUI.h"
#include "hmui/graphics/GraphicsContext.h"

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

class InternalDrawable : public std::enable_shared_from_this<InternalDrawable> {
public:
    InternalDrawable() : bounds(Rect(0, 0, 0, 0)) {}
    virtual ~InternalDrawable() = default;

    virtual void init() {}

    virtual void dispose() {}

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