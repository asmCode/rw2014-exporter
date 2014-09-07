#pragma once

#include <Math\Vec3.h>
#include <Math\Vec2.h>

class Scene3DVertex
{
public:
	sm::Vec3 position;
	uint8_t boneIndex[4];
	float weight[4];
};
