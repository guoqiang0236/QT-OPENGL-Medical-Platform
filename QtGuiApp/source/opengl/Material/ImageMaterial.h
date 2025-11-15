#pragma once

#include "Material.h"
#include "../Texture.h"

class ImageMaterial : public Material {
    Q_OBJECT
public:
    ImageMaterial();
    ~ImageMaterial();

public:
    Texture* mDiffuse{ nullptr };
    Texture* mSpecularMask{ nullptr };
    float mShiness{ 1.0f };
};
