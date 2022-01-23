#include "OVAFTTextureManager.h"


#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <fstream>

using namespace Csm;

OVAFTTextureManager::~OVAFTTextureManager() {
    ReleaseTextures();
}

OVAFTTextureManager::TextureInfo *OVAFTTextureManager::CreateTextureFromPngFile(const std::string &fileName) {
    // search loaded texture already.
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++) {
        if (textures[static_cast<int>(i)]->fileName == fileName) {
            return textures[static_cast<int>(i)];
        }
    }

    GLuint textureId;
    int width, height, channels;
    unsigned char *png;
    unsigned char *address;

    const char *path = fileName.c_str();
    int size = 0;
    struct stat statBuf{};
    if (stat(path, &statBuf) == 0) {
        size = statBuf.st_size;
    }
    std::fstream file;
    char *buf = new char[size];
    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return nullptr;
    }
    file.read(buf, size);
    file.close();
    address = reinterpret_cast<csmByte *>(buf);

    // get png information
    png = stbi_load_from_memory(
            address,
            static_cast<int>(size),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha);

    // Generate OpenGL Texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Release
    stbi_image_free(png);
    delete[] address;

    auto *textureInfo = new OVAFTTextureManager::TextureInfo();
    if (textureInfo != nullptr) {
        textureInfo->fileName = fileName;
        textureInfo->width = width;
        textureInfo->height = height;
        textureInfo->id = textureId;

        textures.PushBack(textureInfo);
    }
    return textureInfo;
}

OVAFTTextureManager::TextureInfo *OVAFTTextureManager::GetTextureInfoById(GLuint textureId) const {
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++) {
        if (textures[static_cast<int>(i)]->id == textureId) {
            return textures[static_cast<int>(i)];
        }
    }

    return nullptr;
}

void OVAFTTextureManager::ReleaseTexture(Csm::csmUint32 textureId) {
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++) {
        if (textures[static_cast<int>(i)]->id != textureId) continue;
        delete textures[static_cast<int>(i)];
        textures.Remove(static_cast<int>(i));
        break;
    }
}

void OVAFTTextureManager::ReleaseTexture(const std::string &fileName) {
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++) {
        if (textures[static_cast<int>(i)]->fileName == fileName) {
            delete textures[static_cast<int>(i)];
            textures.Remove(static_cast<int>(i));
            break;
        }
    }
}

void OVAFTTextureManager::ReleaseTextures() {
    for (Csm::csmUint32 i = 0; i < textures.GetSize(); i++) {
        delete textures[static_cast<int>(i)];
    }

    textures.Clear();
}
