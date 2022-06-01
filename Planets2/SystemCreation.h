#pragma once
#include "InteractionState.h"

struct Vector2;

class SystemCreation : public InteractionState
{

	std::vector<std::unique_ptr<Body>> system;

public:

	void enter(Vector2 mouse_pos, Universe& universe);

	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	const std::string& get_name() const override { return "System creation mode"; }

	const std::string& get_help_text() const override;
	std::span<const std::unique_ptr<Body>> get_creating_bodies() const override;

};

