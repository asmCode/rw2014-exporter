#pragma once

#include <Math/Vec3.h>
#include <vector>
#include <string>

template <typename T> class Key;

class Material
{
public:
	std::string Name;
	sm::Vec3 DiffuseColor;
	float Opacity;
	bool UseSolid;
	bool UseWire;
	float SolidGlowPower;
	float SolidGlowMultiplier;
	float WireGlowPower;
	float WireGlowMultiplier;
	std::vector<Key<float>*> OpacityAnim;
};

