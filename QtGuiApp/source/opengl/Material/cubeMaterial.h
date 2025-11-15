#pragma once
#include "Material.h"
#include "../Texture.h"

class CubeMaterial : public Material
{
public:
	CubeMaterial();
	~CubeMaterial();

	Texture* mDiffuse{ nullptr };

private:

};
