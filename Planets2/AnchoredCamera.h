#pragma once
#include "CameraState.h"
#include <functional>
#include "Removal.h"
#include <optional>

class FreeCamera;
class Body;
struct CameraList;

class AnchoredCamera : public CameraState
{

	AdvCamera camera;

	// If null, will switch to a different camera on update call.
	Body* anchored_to = nullptr;
	std::function<void(Removal)> on_body_removal = [this](Removal remove_event) { this->switch_to(remove_event.absorbed_by); };
	int listener_id = 0;

	void snap_camera_to_target();
	CameraState* goto_free_camera();

	void switch_to(Body* anchor_to);
	void switch_to(Body& anchor_to);

public:

	void init(const AdvCamera& starting_config);

	CameraState* update(const Universe& universe) override;

	void enter(const AdvCamera& prev_camera, Body& anchor_to);

	// Returns this state's camera.
	const AdvCamera& get_camera() const;

	// Returns the underlying raylib Camera2D struct of this state's camera.
	const Camera2D& get_raylib_camera() const;

};

