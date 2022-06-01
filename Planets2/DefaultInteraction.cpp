#include "DefaultInteraction.h"

#include "Body.h"
#include "universe.h"
#include "CameraState.h"

InteractionState* DefaultInteraction::process_input(const CameraState& camera_state, Universe& universe)
{
}

const std::string& DefaultInteraction::get_help_text() const
{
	return std::string();
}

const std::vector<std::unique_ptr<Body>>& DefaultInteraction::get_creating_bodies() const
{
	return {};
}
