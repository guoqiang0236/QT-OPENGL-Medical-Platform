#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include "../camera/Camera.h"
#include "../camera/CameraControl.h"
#include "Shader.h"
class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	explicit MyOpenGLWidget(QWidget* parent = nullptr);

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;
	unsigned int VBO, VAO=0;

public:
	void paperrectangle();
	void papershader(std::string vert, std::string frag);
	void papaercamera();
protected:
	void keyPressEvent(QKeyEvent* event) override;       // 对应键盘按下
	void keyReleaseEvent(QKeyEvent* event) override;     // 对应键盘释放
	void mousePressEvent(QMouseEvent* event) override;   // 对应鼠标按下
	void mouseReleaseEvent(QMouseEvent* event) override; // 对应鼠标释放
	void mouseMoveEvent(QMouseEvent* event) override;    // 对应鼠标移动
	void wheelEvent(QWheelEvent* event)override;
	void showEvent(QShowEvent* event) override; // 对应窗口显示

private:

	//着色器
	std::unique_ptr<Shader> m_Shader;

	//摄像机
	Camera* m_camera = nullptr; // 透视摄像机
	CameraControl* m_cameraControl = nullptr; // 摄像机控制器

	float m_rotationAngle = 0.0f;  // 添加旋转角度变量
};