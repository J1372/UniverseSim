#pragma once
#include "CameraState.h"

struct CameraList;

class FreeCamera : public CameraState
{

	AdvCamera camera;

public:

	void init(const AdvCamera& starting_config);

	CameraState* update(const Universe& universe);

	void enter(const AdvCamera& prev_camera);

	// Returns this state's camera.
	const AdvCamera& get_camera() const;

	// Returns the underlying raylib Camera2D struct of this state's camera.
	const Camera2D& get_raylib_camera() const;
};
