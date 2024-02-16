#pragma once
#include "CameraState.h"
#include <functional>
#include "Removal.h"
#include "Event.h"

class FreeCamera;
class Body;
struct CameraList;

// Represents an anchored camera state which keeps the camera centered on a planetary body.
// Allows the user to switch to another planet, and pan the camera around the anchored body.
class AnchoredCamera : public CameraState
{
	// The body to be followed.
	// If nullptr, will switch to a different camera on update call.
	int anchored_to = -1;
	
	// The listener returned when adding camera as an observer to anchored_to's remove event observer list.
	EventHandle<Removal> listener;

	// Moves the camera's target position to the center of the anchored body.
	void snap_camera_to_target(const Body& target);

	// Given an id, switches the camera to follow the body whose id matches.
	void switch_to(int anchor_to);

	// Unanchors the camera;
	void unanchor();

	// Returns whether the canera is still anchored to a body.
	bool is_anchored() const;

public:

	// Sets the anchored camera to the provided configuration.
	AnchoredCamera(const AdvCamera& starting_config, const Body& anchor_to, Universe& universe);

	// Adjusts the camera to focus on the given body.
	void goto_body(Body& body);

	// Snaps camera target to the currently anchored body's center.
	// Processes input related to the camera, and updates and returns next camera state.
	CameraState* update(Universe& universe) override;

	// Notifies the camera that the screen has been resized
	void notify_resize(int width, int height);

};

