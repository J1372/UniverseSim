#include "SystemCreation.h"

#include "Body.h"
#include "universe.h"
#include "CameraState.h"

#include "DefaultInteraction.h"
#include "PlanetCreation.h"


void SystemCreation::enter(Vector2 mouse_pos, Universe& universe)
{
    system = std::move(universe.generate_rand_system(mouse_pos.x, mouse_pos.y));
}

InteractionState* SystemCreation::process_input(const CameraState& camera_state, Universe& universe)
{
	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		universe.add_bodies(system);
		system = universe.generate_rand_system(universe_point.x, universe_point.y);

		return this;
	}
	else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) or IsKeyPressed(KEY_ONE)) { // if anchored and right click, camera will unanchor.
		system.clear();

		DefaultInteraction& ret_state = InteractionState::default_interaction;
		return &ret_state;
	}
	else if (IsKeyPressed(KEY_TWO)) {
		system.clear();

		PlanetCreation& ret_state = InteractionState::planet_interaction;
		ret_state.enter(universe_point);
		return &ret_state;
	}
	else if (IsKeyPressed(KEY_THREE)) {
		system.clear();
		system = universe.generate_rand_system(universe_point.x, universe_point.y);

		return this;
	}

	return this;
}

const std::string& SystemCreation::get_help_text() const
{
    return std::string();
}

std::span<const std::unique_ptr<Body>> SystemCreation::get_creating_bodies() const
{
    return system;
}
