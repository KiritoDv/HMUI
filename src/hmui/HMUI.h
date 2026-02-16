#pragma once

#include <vector>
#include <memory>
#include "graphics/GraphicsContext.h"
#include "os/OSContext.h"

class InternalDrawable;

class HMUI : public std::enable_shared_from_this<HMUI> {
public:
    static HMUI* Instance;

    virtual ~HMUI();
    void initialize(std::shared_ptr<GraphicsContext> ctx, std::shared_ptr<OSContext> osCtx);
    void setRouter(const std::shared_ptr<InternalDrawable>& drawable);
    void close();

    void setActive(bool state);
    [[nodiscard]] bool isActive() const {
        return active;
    }

    void draw(GfxList* out, int width, int height);
    void update(float delta);

    std::shared_ptr<GraphicsContext> getGraphicsContext() {
        return this->context;
    }

    std::shared_ptr<OSContext> getOSContext() {
        return this->osContext;
    }
private:
    std::shared_ptr<InternalDrawable> drawable;
    std::shared_ptr<GraphicsContext> context;
    std::shared_ptr<OSContext> osContext;
    bool active;

    // Implement this later to avoid hitting multiple widgets when using GestureDetector
    std::vector<std::shared_ptr<InternalDrawable>> searchTree;
};
