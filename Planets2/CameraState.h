#pragma once
#include "AdvCamera.h"
#include <utility>

struct CameraList;
struct Camera2D;
class Universe;
class FreeCamera;
class AnchoredCamera;

// Camera mode state machine.
// Handles the policy and keybindings for updating and using a camera.
class CameraState {
public:

	// Camera state implementations
	// Since these are static, they are initialized before Raylib runs its init.
	// This means raylib functions wont work correctly in their constructors.
	// Need to call their inits later, after Raylib has initialized itself and the window.

	static FreeCamera free_camera;
	static AnchoredCamera anchored_camera;

	static void init_cameras(const AdvCamera& starting_config);

	virtual CameraState* update(const Universe& universe) = 0;

	// Returns this state's camera.
	virtual const AdvCamera& get_camera() const = 0;

	// Returns the underlying raylib Camera2D struct of this state's camera.
	virtual const Camera2D& get_raylib_camera() const = 0;

};
