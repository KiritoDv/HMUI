#pragma once

#include <vector>
#include <memory>

#include "graphics/GraphicsContext.h"

class InternalDrawable; // Forward declare

class Navigator {
public:
    static void pushReplacement(const std::string& route);
    static void pushReplacement(const std::shared_ptr<InternalDrawable>& view);
    static void push(const std::string& route);
    static void push(const std::shared_ptr<InternalDrawable>& view);
    static void pop();
};