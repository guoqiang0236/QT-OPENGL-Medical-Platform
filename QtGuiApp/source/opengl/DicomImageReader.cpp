#include "DicomImageReader.h"
#include <algorithm>
#include <cmath>
#include <dcmtk/dcmimgle/dcmimage.h>  
#include <dcmtk/dcmdata/dctk.h>     
DicomImageReader::DicomImageReader()
{
}

DicomImageReader::~DicomImageReader()
{
    release();
}

bool DicomImageReader::loadDicomFile(const std::string& filePath)
{
    qDebug() << "📂 正在加载 DICOM 文件:" << QString::fromStdString(filePath);

    // 释放旧资源
    release();

    // 1. 加载 DICOM 文件
    mFileFormat = std::make_unique<DcmFileFormat>();
    OFCondition status = mFileFormat->loadFile(filePath.c_str());

    if (status.bad()) {
        qDebug() << "❌ DICOM 文件加载失败:" << status.text();
        return false;
    }

    // 2. 创建图像对象
    mDicomImage = std::make_unique<DicomImage>(filePath.c_str());

    if (mDicomImage->getStatus() != EIS_Normal) {
        qDebug() << "❌ DICOM 图像创建失败:" << DicomImage::getString(mDicomImage->getStatus());
        return false;
    }

    // 3. 获取图像信息
    mWidth = mDicomImage->getWidth();
    mHeight = mDicomImage->getHeight();
    mBitsPerPixel = mDicomImage->getDepth();

    qDebug() << "✅ DICOM 加载成功:";
    qDebug() << "   尺寸:" << mWidth << "x" << mHeight;
    qDebug() << "   位深:" << mBitsPerPixel << "bits";
    qDebug() << "   患者:" << QString::fromStdString(getPatientName());
    qDebug() << "   检查日期:" << QString::fromStdString(getStudyDate());

    // 4. ✅ 修复：从 DICOM 数据集中读取窗宽窗位
    DcmDataset* dataset = mFileFormat->getDataset();

    // 尝试读取窗宽
    Float64 windowWidth = 0.0;
    if (dataset->findAndGetFloat64(DCM_WindowWidth, windowWidth).good()) {
        mWindowWidth = windowWidth;
    }

    // 尝试读取窗位
    Float64 windowCenter = 0.0;
    if (dataset->findAndGetFloat64(DCM_WindowCenter, windowCenter).good()) {
        mWindowCenter = windowCenter;
    }

    // 如果没有找到窗宽窗位，使用像素值范围计算
    if (mWindowWidth <= 0 || mWindowCenter == 0) {
        double minValue = 0.0;
        double maxValue = 0.0;

        // 使用 getMinMaxValues() 获取最小最大值
        if (mDicomImage->getMinMaxValues(minValue, maxValue) != 0) {
            mWindowCenter = (maxValue + minValue) / 2.0;
            mWindowWidth = maxValue - minValue;
            qDebug() << "   使用计算的窗宽窗位";
        }
        else {
            // 如果仍然失败，使用默认值
            qWarning() << "⚠️ 无法获取窗宽窗位，使用默认值";

            // 根据模态使用不同的默认值
            std::string modality = getModality();
            if (modality == "CT") {
                mWindowCenter = 40.0;   // CT 软组织窗位
                mWindowWidth = 400.0;   // CT 软组织窗宽
            }
            else if (modality == "MR") {
                mWindowCenter = 128.0;
                mWindowWidth = 256.0;
            }
            else {
                mWindowCenter = 128.0;
                mWindowWidth = 256.0;
            }
        }
    }

    qDebug() << "   窗宽:" << mWindowWidth << " 窗位:" << mWindowCenter;
    qDebug() << "   模态:" << QString::fromStdString(getModality());

    // 5. 转换为 RGBA
    convertToRGBA();

    mIsLoaded = true;
    return true;
}

void DicomImageReader::convertToRGBA()
{
    if (!mDicomImage) return;

    // 1. 获取像素数据
    const DiPixel* pixelData = mDicomImage->getInterData();
    if (!pixelData) {
        qDebug() << "❌ 无法获取像素数据";
        return;
    }

    // 2. 分配 RGBA 缓冲区
    size_t totalPixels = mWidth * mHeight;
    mPixelDataRGBA.resize(totalPixels * 4);

    // 3. 应用窗宽窗位并转换为 8 位灰度
    const void* rawData = pixelData->getData();

    double minValue = mWindowCenter - mWindowWidth / 2.0;
    double maxValue = mWindowCenter + mWindowWidth / 2.0;

    for (size_t i = 0; i < totalPixels; ++i) {
        double pixelValue = 0.0;

        // 根据位深读取像素值
        if (mBitsPerPixel <= 16) {
            const Uint16* data16 = static_cast<const Uint16*>(rawData);
            pixelValue = static_cast<double>(data16[i]);
        }
        else {
            const Uint8* data8 = static_cast<const Uint8*>(rawData);
            pixelValue = static_cast<double>(data8[i]);
        }

        // 应用窗宽窗位
        unsigned char grayValue = mapValueToGray(pixelValue);

        // 转换为 RGBA（灰度图）
        mPixelDataRGBA[i * 4 + 0] = grayValue; // R
        mPixelDataRGBA[i * 4 + 1] = grayValue; // G
        mPixelDataRGBA[i * 4 + 2] = grayValue; // B
        mPixelDataRGBA[i * 4 + 3] = 255;       // A
    }

    qDebug() << "✅ DICOM 图像已转换为 RGBA 格式";
}

unsigned char DicomImageReader::mapValueToGray(double value)
{
    double minValue = mWindowCenter - mWindowWidth / 2.0;
    double maxValue = mWindowCenter + mWindowWidth / 2.0;

    // 窗宽窗位映射
    if (value <= minValue) return 0;
    if (value >= maxValue) return 255;

    // 线性映射到 0-255
    double normalized = (value - minValue) / (maxValue - minValue);
    return static_cast<unsigned char>(normalized * 255.0);
}

unsigned char* DicomImageReader::getPixelData()
{
    if (mPixelDataRGBA.empty()) return nullptr;
    return mPixelDataRGBA.data();
}

std::string DicomImageReader::getPatientName() const
{
    if (!mFileFormat) return "";

    OFString patientName;
    mFileFormat->getDataset()->findAndGetOFString(DCM_PatientName, patientName);
    return patientName.c_str();
}

std::string DicomImageReader::getStudyDate() const
{
    if (!mFileFormat) return "";

    OFString studyDate;
    mFileFormat->getDataset()->findAndGetOFString(DCM_StudyDate, studyDate);
    return studyDate.c_str();
}

std::string DicomImageReader::getModality() const
{
    if (!mFileFormat) return "";

    OFString modality;
    mFileFormat->getDataset()->findAndGetOFString(DCM_Modality, modality);
    return modality.c_str();
}

void DicomImageReader::setWindowLevel(double center, double width)
{
    mWindowCenter = center;
    mWindowWidth = width;

    // 重新应用窗宽窗位
    convertToRGBA();

    qDebug() << "🔧 窗宽窗位已更新: 窗宽=" << mWindowWidth << " 窗位=" << mWindowCenter;
}

void DicomImageReader::getWindowLevel(double& center, double& width)
{
    center = mWindowCenter;
    width = mWindowWidth;
}

void DicomImageReader::release()
{
    mDicomImage.reset();
    mFileFormat.reset();
    mPixelDataRGBA.clear();
    mIsLoaded = false;
}