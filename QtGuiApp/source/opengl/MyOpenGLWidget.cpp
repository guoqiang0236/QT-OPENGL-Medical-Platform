#include "MyOpenGLWidget.h"
#include "../camera/PerspectiveCamera.h"
#include "../camera/TrackBallCameraControl.h"
#include "../camera/OrthographicCamera.h"
#include "../camera/GameCameraControl.h"
#include "../camera/Camera2DControl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DicomTexture.h"

#include "renderer/Renderer.h"
#include "Scene.h"
#include "Geometry.h"                        
#include "material/imageMaterial.h"
#include "material/DicomMaterial.h"
#include "material/cubeMaterial.h"
#include "material/PhongMaterial.h"
#include "Material/LightTestMaterial.h"
#include "mesh/Mesh.h"                       
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"
#include "Light/AmbientLight.h"

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent):QOpenGLWidget(parent)
{
}

void MyOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	//初始化渲染器
	m_renderer = new Renderer();
	m_renderer->setWidth(width());
	m_renderer->setHeight(height());

	// 初始化场景
	m_scene = new Scene();

	// 初始化光源(ImageMaterial不需要光照,但Renderer需要这些参数)
	m_dirLight = new DirectionalLight();
	m_ambLight = new AmbientLight();
	m_spotLight = nullptr;
	paperbox();
	papaercamera();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void MyOpenGLWidget::paintGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
	if (m_renderer && m_scene && m_camera) {
		// 使用渲染器渲染场景
		m_renderer->render(
			m_scene,
			m_camera,
			m_dirLight,
			m_pointLights,
			m_spotLight,
			m_ambLight
		);
	}

	update(); // 请求下一帧重绘
}


