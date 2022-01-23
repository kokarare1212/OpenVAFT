#pragma once

#include <GL/glew.h>

class OVAFTSprite {
public:
    struct Rect {
    public:
        float left;
        float right;
        float up;
        float down;
    };

    OVAFTSprite(float x, float y, float width, float height, GLuint textureId, GLuint programId);

    GLuint GetTextureId() { return _textureId; }

    bool IsHit(float pointX, float pointY) const;

    void Render() const;

    void RenderImmidiate(GLuint textureId, const GLfloat uvVertex[8]) const;

    void ResetRect(float x, float y, float width, float height);

    void SetColor(float r, float g, float b, float a);

private:
    int colorLocation;
    int positionLocation;
    Rect rect;
    float spriteColor[4];
    GLuint _textureId;
    int textureLocation;
    int uvLocation;
};
