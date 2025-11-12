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

    unsigned char* pixelData = reader->getPixelData();
    if (!pixelData) {
        qDebug() << "❌ 无法获取 DICOM 像素数据";
        return false;
    }

    mWidth = reader->getWidth();
    mHeight = reader->getHeight();

    // 生成 OpenGL 纹理
    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // 上传 DICOM 数据（已经是 RGBA 格式）
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE16, img_width, img_height, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, pData);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE16, mWidth, mHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixelData);

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
    unsigned char* pixelData = mDicomReader->getPixelData();
    if (!pixelData) return;

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