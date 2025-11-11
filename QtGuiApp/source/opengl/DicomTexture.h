#pragma once
#include "Texture.h"
#include "DicomImageReader.h"
#include <memory>

class DicomTexture : public Texture
{
public:
    DicomTexture(const std::string& dicomPath, unsigned int unit);
    ~DicomTexture();

    // 从 DICOM 读取器加载纹理
    bool loadFromDicomReader(DicomImageReader* reader);

    // 更新窗宽窗位
    void updateWindowLevel(double center, double width);

    // 获取 DICOM 信息
    std::string getPatientName() const;
    std::string getStudyDate() const;
    std::string getModality() const;

private:
    std::unique_ptr<DicomImageReader> mDicomReader;
};