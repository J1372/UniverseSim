#pragma once
#include "InteractionState.h"

class DefaultInteraction : public InteractionState
{

public:

	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	std::string get_name() const override { return "Default interaction mode"; }

	std::string get_help_text() const override;
	std::span<const std::unique_ptr<Body>> get_creating_bodies() const override;

};

