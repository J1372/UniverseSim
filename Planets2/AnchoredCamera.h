#pragma once
#include "CameraState.h"
#include "BodyObserver.h"

class FreeCamera;
class Body;
struct CameraList;

class AnchoredCamera : public CameraState, public BodyObserver
{
	// If null, will switch to a different camera on update call.
	const Body* anchored_to;

	void snap_camera_to_target();
	FreeCamera* goto_free_camera(CameraList& cameras);

public:

	AnchoredCamera(const AdvCamera& starting_config);
	AnchoredCamera(AdvCamera&& starting_config);

	AnchoredCamera(const AdvCamera& starting_config, const Body& anchor_to);
	AnchoredCamera(AdvCamera&& starting_config, const Body& anchor_to);

	CameraState* update(const Universe& universe, CameraList& cameras) override;

	void notify_body_no_longer_exists(const Body* absorbed_by);

	void enter(const AdvCamera& prev_camera, const Body& anchor_to);
};

