#include "Texture.h"
#include "DicomTexture.h"
#include <opencv2/opencv.hpp>

std::map<std::string, Texture*> Texture::mTextureCache;

Texture::Texture(const std::string& path, unsigned int unit)
{
    initializeOpenGLFunctions();
    mUnit = unit;

    // 1. 用 OpenCV 读取图片（注意：imread 默认是 BGR）
    cv::Mat img = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        qDebug() << "图片加载失败";
        return;
    }
    //OpenCV 默认左上角为原点，OpenGL 纹理左下角为原点，需要上下翻转
    cv::flip(img, img, 0);
		
     
    if (img.channels() == 1) {
        cv::cvtColor(img, img, cv::COLOR_GRAY2RGBA);
    }
    else if (img.channels() == 3) {
        cv::cvtColor(img, img, cv::COLOR_BGR2RGBA);
    }
    else if (img.channels() == 4) {
       
        cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
    }

    //2 生成纹理并且激活单元绑定
    glGenTextures(1, &mTexture);
    //--激活纹理单元--
    glActiveTexture(GL_TEXTURE0 + mUnit);
    //--绑定纹理对象--
    glBindTexture(GL_TEXTURE_2D, mTexture);

    //3 传输纹理数据,开辟显存
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth = img.cols, mHeight = img.rows,//第一个GL_RGBA告诉 OpenGL ​​如何存储纹理数据​​（在 GPU 内存中的格式）
        0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);//第二个GL_RGBA参数告诉 OpenGL ​​输入数据（img.data）的排列方式​​。

    //opengl生成mipmap
    glGenerateMipmap(GL_TEXTURE_2D);

    //手动写mipmap
    //{
    //    int width = img.cols, height = img.rows;
    //    //遍历每个mipmap的层级，为每个级别的mipmap填充图片数据
    //    for (int level = 0; true; ++level) {
    //        //1 将当前级别的mipmap对应的数据发往gpu端
    //        glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height,
    //            0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);

    //        //2 判断是否退出循环
    //        if (width == 1 && height == 1) {
    //            break;
    //        }

    //        //3 计算下一次循环的宽度/高度，除以2
    //        width = width > 1 ? width / 2 : 1;
    //        height = height > 1 ? height / 2 : 1;
    //    }
    //}
    img.release();

    //4 设置纹理的过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //用mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    //*****重要*****//
    //GL_NEAREST:在单个mipmap上采用最邻近采样
    //GL_LINEAR   
    //   
    //MIPMAP_LINEAR：在两层mipmap之间采用线性插值
    //MIPMAP_NEAREST
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //5 设置纹理的包裹方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // u
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // v

    /*  •	GL_CLAMP_TO_EDGE：坐标超出范围时，采样纹理边缘的颜色，不会重复或镜像。
        •	GL_CLAMP_TO_BORDER：坐标超出范围时，采样指定的边界颜色（需设置 GL_TEXTURE_BORDER_COLOR）。
        •	GL_MIRRORED_REPEAT：超出范围时，纹理坐标以镜像的方式重复。
        •	GL_MIRROR_CLAMP_TO_EDGE（OpenGL 4.4 + ）：超出范围时，先镜像再 clamp 到边缘。*/
}

