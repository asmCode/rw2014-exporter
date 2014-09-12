#pragma once

#include <Math/Vec3.h>
#include <vector>

template <typename T> class Key;

class Material
{
public:
	sm::Vec3 DiffuseColor;
	float Opacity;
	std::vector<Key<float>*> OpacityAnim;
};

