#pragma once
#include "InteractionState.h"

// not sure why I need to include Body in header for a unique_ptr, but I get a compiler error if I don't.
#include "Body.h"

class PlanetCreation : public InteractionState
{
	// When dragging mouse, determines whether to change the creation body's mass or velocity.
	enum class Modifying {
		NONE,
		MASS,
		VELOCITY
	};

	Modifying modify_mode = Modifying::NONE;

	std::unique_ptr<Body> creating;

public:

	void init();

	void enter(Vector2 mouse_pos);

	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	std::string get_name() const override { return "Planet creation mode"; }

	std::string get_help_text() const override;
	std::span<const std::unique_ptr<Body>> get_creating_bodies() const override;

};

