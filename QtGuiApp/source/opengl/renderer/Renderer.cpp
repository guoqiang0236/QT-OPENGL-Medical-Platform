#include "Renderer.h"
#include "../Material/PhongMaterial.h"
#include "../Material/WhiteMaterial.h"
#include "../Material/ImageMaterial.h"
#include "../Material/opacityMaskMaterial.h"
#include "../Material/screenMaterial.h"
#include "../Material/cubeMaterial.h"
#include "../Material/phongEnvMaterial.h"
#include "../Material/phongInstanceMaterial.h"
#include "../Material/grassInstanceMaterial.h"
#include "../Material/DicomMaterial.h"
#include "../Mesh/instancedMesh.h"

Renderer::Renderer()
{
	initializeOpenGLFunctions();
	mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
	mImageShader = new Shader("assets/shaders/image.vert", "assets/shaders/image.frag");
	mDepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");
	mOpacityMaskShader = new Shader("assets/shaders/phongOpacityMask.vert", "assets/shaders/phongOpacityMask.frag");
	mScreenShader = new Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
	mCubeShader = new Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");
	mPhongEnvShader = new Shader("assets/shaders/phongEnv.vert", "assets/shaders/PhongEnv.frag");
	mPhongInstancedShader = new Shader("assets/shaders/phongInstance.vert", "assets/shaders/phongInstance.frag");
	mGrassInstanceShader = new Shader("assets/shaders/grassInstance.vert", "assets/shaders/grassInstance.frag");
	mDicomShader = new Shader("assets/shaders/dicom.vert", "assets/shaders/dicom.frag");  
	m_timer.start();
}

Renderer::~Renderer()
{
	if (mPhongShader)
	{
		delete mPhongShader;
		mPhongShader = nullptr;
	}
}



void Renderer::render(Scene* scene,
	Camera* camera, DirectionalLight* dirLight, std::vector<PointLight*>& pointLights,
	SpotLight* spotLight, AmbientLight* ambLight, unsigned int fbo)
{
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);// 绑定帧缓冲对象

	//1 设置当前帧绘制的时候,opengl的必要状态机参数
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);//保证每一帧能顺利清楚深度缓冲(否则如果最后一个mesh把深度写入关了,就没法clear,产生bug)

	glDisable(GL_POLYGON_OFFSET_FILL);// 关闭多边形偏移填充
	glDisable(GL_POLYGON_OFFSET_LINE);// 关闭多边形偏移线
 
	//开启测试 设置基本写入状态 打开模版测试写入
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);// fail/zfail/zpass 的处理
	glStencilMask(0xff);//开启写入 保证了模版缓冲可以被清理

	//默认颜色混合
	glDisable(GL_BLEND);

	//2 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
	//清空两个队列
	mOpacityObjects.clear();
	mTransparentObjects.clear();
	projectObject(scene);

	std::sort(mTransparentObjects.begin(), mTransparentObjects.end(),
		[camera](Mesh* a, Mesh* b) {
			auto viewMatrix = camera->getViewMatrix();
			//1 计算A的相机系的Z
			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			//乘上他的模型变换矩阵就是世界空间的坐标
			auto cameraPositionA = viewMatrix * worldPositionA;

			//2 计算B的相机系的Z
			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB= modelMatrixB * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z; // Z值越小，越远离相机
		});

	//3 渲染两个队列
	//先渲染不透明
	for (int i = 0; i < mOpacityObjects.size(); i++)
	{
		rendererObject(mOpacityObjects[i], camera, dirLight, pointLights, spotLight, ambLight);
	}

	//再渲染透明
	for (int i = 0; i < mTransparentObjects.size(); i++)
	{
		rendererObject(mTransparentObjects[i], camera, dirLight, pointLights, spotLight, ambLight);

	}
}

