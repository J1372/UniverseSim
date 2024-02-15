#include "PlanetCreation.h"

#include "Universe.h"
#include "CameraState.h"
#include "Physics.h"

#include "SystemCreation.h"
#include "DefaultInteraction.h"
#include "RenderUtil.h"
#include "DebugInfo.h"

Body PlanetCreation::create_default_body(Vector2 pos) const
{
	long mass = 200;
	return { pos.x, pos.y, mass };
}

PlanetCreation::PlanetCreation(Vector2 mouse_pos) // universe point of mouse param
	: creating(create_default_body(mouse_pos))
{}

InteractionState* PlanetCreation::process_input(const CameraState& camera_state, Universe& universe)
{
	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

		if (IsKeyDown(KEY_LEFT_CONTROL)) {
			// Ctrl-click = try to delete body.

			Body* body = universe.get_body(universe_point);

			if (body) {
				universe.rem_body(*body);
			}
		}
		else if (IsKeyDown(KEY_LEFT_SHIFT)) {
			// Shift-click = add current creation, start new user body creation.
			universe.add_body(std::move(creating));
			creating = create_default_body(universe_point);
		}
		else {
			Vector2 creating_pos = creating.pos();
			// select modifying mode based on where user clicked.
			float inner_radius = .6 * creating.get_radius();
			if (Physics::point_in_circle(universe_point, creating_pos.x, creating_pos.y, inner_radius)) {
				// Clicked on the inside section of the body.
				modify_mode = Modifying::VELOCITY;

			}
			else if (creating.contains_point(universe_point)) {
				// Clicked on the outside section of the body.
				modify_mode = Modifying::MASS;

			}
			else {
				// User did not click on the body they are creating.
				modify_mode = Modifying::NONE;
			}
		}
	}
	else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		// Mouse is being dragged

		Vector2 movement = GetMouseDelta();
		// Can compare difference universe point (0,0) to (movement.x, movement.y)
		// So zooming out would increase rate of change of i.e mass.
		// Currently flat amount based on movement on screen.

		if (modify_mode == Modifying::MASS) {
			// User is altering body's mass.
			Vector2 creating_pos = creating.pos();

			// This determines whether the user dragged the mouse towards the body.
			float towards_body_x = creating_pos.x < universe_point.x ? -movement.x : movement.x;
			float towards_body_y = creating_pos.y < universe_point.y ? -movement.y : movement.y;
			float towards_body_delta = towards_body_x + towards_body_y;

			// Increase mass if dragged towards, else decrease.
			creating.change_mass(5 * -towards_body_delta);
		}
		else if (modify_mode == Modifying::VELOCITY) {
			// User is altering body's velocity.

			// Increase velocity in opposite direction of the mouse drag.
			constexpr float scale_down = 10.0f;
			creating.change_vel({ -movement.x / scale_down , -movement.y / scale_down });
		}
	}
	else if (IsKeyPressed(KEY_ENTER)) {
		// Add user body to universe, and end creation mode. 
		universe.add_body(std::move(creating));
		return new DefaultInteraction;
	}
	else if (IsKeyPressed(KEY_ONE)) {
		return new DefaultInteraction;
	}
	else if (IsKeyPressed(KEY_THREE)) {
		return new SystemCreation{ universe_point, universe };
	}

	return this;
}

std::string_view PlanetCreation::get_name() const
{
	return "Planet creation";
}

std::string_view PlanetCreation::get_help_text() const
{
	return
		"Drag center of planet to change velocity\n"
		"Drag edge of planet to change mass\n"
		"Shift+click to add to universe\n"
		"[Enter] to add to universe and return\n"
		"[1] to go to default mode\n"
		"[3] to go to system generator\n";

}
void PlanetCreation::render_world(const AdvCamera& camera, const Universe& universe)
{
	if (camera.in_view(creating))
	{
		RenderUtil::render_body(creating);

		Vector2 pos = creating.pos();
		Vector2 vel = creating.vel();
		DebugInfo info{ "Pos(x): " + std::to_string(pos.x) };

		info.add("Pos(y): " + std::to_string(pos.y));
		info.add("Vel(x): " + std::to_string(vel.x));
		info.add("Vel(y): " + std::to_string(vel.y));
		info.add("Mass: " + std::to_string(creating.get_mass()));

		RenderUtil::render_near_body(creating, info.c_str());
		RenderUtil::render_velocity(creating);
	}
}
