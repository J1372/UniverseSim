#include "DefaultInteraction.h"

#include "Body.h"
#include "universe.h"
#include "CameraState.h"

#include "PlanetCreation.h"
#include "SystemCreation.h"


void DefaultInteraction::init()
{

}


InteractionState* DefaultInteraction::process_input(const CameraState& camera_state, Universe& universe)
{
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 screen_point = GetMousePosition();
		Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

		if (IsKeyDown(KEY_LEFT_CONTROL)) {
			// Ctrl-click = try to delete body.

			Body* body = universe.get_body(universe_point);

			if (body) {
				universe.rem_body(*body);
			}

		}
		else if (IsKeyDown(KEY_LEFT_SHIFT)) {
			// Shift-click = start user body creation.
			PlanetCreation& ret_state = InteractionState::planet_interaction;
			ret_state.enter(universe_point);
			return &ret_state;
		}
	}
	else if (IsKeyPressed(KEY_TWO)) {
		// Start user body creation.
		Vector2 screen_point = GetMousePosition();
		Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

		PlanetCreation& ret_state = InteractionState::planet_interaction;
		ret_state.enter(universe_point);
		return &ret_state;
	}
	else if (IsKeyPressed(KEY_THREE)) {
		// Start system generation.
		Vector2 screen_point = GetMousePosition();
		Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

		SystemCreation& ret_state = InteractionState::system_interaction;
		ret_state.enter(universe_point, universe);
		return &ret_state;
	}

	return this;
}

std::string DefaultInteraction::get_help_text() const
{
	return
		"[V] to show partitioning representation\n"
		"[B] to show debug text info\n"
		"[N] to show tick # and collision stats\n"
		"[SPACE] to toggle pause\n"
		"[W] [A] [S] [D] to move the camera\n"
		"[-, +] to control the camera's speed\n"
		"[COMMA] or scroll down to zoom out\n"
		"[PERIOD] or scroll up to zoom in\n"
		"[ESCAPE] to go back to settings\n"
		"Right click to anchor camera to a planet\n"
		"Shift+click to go to create planet\n"
		"[2] to go to planet creator\n"
		"[3] to go to system generator\n";
}

std::span<const std::unique_ptr<Body>> DefaultInteraction::get_creating_bodies() const
{
	return {};
}
