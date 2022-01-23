#include "OVAFTFaceTracker.h"

#include <algorithm>
#include <chrono>
#include <dlib/dnn.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <filesystem>
#include <thread>
#include "OVAFTUtility.h"

namespace {
    OVAFTFaceTracker *instance = nullptr;
}

OVAFTFaceTracker::OVAFTFaceTracker() : capture(), face() {
    DetectCamera();
    capture.open(0);
}

void OVAFTFaceTracker::ChangeCamera() {
    cameraIndex++;
    if (cameraIndex == cameraCount) cameraIndex = 0;
    capture.open(cameraIndex);
}

void OVAFTFaceTracker::DetectCamera() {
    cv::VideoCapture checkCamera;
    cameraCount = 0;
    while (true) {
        if (!checkCamera.open(cameraCount++)) break;
    }
    checkCamera.release();
}

void OVAFTFaceTracker::DetectLandmark() {
    dlib::shape_predictor landmark;
    float mouthX = 0.0f, mouthY = 0.0f;
    long long lastUpdatedTime = 0;
    std::filesystem::path landmarkPath(OVAFTUtility::dirname(OVAFTUtility::getExecutablePath()));
    landmarkPath.append("resources");
    landmarkPath.append("landmark");
    landmarkPath.append("shape_predictor_68_face_landmarks.dat");
    dlib::deserialize(landmarkPath.string()) >> landmark;
    while (!stopping) {
        if (face.is_empty()) continue;
        if (lastUpdatedTime == updatedTime) continue;

        dlib::full_object_detection shape(landmark(frame, face));

        // Angle
        if (initialBodyX == 0.0f) initialBodyX = static_cast<float>(shape.part(8).x());
        tmpAngleX = tmpBodyAngleX = -(static_cast<float>(abs(shape.part(27).x() - shape.part(0).x())) /
                                      static_cast<float>(abs(shape.part(16).x() - shape.part(0).x())) - 0.5f) * 90.0f;
        tmpAngleY = tmpBodyAngleY = -(static_cast<float>(abs(shape.part(8).y() - shape.part(33).y())) /
                                      static_cast<float>(abs(shape.part(8).y() - shape.part(27).y())) - 0.6f) * 180.0f;
        tmpAngleZ = -((((
                                (180.0f / 3.1415927f *
                                 atan2(static_cast<float>(shape.part(33).y()) - static_cast<float>(shape.part(27).y()),
                                       static_cast<float>(shape.part(33).x()) -
                                       static_cast<float>(shape.part(27).x()))) - 90.0f +
                                (180.0f / 3.1415927f *
                                 atan2(static_cast<float>(shape.part(35).y()) - static_cast<float>(shape.part(31).y()),
                                       static_cast<float>(shape.part(35).x()) -
                                       static_cast<float>(shape.part(31).x()))) / 2.0f) / 20.0f * 30.0f - tmpAngleZ) -
                       tmpAngleZ) * 0.2f + tmpAngleZ);
        tmpBodyAngleZ = -std::clamp(((180.0f / 3.1415927f *
                                      atan2(480.0f - static_cast<float>(shape.part(8).y()),
                                            initialBodyX - static_cast<float>(shape.part(8).x()))) - 90.0f) -
                                    tmpBodyAngleZ, -10.0f, 10.0f);

        // Eyes
        tmpEyeLOpen = (static_cast<float>(abs(shape.part(43).y() - shape.part(47).y())) /
                       static_cast<float>(abs(shape.part(43).x() - shape.part(44).x())) - 0.3f) * 2.0f;
        tmpEyeROpen = (static_cast<float>(abs(shape.part(38).y() - shape.part(40).y())) /
                       static_cast<float>(abs(shape.part(37).x() - shape.part(38).x())) - 0.3f) * 2.0f;

        // Mouth
        tmpMouthOpenY = static_cast<float>(abs(shape.part(62).y() - shape.part(66).y())) /
                        static_cast<float>((abs(shape.part(51).y() - shape.part(62).y()) +
                                            abs(shape.part(66).y() - shape.part(57).y())));
        mouthX = (static_cast<float>(shape.part(62).y()) -
                  abs(static_cast<float>(shape.part(48).y()) - static_cast<float>(shape.part(54).y())) / 2.0f) /
                 (abs(static_cast<float>(shape.part(51).y()) - static_cast<float>(shape.part(62).y())) +
                  abs(static_cast<float>(shape.part(66).y()) - static_cast<float>(shape.part(57).y())));
        mouthY = abs(static_cast<float>(shape.part(31).x()) - static_cast<float>(shape.part(35).x())) /
                 (abs(static_cast<float>(shape.part(51).x()) - static_cast<float>(shape.part(62).x())) +
                  abs(static_cast<float>(shape.part(66).x()) - static_cast<float>(shape.part(57).x())));
        if (mouthY < 14.0f) {
            tmpCheek = 0.0f;
            tmpEyeLSmile = 0.0f;
            tmpEyeRSmile = 0.0f;
            tmpMouthForm = 0.0f;
        } else if (mouthX > 2.0f) {
            tmpCheek = -0.5f + mouthY / 2.0f;
            tmpEyeLSmile = abs((mouthX - 2.0f) / 0.5f);
            tmpEyeRSmile = abs((mouthX - 2.0f) / 0.5f);
            tmpMouthForm = 1.0f;
        }

        lastUpdatedTime = updatedTime;
    }
}

