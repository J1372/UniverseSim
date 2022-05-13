#pragma once
#include "AdvCamera.h"
#include <utility>

struct CameraList;
struct Camera2D;
class Universe;

// Handles the policy and keybindings for updating and using a camera.
class CameraState {
protected:

	AdvCamera camera;

public:

	CameraState(const AdvCamera& starting_config) : camera(starting_config)
	{}

	CameraState(AdvCamera&& starting_config) : camera(std::move(starting_config))
	{}

	virtual CameraState* update(const Universe& universe, CameraList& cameras) = 0;

	// Returns this state's camera.
	const AdvCamera& get_camera() const { return camera; }

	// Returns the underlying raylib Camera2D struct of this state's camera.
	const Camera2D& get_raylib_camera() const { return camera.get_raylib_camera(); }

};
