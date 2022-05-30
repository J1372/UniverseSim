#pragma once
#include "CameraState.h"

struct CameraList;
class Body;

class FreeCamera : public CameraState
{
public:

	void init(const AdvCamera& starting_config);

	void goto_body(Body& body);

	CameraState* update(const Universe& universe);

	void enter(const AdvCamera& prev_camera);
};
