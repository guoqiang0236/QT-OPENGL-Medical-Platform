#pragma once
#include <string>
#include <vector>
#include <memory>
#include <QDebug>


#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmimage/diregist.h>

class DicomImageReader
{
public:
    DicomImageReader();
    ~DicomImageReader();

    // 加载 DICOM 文件
    bool loadDicomFile(const std::string& filePath);

    // 获取图像数据
    const Uint16* getPixelData() const { return mPixelData; }

    // 获取图像尺寸
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
    int getBitsPerPixel() const { return mBitsPerPixel; }

    // 获取 DICOM 标签信息
    std::string getPatientName() const;
    std::string getStudyDate() const;
    std::string getModality() const;

    // 窗宽窗位调整
    void setWindowLevel(double center, double width);
    void getWindowLevel(double& center, double& width);

    // 释放资源
    void release();

   

private:
    std::unique_ptr<DicomImage> mDicomImage;
    std::unique_ptr<DcmFileFormat> mFileFormat;

    Uint16* mPixelData{ nullptr };

    int mWidth{ 0 };
    int mHeight{ 0 };
    int mBitsPerPixel{ 0 };

    double mWindowCenter{ 0.0 };
    double mWindowWidth{ 0.0 };

    bool mIsLoaded{ false };
};