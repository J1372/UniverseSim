#include "SystemCreation.h"

#include "Body.h"
#include "Universe.h"
#include "CameraState.h"

#include "DefaultInteraction.h"
#include "PlanetCreation.h"
#include "RenderUtil.h"
#include "DebugInfo.h"

void SystemCreation::add_system_to_universe(Universe& universe)
{
	// Convert the relative velocities of all satellites to absolute velocities.
	Vector2 star_vel = system[0].vel();
	for (auto it = system.begin() + 1; it != system.end(); ++it)
	{
		it->change_vel(star_vel);
	}

	universe.add_bodies(std::move(system));
}

SystemCreation::SystemCreation(Vector2 mouse_pos, Universe& universe)
	: system(universe.generate_rand_system(mouse_pos.x, mouse_pos.y))
{}

InteractionState* SystemCreation::process_input(const CameraState& camera_state, Universe& universe)
{
	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

	if (user_clicked)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			// Mouse is being dragged
			// User is altering star's velocity.

			Vector2 movement = GetMouseDelta();

			// Increase velocity in opposite direction of the mouse drag.
			constexpr float scale_down = 10.0f;
			system[0].change_vel({ -movement.x / scale_down , -movement.y / scale_down });
		}
		else
		{
			// User not dragging -> has finished velocity customization.
			add_system_to_universe(universe);
			system = universe.generate_rand_system(universe_point.x, universe_point.y);

			// System added and new one generated = click processed.
			// Reset click state.
			user_clicked = false;
		}
	}
	else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		// Set to process user click.
		// User may be clicking to add to universe or dragging mouse to customize velocity.
		user_clicked = true;
	}
	else
	{
		// Snap the system position to mouse position.

		Body& central_body = system[0];
		Vector2 central_pos = central_body.pos();

		// Check if mouse is still centered on the system's central body.
		if (universe_point.x != central_pos.x or universe_point.y != central_pos.y)
		{
			// Mouse's position in universe has changed, so update the system.
			// Keep central body of new system centered on the mouse.
			// By extension, keep entire system centered on the mouse.


			// The central body was previously centered on the mouse.
			Vector2 movement = { universe_point.x - central_pos.x,
										universe_point.y - central_pos.y };

			// Update system positions to be centered around the mouse.
			for (Body& body : system)
			{
				body.change_pos(movement);
			}
		}
	}

	// Keys - mostly state transitions.

	if (IsKeyPressed(KEY_ENTER))
	{
		// Add the current planetary system to the universe and goto default interaction.
		add_system_to_universe(universe);
		return new DefaultInteraction;
	}
	else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) or IsKeyPressed(KEY_ONE))
	{
		// Side effect - if anchored and right click, camera will unanchor.
		return new DefaultInteraction;
	}
	else if (IsKeyPressed(KEY_TWO))
	{
		// Switch to planet generation.
		return new PlanetCreation { universe_point };
	}
	else if (IsKeyPressed(KEY_THREE))
	{
		// Generate a new system, but don't add the current one to the universe.
		system = universe.generate_rand_system(universe_point.x, universe_point.y);
		user_clicked = false;
		return this;
	}

	return this;
}

std::string_view SystemCreation::get_name() const
{
	return "System creation";
}

std::string_view SystemCreation::get_help_text() const
{
	return
		"Left click to add to universe\n"
		"Right click to go to default mode\n"
		"[Enter] to add to universe and return\n"
		"[1] to go to default mode\n"
		"[2] to go to planet creator\n"
		"[3] to generate another system\n";

}

void SystemCreation::render_world(const AdvCamera& camera, const Universe& universe)
{
	for (const Body& body : system)
	{
		if (camera.in_view(body))
		{
			RenderUtil::render_body(body);

			Vector2 pos = body.pos();
			Vector2 vel = body.vel();
			DebugInfo info{ "Pos(x): " + std::to_string(pos.x) };

			info.add("Pos(y): " + std::to_string(pos.y));
			info.add("Vel(x): " + std::to_string(vel.x));
			info.add("Vel(y): " + std::to_string(vel.y));
			info.add("Mass: " + std::to_string(body.get_mass()));

			RenderUtil::render_near_body(body, info.c_str());
			RenderUtil::render_velocity(body);
		}
	}
}
