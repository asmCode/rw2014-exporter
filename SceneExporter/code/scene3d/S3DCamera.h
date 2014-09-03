#pragma once

#include <windows.h>
#include <gl\gl.h>
#include <Vec3.h>

class S3DCamera
{
public:
	void SetWorldMatrix();

	void SetPosition(float x, float y, float z);
	void SetTarget(float x, float y, float z);
	void SetRoll(float roll);

protected:
	Vec3 position;
	Vec3 target;
	float roll;
};
