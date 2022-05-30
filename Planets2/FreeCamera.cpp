#include "FreeCamera.h"
#include "Body.h"
#include "universe.h"
#include "AnchoredCamera.h"
#include <utility>

#include "Body.h"

#include "raylib.h"

void FreeCamera::init(const AdvCamera & starting_config)
{
	camera = starting_config;
	Vector2 center = { GetScreenWidth() / 2, GetScreenHeight() / 2 };
	camera.set_offset(center);
}

void FreeCamera::goto_body(Body& body)
{
	camera.set_target({ body.x, body.y });
}

CameraState* FreeCamera::update(const Universe& universe)
{
	// Camera state change

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
		Vector2 screen_point = GetMousePosition();
		Vector2 universe_point = GetScreenToWorld2D(screen_point, camera.get_raylib_camera());

		Body* body = universe.get_body(universe_point);

		if (body) {
			AnchoredCamera& transition_to = CameraState::anchored_camera;
			transition_to.enter(camera, *body);
			return &transition_to;
		}
	}

	// Camera movement

	if (IsKeyDown(KEY_W)) {
		camera.move_target(Direction::UP);
	}
	if (IsKeyDown(KEY_A)) {
		camera.move_target(Direction::LEFT);
	}
	if (IsKeyDown(KEY_S)) {
		camera.move_target(Direction::DOWN);
	}
	if (IsKeyDown(KEY_D)) {
		camera.move_target(Direction::RIGHT);
	}

	// Camera speed

	if (IsKeyPressed(KEY_MINUS)) {
		camera.decrease_speed_target();
	}
	else if (IsKeyPressed(KEY_EQUAL)) {
		camera.increase_speed_target();
	}

	// Camera zoom for keys and mousewheel

	float wheel_move = GetMouseWheelMove();

	if (wheel_move < 0 or IsKeyPressed(KEY_COMMA)) {
		camera.zoom_out();
	}
	else if (wheel_move > 0 or IsKeyPressed(KEY_PERIOD)) {
		camera.zoom_in();
	}

	

    return this;
}

void FreeCamera::enter(const AdvCamera& prev_camera)
{
	const Camera2D& ray_cam = prev_camera.get_raylib_camera();
	Vector2 center = { GetScreenWidth() / 2, GetScreenHeight() / 2 };

	Vector2 my_target = GetScreenToWorld2D(center, ray_cam);

	camera.set_offset(center);
	camera.set_target(my_target);
	camera.set_zoom(prev_camera.get_zoom());

}
