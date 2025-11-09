#include "PerspectiveCamera.h"
#include <glm/ext/matrix_clip_space.hpp>

PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float near, float far)
{
	mFovy = fovy;
	mAspect = aspect;
	mNear = near;
	mFar = far;
}

PerspectiveCamera::~PerspectiveCamera()
{
}

glm::mat4 PerspectiveCamera::getProjectionMatrix()
{
	//传入的fov是角度，需要转换为弧度
	return glm::perspective(glm::radians(mFovy), mAspect, mNear, mFar);
}

void PerspectiveCamera::scale(float deltaScale)
{
	auto front = glm::cross(mUp, mRight);
	mPosition += front * deltaScale; // 沿着前方向量移动
}
