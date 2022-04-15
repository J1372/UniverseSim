#include "SettingsScene.h"
#include "SimulationScene.h"

SettingsScene::SettingsScene(int width, int height) : GuiScene{ width, height } {
	start_button.set_on_action([this]() { return_scene = new SimulationScene{ screen_width, screen_height, 0, 1 }; });
	exit_button.set_on_action([this]() { return_scene = nullptr; });

	start_button.set_min_width(100);
	exit_button.set_min_width(100);


	background_color = SKYBLUE;
}
