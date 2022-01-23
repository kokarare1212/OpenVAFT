#pragma once

#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>

class OVAFTFaceTracker {
public:
    void Dispose();

    float AngleX() const { return angleX; };

    float AngleY() const { return angleY; };

    float AngleZ() const { return angleZ; };

    float BodyAngleX() const { return bodyAngleX; };

    float BodyAngleY() const { return bodyAngleY; };

    float BodyAngleZ() const { return bodyAngleZ; };

    void ChangeCamera();

    float Cheek() const { return cheek; };

    float EyeLOpen() const { return eyeLOpen; };

    float EyeLSmile() const { return eyeLSmile; };

    float EyeROpen() const { return eyeROpen; };

    float EyeRSmile() const { return eyeRSmile; };

    static OVAFTFaceTracker *GetInstance();

    float MouthForm() const { return mouthForm; };

    float MouthOpenY() const { return mouthOpenY; };

    void ResetAngle();

    void setAngleXEnabled(bool enable);

    void setAngleYEnabled(bool enable);

    void setAngleZEnabled(bool enable);

    void setBodyAngleXEnabled(bool enable);

    void setBodyAngleYEnabled(bool enable);

    void setBodyAngleZEnabled(bool enable);

    void setCheekEnabled(bool enable);

    void setEyeLOpenEnabled(bool enable);

    void setEyeLSmileEnabled(bool enable);

    void setEyeROpenEnabled(bool enable);

    void setEyeRSmileEnabled(bool enable);

    void setMouthFormEnabled(bool enable);

    void setMouthOpenYEnabled(bool enable);

    void Start();

private:
    OVAFTFaceTracker();

    void CaptureFace();

    void DetectCamera();

    void DetectLandmark();

    void SmoothingParameter();

    float angleX = 0.0f;
    float angleY = 0.0f;
    float angleZ = 0.0f;
    float bodyAngleX = 0.0f;
    float bodyAngleY = 0.0f;
    float bodyAngleZ = 0.0f;
    int cameraCount = 0;
    int cameraIndex = 0;
    cv::VideoCapture capture;
    float cheek = 0.0f;
    bool enableAngleX = true;
    bool enableAngleY = true;
    bool enableAngleZ = true;
    bool enableBodyAngleX = true;
    bool enableBodyAngleY = true;
    bool enableBodyAngleZ = true;
    bool enableCheek = true;
    bool enableEyeLOpen = true;
    bool enableEyeLSmile = true;
    bool enableEyeROpen = true;
    bool enableEyeRSmile = true;
    bool enableMouthForm = true;
    bool enableMouthOpenY = true;
    float eyeLOpen = 1.0f;
    float eyeLSmile = 0.0f;
    float eyeROpen = 1.0f;
    float eyeRSmile = 0.0f;
    dlib::rectangle face;
    dlib::cv_image<dlib::bgr_pixel> frame;
    float initialBodyX = 0.0f;
    float mouthForm = 0.0f;
    float mouthOpenY = 0.0f;
    float tmpAngleX = 0.0f;
    float tmpAngleY = 0.0f;
    float tmpAngleZ = 0.0f;
    float tmpBodyAngleX = 0.0f;
    float tmpBodyAngleY = 0.0f;
    float tmpBodyAngleZ = 0.0f;
    float tmpCheek = 0.0f;
    float tmpEyeLOpen = 1.0f;
    float tmpEyeLSmile = 0.0f;
    float tmpEyeROpen = 1.0f;
    float tmpEyeRSmile = 0.0f;
    float tmpMouthForm = 0.0f;
    float tmpMouthOpenY = 0.0f;
    long long updatedTime = 0;
    bool stopping = false;
};
