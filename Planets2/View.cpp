#include <raylib.h>
#include <iostream>

#include "Scene.h"
#include "SettingsScene.h"

int main() {
	std::ios_base::sync_with_stdio(false);


	float start_width = 1700.0;
	float start_height = 900.0;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(start_width, start_height, "Universe sim");
	SetExitKey(KEY_NULL); // Disable raylib default exit when pressing escape.

	MaximizeWindow();

	std::unique_ptr<Scene> active_scene = std::make_unique<SettingsScene>(GetScreenWidth(), GetScreenHeight());

	while (!WindowShouldClose()) {

		if (IsWindowResized()) {
			active_scene->resize(GetScreenWidth(), GetScreenHeight());
		}

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
