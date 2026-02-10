#include "Navigator.h"
#include "widgets/AppContext.h"

void Navigator::pushReplacement(const std::string& route) {
    auto appContext = D_AppContext::get();
    if (!appContext) {
        throw std::runtime_error("AppContext missing, did you forget to wrap your app with AppContext?");
    }

    auto routeBuilderOpt = appContext->getRouteBuilder(route);
    if (!routeBuilderOpt.has_value()) {
        throw std::runtime_error("Route not found: " + route);
    }

    auto view = routeBuilderOpt.value()();
    appContext->replace(view);
}

void Navigator::pushReplacement(const std::shared_ptr<InternalDrawable>& view) {
    auto appContext = D_AppContext::get();
    if (!appContext) {
        throw std::runtime_error("AppContext missing, did you forget to wrap your app with AppContext?");
    }

    appContext->replace(view);
}

void Navigator::push(const std::string& route) {
    auto appContext = D_AppContext::get();
    if (!appContext) {
        throw std::runtime_error("AppContext missing, did you forget to wrap your app with AppContext?");
    }

    auto routeBuilderOpt = appContext->getRouteBuilder(route);
    if (!routeBuilderOpt.has_value()) {
        throw std::runtime_error("Route not found: " + route);
    }

    auto view = routeBuilderOpt.value()();
    appContext->push(view);
}

void Navigator::push(const std::shared_ptr<InternalDrawable>& view) {
    auto appContext = D_AppContext::get();
    if (!appContext) {
        throw std::runtime_error("AppContext missing, did you forget to wrap your app with AppContext?");
    }

    appContext->push(view);
}

void Navigator::pop() {
    auto appContext = D_AppContext::get();
    if (!appContext) {
        throw std::runtime_error("AppContext missing, did you forget to wrap your app with AppContext?");
    }

    appContext->pop();
}