//针对单个object
void Renderer::rendererObject(Object* object, Camera* camera,
	DirectionalLight* dirLight, std::vector<PointLight*>& pointLights,
	SpotLight* spotLight, AmbientLight* ambLight)
{
	//1 判断是Mesh还是Object，如果是Mesh就需要渲染
	if (object->getType() == ObjectType::Mesh|| object->getType()== ObjectType::InstancedMesh) {
		auto mesh = (Mesh*)object;
		if (mesh->getShow() == false)
			return;
		auto geometry = mesh->mGeometry;

		//如果是全局材质,则使用全局材质
		Material* material = nullptr;
		if (mGlobalMaterial != nullptr)
		{
			material = mGlobalMaterial;
		}
		else
		{
			material = mesh->mMaterial;
		}
			
		//设置渲染状态
		setDepthState(material);
		setPolygonOffsetState(material);
		setStencilState(material);
		setBlendState(material);
		setFaceCullingState(material);

		// 1. 决定使用哪个Shader
		Shader* shader = pickShader(material->mType);


		// 2. 更新shader的uniform
		shader->begin();

		switch (material->mType) {
		case MaterialType::PhongMaterial: {
			PhongMaterial* phongMat = static_cast<PhongMaterial*>(material);

			//diffuse贴图
			//将纹理采样器与纹理单元挂钩
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			phongMat->mDiffuse->bind();

			shader->setInt("specularMaskSampler", 1);
			if(phongMat->mSpecularMask)
				phongMat->mSpecularMask->bind();
			//mvp矩阵的更新
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);



			// 光源参数的uniform更新 
			//SpotLight
			if (spotLight)
			{
				shader->setVector3("SpotLight.position", spotLight->getPosition());
				shader->setVector3("SpotLight.color", spotLight->mColor);
				shader->setVector3("SpotLight.targetDirection", spotLight->mTargetDirection);
				shader->setFloat("SpotLight.specularIntensity", spotLight->mSpecularIntensity);
				shader->setFloat("SpotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
				shader->setFloat("SpotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
			}
				
			//directionalLight
			shader->setVector3("MydirectionalLight.color", dirLight->mColor);
			shader->setVector3("MydirectionalLight.direction", dirLight->mDirection);
			shader->setFloat("MydirectionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("MydirectionalLight.intensity", dirLight->mIntensity);

			//pointLight
			for (int i = 0; i < pointLights.size(); i++) {
				auto pointLight = pointLights[i];

				// 动态构建索引名称
				std::string baseName = "PointLight[";
				baseName.append(std::to_string(i));
				baseName.append("]");
				shader->setVector3(baseName + ".color", pointLight->mColor);
				shader->setVector3(baseName + ".position", pointLight->getPosition());
				shader->setFloat(baseName + ".specularIntensity", pointLight->mSpecularIntensity);
				shader->setFloat(baseName + ".k2", pointLight->mK2);  // 二次项衰减系数
				shader->setFloat(baseName + ".k1", pointLight->mK1);  // 线性衰减系数
				shader->setFloat(baseName + ".kc", pointLight->mKc);  // 常数项衰减系数

			}


			shader->setVector3("ambientColor", ambLight->mColor);
			shader->setFloat("shiness", phongMat->mShiness);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);
		}
										break;
		case MaterialType::WhiteMaterial: {
			//mvp矩阵的更新
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

		}
										break;
			
		case MaterialType::ImageMaterial: {
			ImageMaterial* imageMat = static_cast<ImageMaterial*>(material);
		

			if (imageMat->mDiffuse) {  // ✅ 检查纹理是否存在
				imageMat->mDiffuse->bind();  // ✅ 绑定纹理到激活的纹理单元
			}

			//mvp矩阵的更新
			shader->setInt("imagesampler", 2);
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

		}
										break;
		case MaterialType::DepthMaterial: {
			//mvp矩阵的更新
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			shader->setFloat("near", camera->mNear);
			shader->setFloat("far", camera->mFar);

		}
										break;
		case MaterialType::OpacityMaskMaterial: {
			OpacityMaskMaterial* opacityMat = static_cast<OpacityMaskMaterial*>(material);
			//将纹理采样器与纹理单元挂钩
			//diffuse蒙版的帧更新
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			opacityMat->mDiffuse->bind();
			//opacityMask蒙版的帧更新
			shader->setInt("opacityMaskSampler", 1);
			opacityMat->mOpacityMask->bind();
			//MVP
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新 
			//directionalLight的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);

			shader->setFloat("shiness", opacityMat->mShiness);
			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);
		}
										break;
		case MaterialType::ScreenMaterial: {
			ScreenMaterial* screenMat = static_cast<ScreenMaterial*>(material);
			shader->setInt("screenTexSampler", 0);
			screenMat->mScreenTexture->bind();
			shader->setFloat("texWidth", m_width);//先写死 后面再优化
			shader->setFloat("texHeight", m_height);
		}
											break;
		case MaterialType::CubeMaterial: {
			
			CubeMaterial* cubeMat = static_cast<CubeMaterial*>(material);

			mesh->setPosition(camera->mPosition);
			//sphericalSampler
			shader->setInt("cubeSampler", 0);
			cubeMat->mDiffuse->bind();
			//MVP
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
				

			
		}
										break;
		case MaterialType::PhongEnvMaterial: {
			MyPhongEnvMaterial* phongMat = static_cast<MyPhongEnvMaterial*>(material);

			shader->setInt("sampler", 0);
			phongMat->mDiffuse->bind();

			shader->setInt("envSampler", 1);
			phongMat->mEnv->bind();

			shader->setInt("specularMaskSampler", 1);
			if (phongMat->mSpecularMask)
				phongMat->mSpecularMask->bind();
			//mvp矩阵的更新
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);



			// 光源参数的uniform更新 
			//SpotLight
			if (spotLight)
			{
				shader->setVector3("SpotLight.position", spotLight->getPosition());
				shader->setVector3("SpotLight.color", spotLight->mColor);
				shader->setVector3("SpotLight.targetDirection", spotLight->mTargetDirection);
				shader->setFloat("SpotLight.specularIntensity", spotLight->mSpecularIntensity);
				shader->setFloat("SpotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
				shader->setFloat("SpotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
			}

			//directionalLight
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			//pointLight
			for (int i = 0; i < pointLights.size(); i++) {
				auto pointLight = pointLights[i];

				// 动态构建索引名称
				std::string baseName = "PointLight[";
				baseName.append(std::to_string(i));
				baseName.append("]");
				shader->setVector3(baseName + ".color", pointLight->mColor);
				shader->setVector3(baseName + ".position", pointLight->getPosition());
				shader->setFloat(baseName + ".specularIntensity", pointLight->mSpecularIntensity);
				shader->setFloat(baseName + ".k2", pointLight->mK2);  // 二次项衰减系数
				shader->setFloat(baseName + ".k1", pointLight->mK1);  // 线性衰减系数
				shader->setFloat(baseName + ".kc", pointLight->mKc);  // 常数项衰减系数

			}


			shader->setVector3("ambientColor", ambLight->mColor);
			shader->setFloat("shiness", phongMat->mShiness);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);
		}
										break;
		case MaterialType::PhongInstanceMaterial: {
			PhongInstanceMaterial* phongMat = static_cast<PhongInstanceMaterial*>(material);
			InstancedMesh* im = static_cast<InstancedMesh*>(mesh);
			//diffuse贴图
			//将纹理采样器与纹理单元挂钩
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			phongMat->mDiffuse->bind();

			
			//mvp矩阵的更新
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);



			// 光源参数的uniform更新 
			//SpotLight
			if (spotLight)
			{
				shader->setVector3("SpotLight.position", spotLight->getPosition());
				shader->setVector3("SpotLight.color", spotLight->mColor);
				shader->setVector3("SpotLight.targetDirection", spotLight->mTargetDirection);
				shader->setFloat("SpotLight.specularIntensity", spotLight->mSpecularIntensity);
				shader->setFloat("SpotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
				shader->setFloat("SpotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
			}

			//directionalLight
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			//pointLight
			for (int i = 0; i < pointLights.size(); i++) {
				auto pointLight = pointLights[i];

				// 动态构建索引名称
				std::string baseName = "PointLight[";
				baseName.append(std::to_string(i));
				baseName.append("]");
				shader->setVector3(baseName + ".color", pointLight->mColor);
				shader->setVector3(baseName + ".position", pointLight->getPosition());
				shader->setFloat(baseName + ".specularIntensity", pointLight->mSpecularIntensity);
				shader->setFloat(baseName + ".k2", pointLight->mK2);  // 二次项衰减系数
				shader->setFloat(baseName + ".k1", pointLight->mK1);  // 线性衰减系数
				shader->setFloat(baseName + ".kc", pointLight->mKc);  // 常数项衰减系数

			}


			shader->setVector3("ambientColor", ambLight->mColor);
			shader->setFloat("shiness", phongMat->mShiness);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);

			//**********传输uniform类型的矩阵变换数组************
			//shader->setMatrix4x4Array("matrices", im->mInstanceMatrices, im->mInstanceCount);
		}
										break;
		case MaterialType::GrassInstanceMaterial: {
			GrassInstanceMaterial* grassMat = static_cast<GrassInstanceMaterial*>(material);
			InstancedMesh* im = static_cast<InstancedMesh*>(mesh);
				
			im->sortMatrices(camera->getViewMatrix());//对实例化的矩阵进行排序
			im->updateMatrices();

			shader->setInt("sampler", 0);
			grassMat->mDiffuse->bind();

			shader->setInt("opacityMask", 1);
			grassMat->mOpacityMask->bind();

			//mvp矩阵的更新
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			/*auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);*/



			// 光源参数的uniform更新 
			//SpotLight
			if (spotLight)
			{
				shader->setVector3("SpotLight.position", spotLight->getPosition());
				shader->setVector3("SpotLight.color", spotLight->mColor);
				shader->setVector3("SpotLight.targetDirection", spotLight->mTargetDirection);
				shader->setFloat("SpotLight.specularIntensity", spotLight->mSpecularIntensity);
				shader->setFloat("SpotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
				shader->setFloat("SpotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
			}

			//directionalLight
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			//pointLight
			for (int i = 0; i < pointLights.size(); i++) {
				auto pointLight = pointLights[i];

				// 动态构建索引名称
				std::string baseName = "PointLight[";
				baseName.append(std::to_string(i));
				baseName.append("]");
				shader->setVector3(baseName + ".color", pointLight->mColor);
				shader->setVector3(baseName + ".position", pointLight->getPosition());
				shader->setFloat(baseName + ".specularIntensity", pointLight->mSpecularIntensity);
				shader->setFloat(baseName + ".k2", pointLight->mK2);  // 二次项衰减系数
				shader->setFloat(baseName + ".k1", pointLight->mK1);  // 线性衰减系数
				shader->setFloat(baseName + ".kc", pointLight->mKc);  // 常数项衰减系数

			}


			shader->setVector3("ambientColor", ambLight->mColor);
			shader->setFloat("shiness", grassMat->mShiness);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);

			//贴图特性
			shader->setFloat("uvScale", grassMat->mUVScale);
			shader->setFloat("brightness", grassMat->mBrightness);
			
			

			//风力
			float time = m_timer.elapsed() / 1000.0f; // 单位：秒
			shader->setFloat("time", time);
			shader->setFloat("windScale", grassMat->mWindScale);
			shader->setVector3("windDirection", grassMat->mWindDirection);
			shader->setFloat("phaseScale", grassMat->mPhaseScale);
			//**********传输uniform类型的矩阵变换数组************
			//shader->setMatrix4x4Array("matrices", im->mInstanceMatrices, im->mInstanceCount);
		}
												break;
		case MaterialType::DicomMaterial: {
			DicomMaterial* dicomMat = static_cast<DicomMaterial*>(material);

			// 绑定纹理
			if (dicomMat->mDiffuse) {
				dicomMat->mDiffuse->bind();
				shader->setInt("dicomTexture", 2);
			}

			// 设置 DICOM 特有的 uniform
			shader->setFloat("minPixelValue", dicomMat->mMinPixelValue);
			shader->setFloat("maxPixelValue", dicomMat->mMaxPixelValue);

			// MVP 矩阵更新
			shader->setMatrix4x4("model", mesh->getModelMatrix());
			shader->setMatrix4x4("view", camera->getViewMatrix());
			shader->setMatrix4x4("projection", camera->getProjectionMatrix());
		}
		default:
			break;
		}

		// 3. 绑定vao
		glBindVertexArray(geometry->getVao());

		// 4. 执行绘制命令
		if(object->getType() == ObjectType::InstancedMesh)
		{
			//如果是实例化的mesh,则需要传入实例化的数量
			auto im = static_cast<InstancedMesh*>(object);
			glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, (void*)(sizeof(int) * 0), im->mInstanceCount);
		}
		else {
			glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, (void*)(sizeof(int) * 0));

		}
	}

	//2 遍历Object的子节点，对每个子节点都需要调用rendererObject
	auto children = object->getChildren();
	for (int i = 0;i < children.size();i++)
	{
			
		rendererObject(children[i], camera, dirLight, pointLights, spotLight, ambLight);
	}
}

