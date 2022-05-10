#include "CameraState.h"

CameraState::CameraState(int cam_speed_multiplier) : cam_speed_multiplier{ cam_speed_multiplier }
{
	recalculate_cam_speed();
}

CameraState::CameraState(Camera2D&& camera, int cam_speed_multiplier) : camera{ camera }, cam_speed_multiplier{ cam_speed_multiplier }
{
	recalculate_cam_speed();
}