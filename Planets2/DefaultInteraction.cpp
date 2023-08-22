#include "DefaultInteraction.h"

#include "Body.h"
#include "Universe.h"
#include "CameraState.h"

#include "PlanetCreation.h"
#include "SystemCreation.h"

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
			return new PlanetCreation{ universe_point };
		}
	}
	else if (IsKeyPressed(KEY_TWO)) {
		// Start user body creation.
		Vector2 screen_point = GetMousePosition();
		Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

		return new PlanetCreation{ universe_point };
	}
	else if (IsKeyPressed(KEY_THREE)) {
		// Start system generation.
		Vector2 screen_point = GetMousePosition();
		Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

		return new SystemCreation{ universe_point, universe };
	}

	return this;
}

std::string DefaultInteraction::get_help_text() const
{
	return
		"[B] to show partitioning representation\n"
		"[N] to show debug text info\n"
		"[M] to show tick # and collision stats\n"
		"[F] to show show force directions\n"
		"[V] to show show velocity directions\n"
		"[SPACE] to toggle pause\n"
		"[W] [A] [S] [D] to move the camera\n"
		"[-, +] to control the camera's speed\n"
		"[COMMA] or scroll down to zoom out\n"
		"[PERIOD] or scroll up to zoom in\n"
		"[ESCAPE] to go back to settings\n"
		"Right click to anchor camera to a planet\n"
		"Ctrl click to delete planet\n"
		"Shift click to go to planet creator\n"
		"[2] to go to planet creator\n"
		"[3] to go to system generator\n";
}

std::span<const Body> DefaultInteraction::get_creating_bodies() const
{
	return {};
}
