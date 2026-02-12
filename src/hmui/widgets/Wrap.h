#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

struct WrapProperties {
    Direction direction = Direction::Horizontal;
    float spacing = 0.0f;     // Space between children in the main axis
    float runSpacing = 0.0f;  // Space between runs (lines) in the cross axis
    MainAxisAlignment alignment = MainAxisAlignment::START;       // Alignment in the main axis
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::START; // Alignment in the cross axis
    std::vector<std::shared_ptr<InternalDrawable>> children;
};

class D_Wrap : public InternalDrawable {
private:
    struct Run {
        std::vector<size_t> childIndices;
        float mainAxisExtent = 0.0f;
        float crossAxisExtent = 0.0f;
    };

public:
    explicit D_Wrap(const WrapProperties& properties) 
        : properties(properties) {};

    void init() override {
        for (const auto& child : properties.children) {
            child->init();
            child->setParent(shared_from_this());
        }
    }

    void layout(BoxConstraints constraints) override {
        std::vector<Rect> childSizes(properties.children.size());
        std::vector<Run> runs;
        Run currentRun;

        bool isHoriz = (properties.direction == Direction::Horizontal);
        float maxMainExtent = isHoriz ? constraints.maxWidth : constraints.maxHeight;

        // 1. Measure children and group into runs
        BoxConstraints looseConstraints = BoxConstraints::loose(constraints.maxWidth, constraints.maxHeight);

        for (size_t i = 0; i < properties.children.size(); ++i) {
            auto& child = properties.children[i];
            
            // Note: Expanded is generally not supported in Wrap because wrap sizes intrinsicly.
            child->layout(looseConstraints);
            Rect size = child->getBounds();
            childSizes[i] = size;

            float childMain = isHoriz ? size.width : size.height;
            float childCross = isHoriz ? size.height : size.width;

            // If adding this child exceeds the max size (and it's not the first child in the run), wrap!
            if (!currentRun.childIndices.empty() && 
                (currentRun.mainAxisExtent + properties.spacing + childMain > maxMainExtent)) {
                runs.push_back(currentRun);
                currentRun = Run();
            }

            currentRun.childIndices.push_back(i);
            currentRun.mainAxisExtent += childMain + (currentRun.childIndices.size() > 1 ? properties.spacing : 0);
            currentRun.crossAxisExtent = std::max(currentRun.crossAxisExtent, childCross);
        }

        if (!currentRun.childIndices.empty()) {
            runs.push_back(currentRun);
        }

        // 2. Determine final bounds for the Wrap itself
        float totalMainAxis = 0.0f;
        float totalCrossAxis = 0.0f;

        for (size_t i = 0; i < runs.size(); ++i) {
            totalMainAxis = std::max(totalMainAxis, runs[i].mainAxisExtent);
            totalCrossAxis += runs[i].crossAxisExtent;
            if (i > 0) totalCrossAxis += properties.runSpacing;
        }

        float finalW = isHoriz ? totalMainAxis : totalCrossAxis;
        float finalH = isHoriz ? totalCrossAxis : totalMainAxis;

        bounds.width = std::clamp(finalW, constraints.minWidth, constraints.maxWidth);
        bounds.height = std::clamp(finalH, constraints.minHeight, constraints.maxHeight);

        // 3. Position children
        float currentCrossOffset = 0.0f;

        for (const auto& run : runs) {
            float freeMainSpace = (isHoriz ? bounds.width : bounds.height) - run.mainAxisExtent;
            float currentMainOffset = 0.0f;
            float spaceBetween = properties.spacing;

            // Main Axis Alignment (per run)
            if (freeMainSpace > 0) {
                switch (properties.alignment) {
                    case MainAxisAlignment::START: currentMainOffset = 0.0f; break;
                    case MainAxisAlignment::END: currentMainOffset = freeMainSpace; break;
                    case MainAxisAlignment::CENTER: currentMainOffset = freeMainSpace / 2.0f; break;
                    case MainAxisAlignment::SPACE_BETWEEN:
                        if (run.childIndices.size() > 1) spaceBetween += freeMainSpace / (run.childIndices.size() - 1);
                        break;
                    case MainAxisAlignment::SPACE_AROUND:
                        spaceBetween += freeMainSpace / run.childIndices.size();
                        currentMainOffset = spaceBetween / 2.0f;
                        break;
                }
            }

            for (size_t i : run.childIndices) {
                Rect size = childSizes[i];
                float childCross = isHoriz ? size.height : size.width;
                float childMain = isHoriz ? size.width : size.height;

                // Cross Axis Alignment (within the current run)
                float childCrossOffset = currentCrossOffset;
                float freeCrossSpace = run.crossAxisExtent - childCross;
                
                if (freeCrossSpace > 0) {
                    switch (properties.crossAxisAlignment) {
                        case CrossAxisAlignment::START: break;
                        case CrossAxisAlignment::CENTER: childCrossOffset += freeCrossSpace / 2.0f; break;
                        case CrossAxisAlignment::END: childCrossOffset += freeCrossSpace; break;
                    }
                }

                float x = isHoriz ? currentMainOffset : childCrossOffset;
                float y = isHoriz ? childCrossOffset : currentMainOffset;

                properties.children[i]->setBounds(Rect(x, y, size.width, size.height));

                currentMainOffset += childMain + spaceBetween;
            }

            currentCrossOffset += run.crossAxisExtent + properties.runSpacing;
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
    WrapProperties properties;
    Rect bounds;
};

#define Wrap(...) std::make_shared<D_Wrap>(WrapProperties{__VA_ARGS__})