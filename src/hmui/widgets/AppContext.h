#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <optional>
#include "InternalDrawable.h"

using RouteBuilder = std::function<std::shared_ptr<InternalDrawable>()>;

struct AppContextProperties {
    std::unordered_map<std::string, RouteBuilder> routes;
    std::string initialRoute;
};

class D_AppContext : public InternalDrawable {
public:
    explicit D_AppContext(AppContextProperties props)
        : properties(std::move(props)) {}

    static std::shared_ptr<D_AppContext> create(AppContextProperties props) {
        if (instance) throw std::runtime_error("AppContext already exists");
        auto ptr = std::make_shared<D_AppContext>(std::move(props));
        instance = ptr;
        return ptr;
    }

    static std::shared_ptr<D_AppContext> get() { return instance; }

    std::optional<RouteBuilder> getRouteBuilder(const std::string& route) {
        auto it = properties.routes.find(route);
        if (it != properties.routes.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void push(std::shared_ptr<InternalDrawable> view) {
        if (!view) return;

        view->init();
        view->setParent(shared_from_this());
        view->setBounds(getBounds());

        stack.push_back(view);
    }

    void pop() {
        if (stack.size() <= 1) return;
        auto oldView = stack.back();
        oldView->dispose();
        stack.pop_back();

        if (!stack.empty()) stack.back()->setBounds(getBounds());
    }

    void replace(std::shared_ptr<InternalDrawable> view) {
        if (!stack.empty()) {
            stack.pop_back();
        }
        push(view);
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (!stack.empty()) {
            stack.back()->onDraw(ctx, x, y);
        }
    }

    void onUpdate(float delta) override {
        if (!stack.empty()) {
            stack.back()->onUpdate(delta);
        }
    }

    void setBounds(const Rect& rect) override {
        InternalDrawable::setBounds(rect);
        if (!stack.empty()) {
            stack.back()->setBounds(rect);
        }
    }
    void setParent(const std::shared_ptr<InternalDrawable>&) override { 
        throw std::runtime_error("AppContext is root"); 
    }
    std::shared_ptr<InternalDrawable> getParent() const override { return nullptr; }

protected:
    inline static std::shared_ptr<D_AppContext> instance = nullptr;
    std::vector<std::shared_ptr<InternalDrawable>> stack;
    AppContextProperties properties;
};

#define AppContext(...) D_AppContext::create({__VA_ARGS__})