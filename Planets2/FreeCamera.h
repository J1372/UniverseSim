#pragma once
#include "CameraState.h"

struct CameraList;

class FreeCamera : public CameraState
{
public:

	void init(const AdvCamera& starting_config);

	CameraState* update(const Universe& universe);

	void enter(const AdvCamera& prev_camera);
};
