#pragma once
#include "Material.h"
#include "../Texture.h"

class MyPhongEnvMaterial : public Material
{
public:
	MyPhongEnvMaterial();
	~MyPhongEnvMaterial();

	Texture* mDiffuse{ nullptr };
	Texture* mSpecularMask{ nullptr };
	float mShiness{ 1.0f };
	Texture* mEnv{ nullptr };

private:

};

