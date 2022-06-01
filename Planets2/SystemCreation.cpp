#include "SystemCreation.h"

#include "Body.h"
#include "universe.h"
#include "CameraState.h"

void SystemCreation::enter(Vector2 mouse_pos)
{
}

InteractionState* SystemCreation::process_input(const CameraState& camera_state, Universe& universe)
{
    return this;
}

const std::string& SystemCreation::get_help_text() const
{
    return std::string();
}

std::span<const std::unique_ptr<Body>> SystemCreation::get_creating_bodies() const
{
    return {};
}
