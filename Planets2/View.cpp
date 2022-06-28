#include <raylib.h>
#include <chrono>

#include "Scene.h"
#include "SettingsScene.h"
#include "MyRandom.h"

int main() {

	std::ios_base::sync_with_stdio(false);

	// Initialize random number generator seed using the current time.
	Rand::set_seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());


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
			active_scene->notify_resize(GetScreenWidth(), GetScreenHeight());
		}

		active_scene = active_scene->update();

		if (active_scene == nullptr) {
			break;
		}
	}


	CloseWindow();

	return 0;
}
