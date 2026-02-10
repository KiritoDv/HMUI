#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

enum class CrossAxisAlignment {
    LEFT,
    CENTER,
    RIGHT
};

enum class MainAxisAlignment {
    START,
    CENTER,
    END,
    SPACE_BETWEEN,
    SPACE_AROUND
};

typedef std::vector<std::shared_ptr<InternalDrawable>> ChildrenList;

struct ColumnProperties {
    MainAxisAlignment mainAxisAlignment = MainAxisAlignment::START;
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::LEFT;
    ChildrenList children;
};

class D_Column : public InternalDrawable {
public:
    explicit D_Column(
        const ColumnProperties& properties
    ) : children(properties.children), 
        mainAxisAlignment(properties.mainAxisAlignment), 
        crossAxisAlignment(properties.crossAxisAlignment) {};

    void init() override {
        for (const auto& child : children) {
            child->init();
            child->setParent(shared_from_this());
        }
    }

    void layout(BoxConstraints constraints) override {
        float maxChildWidth = 0.0f;
        float totalChildrenHeight = 0.0f;
        std::vector<Rect> childSizes;
        childSizes.reserve(children.size());

        BoxConstraints childConstraints(0.0f, constraints.maxWidth, 0.0f, INFINITY);

        for (auto& child : children) {
            child->layout(childConstraints);
            Rect size = child->getBounds();
            childSizes.push_back(size);
            
            maxChildWidth = std::max(maxChildWidth, size.width);
            totalChildrenHeight += size.height;
        }

        float finalWidth = std::max(constraints.minWidth, maxChildWidth);
        float finalHeight = std::max(constraints.minHeight, totalChildrenHeight);

        if (constraints.maxWidth != INFINITY && constraints.maxWidth == constraints.minWidth) {
             finalWidth = constraints.maxWidth;
        }
        if (constraints.maxHeight != INFINITY && constraints.maxHeight == constraints.minHeight) {
             finalHeight = constraints.maxHeight;
        }

        bounds.width = std::clamp(finalWidth, constraints.minWidth, constraints.maxWidth);
        bounds.height = std::clamp(finalHeight, constraints.minHeight, constraints.maxHeight);

        float freeSpace = bounds.height - totalChildrenHeight;
        float yOffset = 0.0f;
        float spaceBetween = 0.0f;

        if (freeSpace > 0 && !children.empty()) {
            switch(mainAxisAlignment) {
                case MainAxisAlignment::START: 
                    yOffset = 0.0f; 
                    break;
                case MainAxisAlignment::END: 
                    yOffset = freeSpace; 
                    break;
                case MainAxisAlignment::CENTER: 
                    yOffset = freeSpace / 2.0f; 
                    break;
                case MainAxisAlignment::SPACE_BETWEEN: 
                    if (children.size() > 1) {
                        spaceBetween = freeSpace / (children.size() - 1);
                    }
                    break;
                case MainAxisAlignment::SPACE_AROUND: 
                    spaceBetween = freeSpace / children.size();
                    yOffset = spaceBetween / 2.0f;
                    break;
            }
        }

        float currentY = yOffset;
        for (size_t i = 0; i < children.size(); ++i) {
            auto& child = children[i];
            Rect size = childSizes[i];

            float xOffset = 0.0f;
            switch(crossAxisAlignment) {
                case CrossAxisAlignment::LEFT: 
                    xOffset = 0.0f; 
                    break;
                case CrossAxisAlignment::CENTER: 
                    xOffset = (bounds.width - size.width) / 2.0f; 
                    break;
                case CrossAxisAlignment::RIGHT: 
                    xOffset = bounds.width - size.width; 
                    break;
            }

            child->setBounds(Rect(xOffset, currentY, size.width, size.height));
            currentY += size.height + spaceBetween;
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

protected:
    std::vector<std::shared_ptr<InternalDrawable>> children;
    MainAxisAlignment mainAxisAlignment;
    CrossAxisAlignment crossAxisAlignment;
    Rect bounds;
};

#define Column(...) std::make_shared<D_Column>(ColumnProperties{__VA_ARGS__})