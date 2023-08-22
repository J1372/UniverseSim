#pragma once
#include "InteractionState.h"

// Default interaction.
class DefaultInteraction : public InteractionState
{

public:

	DefaultInteraction() = default;

	// Handles user deleting a body and state transitions.
	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	std::string get_name() const override { return "Default interaction mode"; }

	// Returns default help text.
	std::string get_help_text() const override;

	std::span<const Body> get_creating_bodies() const override;

};

