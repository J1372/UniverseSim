#pragma once
#include "raylib.h"

class Universe;

class CameraState {
protected:

	CameraState* ret_state = this;

	Camera2D camera;
	int cam_speed_multiplier;
	float cam_speed;


	void zoom_in() { camera.zoom *= 2; }

	void zoom_out() { camera.zoom /= 2; }

	void recalculate_cam_speed() { cam_speed = 5 * cam_speed_multiplier / camera.zoom; }

public:

	CameraState(int cam_speed_multiplier);

	CameraState(Camera2D&& camera, int cam_speed_multiplier);

	virtual CameraState* update(const Universe& universe) = 0;
	virtual void resize(float width, float height) = 0;
	Camera2D& get_camera() { return camera; }



};