#pragma once
#include "FreeCamera.h"
#include "AnchoredCamera.h"

struct CameraList
{
	FreeCamera free_camera;
	AnchoredCamera anchored_camera;

	CameraList(const AdvCamera& starting_config);

};
