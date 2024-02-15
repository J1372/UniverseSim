#include "PlanetMouseModifier.h"
#include <raylib.h>
#include "AdvCamera.h"
#include "universe.h"
#include "Physics.h"

bool PlanetMouseModifier::process_input(Body& modifying, const AdvCamera& camera, const Universe& universe)
{
	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera.get_raylib_camera());

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		Vector2 modifying_pos = modifying.pos();
		// select modifying mode based on where user clicked.
		float inner_radius = .6 * modifying.get_radius();
		if (Physics::point_in_circle(universe_point, modifying_pos.x, modifying_pos.y, inner_radius))
		{
			// Clicked on the inside section of the body.
			modify_mode = Modifying::VELOCITY;
		}
		else if (modifying.contains_point(universe_point))
		{
			// Clicked on the outside section of the body.
			modify_mode = Modifying::MASS;
		}
		else
		{
			// User did not click on the body they are creating.
			modify_mode = Modifying::NONE;
		}
	}
	else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		// Mouse is being dragged

		Vector2 movement = GetMouseDelta();
		// Can compare difference universe point (0,0) to (movement.x, movement.y)
		// So zooming out would increase rate of change of i.e mass.
		// Currently flat amount based on movement on screen.

		if (modify_mode == Modifying::MASS)
		{
			// User is altering body's mass.
			Vector2 modifying_pos = modifying.pos();

			// This determines whether the user dragged the mouse towards the body.
			float towards_body_x = modifying_pos.x < universe_point.x ? -movement.x : movement.x;
			float towards_body_y = modifying_pos.y < universe_point.y ? -movement.y : movement.y;
			float towards_body_delta = towards_body_x + towards_body_y;

			// Increase mass if dragged towards, else decrease.
			modifying.change_mass(5 * -towards_body_delta);
		}
		else if (modify_mode == Modifying::VELOCITY)
		{
			// User is altering body's velocity.

			// Increase velocity in opposite direction of the mouse drag.
			constexpr float scale_down = 10.0f;
			modifying.change_vel({ -movement.x / scale_down , -movement.y / scale_down });
		}

		return movement.x != 0.0f or movement.y != 0.0f;
	}

	return false;
}
