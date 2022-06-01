#include "PlanetCreation.h"

#include "Body.h"
#include "universe.h"
#include "CameraState.h"

InteractionState* PlanetCreation::process_input(const CameraState& camera_state, Universe& universe)
{
}

const std::string& PlanetCreation::get_help_text() const
{
    return std::string();
}

const std::vector<std::unique_ptr<Body>>& PlanetCreation::get_creating_bodies() const
{
    return {};
}
