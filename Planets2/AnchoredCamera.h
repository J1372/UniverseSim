#pragma once
#include "CameraState.h"
#include <functional>
#include "Removal.h"

class FreeCamera;
class Body;
struct CameraList;

class AnchoredCamera : public CameraState
{
	// If null, will switch to a different camera on update call.
	Body* anchored_to;
	std::function<void(Removal)> on_body_removal = [this](Removal remove_event) { this->switch_to(remove_event.absorbed_by); };

	void snap_camera_to_target();
	FreeCamera* goto_free_camera(CameraList& cameras);

	void switch_to(Body* anchor_to);
	void switch_to(Body& anchor_to);

public:

	AnchoredCamera(const AdvCamera& starting_config);
	AnchoredCamera(AdvCamera&& starting_config);

	AnchoredCamera(const AdvCamera& starting_config, Body& anchor_to);
	AnchoredCamera(AdvCamera&& starting_config, Body& anchor_to);

	CameraState* update(const Universe& universe, CameraList& cameras) override;

	void enter(const AdvCamera& prev_camera, Body& anchor_to);
};

