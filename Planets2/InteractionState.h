#pragma once
#include <string_view>

class CameraState;
class Universe;
class Body;
class AdvCamera;

class InteractionState
{

public:

	// Processes state-specific user input and returns the next interaction state.
	virtual InteractionState* process_input(const CameraState& camera_state, Universe& universe) = 0;

	// Returns the state's title.
	virtual std::string_view get_name() const = 0;

	// Returns state-specific help text.
	virtual std::string_view get_help_text() const = 0;

	// State renders state-specific world-position elements.
	virtual void render_world(const AdvCamera& camera, const Universe& universe)
	{}

	virtual ~InteractionState() = default;

};

