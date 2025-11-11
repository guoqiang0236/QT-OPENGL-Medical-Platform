#pragma once
#include "CameraControl.h"


class TrackBallCameraControl : public CameraControl
{
public:
    TrackBallCameraControl();
    ~TrackBallCameraControl();

    // 重写父类光标事件处理函数
    void onCursor(double xpos, double ypos) override;

    void onScroll(double offset) override;
private:
    void rotatePitch(float angle);
    void rotateYaw(float angle);

private:
    float mMoveSpeed = 0.01f;

};

