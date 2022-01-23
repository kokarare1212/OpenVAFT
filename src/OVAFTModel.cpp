#include "OVAFTModel.h"

#include <CubismDefaultParameterId.hpp>
#include <CubismModelSettingJson.hpp>
#include <Motion/CubismMotion.hpp>
#include <fstream>
#include <Id/CubismIdManager.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Utils/CubismString.hpp>
#include "OVAFTFaceTracker.h"
#include "OVAFTGLWidget.h"
#include "OVAFTTextureManager.h"

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace std;

namespace {
    csmByte *CreateBuffer(const string &filePath, csmSizeInt *outSize) {
        const char *path = filePath.c_str();
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
        *outSize = size;
        return reinterpret_cast<csmByte *>(buf);
    }
}

OVAFTModel::OVAFTModel() : CubismUserModel(), modelSetting(nullptr), userTimeSeconds(0.0f) {
    idParamAngleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    idParamAngleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    idParamAngleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    idParamBodyAngleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    idParamBodyAngleY = CubismFramework::GetIdManager()->GetId(ParamBodyAngleY);
    idParamBodyAngleZ = CubismFramework::GetIdManager()->GetId(ParamBodyAngleZ);
    idParamCheek = CubismFramework::GetIdManager()->GetId(ParamCheek);
    idParamEyeLOpen = CubismFramework::GetIdManager()->GetId(ParamEyeLOpen);
    idParamEyeLSmile = CubismFramework::GetIdManager()->GetId(ParamEyeLSmile);
    idParamEyeROpen = CubismFramework::GetIdManager()->GetId(ParamEyeROpen);
    idParamEyeRSmile = CubismFramework::GetIdManager()->GetId(ParamEyeRSmile);
    idParamMouthForm = CubismFramework::GetIdManager()->GetId(ParamMouthForm);
    idParamMouthOpenY = CubismFramework::GetIdManager()->GetId(ParamMouthOpenY);
}

OVAFTModel::~OVAFTModel() {
    renderBuffer.DestroyOffscreenFrame();

    ReleaseMotions();
    ReleaseExpressions();

    for (csmInt32 i = 0; i < modelSetting->GetMotionGroupCount(); i++) {
        const csmChar *group = modelSetting->GetMotionGroupName(i);
        ReleaseMotionGroup(group);
    }
    delete (modelSetting);
}

void OVAFTModel::DoDraw() {
    if (_model == nullptr) return;
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
}

void OVAFTModel::Draw(CubismMatrix44 &matrix) {
    if (_model == nullptr) return;
    matrix.MultiplyByMatrix(_modelMatrix);
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&matrix);
    DoDraw();
}

Csm::Rendering::CubismOffscreenFrame_OpenGLES2 &OVAFTModel::GetRenderBuffer() {
    return renderBuffer;
}

void OVAFTModel::LoadAssets(const csmChar *dir, const csmChar *fileName) {
    modelHomeDir = dir;

    csmSizeInt size;
    const csmString path = csmString(dir) + fileName;

    csmByte *buffer = CreateBuffer(path.GetRawString(), &size);
    ICubismModelSetting *setting = new CubismModelSettingJson(buffer, size);
    delete[] buffer;

    SetupModel(setting);
    CreateRenderer();
    SetupTextures();
}

void OVAFTModel::PreloadMotionGroup(const csmChar *group) {
    const csmInt32 count = modelSetting->GetMotionCount(group);
    for (csmInt32 i = 0; i < count; i++) {
        csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = modelSetting->GetMotionFileName(group, i);
        path = modelHomeDir + path;

        csmByte *buffer1;
        csmSizeInt size;
        buffer1 = CreateBuffer(path.GetRawString(), &size);
        auto *tmpMotion = dynamic_cast<CubismMotion *>(LoadMotion(buffer1, size, name.GetRawString()));

        csmFloat32 fadeTime = modelSetting->GetMotionFadeInTimeValue(group, i);
        if (fadeTime >= 0.0f) {
            tmpMotion->SetFadeInTime(fadeTime);
        }

        fadeTime = modelSetting->GetMotionFadeOutTimeValue(group, i);
        if (fadeTime >= 0.0f) {
            tmpMotion->SetFadeOutTime(fadeTime);
        }
        tmpMotion->SetEffectIds(eyeBlinkIds, lipSyncIds);

        if (motions[name] != NULL) {
            ACubismMotion::Delete(motions[name]);
        }
        motions[name] = tmpMotion;
    }
}

