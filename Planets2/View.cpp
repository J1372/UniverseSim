#include <raylib.h>
#include <iostream>
#include <format>
#include <string>

#include "universe.h"
#include "Scene.h"
#include "SimulationScene.h"
#include "SettingsScene.h"

void process_input() {
}

int main() {
	std::ios_base::sync_with_stdio(false);


	float start_width = 1700.0;
	float start_height = 900.0;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(start_width, start_height, "Game");
	SetExitKey(KEY_NULL);

	std::unique_ptr<Scene> active_scene = std::make_unique<SettingsScene>(GetScreenWidth(), GetScreenHeight());

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