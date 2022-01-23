#include "OVAFTLive2DManager.h"

#include <fstream>
#include "OVAFTGLWidget.h"
#include "OVAFTModel.h"
#include "OVAFTUtility.h"
#include "OVAFTView.h"

using namespace Csm;

namespace {
    OVAFTLive2DManager *instance = nullptr;
}

OVAFTLive2DManager::OVAFTLive2DManager() : viewMatrix(nullptr) {
    viewMatrix = new CubismMatrix44;
}

OVAFTLive2DManager *OVAFTLive2DManager::GetInstance() {
    if (instance == nullptr) {
        instance = new OVAFTLive2DManager;
    }
    return instance;
}

OVAFTModel *OVAFTLive2DManager::GetModel() {
    if (model != nullptr) return model;
    return nullptr;
}

void OVAFTLive2DManager::LoadScene(const std::string &moc3Path) {
    std::string moc3Name = OVAFTUtility::basename(moc3Path);
    std::string modelName = moc3Name.substr(0, moc3Name.find_last_of('.'));
    std::string modelPath = OVAFTUtility::dirname(moc3Path);
    std::string modelJsonName = modelName + ".model3.json";
    std::ifstream checkFile(modelPath + modelJsonName);
    if (!checkFile.is_open()) return;
    ReleaseModel();
    model = new OVAFTModel;
    model->LoadAssets(modelPath.c_str(), modelJsonName.c_str());
    OVAFTView::SelectTarget useRenderTarget = OVAFTView::SelectTarget_None;
    OVAFTGLWidget::GetInstance()->GetView()->SwitchRenderingTarget(useRenderTarget);

    // Clear Background Color
    float clearColor[3] = {1.0f, 1.0f, 1.0f};
    OVAFTGLWidget::GetInstance()->GetView()->SetRenderTargetClearColor(clearColor[0], clearColor[1], clearColor[2]);
}

void OVAFTLive2DManager::OnUpdate() const {
    auto *glWidget(OVAFTGLWidget::GetInstance());
    int width = glWidget->width();
    int height = glWidget->height();
    if (model != nullptr) {
        CubismMatrix44 projection;
        if (model->GetModel()->GetCanvasWidth() > 1.0f && width < height) {
            model->GetModelMatrix()->SetWidth(2.0f);
            projection.Scale(1.0f, static_cast<float>(width) / static_cast<float>(height));
        } else {
            projection.Scale(static_cast<float>(height) / static_cast<float>(width), 1.0f);
        }
        if (viewMatrix != nullptr) projection.MultiplyByMatrix(viewMatrix);

        // Pre Call
        OVAFTGLWidget::GetInstance()->GetView()->PreModelDraw(*model);

        model->Update();
        model->Draw(projection);

        // Post Call
        OVAFTGLWidget::GetInstance()->GetView()->PostModelDraw(*model);
    }
}

void OVAFTLive2DManager::ReleaseInstance() {
    delete instance;
    instance = nullptr;
}

void OVAFTLive2DManager::ReleaseModel() {
    delete model;
    model = nullptr;
}

void OVAFTLive2DManager::SetViewMatrix(Live2D::Cubism::Framework::CubismMatrix44 *matrix) {
    for (int i = 0; i < 16; i++) {
        viewMatrix->GetArray()[i] = matrix->GetArray()[i];
    }
}