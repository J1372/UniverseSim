#pragma once
#include "AdvCamera.h"
#include <utility>

struct Camera2D;
class Universe;
class Body;

// Camera mode state machine.
// Handles the policy and keybindings for updating and using a camera.
class CameraState {

protected:

	AdvCamera camera;

public:

	// Adjusts the camera to focus on the given body.
	virtual void goto_body(Body& body) = 0;

	// Handles camera-related input.
	virtual CameraState* update(Universe& universe) = 0;

	// Notifies the camera that the screen has been resized
	virtual void notify_resize(int width, int height) = 0;

	// Returns this state's camera.
	const AdvCamera& get_camera() const { return camera; };

	// Returns the underlying raylib Camera2D struct of this state's camera.
	const Camera2D& get_raylib_camera() const { return camera.get_raylib_camera(); };

	bool in_view(const Body& body) { return camera.in_view(body); }
	bool in_view(Rectangle rect) { return camera.in_view(rect); }

};