void MyOpenGLWidget::switchTexture(const std::string& imagePath)
{
	// 在 OpenGL 线程中执行
	makeCurrent();

	qDebug() << "[切换纹理] 新路径:" << QString::fromStdString(imagePath);

	// 加载新纹理
	Texture* newTexture = Texture::createTexture(imagePath, 2);

	if (!newTexture) {
		qDebug() << "ERROR: 新纹理加载失败:" << QString::fromStdString(imagePath);
		doneCurrent();
		return;
	}

	// ✅ 检测新纹理类型
	DicomTexture* newDicomTex = dynamic_cast<DicomTexture*>(newTexture);
	bool isNewDicom = (newDicomTex != nullptr);

	// ✅ 检测当前材质类型
	bool isCurrentDicom = false;
	if (m_imageMaterial) {
		DicomMaterial* currentDicomMat = dynamic_cast<DicomMaterial*>(m_imageMaterial);
		isCurrentDicom = (currentDicomMat != nullptr);
	}

	// ========== 情况1: 首次创建 ==========
	if (!m_imageMesh || !m_imageMaterial) {
		qDebug() << "🆕 首次创建 Mesh";
		createImageMesh(newTexture);
	}
	// ========== 情况2: 类型相同,仅更新纹理 ==========
	else if (isNewDicom == isCurrentDicom) {
		qDebug() << "🔄 类型相同,更新纹理";

		if (isCurrentDicom) {
			// DICOM -> DICOM
			DicomMaterial* dicomMat = dynamic_cast<DicomMaterial*>(m_imageMaterial);
			dicomMat->mDiffuse = newTexture;

			// ✅ 更新像素值范围
			dicomMat->mMinPixelValue = newDicomTex->getMinPixelValue();
			dicomMat->mMaxPixelValue = newDicomTex->getMaxPixelValue();

			qDebug() << "  ✅ DICOM材质更新完成";
		}
		else {
			// 普通图片 -> 普通图片
			ImageMaterial* imageMat = dynamic_cast<ImageMaterial*>(m_imageMaterial);
			imageMat->mDiffuse = newTexture;
			qDebug() << "  ✅ Image材质更新完成";
		}
	}
	// ========== 情况3: 类型不同,重建整个 Mesh ==========
	else {
		qDebug() << "⚠️ 材质类型切换: "
			<< (isCurrentDicom ? "DICOM" : "Image")
			<< " -> "
			<< (isNewDicom ? "DICOM" : "Image");

		// ✅ 从场景中移除旧 Mesh
		if (m_imageMesh) {
			m_scene->removeChild(m_imageMesh);
			delete m_imageMesh;
			m_imageMesh = nullptr;
		}

		// ✅ 删除旧材质(Mesh 会管理 Geometry,但不管理 Material)
		if (m_imageMaterial) {
			delete m_imageMaterial;
			m_imageMaterial = nullptr;
		}

		// ✅ 创建新 Mesh
		createImageMesh(newTexture);
		qDebug() << "  ✅ Mesh 重建完成";
	}

	// ========== ✅ 新增: 更新 m_texture 指针并发射信号 ==========
	m_texture = newTexture;

	// ✅ 如果是 DICOM 纹理,发射信号通知 MainWindow
	if (newDicomTex) {
		size_t totalSlices = newDicomTex->getTotalSlices();
		size_t currentSlice = newDicomTex->getCurrentSliceIndex();

		qDebug() << "📊 DICOM 加载完成: 总切片数=" << totalSlices
			<< " 当前切片=" << (currentSlice + 1);

		// 发射信号
		emit dicomLoaded(static_cast<int>(totalSlices), static_cast<int>(currentSlice));
	}

	qDebug() << "✅ 纹理切换成功! 新尺寸:"
		<< newTexture->getWidth() << "x" << newTexture->getHeight();

	// 触发重绘
	update();

	doneCurrent();
}
void MyOpenGLWidget::createImageMesh(Texture* texture)
{
    if (!texture || !m_scene) {
        qDebug() << "ERROR: createImageMesh - texture 或 scene 为空";
        return;
    }

    // 计算宽高比和平面尺寸
    float aspect = static_cast<float>(width()) / static_cast<float>(height());
    Geometry* planeGeometry = Geometry::createPlane(2.0f * aspect, 2.0f);

    // ✅ 根据纹理类型智能选择材质
    DicomTexture* dicomTex = dynamic_cast<DicomTexture*>(texture);
    
    if (dicomTex) {
        // ========== DICOM 纹理 -> 使用 DicomMaterial ==========
        qDebug() << "🔬 检测到 DICOM 纹理, 创建 DicomMaterial";
        
        DicomMaterial* dicomMaterial = new DicomMaterial();
        dicomMaterial->mDiffuse = texture;
        
        // 从 DicomTexture 获取像素值范围
        dicomMaterial->mMinPixelValue = dicomTex->getMinPixelValue();
        dicomMaterial->mMaxPixelValue = dicomTex->getMaxPixelValue();
        
        // 配置材质状态
        dicomMaterial->mDepthTest = true;
        dicomMaterial->mDepthWrite = true;
        
        // 创建 Mesh
        m_imageMesh = new Mesh(planeGeometry, dicomMaterial);
        m_imageMaterial = dicomMaterial;  // 保存为基类指针
        
        qDebug() << "  ✅ DicomMaterial 创建完成";
        qDebug() << "     - 像素值范围: [" << dicomMaterial->mMinPixelValue 
                 << ", " << dicomMaterial->mMaxPixelValue << "]";
    }
    else {
        // ========== 普通图像 -> 使用 ImageMaterial ==========
        qDebug() << "🖼️  检测到普通图像, 创建 ImageMaterial";
        
        ImageMaterial* imageMaterial = new ImageMaterial();
        imageMaterial->mDiffuse = texture;
        
        // 配置材质状态
        imageMaterial->mDepthTest = true;
        imageMaterial->mDepthWrite = true;
        
        // 创建 Mesh
        m_imageMesh = new Mesh(planeGeometry, imageMaterial);
        m_imageMaterial = imageMaterial;  // 保存为基类指针
        
        qDebug() << "  ✅ ImageMaterial 创建完成";
    }

    // 设置 Mesh 位置并添加到场景
    m_imageMesh->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    m_scene->addChild(m_imageMesh);

    qDebug() << "✅ Mesh 创建成功, 平面尺寸:" << (2.0f * aspect) << "x 2.0";
}
void MyOpenGLWidget::papaercamera()
{
	// 创建透视相机：视场角 45度，宽高比 800/600，近平面 0.1，远平面 100
    m_camera = new PerspectiveCamera(30.0f, 800.0f / 500.0f, 0.0001f, 100.0f);
	// 动态计算宽高比
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// 创建正交相机 - 无透视变形,完美显示2D图像
	//m_camera = new OrthographicCamera(
	//	-aspect,  // 根据窗口宽高比调整左右边界
	//	aspect,
	//	1.0f,     // 固定上下边界
	//	-1.0f,
	//	0.1f,     // 近平面
	//	100.0f    // 远平面
	//);

	// 相机位置
	m_camera->mPosition = glm::vec3(0.0f, 0.0f, 5.0f);
	// 创建相机控制器
	m_cameraControl = new TrackBallCameraControl();
	//m_cameraControl = new Camera2DControl();
	m_cameraControl->setcamera(m_camera);
}

