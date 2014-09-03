#include "S3DAnimCamera.h"

void S3DAnimCamera::SetCameraOnTime(float time)
{
/*	if ((cam_anim != NULL) && cam_anim ->HavePosition())
		cam_anim ->GetPosition(time, position);

	if ((trg_anim != NULL) && trg_anim ->HavePosition())
		cam_anim ->GetPosition(time, target);

	if ((cam_anim != NULL) && cam_anim ->HaveScalar())
		cam_anim ->GetScalar(time, roll);*/
}

void S3DAnimCamera::SetCameraAnim(S3DAnim *anim)
{
	this ->cam_anim = anim;
}

void S3DAnimCamera::SetTargetAnim(S3DAnim *anim)
{
	this ->trg_anim = anim;
}