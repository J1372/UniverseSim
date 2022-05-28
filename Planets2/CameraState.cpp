#include "CameraState.h"

#include "FreeCamera.h"
#include "AnchoredCamera.h"
#include "AdvCamera.h"

FreeCamera CameraState::free_camera;
AnchoredCamera CameraState::anchored_camera;

void CameraState::init_cameras(const AdvCamera& starting_config)
{
	free_camera.init(starting_config);
	anchored_camera.init(starting_config);
}
