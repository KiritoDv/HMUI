#include "RayImageProvider.h"

#include <unordered_map>

std::unordered_map<std::string, ImageHandle> textureCache;
std::unordered_map<std::string, Texture2D> loadedTextures;

ImageHandle* D_TextureProvider::load() {
#ifdef __SWITCH__
    if (imagePath.rfind("assets/", 0) == 0) {
        imagePath = "romfs:/" + imagePath.substr(7);
        printf("Adjusted image path for Switch: %s\n", imagePath.c_str());
    }
#endif

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

ImageHandle* D_RawTextureProvider::load() {
    if (texture) return texture;

    Image img = LoadImageFromMemory(".png", textureBytes.data(), textureBytes.size());
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    texture = new ImageHandle {
        tex.width,
        tex.height,
        (void*) new Texture2D(tex)
    };

    return texture;
}

void D_RawTextureProvider::dispose() {
    if (texture) {
        UnloadTexture(*(Texture2D*)texture->handle);
        delete (Texture2D*)texture->handle;
        delete texture;
        texture = nullptr;
    }
}