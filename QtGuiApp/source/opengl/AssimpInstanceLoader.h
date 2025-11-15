#pragma once
#include "../pch.h"
#include "Object.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "tools/tools.h"
#include "mesh/instancedMesh.h"
#include "material/phongInstanceMaterial.h"
#include "material/grassInstanceMaterial.h"
#include "Texture.h"

class AssimpInstanceLoader
{
public:
	AssimpInstanceLoader();
	~AssimpInstanceLoader();

	static Object* load(const std::string& path, int instanceCount);

private:
	static void processNode(aiNode* ainode, Object* parent, const aiScene* scene, const std::string& rootPath, int instanceCount);
    static InstancedMesh* processMesh(aiMesh* aimesh, const aiScene* scene, const std::string& rootPath, int instanceCount);
	static Texture* processTexture(const aiMaterial* aimat, const aiTextureType& type, const aiScene* scene, const std::string& rootPath);

	static glm::mat4 getMat4f(aiMatrix4x4 value);
};

