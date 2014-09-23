#pragma once

#include <vector>

class TransformKey;
template <typename T> class Key;

class Path
{
public:
	std::vector<TransformKey*> Keys;
	float Spread;
	float TriangleScale;
	float Delay;
	std::vector<Key<float>*> RibbonWeights;
	bool DontRender;
};

