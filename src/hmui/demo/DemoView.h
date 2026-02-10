#pragma once

#include <utility>
#include <iostream>

#include "hmui/widgets/Column.h"
#include "hmui/widgets/Container.h"
#include "hmui/widgets/GestureDetector.h"
#include "hmui/widgets/Scrollable.h"
#include "hmui/widgets/Drawable.h"
#include "hmui/widgets/AppContext.h"
#include "hmui/graphics/providers/RayImageProvider.h"
#include "hmui/widgets/Image.h"
#include "hmui/widgets/Stack.h"
#include "hmui/Navigator.h"

std::shared_ptr<D_Container> nestedTest(std::vector<Color2D> entries, size_t index = 0) {
    if (index >= entries.size()) {
        return Container(); // base case
    }

    return Container(
        .padding = EdgeInsets::all(10.0f),
        .alignment = Alignment::Center(),
        .color = entries[index],
        .child = GestureDetector(
            // Reordered to match struct: onTap, onTapRelease, onHover, onHoverEnd, child
            .onTap = [](std::shared_ptr<InternalDrawable> child, float x, float y) {
                std::cout << "Hovered/Tapped\n"; // Logic merged for brevity
            },
            .onHover = [](std::shared_ptr<InternalDrawable> child, float x, float y) {
                 std::shared_ptr<D_Container> c = std::dynamic_pointer_cast<D_Container>(child);
                 if(c) c->properties.color = Color2D(1, 0, 1, 1); 
            },
            .child = nestedTest(entries, index + 1)
        )
    );
}

class TestView : public Drawable {
public:
    // std::vector<Color2D> entries;
    std::vector<std::shared_ptr<InternalDrawable>> entries;

    void init() override {
        for(int i = 0; i < 30; ++i) {
            // entries.push_back(Container(
            //     .color = Color2D(
            //         rand() % 256 / 255.0f,
            //         rand() % 256 / 255.0f,
            //         rand() % 256 / 255.0f
            //     ),
            //     .image = {
            //         .provider = TextureProvider("test.png")
            //     },
            //     .width = 200.0f,
            //     .height = 100.0f
            // ));
            entries.push_back(Container(
                .width = 0,
                .height = 100.0f,
                .color = Color2D(
                    rand() % 256 / 255.0f,
                    rand() % 256 / 255.0f,
                    rand() % 256 / 255.0f
                ),
                .child = Image(
                    .provider = TextureProvider("test.png"),
                    .fit = BoxFit::Cover
                )
            ));
        }
        Drawable::init();
    }

    std::shared_ptr<InternalDrawable> build() override {
        return Container(
            .child = Container(
                .width = 200.0f,
                .height = 300.0f,
                .padding = EdgeInsets::all(15.0f),
                .clipToBounds = true,
                .color = Color2D(1.0f, 0.0f, 0.0f, 0.3f),
                .child = GestureDetector(
                    .onTap = [](std::shared_ptr<InternalDrawable> child, float x, float y) {
                        Navigator::push("/alternate");
                    },
                    .child = Scrollable(
                        .direction = Direction::Vertical,
                        .child = Column(
                            .children = entries
                        )
                    )
                )
            )
        );
    }

    ~TestView() override = default;
};

class AlternateTestView : public Drawable {
public:
    std::vector<std::shared_ptr<InternalDrawable>> entries;

    void init() override {
        for(int i = 0; i < 30; ++i) {
            entries.push_back(Container(
                .color = Color2D(
                    rand() % 256 / 255.0f,
                    rand() % 256 / 255.0f,
                    rand() % 256 / 255.0f
                ),
                .width = 100.0f,
                .height = 200.0f
            ));
        }
        Drawable::init();
    }

    std::shared_ptr<InternalDrawable> build() override {
        return Container(
            .color = Color2D(0.0f, 1.0f, 0.0f, 0.5f),
            .child = Container(
                .width = 200.0f,
                .height = 300.0f,
                .padding = EdgeInsets::all(5.0f),
                .clipToBounds = true,
                .color = Color2D(0.0f, 0.0f, 0.0f, 0.3f),
                .child = Scrollable(
                    .direction = Direction::Vertical,
                    .child = Column(
                        .children = entries
                    )
                )
            )
        );
    }

    ~AlternateTestView() override = default;
};

class DemoView : public Drawable {
public:
    std::shared_ptr<InternalDrawable> build() override {
        return AppContext(
            .routes = {
                { "/", []() { return std::make_shared<TestView>(); }},
                { "/alternate", []() { return std::make_shared<AlternateTestView>(); }}
            },
            .initialRoute = "/"
        );
    }

    ~DemoView() override = default;
};

