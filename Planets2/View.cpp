#include <raylib.h>
#include <iostream>
#include <format>
#include <string>

#include "universe.h"
#include "Scene.h"
#include "SimulationScene.h"

void process_input() {
}

int main() {
	std::ios_base::sync_with_stdio(false);


	float start_width = 1700.0;
	float start_height = 900.0;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(start_width, start_height, "Game");
	SetExitKey(KEY_NULL);

	Universe universe;

	int rand_planets = 0;
	int systems = 1;

	for (int i = 0; i < rand_planets; ++i) {
		universe.create_rand_body();
	}

	for (int i = 0; i < systems; ++i) {
		universe.create_rand_system();
	}

	std::unique_ptr<Scene> active_scene = std::make_unique<SimulationScene>(GetScreenWidth(), GetScreenHeight(), universe);

	while (!WindowShouldClose()) {

		if (IsWindowResized()) {
			active_scene->resize(GetScreenWidth(), GetScreenHeight());
		}

		//process_input();

		Scene *next_scene = active_scene->update();

		if (next_scene == nullptr) {
			break;
		}

		if (next_scene != active_scene.get()) {
			active_scene = std::unique_ptr<Scene>(next_scene);
		}
	}


	CloseWindow();

	return 0;
}