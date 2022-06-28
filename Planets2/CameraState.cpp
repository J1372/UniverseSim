#include "CameraState.h"

#include "FreeCamera.h"
#include "AnchoredCamera.h"
#include "AdvCamera.h"

FreeCamera CameraState::free_camera;
AnchoredCamera CameraState::anchored_camera;

void CameraState::init_cameras(const AdvCamera& starting_config, Universe& universe)
{
	// Constructs cameras to be set to the provided camera configuration.
	// Needs to be done since they are static,
	// but rely on accessing Raylib's screen dimensions functions.
	free_camera = { starting_config };

	anchored_camera.exit(universe);
	anchored_camera = AnchoredCamera{ starting_config, universe};
}
