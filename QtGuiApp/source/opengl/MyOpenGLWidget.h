#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include "../camera/Camera.h"
#include "../camera/CameraControl.h"
#include "Shader.h"
#include "Texture.h"

#include "Material/imageMaterial.h"


class Renderer;
class Scene;
class DirectionalLight;
class PointLight;
class SpotLight;
class AmbientLight;
class Mesh;

class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	explicit MyOpenGLWidget(QWidget* parent = nullptr);

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;

public:
	void switchTexture(const std::string& imagePath);
	void createImageMesh(Texture* texture);
	void papaercamera();
	void loadTexture(const std::string& imagePath);
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



	// 纹理
	Texture* m_texture = nullptr;

	float m_rotationAngle = 0.0f;  // 添加旋转角度变量

	// 渲染框架
	Renderer* m_renderer{ nullptr };
	Scene* m_scene{ nullptr };

	// 图像对象
	Mesh* m_imageMesh{ nullptr };
	Material* m_imageMaterial{ nullptr };

	// 相机
	Camera* m_camera{ nullptr };
	CameraControl* m_cameraControl{ nullptr };

	// 光源(为Renderer接口准备)
	DirectionalLight* m_dirLight{ nullptr };
	std::vector<PointLight*> m_pointLights;
	SpotLight* m_spotLight{ nullptr };
	AmbientLight* m_ambLight{ nullptr };
};