void OVAFTFaceTracker::Dispose() {
    stopping = true;
}

OVAFTFaceTracker *OVAFTFaceTracker::GetInstance() {
    if (instance == nullptr) {
        instance = new OVAFTFaceTracker;
    }
    return instance;
}

void OVAFTFaceTracker::CaptureFace() {
    dlib::frontal_face_detector detector(dlib::get_frontal_face_detector());
    while (!stopping) {
        if (!capture.isOpened()) continue;
        cv::Mat cvFrame;
        capture.read(cvFrame);
        dlib::cv_image<dlib::bgr_pixel> dlibFrame(cvIplImage(cvFrame));
        std::vector<dlib::rectangle> faces(detector(dlibFrame, 0));
        if (!faces.empty()) {
            face = faces[0];
            frame = dlibFrame;
            updatedTime = std::chrono::system_clock::now().time_since_epoch().count();
        }
    }
}

void OVAFTFaceTracker::setAngleXEnabled(bool enable) {
    enableAngleX = enable;
    if (!enable) angleX = 0.0f;
}

void OVAFTFaceTracker::setAngleYEnabled(bool enable) {
    enableAngleY = enable;
    if (!enable) angleY = 0.0f;
}

void OVAFTFaceTracker::setAngleZEnabled(bool enable) {
    enableAngleZ = enable;
    if (!enable) angleZ = 0.0f;
}

void OVAFTFaceTracker::setBodyAngleXEnabled(bool enable) {
    enableBodyAngleX = enable;
    if (!enable) bodyAngleX = 0.0f;
}

void OVAFTFaceTracker::setBodyAngleYEnabled(bool enable) {
    enableBodyAngleY = enable;
    if (!enable) bodyAngleY = 0.0f;
}

void OVAFTFaceTracker::setBodyAngleZEnabled(bool enable) {
    enableBodyAngleZ = enable;
    if (!enable) bodyAngleZ = 0.0f;
}

void OVAFTFaceTracker::setCheekEnabled(bool enable) {
    enableCheek = enable;
    if (!enable) cheek = 0.0f;
}

void OVAFTFaceTracker::setEyeLOpenEnabled(bool enable) {
    enableEyeLOpen = enable;
    if (!enable) eyeLOpen = 1.0f;
}

void OVAFTFaceTracker::setEyeLSmileEnabled(bool enable) {
    enableEyeLSmile = enable;
    if (!enable) eyeLSmile = 0.0f;
}

void OVAFTFaceTracker::setEyeROpenEnabled(bool enable) {
    enableEyeROpen = enable;
    if (!enable) eyeROpen = 1.0f;
}

void OVAFTFaceTracker::setEyeRSmileEnabled(bool enable) {
    enableEyeRSmile = enable;
    if (!enable) eyeRSmile = 0.0f;
}

void OVAFTFaceTracker::setMouthFormEnabled(bool enable) {
    enableMouthForm = enable;
    if (!enable) mouthForm = 0.0f;
}

void OVAFTFaceTracker::setMouthOpenYEnabled(bool enable) {
    enableMouthOpenY = enable;
    if (!enable) mouthOpenY = 0.0f;
}

void OVAFTFaceTracker::SmoothingParameter() {
    while (!stopping) {
        if (enableAngleX) angleX += (tmpAngleX - angleX) / 60.0f;
        if (enableAngleY) angleY += (tmpAngleY - angleY) / 60.0f;
        if (enableAngleZ) angleZ += (tmpAngleZ - angleZ) / 60.0f;
        if (enableBodyAngleX) bodyAngleX += (tmpBodyAngleX - bodyAngleX) / 60.0f;
        if (enableBodyAngleY) bodyAngleY += (tmpBodyAngleY - bodyAngleY) / 60.0f;
        if (enableBodyAngleZ) bodyAngleZ += (tmpBodyAngleZ - bodyAngleZ) / 60.0f;
        if (enableCheek) cheek += (tmpCheek - cheek) / 60.0f;
        if (enableEyeLOpen) eyeLOpen += (tmpEyeLOpen - eyeLOpen) / 60.0f;
        if (enableEyeLSmile) eyeLSmile += (tmpEyeLSmile - eyeLSmile) / 60.0f;
        if (enableEyeROpen) eyeROpen += (tmpEyeROpen - eyeROpen) / 60.0f;
        if (enableEyeRSmile) eyeRSmile += (tmpEyeRSmile - eyeRSmile) / 60.0f;
        if (enableMouthForm) mouthForm += (tmpMouthForm - mouthForm) / 60.0f;
        if (enableMouthOpenY) mouthOpenY += (tmpMouthOpenY - mouthOpenY) / 60.0f;
        std::this_thread::sleep_for(std::chrono::microseconds(16));
    }
}

void OVAFTFaceTracker::ResetAngle() {
    initialBodyX = 0.0f;
}

void OVAFTFaceTracker::Start() {
    stopping = false;
    std::thread(&OVAFTFaceTracker::CaptureFace, this).detach();
    std::thread(&OVAFTFaceTracker::DetectLandmark, this).detach();
    std::thread(&OVAFTFaceTracker::SmoothingParameter, this).detach();
}
