#include "MyOpenGLWidget.h"

// 顶点着色器的源代码，顶点着色器就是把 xyz 原封不动的送出去
const char* vertexShaderSource = "#version 450 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";


// 片段着色器的源代码，片段着色器就是给一个固定的颜色
const char* fragmentShaderSource = "#version 450 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


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

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent):QOpenGLWidget(parent)
{
}

void MyOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	
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

	// 编译顶点着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void MyOpenGLWidget::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.32f, 1.0f); //set方法【重点】如果没有 initializeGL，目前是一个空指针状态，没有指向显卡里面的函数，会报错
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // use方法

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
