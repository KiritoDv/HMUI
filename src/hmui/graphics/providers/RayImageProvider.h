#pragma once

#include <string>
#include "raylib.h"
#include "hmui/graphics/GraphicsContext.h"

class D_TextureProvider : public ImageProvider {
public:
    explicit D_TextureProvider(const std::string& path) : imagePath(path) {}

    ImageHandle* load() override;
    void dispose() override;

private:
    std::string imagePath;
    ImageHandle* texture;
};

#define TextureProvider(path) std::dynamic_pointer_cast<ImageProvider>(std::make_shared<D_TextureProvider>(path))