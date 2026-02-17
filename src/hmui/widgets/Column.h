#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

typedef std::vector<std::shared_ptr<InternalDrawable>> ChildrenList;

struct ColumnProperties {
    float spacing = 0.0f;
    MainAxisAlignment mainAxisAlignment = MainAxisAlignment::START;
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::START;
    ChildrenList children;
};

class D_Column : public InternalDrawable {
public:
    explicit D_Column(
        const ColumnProperties& properties
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
        float spaceBetween = spacing;

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
                        spaceBetween = spacing + freeSpace / (children.size() - 1);
                    }
                    break;
                case MainAxisAlignment::SPACE_AROUND: 
                    spaceBetween = spacing + freeSpace / children.size();
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
                case CrossAxisAlignment::START:
                    xOffset = 0.0f; 
                    break;
                case CrossAxisAlignment::CENTER:
                    xOffset = (bounds.width - size.width) / 2.0f; 
                    break;
                case CrossAxisAlignment::END:
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

#define Column(...) std::make_shared<D_Column>(ColumnProperties{__VA_ARGS__})
