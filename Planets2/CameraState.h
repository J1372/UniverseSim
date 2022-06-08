#pragma once
#include "AdvCamera.h"
#include <utility>

struct CameraList;
struct Camera2D;
class Universe;
class FreeCamera;
class AnchoredCamera;
class Body;

// Camera mode state machine.
// Handles the policy and keybindings for updating and using a camera.
class CameraState {

protected:

	AdvCamera camera;

public:

	// Camera state implementations
	// Since these are static, they are initialized before Raylib runs its init.
	// This means raylib functions wont work correctly in their constructors.
	// Need to call their inits later, after Raylib has initialized itself and the window.

	static FreeCamera free_camera;
	static AnchoredCamera anchored_camera;

	// Initialization of cameras to be called after Raylib's init.
	static void init_cameras(const AdvCamera& starting_config);

	// Adjusts the camera to focus on the given body.
	virtual void goto_body(Body& body) = 0;

	// Handles camera-related input.
	virtual CameraState* update(const Universe& universe) = 0;

	// Notifies the camera that the screen has been resized
	virtual void notify_resize(int width, int height) = 0;

	// Returns this state's camera.
	const AdvCamera& get_camera() const { return camera; };

	// Returns the underlying raylib Camera2D struct of this state's camera.
	const Camera2D& get_raylib_camera() const { return camera.get_raylib_camera(); };

};
