#pragma once
#include "Material.h"
#include "../Texture.h"

class ScreenMaterial : public Material
{
public:
	ScreenMaterial();
	~ScreenMaterial();

	Texture* mScreenTexture{ nullptr };

private:

};

