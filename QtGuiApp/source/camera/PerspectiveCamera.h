#pragma once
#include "Camera.h"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float fovy, float aspect, float near, float far);
    ~PerspectiveCamera();

    glm::mat4 getProjectionMatrix() override;

    //缩放
    void scale(float deltaScale) override;
private:
    float mFovy = 0.0f;
    float mAspect = 0.0f;
       
};
