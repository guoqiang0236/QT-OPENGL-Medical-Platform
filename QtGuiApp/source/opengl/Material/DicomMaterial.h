#pragma once

#include "Material.h"
#include "../Texture.h"

class DicomMaterial : public Material {
    Q_OBJECT
public:
    DicomMaterial();
    ~DicomMaterial();

public:
    Texture* mDiffuse{ nullptr };  // DICOM 纹理

    // DICOM 特有参数
    float mMinPixelValue{ 0.0f };
    float mMaxPixelValue{ 65535.0f };
};