Texture::Texture(unsigned int unit, unsigned char* dataIn, uint32_t widthIn, uint32_t heightIn)
{
    initializeOpenGLFunctions();
    mUnit = unit;

    // 1. 用 OpenCV 解码内存图片
    uint32_t dataInSize = heightIn == 0 ? widthIn : widthIn * heightIn * 4;
    std::vector<uchar> buf(dataIn, dataIn + dataInSize);
    cv::Mat img = cv::imdecode(buf, cv::IMREAD_UNCHANGED);

    if (img.empty()) {
        qDebug() << "图片内存解码失败";
        return;
    }

    cv::flip(img, img, 0);

    if (img.channels() == 1) {
        cv::cvtColor(img, img, cv::COLOR_GRAY2RGBA);
    }

    if (img.channels() == 3) {
        cv::cvtColor(img, img, cv::COLOR_BGR2RGBA);
    }
    else if (img.channels() == 4) {
        cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
    }

    mWidth = img.cols;
    mHeight = img.rows;

    // 2. 生成纹理并激活单元绑定
    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    // 3. 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 4. 设置参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 放大过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // 缩小过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture::Texture(unsigned int width, unsigned int height, unsigned int unit)
{
    initializeOpenGLFunctions();
    mWidth = width;
    mHeight = height;
    mUnit = unit;

    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(const std::vector<std::string>& paths, unsigned int unit)
{
    initializeOpenGLFunctions();
	mUnit = unit;
	mTextureTarget = GL_TEXTURE_CUBE_MAP;

    //1 创建CubeMap对象
	glGenTextures(1, &mTexture);
	glActiveTexture(GL_TEXTURE0 + mUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);

    //2 循环读取六张贴图,并且放置到cubemap的六个GPU空间内
    for (int i = 0; i < paths.size(); i++)
    {
        //CubeMap不需要翻转Y轴
        cv::Mat data = cv::imread(paths[i], cv::IMREAD_UNCHANGED);
        if (data.empty()) {
            qDebug() << "图片加载失败";
            return;
        }
        // 保证为RGBA格式
        if (data.channels() == 3)
            cv::cvtColor(data, data, cv::COLOR_BGR2RGBA);
        else if (data.channels() == 4)
            cv::cvtColor(data, data, cv::COLOR_BGRA2RGBA);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, data.cols, data.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data);
    }

    //3 设置纹理的参数
    glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // 放大过滤器
    glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 缩小过滤器
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

Texture::Texture()
{
}

Texture::~Texture()
{
}

Texture* Texture::createTexture(const std::string& path, unsigned int unit)
{
    //1 检查是否缓存过本路径对应的纹理对象
    auto iter = mTextureCache.find(path);
    if (iter != mTextureCache.end())
    {
        //对于iterater, first->key, second->value
        return iter->second;
    }
    //2 ✅ 检查文件扩展名，判断是否为 DICOM 文件
    std::string extension = path.substr(path.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    Texture* texture = nullptr;

    if (extension == "dcm") {
        // 使用 DicomTexture 加载 DICOM 文件
        qDebug() << "🔬 检测到 DICOM 文件，使用 DicomTexture 加载";
        texture = new DicomTexture(path, unit);
    }
    else {
        // 使用普通 Texture 加载图片
        texture = new Texture(path, unit);
    }
    mTextureCache[path] = texture;
    return texture;
}

Texture* Texture::createTextureFromMemory(const std::string& path, unsigned int unit, unsigned char* dataIn, uint32_t widthIn, uint32_t heightIn)
{
    //1 检查是否缓存过本路径对应的纹理对象
    auto iter = mTextureCache.find(path);
    if (iter != mTextureCache.end())
    {
        //对于iterater, first->key, second->value
        return iter->second;
    }

    //2 如果本路径对应的texture没有生成过,则重新生成
    auto texture = new Texture(unit, dataIn, widthIn, heightIn);
    mTextureCache[path] = texture;
    return texture;
}

Texture* Texture::createColorAttachment(unsigned int width, unsigned int height, unsigned int unit)
{
    return new Texture(width, height, unit);
}

Texture* Texture::createDepthStencilAttachment(unsigned int width, unsigned int height, unsigned int unit)
{
        
    Texture* dsTex = new Texture();
    dsTex->initializeOpenGLFunctions();
    unsigned int depthStencil;
    dsTex->glGenTextures(1, &depthStencil);
    dsTex->glBindTexture(GL_TEXTURE_2D, depthStencil);
    dsTex->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    dsTex->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    dsTex->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    dsTex->mTexture = depthStencil;
    dsTex->mWidth = width;
    dsTex->mHeight = height;
    dsTex->mUnit = unit;
    return dsTex;
}

void Texture::bind()
{
    //先切换纹理单元，然后绑定texture对象
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(mTextureTarget, mTexture);
}
