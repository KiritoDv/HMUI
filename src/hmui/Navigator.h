#pragma once

#include <vector>
#include <memory>

#include "widgets/Drawable.h"
#include "graphics/GraphicsContext.h"

class Navigator {
public:
    Navigator() = default;
    ~Navigator() = default;

    // ---- Stack operations ----

    // Overlay a menu (allows back / pop)
    void push(const std::shared_ptr<InternalDrawable>& menu);
    void pushName(const std::string& menuName);

    // Close top menu (back)
    void pop();

    // Replace entire stack (no back)
    void replace(const std::shared_ptr<InternalDrawable>&);

    // Remove all menus
    void clear();

    // ---- Lifecycle ----

    void update(float delta);
    void draw(GraphicsContext* ctx);

    // ---- Queries ----

    bool empty() const; // is empty
    size_t size() const;

    const std::shared_ptr<InternalDrawable> top() const;

private:
    std::vector<std::shared_ptr<InternalDrawable>> stack;
};