#pragma once

#include <Math\Vec3.h>
#include <Math\Vec4.h>

class TransformKey
{
public:
	float Time;
	sm::Vec3 Position;
	sm::Vec4 Rotation;
	sm::Vec3 Scale;
};
