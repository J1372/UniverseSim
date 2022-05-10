#include "FreeCamera.h"
#include "Body.h"
#include "universe.h"
#include "AnchoredCamera.h"
#include <utility>

FreeCamera::FreeCamera(float screen_width, float screen_height, int cam_speed_multiplier) : CameraState{cam_speed_multiplier}
{
	camera.offset = { screen_width / 2, screen_height / 2 };
	camera.target = { 0.0f, 0.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;
}

FreeCamera::FreeCamera(Camera2D&& camera, int cam_speed_multiplier) : CameraState{std::move(camera), cam_speed_multiplier}
{
	// recalc offset and target so camera stays at same point, but offset is centered again.
}

CameraState* FreeCamera::update(const Universe& universe)
{

	// Camera state change

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
		Vector2 screen_point = GetMousePosition();
		Vector2 universe_point = GetScreenToWorld2D(screen_point, camera);

		Body* body = universe.get_body(universe_point);

		if (body) {
			std::cout << body->id << '\n';
			return new AnchoredCamera { std::move(camera), cam_speed_multiplier, *body};
		}
	}

	// Camera movement

	if (IsKeyDown(KEY_W)) {
		camera.target.y -= cam_speed;
	}
	if (IsKeyDown(KEY_A)) {
		camera.target.x -= cam_speed;
	}
	if (IsKeyDown(KEY_S)) {
		camera.target.y += cam_speed;
	}
	if (IsKeyDown(KEY_D)) {
		camera.target.x += cam_speed;
	}

	// Camera speed

	if (IsKeyPressed(KEY_MINUS)) {
		cam_speed_multiplier = std::max(1, cam_speed_multiplier - 1);
		cam_speed = 5 * cam_speed_multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_EQUAL)) {
		cam_speed_multiplier++;
		cam_speed = 5 * cam_speed_multiplier / camera.zoom;
	}

	// Camera zoom for keys and mousewheel

	float wheel_move = GetMouseWheelMove();

	if (wheel_move < 0 or IsKeyPressed(KEY_COMMA)) {
		zoom_out();
		cam_speed = 5 * cam_speed_multiplier / camera.zoom;
	}
	else if (wheel_move > 0 or IsKeyPressed(KEY_PERIOD)) {
		zoom_in();
		cam_speed = 5 * cam_speed_multiplier / camera.zoom;
	}

	

    return ret_state;
}

void FreeCamera::resize(float width, float height)
{
	camera.offset = { width / 2, height / 2 };
}
