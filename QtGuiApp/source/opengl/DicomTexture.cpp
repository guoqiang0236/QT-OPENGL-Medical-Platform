#include "DicomTexture.h"
#include <QDebug>

DicomTexture::DicomTexture(const std::string& dicomPath, unsigned int unit)
{
    initializeOpenGLFunctions();
    mUnit = unit;

    // 1. 创建 DICOM 读取器
    mDicomReader = std::make_unique<DicomImageReader>();

    // 2. 加载 DICOM 文件
    if (!mDicomReader->loadDicomFile(dicomPath)) {
        qDebug() << "❌ DicomTexture: DICOM 加载失败";
        return;
    }

    // 3. 创建 OpenGL 纹理
    loadFromDicomReader(mDicomReader.get());
}

DicomTexture::~DicomTexture()
{
    mDicomReader.reset();
}

bool DicomTexture::loadFromDicomReader(DicomImageReader* reader)
{
    if (!reader) return false;

    const Uint16* pixelData = reader->getPixelData();
    if (!pixelData) {
        qDebug() << "❌ 无法获取 DICOM 像素数据";
        return false;
    }

    mWidth = reader->getWidth();
    mHeight = reader->getHeight();

    // ✅ 计算像素值范围
    size_t totalPixels = static_cast<size_t>(mWidth) * mHeight;
    Uint16 minVal = 65535, maxVal = 0;

    for (size_t i = 0; i < totalPixels; ++i) {
        Uint16 val = pixelData[i];
        if (val < minVal) minVal = val;
        if (val > maxVal) maxVal = val;
    }

    mMinPixelValue = static_cast<float>(minVal);
    mMaxPixelValue = static_cast<float>(maxVal);

    qDebug() << "📊 像素范围: min=" << mMinPixelValue << " max=" << mMaxPixelValue;

    // 生成 OpenGL 纹理
    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // 上传 DICOM 数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, mWidth, mHeight, 0,
        GL_RED, GL_UNSIGNED_SHORT, pixelData);
    // 生成 Mipmap
    glGenerateMipmap(GL_TEXTURE_2D);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    qDebug() << "✅ DICOM 纹理创建成功:" << mWidth << "x" << mHeight;
    return true;
}

void DicomTexture::updateWindowLevel(double center, double width)
{
    if (!mDicomReader) return;

    // 1. 更新窗宽窗位
    mDicomReader->setWindowLevel(center, width);

    // 2. 重新上传纹理数据
 
    const Uint16* pixelData = mDicomReader->getPixelData();
    if (!pixelData) {
        return ;
    }

    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glGenerateMipmap(GL_TEXTURE_2D);

    qDebug() << "🔄 DICOM 窗宽窗位已更新";
}

std::string DicomTexture::getPatientName() const
{
    return mDicomReader ? mDicomReader->getPatientName() : "";
}

std::string DicomTexture::getStudyDate() const
{
    return mDicomReader ? mDicomReader->getStudyDate() : "";
}

std::string DicomTexture::getModality() const
{
    return mDicomReader ? mDicomReader->getModality() : "";
}

