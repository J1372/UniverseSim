#include "SimulationScene.h"
#include "universe.h"
#include "Body.h"
#include "QuadTree.h"
#include <string>

#include "SettingsScene.h"

void SimulationScene::process_input()
{
	static int multiplier = 8;
	static float cam_speed = 5 * multiplier / camera.zoom;

	if (IsKeyPressed(KEY_V)) {
		should_render_partitioning = !should_render_partitioning;
	}

	// Camera movement keys

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

	if (IsKeyPressed(KEY_SPACE)) {
		running = !running;
	}

	if (IsKeyPressed(KEY_MINUS)) {
		multiplier = std::max(1, multiplier - 1);
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_EQUAL)) {
		multiplier++;
		cam_speed = 5 * multiplier / camera.zoom;
	}

	// Camera zoom for keys and mousewheel

	float wheel_move = GetMouseWheelMove();

	if (wheel_move < 0 or IsKeyPressed(KEY_COMMA)) {
		zoom_out();
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (wheel_move > 0 or IsKeyPressed(KEY_PERIOD)) {
		zoom_in();
		cam_speed = 5 * multiplier / camera.zoom;
	}

	// Exiting to settings

	if (IsKeyPressed(KEY_ESCAPE)) {
		return_scene = new SettingsScene(screen_width, screen_height, universe.get_settings());
	}
}

void SimulationScene::render() const
{
	render_system();
	if (should_render_partitioning) {
		universe.get_partitioning()->draw_debug(camera);
	}
}

bool SimulationScene::on_screen(const Body& body) const
{
	Vector2 leftmost = GetWorldToScreen2D({ body.x - body.radius, body.y }, camera);
	Vector2 rightmost = GetWorldToScreen2D({ body.x + body.radius, body.y }, camera);

	Vector2 lowest = GetWorldToScreen2D({ body.x, body.y + body.radius }, camera);
	Vector2 highest = GetWorldToScreen2D({ body.x, body.y - body.radius }, camera);

	// can optimize : screen_pos.x >= -body.radius && screen_pos.y >= -body.radius

	return rightmost.x >= 0 and lowest.y >= 0 and leftmost.x < GetScreenWidth() and highest.y < GetScreenHeight();
}

void SimulationScene::render_system() const
{
	const std::vector<std::unique_ptr<Body>>& bodies = universe.get_bodies();

	for (const auto& body_ptr : bodies) {
		const Body& body = *body_ptr;

		if (on_screen(body)) {
			Color planet_color = body.type->color; // not a reference
			DrawCircle(body.x, body.y, body.radius, planet_color);

#ifdef MY_DEBUG
			int text_x = body.x + body.radius + 20;
			int text_y = body.y + body.radius + 20;
			int font_size = 25;
			int spacing = 20;
			std::string id_str = std::format("ID: {:}", body.id);
			std::string x_str = std::format("X: {:.2f}", body.x);
			std::string y_str = std::format("Y: {:.2f}", body.y);
			std::string vel_x_str = std::format("Vel(x): {:.2f}", body.vel_x);
			std::string vel_y_str = std::format("Vel(y): {:.2f}", body.vel_y);
			std::string mass_str = std::format("Mass: {:}", body.mass);

			DrawText(id_str.c_str(), text_x, text_y, font_size, planet_color);
			DrawText(x_str.c_str(), text_x, text_y + spacing, font_size, planet_color);
			DrawText(y_str.c_str(), text_x, text_y + spacing * 2, font_size, planet_color);
			DrawText(vel_x_str.c_str(), text_x, text_y + spacing * 3, font_size, planet_color);
			DrawText(vel_y_str.c_str(), text_x, text_y + spacing * 4, font_size, planet_color);
			DrawText(mass_str.c_str(), text_x, text_y + spacing * 5, font_size, planet_color);
#endif
}


	}
}

void SimulationScene::resize(int width, int height)
{
	screen_width = width;
	screen_height = height;

	camera.offset = { static_cast<float>(screen_width) / 2, static_cast<float>(screen_height) / 2 };
}

Scene* SimulationScene::update()
{
	process_input();

	if (running) {
		universe.update();
	}

	BeginDrawing();
		ClearBackground(BLACK); // maybe better to have before beginmode2d?

			BeginMode2D(camera);
				render();
			EndMode2D();

		DrawFPS(50, 50);

		std::string num_bodies_str = "Number bodies: " + std::to_string(universe.get_num_bodies());

		DrawText(num_bodies_str.c_str(), 50, 70, 20, RAYWHITE);

	EndDrawing();
	return return_scene;
}
