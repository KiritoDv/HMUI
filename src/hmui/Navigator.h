#pragma once

#include <vector>
#include <memory>

#include "widgets/Drawable.h"
#include "graphics/GraphicsContext.h"

class Navigator {
public:
    static void pushReplacement(const std::string& route);
    static void pushReplacement(const std::shared_ptr<InternalDrawable>& view);
    static void push(const std::string& route);
    static void push(const std::shared_ptr<InternalDrawable>& view);
    static void pop();
};