#include "SettingsScene.h"
#include "SimulationScene.h"

#include "Button.h"
#include "Label.h"
#include "TextBox.h"
#include "Dropdown.h"

#include "SpatialPartitioning.h"
#include "Grid.h"

void SettingsScene::init_default()
{
	start_button.set_on_action([this]() {
		generate_settings();
		return_scene = new SimulationScene{ screen_width, screen_height, settings, gen_partitioning() };

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

	partitioning_dropdown.set_on_selection([this](const std::string& selection) {
		if (selection == "Quad tree") {
			gui.hide(grid_nodes_per_row_input);
			gui.show(quadtree_max_depth_input);
			gui.show(quad_max_bodies_input);
		}
		else if (selection == "Grid") {
			gui.show(grid_nodes_per_row_input);
			gui.hide(quadtree_max_depth_input);
			gui.hide(quad_max_bodies_input);
		}
		else if (selection == "Line Sweep") {
			gui.hide(grid_nodes_per_row_input);
			gui.hide(quadtree_max_depth_input);
			gui.hide(quad_max_bodies_input);
		}
		else {
			gui.hide(grid_nodes_per_row_input);
			gui.hide(quadtree_max_depth_input);
			gui.hide(quad_max_bodies_input);
		}
	});

	partitioning_dropdown.deselect();

	background_color = SKYBLUE;
}

void SettingsScene::generate_settings()
{
	settings.universe_size_start = std::stoi(start_size_input.get_text());
	settings.universe_size_max = 200 * settings.universe_size_start;
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

std::unique_ptr<SpatialPartitioning> SettingsScene::gen_partitioning()
{
	if (!partitioning_dropdown.has_selected()) {
		// return null partitioning object
		return nullptr;
	}
	std::string name_method = partitioning_dropdown.get_selected();

	if (name_method == "Quad tree") {
		int bodies_per_quad = std::stoi(quad_max_bodies_input.get_text());
		int max_depth = std::stoi(quadtree_max_depth_input.get_text());

		return std::make_unique<QuadTree>(settings.universe_size_max);
	}
	else if (name_method == "Grid") {
		int nodes_per_row = std::stoi(grid_nodes_per_row_input.get_text());
		return std::make_unique<Grid>(settings.universe_size_max, nodes_per_row);
	}
	else if (name_method == "Line Sweep") {
		return nullptr;
	}

	return nullptr;
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
