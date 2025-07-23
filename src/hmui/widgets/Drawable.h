#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <stdexcept>

class Drawable : public InternalDrawable {
public:
    Drawable() = default;
    ~Drawable() override = default;

    virtual std::shared_ptr<InternalDrawable> build() {
        throw std::runtime_error("build() method must be implemented");
    }

    virtual void init() {
        self = this->build();
        self->init();
    }

    virtual void dispose() {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }

        self->dispose();
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }

        auto _bounds = self->getBounds();
        self->setBounds(Rect(x, y, _bounds.width, _bounds.height));
        self->onDraw(ctx, x, y);
    }

    void onUpdate(float delta) override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }

        self->onUpdate(delta);
    }

protected:
    std::shared_ptr<InternalDrawable> self;
};