void OVAFTModel::ReleaseExpressions() {
    for (csmMap<csmString, ACubismMotion *>::const_iterator iter = expressions.Begin();
         iter != expressions.End(); ++iter) {
        ACubismMotion::Delete(iter->Second);
    }
    expressions.Clear();
}

void OVAFTModel::ReleaseMotionGroup(const csmChar *group) const {
    const csmInt32 count = modelSetting->GetMotionCount(group);
    for (csmInt32 i = 0; i < count; i++) {
        csmString voice = modelSetting->GetMotionSoundFileName(group, i);
        if (strcmp(voice.GetRawString(), "") != 0) {
            csmString path = voice;
            path = modelHomeDir + path;
        }
    }
}

void OVAFTModel::ReleaseMotions() {
    for (csmMap<csmString, ACubismMotion *>::const_iterator iter = motions.Begin(); iter != motions.End(); ++iter) {
        ACubismMotion::Delete(iter->Second);
    }
    motions.Clear();
}

void OVAFTModel::SetupModel(ICubismModelSetting *setting) {
    _updating = true;
    _initialized = false;

    modelSetting = setting;

    csmSizeInt size;

    // Cubism Model
    if (strcmp(modelSetting->GetModelFileName(), "") != 0) {
        csmString path = modelSetting->GetModelFileName();
        path = modelHomeDir + path;
        csmByte *buffer1;
        buffer1 = CreateBuffer(path.GetRawString(), &size);
        LoadModel(buffer1, size);
    }

    // Expression
    if (modelSetting->GetExpressionCount() > 0) {
        const csmInt32 count = modelSetting->GetExpressionCount();
        for (csmInt32 i = 0; i < count; i++) {
            csmString name = modelSetting->GetExpressionName(i);
            csmString path = modelSetting->GetExpressionFileName(i);
            path = modelHomeDir + path;
            csmByte *buffer2;
            buffer2 = CreateBuffer(path.GetRawString(), &size);
            ACubismMotion *motion = LoadExpression(buffer2, size, name.GetRawString());

            if (expressions[name] != NULL) {
                ACubismMotion::Delete(expressions[name]);
                expressions[name] = NULL;
            }
            expressions[name] = motion;
        }
    }

    // Physics
    if (strcmp(modelSetting->GetPhysicsFileName(), "") != 0) {
        csmString path = modelSetting->GetPhysicsFileName();
        path = modelHomeDir + path;
        csmByte *buffer3;
        buffer3 = CreateBuffer(path.GetRawString(), &size);
        LoadPhysics(buffer3, size);
    }

    // Pose
    if (strcmp(modelSetting->GetPoseFileName(), "") != 0) {
        csmString path = modelSetting->GetPoseFileName();
        path = modelHomeDir + path;
        csmByte *buffer4;
        buffer4 = CreateBuffer(path.GetRawString(), &size);
        LoadPose(buffer4, size);
    }

    // EyeBlink
    if (modelSetting->GetEyeBlinkParameterCount() > 0) {
        _eyeBlink = CubismEyeBlink::Create(modelSetting);
    }

    // Breath
    _breath = CubismBreath::Create();

    csmVector<CubismBreath::BreathParameterData> breathParameters;

    breathParameters.PushBack(CubismBreath::BreathParameterData(idParamAngleX, 0.0f, 15.0f, 6.5345f, 0.5f));
    breathParameters.PushBack(CubismBreath::BreathParameterData(idParamAngleY, 0.0f, 8.0f, 3.5345f, 0.5f));
    breathParameters.PushBack(CubismBreath::BreathParameterData(idParamAngleZ, 0.0f, 10.0f, 5.5345f, 0.5f));
    breathParameters.PushBack(CubismBreath::BreathParameterData(idParamBodyAngleX, 0.0f, 4.0f, 15.5345f, 0.5f));
    breathParameters.PushBack(
            CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBreath), 0.5f, 0.5f, 3.2345f,
                                              0.5f));

    _breath->SetParameters(breathParameters);

    // UserData
    if (strcmp(modelSetting->GetUserDataFile(), "") != 0) {
        csmString path = modelSetting->GetUserDataFile();
        path = modelHomeDir + path;
        csmByte *buffer5;
        buffer5 = CreateBuffer(path.GetRawString(), &size);
        LoadUserData(buffer5, size);
    }

    // EyeBlinkIds
    csmInt32 eyeBlinkIdCount = modelSetting->GetEyeBlinkParameterCount();
    for (csmInt32 i = 0; i < eyeBlinkIdCount; ++i) {
        eyeBlinkIds.PushBack(modelSetting->GetEyeBlinkParameterId(i));
    }

    // LipSyncIds
    csmInt32 lipSyncIdCount = modelSetting->GetLipSyncParameterCount();
    for (csmInt32 i = 0; i < lipSyncIdCount; ++i) {
        lipSyncIds.PushBack(modelSetting->GetLipSyncParameterId(i));
    }

    // Layout
    csmMap<csmString, csmFloat32> layout;
    modelSetting->GetLayoutMap(layout);
    _modelMatrix->SetupFromLayout(layout);

    _model->SaveParameters();

    for (csmInt32 i = 0; i < modelSetting->GetMotionGroupCount(); i++) {
        const csmChar *group = modelSetting->GetMotionGroupName(i);
        PreloadMotionGroup(group);
    }

    _motionManager->StopAllMotions();

    _updating = false;
    _initialized = true;
}

