#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include "hmui/widgets/Expanded.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

typedef std::vector<std::shared_ptr<InternalDrawable>> ChildrenList;

struct FlexBoxProperties {
    Direction direction = Direction::Horizontal;
    MainAxisAlignment mainAxisAlignment = MainAxisAlignment::START;
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::CENTER;
    ChildrenList children;
};

class D_FlexBox : public InternalDrawable {
public:
    explicit D_FlexBox(const FlexBoxProperties& properties) 
        : properties(properties) {};

    void init() override {
        for (const auto& child : properties.children) {
            child->init();
            child->setParent(shared_from_this());
        }
    }

    void layout(BoxConstraints constraints) override {
        float maxCrossSize = 0.0f;
        float usedMainSize = 0.0f;
        int totalFlex = 0;
        
        std::vector<Rect> childSizes(properties.children.size());
        bool isRow = properties.direction == Direction::Horizontal;

        // 1. Layout Non-Flexible Children
        BoxConstraints looseConstraints = isRow 
            ? BoxConstraints(0.0f, INFINITY, 0.0f, constraints.maxHeight)
            : BoxConstraints(0.0f, constraints.maxWidth, 0.0f, INFINITY);

        for (size_t i = 0; i < properties.children.size(); ++i) {
            auto& child = properties.children[i];
            
            if (auto expanded = std::dynamic_pointer_cast<D_Expanded>(child)) {
                totalFlex += expanded->getProps().flex;
            } else {
                child->layout(looseConstraints);
                Rect size = child->getBounds();
                childSizes[i] = size;
                
                maxCrossSize = std::max(maxCrossSize, isRow ? size.height : size.width);
                usedMainSize += isRow ? size.width : size.height;
            }
        }

        // 2. Determine Final Main Dimension
        float maxMainConstraint = isRow ? constraints.maxWidth : constraints.maxHeight;
        float minMainConstraint = isRow ? constraints.minWidth : constraints.minHeight;
        
        float finalMainSize = (maxMainConstraint == INFINITY) ? usedMainSize : maxMainConstraint;
        finalMainSize = std::max(finalMainSize, minMainConstraint);

        // 3. Layout Flexible Children
        float remainingMainSize = std::max(0.0f, finalMainSize - usedMainSize);

        if (totalFlex > 0) {
            float spacePerFlex = remainingMainSize / totalFlex;

            for (size_t i = 0; i < properties.children.size(); ++i) {
                auto& child = properties.children[i];
                if (auto expanded = std::dynamic_pointer_cast<D_Expanded>(child)) {
                    float flexSize = spacePerFlex * expanded->getProps().flex;
                    
                    BoxConstraints flexConstraints = isRow
                        ? BoxConstraints(flexSize, flexSize, 0.0f, constraints.maxHeight)
                        : BoxConstraints(0.0f, constraints.maxWidth, flexSize, flexSize);
                    
                    child->layout(flexConstraints);
                    Rect size = child->getBounds();
                    childSizes[i] = size;

                    maxCrossSize = std::max(maxCrossSize, isRow ? size.height : size.width);
                }
            }
        }

        // 4. Final Dimensions
        float minCrossConstraint = isRow ? constraints.minHeight : constraints.minWidth;
        float maxCrossConstraint = isRow ? constraints.maxHeight : constraints.maxWidth;
        
        float finalCrossSize = std::max(minCrossConstraint, maxCrossSize);
        if (maxCrossConstraint != INFINITY && maxCrossConstraint == minCrossConstraint) {
            finalCrossSize = maxCrossConstraint;
        }

        float finalW = isRow ? finalMainSize : finalCrossSize;
        float finalH = isRow ? finalCrossSize : finalMainSize;

        bounds.width = std::clamp(finalW, constraints.minWidth, constraints.maxWidth);
        bounds.height = std::clamp(finalH, constraints.minHeight, constraints.maxHeight);

        // 5. Positioning
        float actualContentSize = (totalFlex > 0) ? (isRow ? bounds.width : bounds.height) : usedMainSize;
        float freeSpace = (isRow ? bounds.width : bounds.height) - actualContentSize;
        
        float mainOffset = 0.0f;
        float spaceBetween = 0.0f;

        if (freeSpace > 0 && totalFlex == 0) {
            switch(properties.mainAxisAlignment) {
                case MainAxisAlignment::START: mainOffset = 0.0f; break;
                case MainAxisAlignment::END: mainOffset = freeSpace; break;
                case MainAxisAlignment::CENTER: mainOffset = freeSpace / 2.0f; break;
                case MainAxisAlignment::SPACE_BETWEEN: 
                    if (properties.children.size() > 1) spaceBetween = freeSpace / (properties.children.size() - 1);
                    break;
                case MainAxisAlignment::SPACE_AROUND: 
                    spaceBetween = freeSpace / properties.children.size();
                    mainOffset = spaceBetween / 2.0f;
                    break;
            }
        }

        float currentMain = mainOffset;
        for (size_t i = 0; i < properties.children.size(); ++i) {
            auto& child = properties.children[i];
            Rect size = childSizes[i];

            float crossOffset = 0.0f;
            float childCrossSize = isRow ? size.height : size.width;
            float boundCrossSize = isRow ? bounds.height : bounds.width;

            switch(properties.crossAxisAlignment) {
                case CrossAxisAlignment::START: crossOffset = 0.0f; break;
                case CrossAxisAlignment::CENTER: crossOffset = (boundCrossSize - childCrossSize) / 2.0f; break;
                case CrossAxisAlignment::END: crossOffset = boundCrossSize - childCrossSize; break;
            }

            float x = isRow ? currentMain : crossOffset;
            float y = isRow ? crossOffset : currentMain;
            
            child->setBounds(Rect(x, y, size.width, size.height));
            currentMain += (isRow ? size.width : size.height) + spaceBetween;
        }
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        for (const auto& child : properties.children) {
            Rect p = child->getBounds();
            child->onDraw(ctx, x + p.x, y + p.y);
        }
    }

    void onUpdate(float delta) override {
        for (const auto& child : properties.children) child->onUpdate(delta);
    }

    Rect getBounds() const override { return bounds; }
    void setBounds(const Rect& rect) override { bounds = rect; }
    void dispose() override { for (auto& child : properties.children) child->dispose(); }

protected:
    FlexBoxProperties properties;
    Rect bounds;
};

#define FlexBox(...) std::make_shared<D_FlexBox>(FlexBoxProperties{__VA_ARGS__})