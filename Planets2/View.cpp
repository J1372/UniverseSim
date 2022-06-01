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

	Scene::init_scenes();
	Scene* active_scene = &Scene::settings_scene;

	while (!WindowShouldClose()) {

		if (IsWindowResized()) {
			//active_scene->resize(GetScreenWidth(), GetScreenHeight());
		}

		active_scene = active_scene->update();

		if (active_scene == nullptr) {
			break;
		}
	}


	CloseWindow();

	return 0;
}
