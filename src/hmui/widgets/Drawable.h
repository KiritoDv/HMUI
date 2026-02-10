#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <stdexcept>
#include <memory>

class Drawable : public InternalDrawable {
public:
    Drawable() = default;
    ~Drawable() override = default;

    virtual std::shared_ptr<InternalDrawable> build() {
        throw std::runtime_error("build() method must be implemented");
    }

    void init() override {
        self = this->build();
        if (!self) {
             throw std::runtime_error("build() returned nullptr");
        }
        self->init();
    }

    void layout(BoxConstraints constraints) override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }
        self->layout(constraints);
    }

    void dispose() override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }
        self->dispose();
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }
        // Just forward the draw call. Position is handled by the parent calling this with correct x/y.
        self->onDraw(ctx, x, y);
    }

    void onUpdate(float delta) override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }
        self->onUpdate(delta);
    }

    void setBounds(const Rect& rect) override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }
        self->setBounds(rect);
        InternalDrawable::setBounds(rect);
    }

    Rect getBounds() const override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }
        return self->getBounds();
    }

    std::shared_ptr<InternalDrawable> getParent() const override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }
        return self->getParent();
    }

    void setParent(const std::shared_ptr<InternalDrawable>& _parent) override {
        if (self == nullptr) {
            throw std::runtime_error("Drawable has not been initialized, forgot to call super.init()?");
        }
        self->setParent(_parent);
    }

protected:
    std::shared_ptr<InternalDrawable> self;
};