void MyOpenGLWidget::paperbox()
{
	if (!m_scene) {
		qDebug() << "ERROR: scene 为空";
		return;
	}
	Texture* envTexBox = new Texture("assets/textures/box.png", 0);
	auto box = Geometry::createBox(1.0f);
	auto boxmat = new LightTestMaterial();
	boxmat->mDiffuse = envTexBox;

	
	Mesh* boxMesh = new Mesh(box, boxmat);
	boxMesh->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	m_scene->addChild(boxMesh);

	if (m_dirLight)
	{
		m_dirLight->mDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
		m_dirLight->mColor = glm::vec3(0.9, 0.85f, 0.75f);
	}
}

void MyOpenGLWidget::loadTexture(const std::string& imagePath)
{
	m_texture = Texture::createTexture(imagePath, 0);
	if (!m_texture) {
		qDebug() << "纹理加载失败:" << QString::fromStdString(imagePath);
	}
}

void MyOpenGLWidget::setCurrentSlice(int index)
{
	DicomTexture* dicomTex = dynamic_cast<DicomTexture*>(m_texture);
	if (dicomTex) {
		if (dicomTex->setCurrentSlice(static_cast<size_t>(index))) {
			qDebug() << "✅ 切换到切片:" << (index + 1) << "/" << dicomTex->getTotalSlices();
			update();  // 刷新显示
		}
	}
}

void MyOpenGLWidget::keyPressEvent(QKeyEvent* event)
{
	if (m_cameraControl)
	{
		// 1 表示按下，0 表示释放（你可以自定义，常用1=Press, 0=Release）
		m_cameraControl->onKey(event->key(), 1, event->modifiers());
	}
	QOpenGLWidget::keyPressEvent(event); // 保留父类行为
}

void MyOpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (m_cameraControl)
	{
		// 1 表示按下，0 表示释放（你可以自定义，常用1=Press, 0=Release）
		m_cameraControl->onKey(event->key(), 0, event->modifiers());
	}
	QOpenGLWidget::keyPressEvent(event); // 保留父类行为
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	//qDebug() << "鼠标点击: 按钮" << event->button() << ", 按下";
	if (m_cameraControl)
	{
		// 1 表示按下，0 表示释放（你可以自定义，常用1=Press, 0=Release）
		m_cameraControl->onMouse(static_cast<int>(event->button()), 1, event->position().x(), event->position().y());
	}
	// 其他处理逻辑...
	QOpenGLWidget::mousePressEvent(event); // 保留父类行为（可选）
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	//qDebug() << "鼠标点击: 按钮" << event->button() << ", 松开";
		// 其他处理逻辑...
	if (m_cameraControl)
	{
		// 1 表示按下，0 表示释放（你可以自定义，常用1=Press, 0=Release）
		m_cameraControl->onMouse(static_cast<int>(event->button()), 0, event->position().x(), event->position().y());
	}
	QOpenGLWidget::mouseReleaseEvent(event); // 保留父类行为（可选）
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	//qDebug() << "鼠标移动: 位置" << event->pos();
		// 其他处理逻辑...
	if (m_cameraControl)
	{
		// 1 表示按下，0 表示释放（你可以自定义，常用1=Press, 0=Release）
		m_cameraControl->onCursor(event->position().x(), event->position().y());
	}
	QOpenGLWidget::mouseMoveEvent(event); // 保留父类行为（可选）
}

void MyOpenGLWidget::wheelEvent(QWheelEvent* event)
{
	//qDebug() << "鼠标滚轮: 角度" << event->angleDelta();
		// 其他处理逻辑...
	if (m_cameraControl)
	{
		// 传递滚轮滚动的距离，正值为向上，负值为向下
		m_cameraControl->onScroll(event->angleDelta().y());
	}
	QOpenGLWidget::wheelEvent(event); // 保留父类行为（可选）
}

void MyOpenGLWidget::showEvent(QShowEvent* event)
{
	QOpenGLWidget::showEvent(event);
	setFocus(Qt::OtherFocusReason); // 这里再次请求焦点
}
