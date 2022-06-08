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
	// Use universe's system generator to get a planetary system at the given position.
    system = std::move(universe.generate_rand_system(mouse_pos.x, mouse_pos.y));
}

InteractionState* SystemCreation::process_input(const CameraState& camera_state, Universe& universe)
{
	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		// Add the current planetary system to the universe and generate a new one.
		universe.add_bodies(system);
		system = universe.generate_rand_system(universe_point.x, universe_point.y);

		return this;
	}
	else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) or IsKeyPressed(KEY_ONE)) { // if anchored and right click, camera will unanchor.
		// Switch to default interaction.
		system.clear();

		DefaultInteraction& ret_state = InteractionState::default_interaction;
		return &ret_state;
	}
	else if (IsKeyPressed(KEY_TWO)) {
		// Switch to planet generation.
		system.clear();

		PlanetCreation& ret_state = InteractionState::planet_interaction;
		ret_state.enter(universe_point);
		return &ret_state;
	}
	else if (IsKeyPressed(KEY_THREE)) {
		// Generate a new system, but don't add the current one to the universe.
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
			Vector2 central_pos = central_body.pos();

			// We are assuming that the central body was previously centered on the mouse.
			Vector2 movement = { universe_point.x - central_pos.x,
										universe_point.y - central_pos.y };

			for (std::unique_ptr<Body>& body : system) {
				body->change_pos(movement);
			}
		}

	}

	return this;
}

std::string SystemCreation::get_name() const
{
	return "System creation mode";
}

std::string SystemCreation::get_help_text() const
{
	return
		"Left click to add to universe and generate another system\n"
		"Right click to go to default mode\n"
		"[Enter] to add to universe and return\n"
		"[1] to go to default mode\n"
		"[2] to go to planet creator\n"
		"[3] to generate another system\n";

}

std::span<const std::unique_ptr<Body>> SystemCreation::get_creating_bodies() const
{
    return system;
}        
