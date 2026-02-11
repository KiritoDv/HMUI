#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "hmui/widgets/InternalDrawable.h"

// Simple callback for focus events
using FocusCallback = std::function<void()>;

struct FocusNode {
    std::string id;
    std::weak_ptr<InternalDrawable> widget; // Reference to the widget (to get bounds)
    FocusCallback onFocus;
    FocusCallback onBlur;
    FocusCallback onSubmit;
};

enum class FocusDirection { Up, Down, Left, Right };

class FocusManager {
public:
    static std::shared_ptr<FocusManager> instance;
    static std::shared_ptr<FocusManager> get() { 
        if(!instance) instance = std::make_shared<FocusManager>();
        return instance; 
    }

    void registerNode(std::shared_ptr<FocusNode> node);
    void unregisterNode(std::shared_ptr<FocusNode> node);

    // Call this when the UI rebuilds or a view is popped
    void clear();
    void blur();

    // The core navigation logic
    void moveFocus(FocusDirection dir);
    void submit();

    // Check if a specific node is focused (for visual styling)
    bool isFocused(const std::shared_ptr<FocusNode>& node);

private:
    std::vector<std::shared_ptr<FocusNode>> nodes;
    std::shared_ptr<FocusNode> currentFocus;

    std::vector<std::weak_ptr<FocusNode>> focusHistory; 

    void setFocus(std::shared_ptr<FocusNode> node);

    // Helpers for spatial math
    float getDistance(const Rect& r1, const Rect& r2);
    Rect getRect(const std::weak_ptr<InternalDrawable>& ptr);
};