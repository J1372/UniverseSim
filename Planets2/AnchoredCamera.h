#pragma once
#include "CameraState.h"
#include "BodyObserver.h"

class Body;

class AnchoredCamera : public CameraState, public BodyObserver
{
	// Should be non-null, but can switch to a different body.
	const Body* anchored_to;

public:
	
	AnchoredCamera(Camera2D&& camera, int cam_speed_multiplier, const Body& anchor_to);

	CameraState* update(const Universe& universe) override;

	void resize(float width, float height);

	void notify_body_no_longer_exists(const Body* absorbed_by);
};

