#pragma once

#include <string>

class Material;

class Source
{
public:
	std::string MeshName;
	Material* Material;
};
