#include "MyOpenGLWidget.h"
#include "../camera/PerspectiveCamera.h"
#include "../camera/TrackBallCameraControl.h"
#include "../camera/OrthographicCamera.h"
#include "../camera/GameCameraControl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


MyOpenGLWidget::MyOpenGLWidget(QWidget* parent):QOpenGLWidget(parent)
{
}

void MyOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	paperrectangle();
	papershader("../assets/shaders/rectangle_image.vert", "../assets/shaders/rectangle_image.frag");
	papaercamera();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void MyOpenGLWidget::paintGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // use方法
	if (m_Shader && m_camera && m_texture)
	{
		m_Shader->begin();

		// 设置 Model 矩阵（模型变换，这里保持单位矩阵）
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(m_rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		m_Shader->setMatrix4x4("model", model);



		// 设置 View 矩阵（相机视图）
		glm::mat4 view = m_camera->getViewMatrix();
		m_Shader->setMatrix4x4("view", view);

		// 设置 Projection 矩阵（透视投影）
		glm::mat4 projection = m_camera->getProjectionMatrix();
		m_Shader->setMatrix4x4("projection", projection);
		
		m_texture->bind();
		m_Shader->setInt("imageTexture", 0); // 纹理单元 0

		glBindVertexArray(VAO);
		// 参数：绘制模式、索引数量、索引数据类型、索引缓冲偏移
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		m_Shader->end();
	}

	// 更新旋转角度
	/*m_rotationAngle += 1.0f;
	if (m_rotationAngle >= 360.0f) {
		m_rotationAngle = 0.0f;
	}*/

	update(); // 请求下一帧重绘
}

// ✨ 核心方法：运行时切换纹理
void MyOpenGLWidget::switchTexture(const std::string& imagePath)
{
	// 在 OpenGL 线程中执行
	makeCurrent();

	qDebug() << "[切换纹理] 新路径:" << QString::fromStdString(imagePath);

	// 加载新纹理
	Texture* newTexture = Texture::createTexture(imagePath, 0);

	if (!newTexture) {
		qDebug() << "ERROR: 新纹理加载失败:" << QString::fromStdString(imagePath);
		doneCurrent();
		return;
	}

	// 替换旧纹理
	m_texture = newTexture;

	qDebug() << "SUCCESS: 纹理切换完成! 新尺寸:" << m_texture->getWidth() << "x" << m_texture->getHeight();

	// 触发重绘
	update();

	doneCurrent();
}

void MyOpenGLWidget::paperrectangle()
{
	// 顶点数据
	// 交错存储：位置(3 float) + UV(2 float) = 5 float 每顶点
	std::vector<float> vertices = {
		// 位置 (x, y, z)      // UV (u, v)
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f,   // 顶点 0：左下
		 0.5f, -0.5f, 0.0f,    1.0f, 0.0f,   // 顶点 1：右下
		 0.5f,  0.5f, 0.0f,    1.0f, 1.0f,   // 顶点 2：右上
		-0.5f,  0.5f, 0.0f,    0.0f, 1.0f    // 顶点 3：左上
	};
	// 索引数据（两个三角形，6 个索引）
	std::vector<unsigned int> indices = {
		0, 1, 2, 
		0, 2, 3  
	};
	// 创建 VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// 创建 EBO（索引缓冲）
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// 创建 VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 绑定 VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// 位置属性（location 0）
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	
	// UV 属性（location 1）
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// 绑定 EBO 到 VAO（重要！）
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);
}

void MyOpenGLWidget::papershader(std::string vert, std::string frag)
{
	if(!m_Shader)
		m_Shader = std::make_unique<Shader>(vert.c_str(), frag.c_str());
}

void MyOpenGLWidget::papaercamera()
{
	// 创建透视相机：视场角 45度，宽高比 800/600，近平面 0.1，远平面 100
	//m_camera = new PerspectiveCamera(30.0f, 800.0f / 500.0f, 0.0001f, 100.0f);
	// 动态计算宽高比
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// 创建正交相机 - 无透视变形,完美显示2D图像
	m_camera = new OrthographicCamera(
		-aspect,  // 根据窗口宽高比调整左右边界
		aspect,
		1.0f,     // 固定上下边界
		-1.0f,
		0.1f,     // 近平面
		100.0f    // 远平面
	);

	// 相机位置
	m_camera->mPosition = glm::vec3(0.0f, 0.0f, 1.0f);
	// 创建相机控制器
	m_cameraControl = new TrackBallCameraControl();
	m_cameraControl->setcamera(m_camera);
}

void MyOpenGLWidget::loadTexture(const std::string& imagePath)
{
	m_texture = Texture::createTexture(imagePath, 0);
	if (!m_texture) {
		qDebug() << "纹理加载失败:" << QString::fromStdString(imagePath);
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
