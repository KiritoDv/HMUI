#include "HMUI.h"

#include <utility>
#include <stdexcept>

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

        // Init resources (textures, etc.)
        drawable->init();
    } else {
        throw std::invalid_argument("View cannot be null");
    }
}

void HMUI::draw(GfxList* out, int width, int height) {
    if(this->drawable == nullptr) {
        return;
    }

    this->context->build(out);

    // --- 1. Layout Phase ---
    // The root of the tree gets "Tight" constraints, forcing it to fill the window.
    // This triggers the cascade of layout() calls down the widget tree.
    BoxConstraints rootConstraints = BoxConstraints::tight((float)width, (float)height);
    this->drawable->layout(rootConstraints);

    // --- 2. Positioning Phase ---
    // As the root parent, we dictate that the root widget sits at (0,0).
    // layout() calculated the size, we now ensure the position is correct.
    Rect calculatedBounds = this->drawable->getBounds();
    this->drawable->setBounds(Rect(0, 0, calculatedBounds.width, calculatedBounds.height));

    // --- 3. Paint Phase ---
    // Render the tree at the determined position.
    this->drawable->onDraw(context.get(), 0, 0);
}

void HMUI::update(float delta){
    if(this->drawable == nullptr) {
        return;
    }

    this->drawable->onUpdate(delta);
}

void HMUI::close(){
    if(this->drawable == nullptr) {
        return;
    }

    this->drawable->dispose();
    this->drawable = nullptr;
}

HMUI::~HMUI() {
    this->close();
    if(this->context) {
        this->context->dispose();
        this->context = nullptr;
    }
}