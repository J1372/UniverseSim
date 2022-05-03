#include "SettingsScene.h"
#include "SimulationScene.h"

void SettingsScene::generate_settings()
{
	settings.universe_size_start = std::stoi(start_size_input.get_text());
	settings.universe_size_max = 100 * settings.universe_size_start;
	settings.num_rand_planets = std::stoi(num_planets_input.get_text());
	settings.num_rand_systems = std::stoi(num_systems_input.get_text());
	settings.grav_const = std::stod(grav_const_input.get_text());
}

SettingsScene::SettingsScene(int width, int height) : GuiScene{ width, height } {

	start_button.set_on_action([this]() {
		generate_settings();
		return_scene = new SimulationScene{ screen_width, screen_height, settings };
		
		});
	exit_button.set_on_action([this]() { return_scene = nullptr; });

	start_button.set_min_width(100);
	exit_button.set_min_width(100);

	//num_planets_input.set_on_exit([this](const std::string& text) { settings.num_rand_planets = std::stoi(text); });

	num_planets_input.set_prompt_text("Number of random planets to generate");
	num_systems_input.set_prompt_text("Number of random systems to generate");

	background_color = SKYBLUE;
}
