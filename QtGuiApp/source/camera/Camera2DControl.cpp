#include "Camera2DControl.h"

Camera2DControl::Camera2DControl()
{
}

Camera2DControl::~Camera2DControl()
{
}

void Camera2DControl::onCursor(double xpos, double ypos)
{
    // 只有左键按下时才允许平移(水平+垂直)
    if (mLeftMouseDown)
    {
        float deltaX = static_cast<float>(xpos - mCurrentX) * mMoveSpeed;
        float deltaY = static_cast<float>(ypos - mCurrentY) * mMoveSpeed;

        // 2D平移:只移动X和Y轴
        mCamera->mPosition.x -= deltaX;
        mCamera->mPosition.y += deltaY; // 注意Y轴方向
    }

    // 更新当前鼠标位置
    mCurrentX = xpos;
    mCurrentY = ypos;
}

void Camera2DControl::onScroll(double offset)
{
    // 滚轮缩放
    mCamera->scale(offset * mScaleSpeed);
}