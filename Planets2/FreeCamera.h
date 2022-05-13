#pragma once
#include "CameraState.h"

struct CameraList;

class FreeCamera : public CameraState
{
public:

	FreeCamera(const AdvCamera& starting_config);
	FreeCamera(AdvCamera&& starting_config);

	CameraState* update(const Universe& universe, CameraList& cameras) override;

	void enter(const AdvCamera& prev_camera);

};
