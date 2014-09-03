#include "S3DCamera.h"

void S3DCamera::SetWorldMatrix()
{
	Vec3 v_front, eye_target, axis;
	float angle;

	v_front.Set(0, 0, -1);

	eye_target = target - position;
	eye_target.Normalize();

	axis = v_front * eye_target;
	axis.Normalize();

	angle = Vec3::GetAngle(v_front, eye_target);

	glRotatef(angle, axis.x, axis.y, axis.z);
	glTranslatef(position.x, position.y, position.z);
}

void S3DCamera::SetTarget(float x, float y, float z)
{
	target.Set(x, y, z);
	/*Vec3 target, v, minus_check;

	target.Set(x, y, z);
	target = target - pos;
	target.y = 0.0f;
	target.Normalize();

	v.Set(0.0f, 0.0f, -1.0f);
	minus_check.Set(1.0f, 0.0f, 0.0);

	hori = Vec3::Dot(v, target);
	if (hori > 1.0f) hori = 1.0f;
	if (hori < -1.0f) hori = -1.0f;
	hori = deg(acosf(hori));
	if (Vec3::Dot(minus_check, target) < 0.0f) hori = -hori;

	target.Set(x, y, z);
	target = target - pos;
	target.Normalize();
	v = target;

	v.y = 0.0f;
	v.Normalize();
	minus_check.Set(0.0f, -1.0f, 0.0);

	vert = Vec3::Dot(v, target);
	if (vert > 1.0f) vert = 1.0f;
	if (vert < -1.0f) vert = -1.0f;
	vert = deg(acosf(vert));
	if (Vec3::Dot(minus_check, target) < 0.0f) vert = -vert;*/
}

void S3DCamera::SetPosition(float x, float y, float z)
{
	position.Set(x, y, z);
}

void S3DCamera::SetRoll(float roll)
{
	this ->roll = roll;
}