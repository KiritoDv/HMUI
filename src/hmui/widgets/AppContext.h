#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <optional>
#include <string>
#include "InternalDrawable.h"
#include "hmui/input/FocusManager.h"

// Factory for creating routes
using RouteBuilder = std::function<std::shared_ptr<InternalDrawable>()>;

struct AppContextProperties {
    std::unordered_map<std::string, RouteBuilder> routes;
    std::string initialRoute;
};

class D_AppContext : public InternalDrawable {
public:
    explicit D_AppContext(AppContextProperties props)
        : properties(std::move(props)) {}

    // --- Singleton / Factory Management ---

    static std::shared_ptr<D_AppContext> create(AppContextProperties props) {
        if (instance) throw std::runtime_error("AppContext already exists");
        auto ptr = std::make_shared<D_AppContext>(std::move(props));
        instance = ptr;
        return ptr;
    }

    static std::shared_ptr<D_AppContext> get() { return instance; }

    // --- Layout Protocol ---

    void layout(BoxConstraints constraints) override {
        // AppContext fills the Window (or parent) completely
        bounds.width = constraints.maxWidth;
        bounds.height = constraints.maxHeight;

        // Force the active view to also fill the AppContext
        if (!stack.empty()) {
            auto& activeView = stack.back();
            
            // Pass tight constraints: Child MUST be exactly this size
            activeView->layout(BoxConstraints::tight(bounds.width, bounds.height));
            
            // Ensure child is positioned at (0,0) relative to AppContext
            activeView->setBounds(Rect(0, 0, bounds.width, bounds.height));
        }
    }

    // --- Navigation Logic ---

    std::optional<RouteBuilder> getRouteBuilder(const std::string& route) {
        auto it = properties.routes.find(route);
        if (it != properties.routes.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void push(std::shared_ptr<InternalDrawable> view) {
        if (!view) return;

        FocusManager::get()->blur();
        view->init();
        view->setParent(shared_from_this());
        stack.push_back(view);
    }
    
    // Overload to push by route name
    void pushNamed(const std::string& routeName) {
        auto builder = getRouteBuilder(routeName);
        if (builder.has_value()) {
            push(builder.value()());
        }
    }

    void pop() {
        if (stack.size() <= 1) return; // Don't pop the last view

        auto oldView = stack.back();
        oldView->dispose();
        stack.pop_back();
    }

    void replace(std::shared_ptr<InternalDrawable> view) {
        if (!stack.empty()) {
            auto oldView = stack.back();
            oldView->dispose();
            stack.pop_back();
        }
        push(view);
    }

    // --- Lifecycle ---

    void init() override {
        if (properties.initialRoute.empty()) {
            throw std::runtime_error("Initial route cannot be empty");
        }

        auto routeBuilderOpt = getRouteBuilder(properties.initialRoute);
        if (!routeBuilderOpt.has_value()) {
            throw std::runtime_error("Initial route not found: " + properties.initialRoute);
        }

        // Initialize the first view
        auto initialView = routeBuilderOpt.value()();
        push(initialView);
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        // Only draw the top-most view
        if (!stack.empty()) {
            // x, y are the absolute coordinates of AppContext (usually 0,0)
            stack.back()->onDraw(ctx, x, y);
        }
    }

    void onUpdate(float delta) override {
        // Only update the top-most view
        if (!stack.empty()) {
            stack.back()->onUpdate(delta);
        }
    }

    void dispose() override {
        // Clean up entire stack
        while (!stack.empty()) {
            stack.back()->dispose();
            stack.pop_back();
        }
    }

    // --- Bounds/Parent Boilerplate ---

    Rect getBounds() const override { return bounds; }
    
    void setBounds(const Rect& rect) override {
        bounds = rect;
        // If we are resized, we update the child immediately if it exists,
        // though usually layout() handles this logic.
        if (!stack.empty()) {
            stack.back()->setBounds(Rect(0, 0, rect.width, rect.height));
        }
    }

    void setParent(const std::shared_ptr<InternalDrawable>& parent) override { 
        // AppContext is usually the root, but if embedded, this is fine.
    }

protected:
    inline static std::shared_ptr<D_AppContext> instance = nullptr;
    std::vector<std::shared_ptr<InternalDrawable>> stack;
    AppContextProperties properties;
    Rect bounds;
};

#define AppContext(...) D_AppContext::create(AppContextProperties{__VA_ARGS__})