#pragma once
#include "CameraControl.h"

class Camera2DControl : public CameraControl {
public:
    Camera2DControl();
    ~Camera2DControl() override;

    void onCursor(double xpos, double ypos) override;
    void onScroll(double offset) override;

private:
    float mMoveSpeed = 0.005f; // Æ½ÒÆËÙ¶È
};