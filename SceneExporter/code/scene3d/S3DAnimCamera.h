#pragma once

#include <Vec3.h>

#include "S3DAnim.h"
#include "S3DCamera.h"

class S3DAnimCamera : S3DCamera
{
public:
	void SetCameraOnTime(float time);

	void SetCameraAnim(S3DAnim *anim);
	void SetTargetAnim(S3DAnim *anim);

private:
	S3DAnim *cam_anim;
	S3DAnim *trg_anim;
};
