#include "PlanetCreation.h"

#include "universe.h"
#include "CameraState.h"
#include "Physics.h"

#include "SystemCreation.h"
#include "DefaultInteraction.h"

void PlanetCreation::enter(Vector2 mouse_pos) // universe point of mouse param
{
	long mass = 100;
	creating = std::make_unique<Body>(mouse_pos.x, mouse_pos.y, mass);
}

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

			long mass = 100;
			creating = std::make_unique<Body>(universe_point.x, universe_point.y, mass);

		}
		else {
			// select modifying mode based on where user clicked.
			if (Physics::point_in_circle(universe_point, creating->x, creating->y, creating->radius * .7)) {
				// Clicked on the inside section of the body.
				modify_mode = Modifying::VELOCITY;

			}
			else if (creating->contains_point(universe_point)) {
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

			// This determines whether the user dragged the mouse towards the body.
			float towards_body_x = creating->x < universe_point.x ? -movement.x : movement.x;
			float towards_body_y = creating->y < universe_point.y ? -movement.y : movement.y;
			float towards_body_delta = towards_body_x + towards_body_y;

			// Increase mass if dragged towards, else decrease.
			creating->change_mass(5 * -towards_body_delta);

		}
		else if (modify_mode == Modifying::VELOCITY) {
			// User is altering body's velocity.

			// Increase velocity in opposite direction of the mouse drag.
			creating->vel_x -= movement.x / 2;
			creating->vel_y -= movement.y / 2;
		}
	}
	else if (IsKeyPressed(KEY_ENTER)) {
		// Add user body to universe, and end creation mode. 
		universe.add_body(std::move(creating));
		creating = nullptr;

		DefaultInteraction& ret_state = InteractionState::default_interaction;
		return &ret_state;
	}
	else if (IsKeyPressed(KEY_THREE)) {
		creating = nullptr;

		SystemCreation& ret_state = InteractionState::system_interaction;
		ret_state.enter(universe_point, universe);
		return &ret_state;
	}

	return this;
}

const std::string& PlanetCreation::get_help_text() const
{
    return std::string();
}

std::span<const std::unique_ptr<Body>> PlanetCreation::get_creating_bodies() const
{
	return { &creating, 1 };
}
