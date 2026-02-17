#include "HMUI.h"

#include <utility>
#include <stdexcept>

#include "widgets/AppContext.h"
#include "widgets/InternalDrawable.h"
#include "graphics/GraphicsContext.h"
#include "input/FocusManager.h"
#include "Navigator.h"

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

    // Call setVisibility(false) if UI should be disabled by default
    this->active = true;
}

void HMUI::setRouter(const std::shared_ptr<InternalDrawable>& _drawable) {
    if (nullptr == _drawable) {
        throw std::invalid_argument("Router/root view cannot be null");
    }

    this->drawable = _drawable;
    this->drawable->init(); // Init resources (textures, etc.)
}

void HMUI::draw(GfxList* out, int width, int height) {
    if(!this->active || (nullptr == this->drawable)) {
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

void HMUI::update(float delta) {
    if(!this->active || (nullptr == this->drawable)) {
        return;
    }

    this->drawable->onUpdate(delta);

    // --- Controller Input Handling ---
    auto os = this->osContext;
    os->update();

    Coord mouseDelta = os->getMouseDelta();
    if (std::abs(mouseDelta.x) > 0.0f || std::abs(mouseDelta.y) > 0.0f) {
        FocusManager::get()->blur();
    }

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

        handleBack();
    }
}

void HMUI::handleBack() {
    auto os = HMUI::Instance->getOSContext();
    auto current = D_AppContext::get();
    if (!current) {
        return;
    }
    auto page = current->getView();

    for (size_t i = 0; i < 4; i++) {
        if (os->isBackButtonPressed(i)) {
            if (page) {
                page->onBack(i);
            } else {
                Navigator::pop();
            }
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
