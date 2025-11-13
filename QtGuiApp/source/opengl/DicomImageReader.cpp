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

    // 4. 从 DICOM 数据集中读取窗宽窗位
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

        if (mDicomImage->getMinMaxValues(minValue, maxValue) != 0) {
            mWindowCenter = (maxValue + minValue) / 2.0;
            mWindowWidth = maxValue - minValue;
            qDebug() << "   使用计算的窗宽窗位";
        }
        else {
            qWarning() << "⚠️ 无法获取窗宽窗位，使用默认值";
            std::string modality = getModality();
            if (modality == "CT") {
                mWindowCenter = 40.0;
                mWindowWidth = 400.0;
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

    // ✅ 5. 获取原始像素数据并存储为成员变量
    mPixelData = (Uint16*)mDicomImage->getOutputData(16);
    if (!mPixelData) {
        qDebug() << "❌ 无法获取像素数据";
        return false;
    }

    qDebug() << "✅ 原始像素数据已加载到成员变量";

    mIsLoaded = true;
    return true;
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
    mPixelData = nullptr;
    mIsLoaded = false;
}