#include "hmui/widgets/InternalDrawable.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

typedef std::vector<std::shared_ptr<InternalDrawable>> ChildrenList;

struct RowProperties {
    float spacing = 0.0f;
    MainAxisAlignment mainAxisAlignment = MainAxisAlignment::START;
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::CENTER;
    ChildrenList children;
};

class D_Row : public InternalDrawable {
public:
    explicit D_Row(
        const RowProperties& properties
    ) : children(properties.children), 
        mainAxisAlignment(properties.mainAxisAlignment), 
        crossAxisAlignment(properties.crossAxisAlignment), spacing(properties.spacing) {};

    void init() override {
        for (const auto& child : children) {
            child->init();
            child->setParent(shared_from_this());
        }
    }

    void layout(BoxConstraints constraints) override {
        float maxChildHeight = 0.0f;
        float totalChildrenWidth = 0.0f;
        std::vector<Rect> childSizes;
        childSizes.reserve(children.size());

        // Row Constraints: Unbounded width for children (to get intrinsic size), bounded height.
        BoxConstraints childConstraints(0.0f, INFINITY, 0.0f, constraints.maxHeight);

        // 1. Measure Children
        for (auto& child : children) {
            child->layout(childConstraints);
            Rect size = child->getBounds();
            childSizes.push_back(size);

            maxChildHeight = std::max(maxChildHeight, size.height);
            totalChildrenWidth += size.width;
        }

        // 2. Determine Final Dimensions
        float finalWidth = std::max(constraints.minWidth, totalChildrenWidth);
        float finalHeight = std::max(constraints.minHeight, maxChildHeight);

        if (constraints.maxWidth != INFINITY && constraints.maxWidth == constraints.minWidth) {
            finalWidth = constraints.maxWidth;
        }
        if (constraints.maxHeight != INFINITY && constraints.maxHeight == constraints.minHeight) {
            finalHeight = constraints.maxHeight;
        }

        bounds.width = std::clamp(finalWidth, constraints.minWidth, constraints.maxWidth);
        bounds.height = std::clamp(finalHeight, constraints.minHeight, constraints.maxHeight);

        // 3. Calculate Main Axis (Horizontal) Spacing
        float freeSpace = bounds.width - totalChildrenWidth;
        float xOffset = 0.0f;
        float spaceBetween = spacing;

        if (freeSpace > 0 && !children.empty()) {
            switch(mainAxisAlignment) {
                case MainAxisAlignment::START: 
                    xOffset = 0.0f; 
                    break;
                case MainAxisAlignment::END: 
                    xOffset = freeSpace; 
                    break;
                case MainAxisAlignment::CENTER: 
                    xOffset = freeSpace / 2.0f; 
                    break;
                case MainAxisAlignment::SPACE_BETWEEN: 
                    if (children.size() > 1) {
                        spaceBetween = spacing + freeSpace / (children.size() - 1);
                    }
                    break;
                case MainAxisAlignment::SPACE_AROUND: 
                    spaceBetween = spacing + freeSpace / children.size();
                    xOffset = spaceBetween / 2.0f;
                    break;
            }
        }

        // 4. Position Children
        float currentX = xOffset;
        for (size_t i = 0; i < children.size(); ++i) {
            auto& child = children[i];
            Rect size = childSizes[i];

            // Calculate Cross Axis (Vertical) Alignment
            float yOffset = 0.0f;
            switch(crossAxisAlignment) {
                case CrossAxisAlignment::START: // Top align
                    yOffset = 0.0f; 
                    break;
                case CrossAxisAlignment::CENTER: // Center align
                    yOffset = (bounds.height - size.height) / 2.0f; 
                    break;
                case CrossAxisAlignment::END: // Bottom align
                    yOffset = bounds.height - size.height; 
                    break;
            }

            child->setBounds(Rect(currentX, yOffset, size.width, size.height));
            currentX += size.width + spaceBetween;
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        for (const auto& child : children) {
            Rect childLocalParams = child->getBounds();
            child->onDraw(ctx, x + childLocalParams.x, y + childLocalParams.y);
        }
    }

    void onUpdate(float delta) override {
        for (const auto& child : children) {
            child->onUpdate(delta);
        }
    }

    Rect getBounds() const override {
        return bounds;
    }

    void setBounds(const Rect& rect) override {
        bounds = rect;
    }

    void dispose() override {
        for (auto& child : children) {
            child->dispose();
        }
    }

protected:
    std::vector<std::shared_ptr<InternalDrawable>> children;
    float spacing;
    MainAxisAlignment mainAxisAlignment;
    CrossAxisAlignment crossAxisAlignment;
    Rect bounds;
};

#define Row(...) std::make_shared<D_Row>(RowProperties{__VA_ARGS__})
