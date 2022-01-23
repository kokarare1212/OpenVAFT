#pragma once

#include <CubismFramework.hpp>
#include <ICubismModelSetting.hpp>
#include <Model/CubismUserModel.hpp>
#include <Type/csmRectF.hpp>
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>

class OVAFTModel : public Csm::CubismUserModel {
public:
    OVAFTModel();

    ~OVAFTModel() override;

    void Draw(Csm::CubismMatrix44 &matrix);

    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 &GetRenderBuffer();

    void LoadAssets(const Csm::csmChar *dir, const Csm::csmChar *fileName);

    void Update();

protected:
    void DoDraw();

private:
    void PreloadMotionGroup(const Csm::csmChar *group);

    void ReleaseExpressions();

    void ReleaseMotionGroup(const Csm::csmChar *group) const;

    void ReleaseMotions();

    void SetupModel(Csm::ICubismModelSetting *setting);

    void SetupTextures();

    Csm::csmMap<Csm::csmString, Csm::ACubismMotion *> expressions;
    Csm::csmVector<Csm::CubismIdHandle> eyeBlinkIds;
    const Csm::CubismId *idParamAngleX;
    const Csm::CubismId *idParamAngleY;
    const Csm::CubismId *idParamAngleZ;
    const Csm::CubismId *idParamBodyAngleX;
    const Csm::CubismId *idParamBodyAngleY;
    const Csm::CubismId *idParamBodyAngleZ;
    const Csm::CubismId *idParamCheek;
    const Csm::CubismId *idParamEyeLOpen;
    const Csm::CubismId *idParamEyeLSmile;
    const Csm::CubismId *idParamEyeROpen;
    const Csm::CubismId *idParamEyeRSmile;
    const Csm::CubismId *idParamMouthForm;
    const Csm::CubismId *idParamMouthOpenY;
    Csm::csmString modelHomeDir;
    Csm::ICubismModelSetting *modelSetting;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion *> motions;
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 renderBuffer;
    Csm::csmVector<Csm::CubismIdHandle> lipSyncIds;
    Csm::csmFloat32 userTimeSeconds;
};
