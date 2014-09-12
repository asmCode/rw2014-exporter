#pragma once

#include <string>

class Material;

class StaticSource
{
public:
	std::string MeshName;
	Material* Material;
};
