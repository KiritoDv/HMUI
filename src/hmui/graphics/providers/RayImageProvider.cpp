#include "RayImageProvider.h"

#include <unordered_map>

std::unordered_map<std::string, ImageHandle> textureCache;
std::unordered_map<std::string, Texture2D> loadedTextures;

ImageHandle* D_TextureProvider::load() {
    auto it = textureCache.find(imagePath);
    if (it != textureCache.end()) {
        return &it->second;
    }

    Texture2D texture = LoadTexture(imagePath.c_str());
    loadedTextures[imagePath] = texture;
    textureCache[imagePath] = {
        texture.width,
        texture.height,
        (void*) &loadedTextures[imagePath]
    };

    return &textureCache[imagePath];
}

void D_TextureProvider::dispose() {
    auto it = textureCache.find(imagePath);
    if (it != textureCache.end()) {
        UnloadTexture(*(Texture2D*)it->second.handle);
        textureCache.erase(it);
    }
}