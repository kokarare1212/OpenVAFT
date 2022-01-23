#pragma once

#include <GL/glew.h>
#include <QColor>
#include <QOpenGLWidget>
#include "OVAFTAllocator.h"

class OVAFTTextureManager;

class OVAFTView;

class OVAFTGLWidget : public QOpenGLWidget {
public:
    QColor GetBackgroundColor();

    static Csm::csmFloat32 GetDeltaTime();

    static OVAFTGLWidget *GetInstance();

    OVAFTTextureManager *GetTextureManager() { return textureManager; }

    OVAFTView *GetView() { return view; }

    void Release();

    static void ReleaseInstance();

    void SetBackgroundColor(QColor color);

    static void UpdateTime();

protected:
    void initializeGL() override;

    void paintGL() override;

private:
    OVAFTGLWidget();

    void Initialize();

    void InitializeCubism();

    float backgroundRed = 0.0f;
    float backgroundGreen = 0.0f;
    float backgroundBlue = 0.0f;
    float backgroundAlpha = 0.0f;
    OVAFTAllocator cubismAllocator;
    Csm::CubismFramework::Option cubismOption;
    static double currentFrame;
    static double deltaTime;
    bool initialized = false;
    static double lastFrame;
    OVAFTTextureManager *textureManager;
    OVAFTView *view;
    int windowWidth;
    int windowHeight;
};
