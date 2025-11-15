#include "Mesh.h"

Mesh::Mesh(Geometry* geometry, Material* material)
{
	mGeometry = geometry;
	mMaterial = material;
	m_Type = ObjectType::Mesh;
}

Mesh::~Mesh()
{
}
