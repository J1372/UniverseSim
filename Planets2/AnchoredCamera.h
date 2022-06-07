#pragma once
#include "CameraState.h"
#include <functional>
#include "Removal.h"

class FreeCamera;
class Body;
struct CameraList;

// Represents an anchored camera state which keeps the camera centered on a planetary body.
// Allows the user to switch to another planet, and pan the camera around the anchored body.
class AnchoredCamera : public CameraState
{

	// The body to be followed.
	// If nullptr, will switch to a different camera on update call.
	Body* anchored_to = nullptr;

	// If anchored_to is set to be removed, switch the camera's anchored body to the body that removed it.
	std::function<void(Removal)> on_body_removal = [this](Removal remove_event) { this->switch_to(remove_event.absorbed_by); };
	
	// The listener id returned when adding camera as an observer to anchored_to's remove event observer list.
	int listener_id = 0;

	// Moves the camera's target position to the center of the anchored body.
	void snap_camera_to_target();

	// Enters a free camera state and returns a pointer to it.
	CameraState* goto_free_camera();

	// Switches the camera to follow a different body.
	void switch_to(Body* anchor_to);

	// Switches the camera to follow a different body.
	void switch_to(Body& anchor_to);

public:

	// Initialization to be called after Raylib's init.
	void init(const AdvCamera& starting_config);

	// Attaches the camera to the given body.
	void goto_body(Body& body);

	// Snaps camera target to the currently anchored body's center.
	// Processes input related to the camera, and updates and returns next camera state.
	CameraState* update(const Universe& universe) override;

	// Enters an anchored camera state using the previous camera information
	// and anchors camera to the given body.
	void enter(const AdvCamera& prev_camera, Body& anchor_to);

};

