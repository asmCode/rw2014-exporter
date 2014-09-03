#pragma once

#include <Math\Vec3.h>
#include <Math\Vec2.h>

class Scene3DVertex
{
public:
	sm::Vec3 position;
	sm::Vec2 coords1;
	sm::Vec2 coords2;
	sm::Vec2 coords3;
	sm::Vec3 normal;
	sm::Vec3 tangent;
};
