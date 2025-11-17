#pragma once

#include "Material.h"
#include "../Texture.h"

class LightTestMaterial : public Material {
    Q_OBJECT
public:
    LightTestMaterial();
    ~LightTestMaterial();

public:
    Texture* mDiffuse{ nullptr };
    Texture* mSpecularMask{ nullptr };
    float mShiness{ 1.0f };
};
