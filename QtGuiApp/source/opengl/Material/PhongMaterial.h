#pragma once

#include "Material.h"
#include "../Texture.h"

class PhongMaterial : public Material {
    Q_OBJECT
public:
    PhongMaterial();
    ~PhongMaterial();

public:
    Texture* mDiffuse{ nullptr };
    Texture* mSpecularMask{ nullptr };
    float mShiness{ 1.0f };
};
