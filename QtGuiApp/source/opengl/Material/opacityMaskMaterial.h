#pragma once

#include "Material.h"
#include "../Texture.h"

class OpacityMaskMaterial : public Material
{
public:
	OpacityMaskMaterial();
	~OpacityMaskMaterial();

	Texture* mDiffuse{ nullptr };
	Texture* mOpacityMask{ nullptr };
	float mShiness{ 1.0f };

private:

};

