#pragma once
#include "InteractionState.h"

#include "Body.h"
#include "PlanetMouseModifier.h"

// State where user is creating a custom planet.
class PlanetCreation : public InteractionState
{
	// The body being modified, and potentially added to the universe.
	Body creating;
	PlanetMouseModifier creating_modifier;

	// Creates a default body for the user to modify.
	Body create_default_body(Vector2 pos) const;

public:

	// Enters the state by creating a planet at the given position,
	// which the user can modify.
	PlanetCreation(Vector2 mouse_pos);

	// Processes all relevant state input for creating a body and returns the next interaction state.
	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	std::string_view get_name() const override;

	// Returns help text specific to the planet creation state.
	std::string_view get_help_text() const override;

	// Renders the current body.
	void render_world(const AdvCamera& camera, const Universe& universe) override;
};

