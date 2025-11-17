#pragma once

#include "../../pch.h"
#include "../mesh/Mesh.h"
#include "../../camera/Camera.h"
#include "../Light/DirectionalLight.h"
#include "../Light/PointLight.h"
#include "../Light/SpotLight.h"
#include "../Light/AmbientLight.h"
#include "../Shader.h"
#include "../Scene.h"


class Renderer :protected QOpenGLFunctions_4_5_Core {
public:
	Renderer();
	~Renderer();
	void render(Scene* scene,
		Camera* camera, DirectionalLight* dirLight, 
		std::vector<PointLight*>& pointLights,
		SpotLight* spotLight, AmbientLight* ambLight,
		unsigned int fbo = 0
	);

	void rendererObject(
		Object* object,
		Camera* camera, DirectionalLight* dirLight, std::vector<PointLight*>& pointLights,
		SpotLight* spotLight, AmbientLight* ambLight
	);
public:
	Material* mGlobalMaterial{ nullptr }; //全局材质

	void setWidth(int w) { m_width = w; }
	void setHeight(int h) { m_height = h; }

private:

	void projectObject(Object* obj);

	//根据Material类型不同,挑选不同的shader
	Shader* pickShader(MaterialType type);
	void setDepthState(Material* material);
	void setPolygonOffsetState(Material* material);
	void setStencilState(Material* material);
	void setBlendState(Material* material);
	void setFaceCullingState(Material* material);
private:
	//生成多种不同的shader对象
	Shader* mPhongShader{ nullptr };
	Shader* mWhiteShader{ nullptr };
	Shader* mImageShader{ nullptr };
	Shader* mDepthShader{ nullptr };
	Shader* mOpacityMaskShader{ nullptr };
	Shader* mScreenShader{ nullptr };
	Shader* mCubeShader{ nullptr };
	Shader* mPhongEnvShader{ nullptr };
	Shader* mPhongInstancedShader{ nullptr };
	Shader* mGrassInstanceShader{ nullptr };
	Shader* mDicomShader{ nullptr };
	Shader* mLightTestShader{ nullptr };

	//不透明物体与透明物体的队列
	//注意!! 每一帧绘制前需要清空两个队列
	std::vector<Mesh*> mOpacityObjects{};
	std::vector<Mesh*> mTransparentObjects{};

	int m_width{ 0 };
	int m_height{ 0 };

	QElapsedTimer m_timer;
};
