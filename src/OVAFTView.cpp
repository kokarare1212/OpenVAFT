#include "OVAFTView.h"

#include "OVAFTGLWidget.h"
#include "OVAFTLive2DManager.h"
#include "OVAFTModel.h"
#include "OVAFTSprite.h"

using namespace Csm;
using namespace std;

OVAFTView::OVAFTView() : programId(0), back(nullptr), renderSprite(nullptr), renderTarget(SelectTarget_None) {
    clearColor[0] = 1.0f;
    clearColor[1] = 1.0f;
    clearColor[2] = 1.0f;
    clearColor[3] = 0.0f;
    // Device to Screen
    deviceToScreen = new CubismMatrix44();

    // View Matrix
    viewMatrix = new CubismViewMatrix();
}

OVAFTView::~OVAFTView() {
    renderBuffer.DestroyOffscreenFrame();
    delete renderSprite;
    delete viewMatrix;
    delete deviceToScreen;
    delete back;
}

float OVAFTView::GetSpriteAlpha(int assign) const {
    float alpha = 0.25f + static_cast<float>(assign) * 0.5f;
    if (alpha > 1.0f) {
        alpha = 1.0f;
    }
    if (alpha < 0.1f) {
        alpha = 0.1f;
    }
    return alpha;
}

void OVAFTView::Initialize() {
    int width = OVAFTGLWidget::GetInstance()->width();
    int height = OVAFTGLWidget::GetInstance()->height();

    if (width == 0 || height == 0) return;

    // Default vertical
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    float left = -ratio;
    float right = ratio;
    float bottom = -1.0f;
    float top = 1.0;

    viewMatrix->SetScreenRect(left, right, bottom, top);
    viewMatrix->Scale(scaleX, scaleY);

    deviceToScreen->LoadIdentity();
    if (width > height) {
        float screenW = fabsf(right - left);
        deviceToScreen->ScaleRelative(screenW / static_cast<float>(width), -screenW / static_cast<float>(width));
    } else {
        float screenH = fabsf(top - bottom);
        deviceToScreen->ScaleRelative(screenH / static_cast<float>(height), -screenH / static_cast<float>(height));
    }
    deviceToScreen->TranslateRelative(-static_cast<float>(width) * 0.5f, -static_cast<float>(height) * 0.5f);

    // Scale Setting
    viewMatrix->SetMaxScale(0.8f);
    viewMatrix->SetMinScale(2.0f);

    // Max
    viewMatrix->SetMaxScreenRect(
            -2.0f,
            2.0f,
            -2.0f,
            2.0f
    );
}

void OVAFTView::InitializeSprite() {
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vertexShader, nullptr);
    glCompileShader(vertexShaderId);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fragmentShader, nullptr);
    glCompileShader(fragmentShaderId);
    programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);
    glUseProgram(programId);

    int width = OVAFTGLWidget::GetInstance()->width();
    int height = OVAFTGLWidget::GetInstance()->height();

    float x = static_cast<float>(width) * 0.5f;
    float y = static_cast<float>(height) * 0.5f;

//    OVAFTTextureManager* textureManager = OVAFTGLWidget::GetInstance()->GetTextureManager();
//    const string resourcesPath = ResourcesPath;
//
//    string imageName = BackImageName;
//    OVAFTTextureManager::TextureInfo* backgroundTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName);
//
//    float x = static_cast<float>(width) * 0.5f;
//    float y = static_cast<float>(height) * 0.5f;
//    float fWidth = static_cast<float>(backgroundTexture->width * 2.0f);
//    float fHeight = (static_cast<float>(height) * 0.95f);
//    back = new OVAFTSprite(x, y, fWidth, fHeight, backgroundTexture->id, programId);

    // Size
    x = static_cast<float>(width) * 0.5f;
    y = static_cast<float>(height) * 0.5f;
    renderSprite = new OVAFTSprite(x, y, static_cast<float>(width), static_cast<float>(height), 0, programId);
}

void OVAFTView::PostModelDraw(OVAFTModel &refModel) {
    // Frame Buffer
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 *useTarget = nullptr;

    if (renderTarget != SelectTarget_None) {
        // Use Target
        useTarget = (renderTarget == SelectTarget_ViewFrameBuffer) ? &renderBuffer : &refModel.GetRenderBuffer();

        // End Draw
        useTarget->EndDraw();

        if (renderTarget == SelectTarget_ViewFrameBuffer && renderSprite) {
            const GLfloat uvVertex[] = {
                    1.0f, 1.0f,
                    0.0f, 1.0f,
                    0.0f, 0.0f,
                    1.0f, 0.0f,
            };

            renderSprite->SetColor(1.0f, 1.0f, 1.0f, GetSpriteAlpha(0));
            renderSprite->RenderImmidiate(useTarget->GetColorBuffer(), uvVertex);
        }
    }
}

