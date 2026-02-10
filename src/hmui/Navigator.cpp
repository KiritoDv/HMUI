#include "Navigator.h"

#include "widgets/InternalDrawable.h"
#include "graphics/GraphicsContext.h"

void Navigator::push(const std::shared_ptr<InternalDrawable>& menu) {
    stack.push_back(menu);
}

void Navigator::pop() {
    if (!stack.empty())
        stack.pop_back();
}

void Navigator::replace(const std::shared_ptr<InternalDrawable>& menu) {
    if (!menu) return;
    auto newMenu = menu;
    stack.clear();
    stack.push_back(menu);
}

void Navigator::clear() {
    stack.clear();
}

void Navigator::update(float delta) {
    if (!stack.empty())
        stack.back()->onUpdate(delta); // top only
}

void Navigator::draw(GraphicsContext* ctx) {
    for (auto& menu : stack)
        menu->onDraw(ctx, 0.0f, 0.0f); // bottom → top
}

bool Navigator::empty() const {
    return stack.empty();
}
size_t Navigator::size() const {
    return stack.size();
}

const std::shared_ptr<InternalDrawable> Navigator::top() const {
    if (!stack.empty())
        return stack.back();
    return nullptr;
}