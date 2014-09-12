#pragma once

#include <Math/Vec4.h>
#include <vector>

template <typename T> class Key;

class Material
{
public:
	sm::Vec4 DiffuseColor;
	std::vector<Key<float>*> OpacityAnim;
};

