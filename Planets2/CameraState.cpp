#include "CameraState.h"

void CameraState::resize(float width, float height)
{
}

CameraState::CameraState(int cam_speed_multiplier) : cam_speed_multiplier{ cam_speed_multiplier }
{
	recalculate_cam_speed();
}

void CameraState::increase_cam_speed()
{
	cam_speed_multiplier++;
	recalculate_cam_speed();
}

void CameraState::decrease_cam_speed() 
{
	cam_speed_multiplier--;
	recalculate_cam_speed();
}

CameraState::CameraState(Camera2D&& camera, int cam_speed_multiplier) : camera{ camera }, cam_speed_multiplier{ cam_speed_multiplier }
{
	recalculate_cam_speed();
}