#include "HMUI.h"

#include <utility>
#include <stdexcept>

#include "widgets/InternalDrawable.h"
#include "graphics/GraphicsContext.h"
#include "input/FocusManager.h"

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
    // ... existing update logic ...

    // --- Controller Input Handling ---
    auto os = this->osContext;
    static float inputTimer = 0.0f;
    inputTimer -= delta;

    if (inputTimer <= 0.0f && os->isGamepadAvailable(0)) {
        // D-Pad or Stick Thresholds
        float x = os->getGamepadAxis(0, ControllerAxis::LEFT_X);
        float y = os->getGamepadAxis(0, ControllerAxis::LEFT_Y);
        
        bool moved = false;
        
        if (y < -0.5f || os->isGamepadButtonPressed(0, ControllerButton::LEFT_FACE_UP)) {
            FocusManager::get()->moveFocus(FocusDirection::Up);
            moved = true;
        } else if (y > 0.5f || os->isGamepadButtonPressed(0, ControllerButton::LEFT_FACE_DOWN)) {
            FocusManager::get()->moveFocus(FocusDirection::Down);
            moved = true;
        } else if (x < -0.5f || os->isGamepadButtonPressed(0, ControllerButton::LEFT_FACE_LEFT)) {
            FocusManager::get()->moveFocus(FocusDirection::Left);
            moved = true;
        } else if (x > 0.5f || os->isGamepadButtonPressed(0, ControllerButton::LEFT_FACE_RIGHT)) {
            FocusManager::get()->moveFocus(FocusDirection::Right);
            moved = true;
        }

        if (moved) inputTimer = 0.2f; // simple debounce

        // Handle Submit (A Button)
        if (os->isGamepadButtonPressed(0, ControllerButton::RIGHT_FACE_DOWN)) {
            FocusManager::get()->submit();
            inputTimer = 0.2f;
        }
    }
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