#pragma once

#include <string>
#include <vector>
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

class D_RawTextureProvider : public ImageProvider {
public:
    explicit D_RawTextureProvider(const std::vector<uint8_t>& bytes)
        : textureBytes(bytes), texture(nullptr) {}
    ImageHandle* load() override;
    void dispose() override;
private:
    std::vector<uint8_t> textureBytes;
    ImageHandle* texture;
};

#define TextureProvider(path) std::dynamic_pointer_cast<ImageProvider>(std::make_shared<D_TextureProvider>(path))
#define RawTextureProvider(bytes) std::dynamic_pointer_cast<ImageProvider>(std::make_shared<D_RawTextureProvider>(bytes))