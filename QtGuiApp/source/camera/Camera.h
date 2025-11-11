#pragma once
//GLM]
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

class Camera
{
public:
	Camera();
	virtual ~Camera();
	glm::mat4 getViewMatrix();
	virtual glm::mat4 getProjectionMatrix();
	//缩放
	virtual void scale(float deltaScale);
public:
	glm::vec3 mPosition{ 0.0f,0.0f,3.0f };
	glm::vec3 mUp{ 0.0f,1.0f,0.0f };
	glm::vec3 mRight{ 1.0f, 0.0f, 0.0f };

	float mNear = 0.0f;
	float mFar = 0.0f;
};
