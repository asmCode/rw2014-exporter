#pragma once

#include <Math\Vec3.h>
#include <Math\Matrix.h>
#include <Math\Vec2.h>
#include "Scene3DMeshPart.h"
#include <string>
#include "../Property.h"

class Scene3DMesh
{
public:
	int id;
	std::string name;

	std::vector<Scene3DMeshPart*> meshParts;
	std::vector<Property*> properties;
	sm::Matrix m_worldInverseMatrix;

	~Scene3DMesh()
	{
		for (unsigned i = 0; i < meshParts.size(); i++)
			delete meshParts[i];

		meshParts.clear();
	}
};