void Renderer::projectObject(Object* obj)
{
	if (obj->getType() == ObjectType::Mesh|| obj->getType() == ObjectType::InstancedMesh)
	{
		Mesh* mesh = static_cast<Mesh*>(obj);

		auto material = mesh->mMaterial;

		if (material->mBlend)
		{
			mTransparentObjects.push_back(mesh);
		}
		else
		{
			mOpacityObjects.push_back(mesh);
		}
			
	}

	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		projectObject(children[i]);
	}
		
}

Shader* Renderer::pickShader(MaterialType type)
{
	Shader* result = nullptr;
	switch (type)
	{
	case MaterialType::PhongMaterial:
		result = mPhongShader;
		break;
	case MaterialType::WhiteMaterial:
		result = mWhiteShader;
		break;
	case MaterialType::ImageMaterial:
		result = mImageShader;
		break;
	case MaterialType::DepthMaterial:
		result = mDepthShader;
		break;
	case MaterialType::OpacityMaskMaterial:
		result = mOpacityMaskShader;
		break;
	case MaterialType::ScreenMaterial:
		result = mScreenShader;
		break;
	case MaterialType::CubeMaterial:
		result = mCubeShader;
		break;
	case MaterialType::PhongEnvMaterial:
		result = mPhongEnvShader;
		break;
	case MaterialType::PhongInstanceMaterial:
		result = mPhongInstancedShader;
		break;
	case MaterialType::GrassInstanceMaterial:
		result = mGrassInstanceShader;
		break;
	case MaterialType::DicomMaterial:  
		result = mDicomShader;
		break;
	default:
		std::cout << "Unknown material type to pick shader" << std::endl;
		break;
	}
	return result;
}


	

