#include "MyOpenGLWidget.h"
#include "../camera/PerspectiveCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


MyOpenGLWidget::MyOpenGLWidget(QWidget* parent):QOpenGLWidget(parent)
{
}

void MyOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	paperrectangle();
	papershader("../assets/shaders/rectangle.vert", "../assets/shaders/rectangle.frag");
	papaercamera();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void MyOpenGLWidget::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.32f, 1.0f); //set方法【重点】如果没有 initializeGL，目前是一个空指针状态，没有指向显卡里面的函数，会报错
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // use方法
	if (m_Shader && m_camera)
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

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_Shader->end();
	}

	// 更新旋转角度
	m_rotationAngle += 1.0f;
	if (m_rotationAngle >= 360.0f) {
		m_rotationAngle = 0.0f;
	}

	update(); // 请求下一帧重绘
}

void MyOpenGLWidget::paperrectangle()
{
	// 顶点数据
	float vertices[] = {
		// 第一个三角形
		-0.5f, -0.5f, 0.0f,  // 左下
		 0.5f, -0.5f, 0.0f,  // 右下
		-0.5f,  0.5f, 0.0f,  // 左上
		// 第二个三角形
		-0.5f,  0.5f, 0.0f,  // 左上
		 0.5f, -0.5f, 0.0f,  // 右下
		 0.5f,  0.5f, 0.0f   // 右上
	};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	m_camera = new PerspectiveCamera(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

	// 创建相机控制器
	m_cameraControl = new CameraControl();
	m_cameraControl->setcamera(m_camera);
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
