#pragma once
#include "Material.h"
#include "../Texture.h"


class PhongInstanceMaterial : public Material
{
public:
	PhongInstanceMaterial();
	~PhongInstanceMaterial();

	Texture* mDiffuse{ nullptr };
	Texture* mSpecularMask{ nullptr };
	float mShiness{ 1.0f };

private:

};

