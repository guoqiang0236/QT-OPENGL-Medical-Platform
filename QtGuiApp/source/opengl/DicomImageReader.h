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

    bool loadDicomFolder(const std::string& folderPath);

    bool setCurrentImage(size_t index);

    // 获取切片信息
    size_t getTotalImages() const { return mDicomImages.empty() ? 1 : mDicomImages.size(); }
    size_t getCurrentImageIndex() const { return mCurrentImageIndex; }


private:
    // 单文件模式使用的 unique_ptr
    std::unique_ptr<DicomImage> mSingleDicomImage;
    std::unique_ptr<DcmFileFormat> mSingleFileFormat;

    // 多文件模式使用裸指针引用容器中的对象
    DicomImage* mDicomImage{ nullptr };
    DcmFileFormat* mFileFormat{ nullptr };

    Uint16* mPixelData{ nullptr };

    int mWidth{ 0 };
    int mHeight{ 0 };
    int mBitsPerPixel{ 0 };

    double mWindowCenter{ 0.0 };
    double mWindowWidth{ 0.0 };

    bool mIsLoaded{ false };

    //多个dicom
    std::vector<std::unique_ptr<DicomImage>> mDicomImages;
    std::vector<std::unique_ptr<DcmFileFormat>> mFileFormats;
    std::vector<std::string> mFilePaths;
    size_t mCurrentImageIndex{ 0 };
};