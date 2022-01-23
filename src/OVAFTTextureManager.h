#pragma once

#include <GL/glew.h>
#include <string>
#include <Type/csmVector.hpp>

class OVAFTTextureManager {
public:
    struct TextureInfo {
        GLuint id;
        int width;
        int height;
        std::string fileName;
    };

    inline unsigned int Premultiply(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
        return static_cast<unsigned>(\
            (red * (alpha + 1) >> 8) | \
            ((green * (alpha + 1) >> 8) << 8) | \
            ((blue * (alpha + 1) >> 8) << 16) | \
            (((alpha)) << 24)   \
        );
    }

    ~OVAFTTextureManager();

    TextureInfo *CreateTextureFromPngFile(const std::string &fileName);

    TextureInfo *GetTextureInfoById(GLuint textureId) const;

    void ReleaseTexture(Csm::csmUint32 textureId);

    void ReleaseTexture(const std::string &fileName);

    void ReleaseTextures();

private:
    Csm::csmVector<TextureInfo *> textures;
};
