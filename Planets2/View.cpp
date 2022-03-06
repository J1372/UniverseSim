#include <raylib.h>
#include <iostream>
#include <format>
#include <string>

#include "universe.h"

//#define MY_DEBUG
//#define RENDER_QUAD_TREE

float screenWidth = 1700.0;
float screenHeight = 900.0;
//const int boardStartX = 100; maybe in GameScene
//const int boardStartY = 100;

bool running;

void processInput(Universe& universe, Camera2D& camera) {
	static int multiplier = 16;
	static float cam_speed = 5 * multiplier / camera.zoom;


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

	if (IsKeyPressed(KEY_COMMA)) {
		camera.zoom /= 2;
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_PERIOD)) {
		camera.zoom *= 2;
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_MINUS)) {
		multiplier = std::max(1, multiplier - 1);
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_EQUAL)) {
		++multiplier;
		cam_speed = 5 * multiplier / camera.zoom;
	}

}

bool on_screen(Vector2 screen_pos, Camera2D& camera) {
	return screen_pos.x >= 0 && screen_pos.y >= 0 && screen_pos.x < GetScreenWidth() && screen_pos.y < GetScreenHeight();
}

#ifdef RENDER_QUAD_TREE
void draw_quad(const QuadTree* cur) {
	Rectangle rec{ cur->x, cur->y, cur->get_width(), cur->get_height() };

	//DrawRectangleLines(cur->x, cur->y, cur->get_width(), cur->get_height(), RAYWHITE);

	DrawRectangleLinesEx(rec, 10, RAYWHITE);


	if (!cur->is_leaf()) {
		const QuadTree* quads[4];
		cur->get_quads(quads);

		for (int i = 0; i < 4; ++i) {
			draw_quad(quads[i]);
		}
	}
}
#endif

void render_system(Universe& universe, Camera2D& camera) {
	const std::unordered_map<int, Body>& body_map = universe.get_map();

#ifdef RENDER_QUAD_TREE

	const QuadTree* cur = universe.get_quad_root();

	draw_quad(cur);

#endif

	for (auto it = body_map.begin(); it != body_map.end(); ++it) {
		const Body& body = it->second;

		Vector2 screen_pos = GetWorldToScreen2D({ body.x, body.y }, camera);
		if (on_screen(screen_pos, camera)) {
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

int main() {
	std::ios_base::sync_with_stdio(false);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "Game");
	SetExitKey(KEY_NULL);
	//SetTargetFPS(60);

	Universe universe;

	Camera2D camera;
	camera.offset = { 0,0 };
	camera.target = { 0, 0 };
	camera.rotation = 0;
	camera.zoom = 1;

	int rand_planets = 0;
	int systems = 1;

	for (int i = 0; i < rand_planets; ++i) {
		universe.create_rand_body();
	}

	for (int i = 0; i < systems; ++i) {
		universe.create_rand_system();
	}

	running = false;

	while (!WindowShouldClose()) {

		if (IsWindowResized()) {

		}

		processInput(universe, camera);

		if (running) {
			universe.update();
		}

		BeginDrawing();
		ClearBackground(BLACK); // maybe better to have before beginmode2d?

		DrawFPS(50, 50);

		BeginMode2D(camera);
		render_system(universe, camera);
		EndMode2D();

		EndDrawing();
	}


	CloseWindow();

	return 0;
}