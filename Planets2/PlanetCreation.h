#pragma once
#include "InteractionState.h"

// not sure why I need to include Body in header for a unique_ptr, but I get a compiler error if I don't.
#include "Body.h"

// State where user is creating a custom planet.
class PlanetCreation : public InteractionState
{
	// When dragging mouse, determines whether to change the creation body's mass or velocity.
	enum class Modifying {
		NONE,
		MASS,
		VELOCITY
	};

	// The current modification being done by a drag.
	Modifying modify_mode = Modifying::NONE;

	// The body being modified, and potentially added to the universe.
	std::unique_ptr<Body> creating;

	// Creates a default body for the user to modify.
	std::unique_ptr<Body> create_default_body(Vector2 pos) const;

public:

	// Initialization to be called after Raylib's init.
	void init();

	// Enters the state by creating a planet at the given position,
	// which the user can modify.
	void enter(Vector2 mouse_pos);

	// Processes all relevant state input for creating a body and returns the next interaction state.
	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;


	std::string get_name() const override;

	// Returns help text specific to the planet creation state.
	std::string get_help_text() const override;

	// Returns the body being created.
	std::span<const std::unique_ptr<Body>> get_creating_bodies() const override;

};

