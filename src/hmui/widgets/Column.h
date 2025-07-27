#pragma once

#include "hmui/widgets/InternalDrawable.h"
#include <vector>
#include <memory>
#include <algorithm>

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
    ) : children(properties.children), mainAxisAlignment(properties.mainAxisAlignment), crossAxisAlignment(properties.crossAxisAlignment) {};

    void init() override {
        for (const auto& child : children) {
            child->init();
            bounds.width = std::max(bounds.width, child->getBounds().width);
            bounds.height += child->getBounds().height;
        }
        int bp = 0;
    }

    void onDraw(GraphicsContext* ctx, float x, float y) override {
        float xPos = x;
        float yPos = mainAxisAlignment == MainAxisAlignment::START ? y : 
                      (mainAxisAlignment == MainAxisAlignment::CENTER ? y + (bounds.height / 2) : y + bounds.height);
        for (const auto& child : children) {
            if (crossAxisAlignment == CrossAxisAlignment::LEFT) {
                xPos = x;
            } else if (crossAxisAlignment == CrossAxisAlignment::CENTER) {
                xPos = x + (bounds.width - child->getBounds().width) / 2;
            } else if (crossAxisAlignment == CrossAxisAlignment::RIGHT) {
                xPos = x + bounds.width - child->getBounds().width;
            }

            child->setBounds(Rect(xPos, yPos, child->getBounds().width, child->getBounds().height));
            child->onDraw(ctx, xPos, yPos);
            
            switch(mainAxisAlignment) {
                case MainAxisAlignment::START:
                    yPos += child->getBounds().height;
                    break;
                case MainAxisAlignment::CENTER:
                    yPos += (child->getBounds().height / 2);
                    break;
                case MainAxisAlignment::END:
                    yPos -= child->getBounds().height;
                    break;
                case MainAxisAlignment::SPACE_BETWEEN:
                    yPos += (bounds.height - child->getBounds().height) / (children.size() - 1);
                    break;
                case MainAxisAlignment::SPACE_AROUND:
                    yPos += (bounds.height - child->getBounds().height) / (children.size() + 1);
                    break;
            }
        }
    }

    void onUpdate(float delta) override {
        for (const auto& child : children) {
            child->onUpdate(delta);
        }
    }

    Rect getBounds() const override {
        return this->bounds;
    }

protected:
    std::vector<std::shared_ptr<InternalDrawable>> children;
    MainAxisAlignment mainAxisAlignment;
    CrossAxisAlignment crossAxisAlignment;
    Rect bounds;
};

#define Column(...) std::make_shared<D_Column>(ColumnProperties{__VA_ARGS__})