void OVAFTModel::SetupTextures() {
    for (csmInt32 modelTextureNumber = 0; modelTextureNumber < modelSetting->GetTextureCount(); modelTextureNumber++) {
        // Skip Bind / Load
        if (strcmp(modelSetting->GetTextureFileName(modelTextureNumber), "") == 0) continue;

        // Load Texture
        csmString texturePath = modelSetting->GetTextureFileName(modelTextureNumber);
        texturePath = modelHomeDir + texturePath;

        OVAFTTextureManager::TextureInfo *texture = OVAFTGLWidget::GetInstance()->GetTextureManager()->CreateTextureFromPngFile(
                texturePath.GetRawString());
        const auto glTextureNumber = static_cast<csmInt32>(texture->id);

        // OpenGL
        GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTextureNumber, glTextureNumber);
    }

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
}

void OVAFTModel::Update() {
    const csmFloat32 deltaTimeSeconds = OVAFTGLWidget::GetDeltaTime();
    userTimeSeconds += deltaTimeSeconds;

    _dragManager->Update(deltaTimeSeconds);
    _dragX = _dragManager->GetX();
    _dragY = _dragManager->GetY();

    // Motion
    _model->LoadParameters();
    _motionManager->UpdateMotion(_model, deltaTimeSeconds);
    _model->SaveParameters();

    if (_expressionManager != nullptr) {
        _expressionManager->UpdateMotion(_model, deltaTimeSeconds);
    }

    // Face Tracking
    auto *faceTracker = OVAFTFaceTracker::GetInstance();
    _model->SetParameterValue(idParamAngleX, faceTracker->AngleX());
    _model->SetParameterValue(idParamAngleY, faceTracker->AngleY());
    _model->SetParameterValue(idParamAngleZ, faceTracker->AngleZ());
    _model->SetParameterValue(idParamBodyAngleX, faceTracker->BodyAngleX());
    _model->SetParameterValue(idParamBodyAngleY, faceTracker->BodyAngleY());
    _model->SetParameterValue(idParamBodyAngleZ, faceTracker->BodyAngleZ());
    _model->SetParameterValue(idParamCheek, faceTracker->Cheek());
    _model->SetParameterValue(idParamEyeLOpen, faceTracker->EyeLOpen());
    _model->SetParameterValue(idParamEyeLSmile, faceTracker->EyeLSmile());
    _model->SetParameterValue(idParamEyeROpen, faceTracker->EyeROpen());
    _model->SetParameterValue(idParamEyeRSmile, faceTracker->EyeRSmile());
    _model->SetParameterValue(idParamMouthForm, faceTracker->MouthForm());
    _model->SetParameterValue(idParamMouthOpenY, faceTracker->MouthOpenY());

    // Breath Setting
    if (_breath != nullptr) {
        _breath->UpdateParameters(_model, deltaTimeSeconds);
    }

    // Physics Setting
    if (_physics != nullptr) {
        _physics->Evaluate(_model, deltaTimeSeconds);
    }

    // Pose Setting
    if (_pose != nullptr) {
        _pose->UpdateParameters(_model, deltaTimeSeconds);
    }

    _model->Update();
}