void Renderer::setDepthState(Material* material)
{
	//检测深度状态
	if (material->mDepthTest)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(material->mDepthFunc);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (material->mDepthWrite)
	{
		glDepthMask(GL_TRUE);
	}
	else
	{
		glDepthMask(GL_FALSE);
	}
}
void Renderer::setPolygonOffsetState(Material* material)
{
	// 检测多边形偏移状态
	if (material->mPolygonOffset)
	{
		glEnable(material->mPolygonOffsetType);
		glPolygonOffset(material->mFactor, material->mUnit);
	}
	else
	{
		glDisable(material->mPolygonOffsetType);
	}
}

void Renderer::setStencilState(Material* material) {
	if (material->mStencilTest)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(material->mSFail, material->mZFail, material->mZPass);
		glStencilMask(material->mStencilMask);
		glStencilFunc(material->mStencilFunc, material->mStencilRef, material->mStencilFuncMask);
	}
	else
	{
		glDisable(GL_STENCIL_TEST);
	}
}
void Renderer::setBlendState(Material* material)
{
	if (material->mBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(material->mSFactor, material->mDFactor);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}
void Renderer::setFaceCullingState(Material* material)
{
	if (material->mFaceCulling)
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(material->mFrontFace);
		glCullFace(material->mCullFace);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}


