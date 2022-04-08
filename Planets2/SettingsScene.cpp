#include "SettingsScene.h"
#include "SimulationScene.h"

SettingsScene::SettingsScene(int width, int height) : Scene{ width, height } {
	start_button.set_on_action([this]() { return_scene = new SimulationScene{ screen_width, screen_height, 0, 1 }; });
	exit_button.set_on_action([this]() { return_scene = nullptr; });
}

void SettingsScene::process_input()
{
	if (IsKeyPressed(KEY_SPACE)) {
		start_button.click();
	}

	if (IsKeyPressed(KEY_ESCAPE)) {
		exit_button.click();
	}
}

void SettingsScene::render() const
{
	BeginDrawing();
		ClearBackground(GRAY); // maybe better to have before beginmode2d?

		DrawFPS(50, 50);

		start_button.render();
		exit_button.render();

	EndDrawing();
}

Scene* SettingsScene::update()
{
	process_input();
	render();
    return return_scene;
}
