#pragma once
#include "Light.h"
#include "../Object.h"

class SpotLight : public Light, public Object
{
public:
	SpotLight();
	~SpotLight();

public:
	glm::vec3 mTargetDirection = glm::vec3(-1.0f, -1.0f, -1.0f);// 看向的方向
	float mInnerAngle{ 0.0f }; //内部角度
	float mOuterAngle{ 0.0f }; //外部角度 

};

