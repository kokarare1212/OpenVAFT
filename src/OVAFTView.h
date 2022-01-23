#pragma once

#include <CubismFramework.hpp>
#include <GL/glew.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>

class OVAFTSprite;

class OVAFTModel;

class OVAFTView {
public:
    const char *vertexShader =
            "#version 120\n"
            "attribute vec3 position;"
            "attribute vec2 uv;"
            "varying vec2 vuv;"
            "void main(void){"
            "    gl_Position = vec4(position, 1.0);"
            "    vuv = uv;"
            "}";
    const char *fragmentShader =
            "#version 120\n"
            "varying vec2 vuv;"
            "uniform sampler2D texture;"
            "uniform vec4 baseColor;"
            "void main(void){"
            "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
            "}";
    enum SelectTarget {
        SelectTarget_None,
        SelectTarget_ModelFrameBuffer,
        SelectTarget_ViewFrameBuffer,
    };

    OVAFTView();

    ~OVAFTView();

    float GetSpriteAlpha(int assign) const;

    void Initialize();

    void InitializeSprite();

    void PostModelDraw(OVAFTModel &refModel);

    void PreModelDraw(OVAFTModel &refModel);

    void Render();

    void ResetPosition();

    void ResetScale();

    void ResizeSprite();

    void SetRenderTargetClearColor(float r, float g, float b);

    void SwitchRenderingTarget(SelectTarget targetType);

    void ToLeft();

    void ToRight();

    void ToTop();

    void ToBottom();

    void ZoomIn();

    void ZoomOut();

private:
    Csm::CubismMatrix44 *deviceToScreen;
    Csm::CubismViewMatrix *viewMatrix;
    GLuint programId;
    OVAFTSprite *back;
    OVAFTSprite *renderSprite;
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 renderBuffer;
    SelectTarget renderTarget;
    float clearColor[4];
    float positionX = 0.0f;
    float positionY = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
};