void OVAFTView::PreModelDraw(OVAFTModel &refModel) {
    // Frame Buffer
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 *useTarget = nullptr;

    if (renderTarget != SelectTarget_None) {
        // Use Target
        useTarget = (renderTarget == SelectTarget_ViewFrameBuffer) ? &renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid()) {
            int width = OVAFTGLWidget::GetInstance()->width();
            int height = OVAFTGLWidget::GetInstance()->height();
            if (width != 0 && height != 0) {
                // Model Draw Canvas
                useTarget->CreateOffscreenFrame(static_cast<csmUint32>(width), static_cast<csmUint32>(height));
            }
        }

        // Start Rendering
        useTarget->BeginDraw();
        useTarget->Clear(clearColor[0], clearColor[1], clearColor[2], clearColor[3]); // 背景クリアカラー
    }
}

void OVAFTView::Render() {
//    _back->Render();

    OVAFTLive2DManager *Live2DManager = OVAFTLive2DManager::GetInstance();

    Live2DManager->SetViewMatrix(viewMatrix);

    // Cubism Update / Draw
    Live2DManager->OnUpdate();

    if (renderTarget == SelectTarget_ModelFrameBuffer && renderSprite) {
        const GLfloat uvVertex[] = {
                1.0f, 1.0f,
                0.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
        };
        float alpha = GetSpriteAlpha(0); // サンプルとしてαに適当な差をつける
        renderSprite->SetColor(1.0f, 1.0f, 1.0f, alpha);

        OVAFTModel *model = Live2DManager->GetModel();
        if (model) {
            renderSprite->RenderImmidiate(model->GetRenderBuffer().GetColorBuffer(), uvVertex);
        }
    }
}

void OVAFTView::ResetPosition() {
    viewMatrix->TranslateX(positionX = 0.0f);
    viewMatrix->TranslateY(positionY = 0.0f);
}

void OVAFTView::ResetScale() {
    viewMatrix->Scale(scaleX = 1.0f, scaleY = 1.0f);
}

void OVAFTView::ResizeSprite() {
    OVAFTTextureManager *textureManager = OVAFTGLWidget::GetInstance()->GetTextureManager();
    if (!textureManager) return;

//    // Draw Size
//    int width = OVAFTGLWidget::GetInstance()->width();
//    int height = OVAFTGLWidget::GetInstance()->height();
//
//    float x = 0.0f;
//    float y = 0.0f;
//    float fWidth = 0.0f;
//    float fHeight = 0.0f;
//
//    if(back) {
//        GLuint id = back->GetTextureId();
//        OVAFTTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
//        if(texInfo) {
//            x = width * 0.5f;
//            y = height * 0.5f;
//            fWidth = static_cast<float>(texInfo->width * 2);
//            fHeight = static_cast<float>(height) * 0.95f;
//            back->ResetRect(x, y, fWidth, fHeight);
//        }
//    }
}

void OVAFTView::SetRenderTargetClearColor(float r, float g, float b) {
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
}

void OVAFTView::SwitchRenderingTarget(SelectTarget targetType) {
    renderTarget = targetType;
}

void OVAFTView::ToLeft() {
    if (positionX > viewMatrix->GetMaxLeft()) viewMatrix->TranslateX(positionX += 0.01f);
}

void OVAFTView::ToRight() {
    if (positionX < viewMatrix->GetMaxRight()) viewMatrix->TranslateX(positionX -= 0.01f);
}

void OVAFTView::ToTop() {
    if (positionY < viewMatrix->GetMaxTop()) viewMatrix->TranslateY(positionY -= 0.01f);
}

void OVAFTView::ToBottom() {
    if (positionY > viewMatrix->GetMaxBottom()) viewMatrix->TranslateY(positionY += 0.01f);
}

void OVAFTView::ZoomIn() {
    if (scaleX < 2.0f && scaleY < 2.0f)
        viewMatrix->Scale(scaleX += 0.01f, scaleY += 0.01f);
}

void OVAFTView::ZoomOut() {
    if (scaleX > 0.8f && scaleY > 0.8f)
        viewMatrix->Scale(scaleX -= 0.01f, scaleY -= 0.01f);
}