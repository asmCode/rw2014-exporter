#pragma once

#include <windows.h>
#include <string>
#include "Scene3DVertex.h"

class Scene3DMeshPart
{
public:
	std::string materialName;
	uint8_t m_vertexType;

	std::vector<Scene3DVertex*> vertices;

	~Scene3DMeshPart()
	{
		for (unsigned i = 0; i < vertices.size(); i++)
			delete vertices[i];

		vertices.clear();
	}
};
