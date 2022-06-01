#include "SystemCreation.h"

#include "Body.h"
#include "universe.h"
#include "CameraState.h"

#include "DefaultInteraction.h"
#include "PlanetCreation.h"


void SystemCreation::init()
{
	system.clear();
}


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
	else {
		// Currently doesn't follow mouse after zoom. But will snap to mouse after mouse moves.
		// Later, can just use screen position and translate to universe coords before we add to universe.
		Vector2 movement = GetMouseDelta();

		// if mouse has moved
		if (movement.x != 0 or movement.y != 0) {
			// Keep central body of new system centered on the mouse.
			// By extension, keep entire system centered on the mouse.
			Body& central_body = *system[0];

			// We are assuming that the central body was previously centered on the mouse.
			Vector2 movement = { universe_point.x - central_body.x,
										universe_point.y - central_body.y };

			for (std::unique_ptr<Body>& body : system) {
				body->x += movement.x;
				body->y += movement.y;
			}
		}

	}

	return this;
}

std::string SystemCreation::get_help_text() const
{
	std::string help_text;

	help_text += "Left click to add to universe and generate another system\n";
	help_text += "Right click to go to default mode\n";
	help_text += "[Enter] to add to universe and return to default mode\n";
	help_text += "[1] to go to default mode\n";
	help_text += "[2] to go to planet creator\n";
	help_text += "[3] to generate another system\n";

	return help_text;
}

std::span<const std::unique_ptr<Body>> SystemCreation::get_creating_bodies() const
{
    return system;
}
