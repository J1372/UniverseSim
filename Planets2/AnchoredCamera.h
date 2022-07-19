#pragma once
#include "CameraState.h"
#include <functional>
#include "Removal.h"
#include <optional>

class FreeCamera;
class Body;
struct CameraList;

// Represents an anchored camera state which keeps the camera centered on a planetary body.
// Allows the user to switch to another planet, and pan the camera around the anchored body.
class AnchoredCamera : public CameraState
{

	// The body to be followed.
	// If nullptr, will switch to a different camera on update call.
	const Body* anchored_to = nullptr;
	
	// The listener id returned when adding camera as an observer to anchored_to's remove event observer list.
	std::optional<int> listener_id;

	// Moves the camera's target position to the center of the anchored body.
	void snap_camera_to_target();

	// Enters a free camera state and returns a pointer to it.
	CameraState* goto_free_camera();

	// Switches the camera to follow a different body.
	void switch_to(const Body* anchor_to);

	// Switches the camera to follow a different body.
	void switch_to(const Body& anchor_to);

public:

	AnchoredCamera() = default;

	// Sets the anchored camera to the provided configuration.
	AnchoredCamera(const AdvCamera& starting_config, Universe& universe);

	// Attaches the camera to the given body.
	void goto_body(Body& body);

	// Snaps camera target to the currently anchored body's center.
	// Processes input related to the camera, and updates and returns next camera state.
	CameraState* update(Universe& universe) override;

	// Enters an anchored camera state using the previous camera information
	// and anchors camera to the given body.
	void enter(const AdvCamera& prev_camera, const Body& anchor_to, Universe& universe);
	void exit(Universe& universe);

	// Notifies the camera that the screen has been resized
	void notify_resize(int width, int height);

};

