#include "HMUI.h"

#include <utility>

#include "widgets/InternalDrawable.h"
#include "graphics/GraphicsContext.h"

HMUI* HMUI::Instance = nullptr;

void HMUI::initialize(std::shared_ptr<GraphicsContext> ctx, std::shared_ptr<OSContext> osCtx) {
    if (Instance != nullptr) {
        throw std::runtime_error("HMUI instance already initialized");
    }
    Instance = this;
    this->context = std::move(ctx);
    this->context->init();
    this->osContext = std::move(osCtx);
    this->osContext->init();
}

void HMUI::show(const std::shared_ptr<InternalDrawable>& _drawable) {
    if (_drawable) {
        this->drawable = _drawable;

        if(drawable == nullptr) {
            throw std::runtime_error("InternalDrawable cannot be null");
        }

        auto self = shared_from_this();
        drawable->init();
    } else {
        throw std::invalid_argument("View cannot be null");
    }
}

void HMUI::draw(GfxList** out, int width, int height) {
    if(this->drawable == nullptr) {
        throw std::runtime_error("InternalDrawable cannot be null");
    }

    this->drawable->setBounds(Rect(0, 0, (float) width, (float) height));
    this->drawable->onDraw(context.get(), 0, 0);
    this->context->build(out);
}

void HMUI::update(float delta){
    if(this->drawable == nullptr) {
        throw std::runtime_error("InternalDrawable cannot be null");
    }

    this->drawable->onUpdate(delta);
}

void HMUI::close(){
    if(this->drawable == nullptr) {
        throw std::runtime_error("InternalDrawable cannot be null");
    }

    this->drawable->dispose();
    this->drawable = nullptr;
}

HMUI::~HMUI() {
    this->close();
    this->context->dispose();
    this->context = nullptr;
}
