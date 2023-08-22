#include "SystemCreation.h"

#include "Body.h"
#include "Universe.h"
#include "CameraState.h"

#include "DefaultInteraction.h"
#include "PlanetCreation.h"

SystemCreation::SystemCreation(Vector2 mouse_pos, Universe& universe)
	: system(universe.generate_rand_system(mouse_pos.x, mouse_pos.y))
{}

InteractionState* SystemCreation::process_input(const CameraState& camera_state, Universe& universe)
{
	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		// Add the current planetary system to the universe and generate a new one.
		universe.add_bodies(std::move(system));
		system = universe.generate_rand_system(universe_point.x, universe_point.y);

		return this;
	}
	else if (IsKeyPressed(KEY_ENTER)) {
		// Add the current planetary system to the universe and goto default interaction.
		universe.add_bodies(std::move(system));
		return new DefaultInteraction;
	}
	else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) or IsKeyPressed(KEY_ONE)) {
		// Side effect - if anchored and right click, camera will unanchor.
		return new DefaultInteraction;
	}
	else if (IsKeyPressed(KEY_TWO)) {
		// Switch to planet generation.
		return new PlanetCreation { universe_point };
	}
	else if (IsKeyPressed(KEY_THREE)) {
		// Generate a new system, but don't add the current one to the universe.
		system = universe.generate_rand_system(universe_point.x, universe_point.y);
		return this;
	}
	else {
		// Potentially snap the system position to mouse position.

		Body& central_body = system[0];
		Vector2 central_pos = central_body.pos();

		// Check if mouse is still centered on the system's central body.
		if (universe_point.x != central_pos.x or universe_point.y != central_pos.y) {
			// Mouse's position in universe has changed, so update the system.
			// Keep central body of new system centered on the mouse.
			// By extension, keep entire system centered on the mouse.
			

			// The central body was previously centered on the mouse.
			Vector2 movement = { universe_point.x - central_pos.x,
										universe_point.y - central_pos.y };

			// Update system positions to be centered around the mouse.
			for (Body& body : system) {
				body.change_pos(movement);
			}
		}

	}

	return this;
}

std::string SystemCreation::get_name() const
{
	return "System creation";
}

std::string SystemCreation::get_help_text() const
{
	return
		"Left click to add to universe\n"
		"Right click to go to default mode\n"
		"[Enter] to add to universe and return\n"
		"[1] to go to default mode\n"
		"[2] to go to planet creator\n"
		"[3] to generate another system\n";

}

std::span<const Body> SystemCreation::get_creating_bodies() const
{
    return system;
}        
