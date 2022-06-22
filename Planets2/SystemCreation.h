#pragma once
#include "InteractionState.h"

struct Vector2;

// Interaction state in the simulation where the user is in the process of generating a planetary system.
class SystemCreation : public InteractionState
{

	// The currently generated system.
	std::vector<std::unique_ptr<Body>> system;

public:

	// Enters system generation state by generating a system at the current mouse position.
	void enter(Vector2 mouse_pos, Universe& universe);

	// Handles user input related to generating a system.
	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	std::string get_name() const override;

	// Returns help text specific to using the system generator.
	std::string get_help_text() const override;

	// Returns the current planetary system that has been generated.
	std::span<const std::unique_ptr<Body>> get_creating_bodies() const override;

};

