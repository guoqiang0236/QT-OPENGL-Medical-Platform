#pragma once
#include "Mesh.h"
#include <algorithm>


class InstancedMesh :public Mesh
{
public:
	InstancedMesh(Geometry* geometry, Material* material, unsigned int instanceCount);
	~InstancedMesh();

		
		

	unsigned int mInstanceCount{ 0 };
	std::vector<glm::mat4> mInstanceMatrices{};
	unsigned int mMatrixVbo{ 0 };
	void updateMatrices();
	void sortMatrices(glm::mat4 viewMatrix);
};

