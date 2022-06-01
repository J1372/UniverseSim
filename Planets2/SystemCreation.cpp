#include "SystemCreation.h"

#include "Body.h"
#include "universe.h"
#include "CameraState.h"

InteractionState* SystemCreation::process_input(const CameraState& camera_state, Universe& universe)
{
}

const std::string& SystemCreation::get_help_text() const
{
    return std::string();
}

const std::vector<std::unique_ptr<Body>>& SystemCreation::get_creating_bodies() const
{
    return {};
}
