#pragma once
#include "InteractionState.h"
#include <vector>

struct Vector2;

// Interaction state in the simulation where the user is in the process of generating a planetary system.
class SystemCreation : public InteractionState
{

	// The currently generated system.
	std::vector<Body> system;

	bool user_clicked = false;

	// Converts satellite velocities to absolute velocities.
	// Then adds all bodies in the system to the universe.
	void add_system_to_universe(Universe& universe);

public:

	// Enters system generation state by generating a system at the current mouse position.
	SystemCreation(Vector2 mouse_pos, Universe& universe);

	// Handles user input related to generating a system.
	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	std::string_view get_name() const override;

	// Returns help text specific to using the system generator.
	std::string_view get_help_text() const override;

	// Renders the currently generated system.
	void render_world(const AdvCamera& camera_state, const Universe& universe) override;

};

