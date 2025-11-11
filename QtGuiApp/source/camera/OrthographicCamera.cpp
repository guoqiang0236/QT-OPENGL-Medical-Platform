#include "OrthographicCamera.h"
#include <glm/ext/matrix_clip_space.hpp>


OrthographicCamera::OrthographicCamera(float l, float r, float t, float b, float n, float f)
{
	mLeft = l;
	mRight = r;
	mTop = t;
	mBottom = b;
	mNear = n;
	mFar = f;
}

OrthographicCamera::~OrthographicCamera()
{
}

glm::mat4 OrthographicCamera::getProjectionMatrix()
{
	float scale = std::pow(2.0f, mScaleFactor); // 使用2的幂次方来计算缩放比例
	return glm::ortho(mLeft * scale, mRight * scale, mBottom * scale, mTop * scale, mNear, mFar);
}

void OrthographicCamera::scale(float deltaScale)
{
	mScaleFactor -= deltaScale;
}


