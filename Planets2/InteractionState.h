#pragma once
#include <string>
#include <vector>
#include <memory>
#include <span>

class CameraState;
class Universe;
class Body;

class InteractionState
{

public:

	// Processes state-specific user input and returns the next interaction state.
	virtual InteractionState* process_input(const CameraState& camera_state, Universe& universe) = 0;

	// Returns the state's title.
	virtual std::string_view get_name() const = 0;

	// Returns state-specific help text.
	virtual std::string_view get_help_text() const = 0;

	// Returns a collection of bodies that are being created by the user.
	// Not all states use this, so it's probably better to append render commands to the Sim scene instead in 
	// process_input method.
	virtual std::span<const Body> get_creating_bodies() const = 0;

	virtual ~InteractionState() = default;

};

