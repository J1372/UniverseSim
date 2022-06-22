#pragma once
#include "CameraState.h"

struct CameraList;
class Body;

// This represents a freely moving camera that the user can move.
class FreeCamera : public CameraState
{
	
	// Changes camera offset so that the target is in the center of the screen.
	void center_camera();

public:

	FreeCamera() = default;

	// Sets the free camera to the provided configuration.
	FreeCamera(const AdvCamera& starting_config);

	// Moves the camera's center to the given body.
	void goto_body(Body& body);

	// Processes input related to the camera, and updates and returns next camera state.
	CameraState* update(const Universe& universe);

	// Enters a free camera state using the previous camera information to initialize.
	void enter(const AdvCamera& prev_camera);

	// Notifies the camera that the screen has been resized
	void notify_resize(int width, int height);

};
