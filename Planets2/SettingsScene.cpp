#include "SettingsScene.h"
#include "SimulationScene.h"

#include "Button.h"
#include "Label.h"
#include "TextBox.h"
#include "Dropdown.h"

void SettingsScene::init_default()
{
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

	partitioning_dropdown.add_choice("Quad tree");
	partitioning_dropdown.add_choice("Grid");
	partitioning_dropdown.add_choice("AABB");

	background_color = SKYBLUE;
}

void SettingsScene::generate_settings()
{
	settings.universe_size_start = std::stoi(start_size_input.get_text());
	settings.universe_size_max = 100 * settings.universe_size_start;
	settings.num_rand_planets = std::stoi(num_planets_input.get_text());
	settings.num_rand_systems = std::stoi(num_systems_input.get_text());

	settings.grav_const = std::stod(grav_const_input.get_text());

	settings.system_min_planets = std::stod(sys_min_planets_input.get_text());
	settings.system_max_planets = std::stod(sys_max_planets_input.get_text());
}

void SettingsScene::read_settings_to_gui()
{
	start_size_input.set_text(std::to_string(settings.universe_size_start));
	num_planets_input.set_text(std::to_string(settings.num_rand_planets));
	num_systems_input.set_text(std::to_string(settings.num_rand_systems));

	grav_const_input.set_text(std::to_string(settings.grav_const));

	sys_min_planets_input.set_text(std::to_string(settings.system_min_planets));
	sys_max_planets_input.set_text(std::to_string(settings.system_max_planets));
}

SettingsScene::SettingsScene(int width, int height) : GuiScene{ width, height }
{
	init_default();
	read_settings_to_gui();
}

SettingsScene::SettingsScene(int width, int height, UniverseSettings& settings) : GuiScene{ width, height }, settings(settings)
{
	init_default();
	read_settings_to_gui();
}
