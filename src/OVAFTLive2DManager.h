#pragma once

#include <CubismFramework.hpp>
#include <Math/CubismMatrix44.hpp>
#include <string>
#include <Type/csmVector.hpp>

class OVAFTModel;

class OVAFTLive2DManager {
public:
    static OVAFTLive2DManager *GetInstance();

    OVAFTModel *GetModel();

    void LoadScene(const std::string &moc3Path);

    void OnUpdate() const;

    static void ReleaseInstance();

    void ReleaseModel();

    void SetViewMatrix(Live2D::Cubism::Framework::CubismMatrix44 *matrix);

private:
    OVAFTLive2DManager();

    OVAFTModel *model = nullptr;
    Csm::CubismMatrix44 *viewMatrix;
};
