#pragma once

#include <vector>

class TransformKey;

class Path
{
public:
	std::vector<TransformKey*> Keys;
	float Spread;
	float TriangleScale;
	float Delay;
};

