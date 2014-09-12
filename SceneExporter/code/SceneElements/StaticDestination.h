#pragma once

#include <string>

class Material;

class StaticDestination
{
public:
	std::string MeshName;
	Material* Material;
};
