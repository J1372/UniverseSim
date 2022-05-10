#pragma once
#include "CameraState.h"

class FreeCamera : public CameraState
{
public:

	FreeCamera(float screen_width, float screen_height, int cam_speed_multiplier);
	FreeCamera(Camera2D&& camera, int cam_speed_multiplier);
	CameraState* update(const Universe& universe) override;

	void resize(float width, float height);

};
