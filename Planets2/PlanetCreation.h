#pragma once
#include "InteractionState.h"

class PlanetCreation : public InteractionState
{

public:

	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	const std::string& get_name() const override { return "Planet creation mode"; }

	const std::string& get_help_text() const override;
	const std::vector<std::unique_ptr<Body>>& get_creating_bodies() const override;
};

