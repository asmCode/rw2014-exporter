#pragma once

#include "Scene3DVertex.h"
#include <Math\Vec3.h>
#include <Math\Matrix.h>
#include <Math\Vec2.h>
#include <string>
#include "../Property.h"

class Scene3DMesh
{
public:
	int id;
	std::string name;

	std::vector<int> bonesIds;
	std::vector<Scene3DVertex*> vertices;

	std::vector<Property*> properties;
	sm::Matrix m_worldInverseMatrix;

	std::string materialName;

	~Scene3DMesh()
	{
		for (unsigned i = 0; i < vertices.size(); i++)
			delete vertices[i];

		vertices.clear();
	}
};

