#include "OVAFTGLWidget.h"

#include <chrono>
#include <CubismFramework.hpp>
#include <QTimer>
#include "OVAFTFaceTracker.h"
#include "OVAFTLive2DManager.h"
#include "OVAFTTextureManager.h"
#include "OVAFTView.h"

using namespace Csm;

double OVAFTGLWidget::currentFrame = 0.0;
double OVAFTGLWidget::deltaTime = 0.0;
double OVAFTGLWidget::lastFrame = 0.0;

namespace {
    OVAFTGLWidget *instance = nullptr;
}

OVAFTGLWidget::OVAFTGLWidget() : cubismOption(), windowWidth(0), windowHeight(0) {
    textureManager = new OVAFTTextureManager;
    view = new OVAFTView;

    OVAFTFaceTracker::GetInstance()->Start();

    // Set Update Duration
    auto *timer(new QTimer);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    if (format().swapInterval() == -1) {
        timer->setInterval(17);
    } else {
        timer->setInterval(0);
    }
    timer->start();
}

QColor OVAFTGLWidget::GetBackgroundColor() {
    QColor color(static_cast<int>(backgroundRed * 255),
                 static_cast<int>(backgroundGreen * 255),
                 static_cast<int>(backgroundBlue * 255),
                 static_cast<int>(backgroundAlpha * 255));
    return color;
}

csmFloat32 OVAFTGLWidget::GetDeltaTime() {
    return static_cast<csmFloat32>(deltaTime);
}

OVAFTGLWidget *OVAFTGLWidget::GetInstance() {
    if (instance == nullptr) {
        instance = new OVAFTGLWidget;
    }
    return instance;
}

void OVAFTGLWidget::Initialize() {
    glewInit();

    // Texture Sampling Setting
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Transparent Setting
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    windowWidth = width();
    windowHeight = height();

    view->Initialize();

    InitializeCubism();

    initialized = true;
}

void OVAFTGLWidget::InitializeCubism() {
    // Setup Cubism Framework
    Csm::CubismFramework::StartUp(&cubismAllocator, &cubismOption);

    // Initialize Cubism Framework
    CubismFramework::Initialize();

    // Load Model
    OVAFTLive2DManager::GetInstance();

    // Default Projection
    CubismMatrix44 projection;

    UpdateTime();

    view->InitializeSprite();
}

void OVAFTGLWidget::initializeGL() {
    Initialize();
}

void OVAFTGLWidget::paintGL() {
    if (!initialized) return;
    int winWidth = width();
    int winHeight = height();
    if ((windowWidth != winWidth || windowHeight != winHeight) && winWidth > 0 && winHeight > 0) {
        // Initialize View
        view->Initialize();
        // Resize Sprite
        view->ResizeSprite();
        // Save Size
        windowWidth = winWidth;
        windowHeight = winHeight;

        // Change Viewport
        glViewport(0, 0, winWidth, winHeight);
    }

    // Update Time
    UpdateTime();

    // Initialize Screen
    glClearColor(backgroundRed, backgroundGreen, backgroundBlue, backgroundAlpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);

    // Update Draw
    view->Render();
}

void OVAFTGLWidget::Release() {
    delete view;
    delete textureManager;

    // Release Resource
    OVAFTLive2DManager::ReleaseInstance();

    // Release Cubism
    CubismFramework::Dispose();
}

void OVAFTGLWidget::ReleaseInstance() {
    delete instance;
    instance = nullptr;
}

void OVAFTGLWidget::SetBackgroundColor(QColor color) {
    backgroundRed = color.redF();
    backgroundGreen = color.greenF();
    backgroundBlue = color.blueF();
    backgroundAlpha = color.alphaF();
}

void OVAFTGLWidget::UpdateTime() {
    currentFrame = static_cast<double>(std::chrono::system_clock::now().time_since_epoch().count()) / 10000000.0;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}
