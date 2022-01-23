#include "OVAFTSprite.h"

#include "OVAFTGLWidget.h"

OVAFTSprite::OVAFTSprite(float x, float y, float width, float height, GLuint texture_id, GLuint programId)
        : rect(), spriteColor() {
    rect.left = (x - width * 0.5f);
    rect.right = (x + width * 0.5f);
    rect.up = (y + height * 0.5f);
    rect.down = (y - height * 0.5f);
    _textureId = texture_id;

    // A Number of Attribute
    positionLocation = glGetAttribLocation(programId, "position");
    uvLocation = glGetAttribLocation(programId, "uv");
    textureLocation = glGetUniformLocation(programId, "texture");
    colorLocation = glGetUniformLocation(programId, "baseColor");

    spriteColor[0] = 1.0f;
    spriteColor[1] = 1.0f;
    spriteColor[2] = 1.0f;
    spriteColor[3] = 1.0f;
}

bool OVAFTSprite::IsHit(float pointX, float pointY) const {
    // Get Screen Size
    int maxWidth = OVAFTGLWidget::GetInstance()->width();
    int maxHeight = OVAFTGLWidget::GetInstance()->height();

    if (maxWidth == 0 || maxHeight == 0) return false;

    // Transform Y
    float y = static_cast<float>(maxHeight) - pointY;

    return (pointX >= rect.left && pointX <= rect.right && y <= rect.up && y >= rect.down);
}

void OVAFTSprite::Render() const {
    // Get Screen Size
    int maxWidth = OVAFTGLWidget::GetInstance()->width();
    int maxHeight = OVAFTGLWidget::GetInstance()->height();

    if (maxWidth == 0 || maxHeight == 0) return;

    glEnable(GL_TEXTURE_2D);
    const GLfloat uvVertex[] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

    // Enable Attribute
    glEnableVertexAttribArray(positionLocation);
    glEnableVertexAttribArray(uvLocation);

    // Register Uniform
    glUniform1i(textureLocation, 0);

    // Peak Data
    float positionVertex[] = {
            (rect.right - static_cast<float>(maxWidth) * 0.5f) / (static_cast<float>(maxWidth) * 0.5f),
            (rect.up - static_cast<float>(maxHeight) * 0.5f) / (static_cast<float>(maxHeight) * 0.5f),
            (rect.left - static_cast<float>(maxWidth) * 0.5f) / (static_cast<float>(maxWidth) * 0.5f),
            (rect.up - static_cast<float>(maxHeight) * 0.5f) / (static_cast<float>(maxHeight) * 0.5f),
            (rect.left - static_cast<float>(maxWidth) * 0.5f) / (static_cast<float>(maxWidth) * 0.5f),
            (rect.down - static_cast<float>(maxHeight) * 0.5f) / (static_cast<float>(maxHeight) * 0.5f),
            (rect.right - static_cast<float>(maxWidth) * 0.5f) / (static_cast<float>(maxWidth) * 0.5f),
            (rect.down - static_cast<float>(maxHeight) * 0.5f) / (static_cast<float>(maxHeight) * 0.5f)
    };

    // Register Attribute
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, positionVertex);
    glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, 0, uvVertex);

    glUniform4f(colorLocation, spriteColor[0], spriteColor[1], spriteColor[2], spriteColor[3]);


    // Draw Model
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void OVAFTSprite::RenderImmidiate(GLuint textureId, const GLfloat uvVertex[8]) const {
    // Get Screen Size
    int maxWidth = OVAFTGLWidget::GetInstance()->width();
    int maxHeight = OVAFTGLWidget::GetInstance()->height();

    if (maxWidth == 0 || maxHeight == 0) return;

    glEnable(GL_TEXTURE_2D);

    // Enable Attribute
    glEnableVertexAttribArray(positionLocation);
    glEnableVertexAttribArray(uvLocation);

    // Register Uniform
    glUniform1i(textureLocation, 0);

    // Peak Data
    float positionVertex[] = {
            (rect.right - static_cast<float>(maxWidth) * 0.5f) / (static_cast<float>(maxWidth) * 0.5f),
            (rect.up - static_cast<float>(maxHeight) * 0.5f) / (static_cast<float>(maxHeight) * 0.5f),
            (rect.left - static_cast<float>(maxWidth) * 0.5f) / (static_cast<float>(maxWidth) * 0.5f),
            (rect.up - static_cast<float>(maxHeight) * 0.5f) / (static_cast<float>(maxHeight) * 0.5f),
            (rect.left - static_cast<float>(maxWidth) * 0.5f) / (static_cast<float>(maxWidth) * 0.5f),
            (rect.down - static_cast<float>(maxHeight) * 0.5f) / (static_cast<float>(maxHeight) * 0.5f),
            (rect.right - static_cast<float>(maxWidth) * 0.5f) / (static_cast<float>(maxWidth) * 0.5f),
            (rect.down - static_cast<float>(maxHeight) * 0.5f) / (static_cast<float>(maxHeight) * 0.5f)
    };

    // Register Attribute
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, positionVertex);
    glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, 0, uvVertex);

    glUniform4f(colorLocation, spriteColor[0], spriteColor[1], spriteColor[2], spriteColor[3]);

    // Draw Model
    glBindTexture(GL_TEXTURE_2D, textureId);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void OVAFTSprite::ResetRect(float x, float y, float width, float height) {
    rect.left = (x - width * 0.5f);
    rect.right = (x + width * 0.5f);
    rect.up = (y + height * 0.5f);
    rect.down = (y - height * 0.5f);
}

void OVAFTSprite::SetColor(float r, float g, float b, float a) {
    spriteColor[0] = r;
    spriteColor[1] = g;
    spriteColor[2] = b;
    spriteColor[3] = a;
}
