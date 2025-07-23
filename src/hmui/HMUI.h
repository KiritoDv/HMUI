#pragma once

#include <memory>
#include "hmui/graphics/GraphicsContext.h"
#include "hmui/os/OSContext.h"

class InternalDrawable;

class HMUI : public std::enable_shared_from_this<HMUI> {
public:
    static HMUI* Instance;

    virtual ~HMUI();
    void initialize(std::shared_ptr<GraphicsContext> ctx, std::shared_ptr<OSContext> osCtx);
    void show(const std::shared_ptr<InternalDrawable>& drawable);
    void close();
    [[nodiscard]] bool isActive() const {
        return drawable != nullptr;
    }
    void draw(GfxList** out, int width, int height);
    void update(float delta);

    std::shared_ptr<OSContext> getOSContext() {
        return this->osContext;
    }
private:
    std::shared_ptr<InternalDrawable> drawable;
    std::shared_ptr<GraphicsContext> context;
    std::shared_ptr<OSContext> osContext;
};