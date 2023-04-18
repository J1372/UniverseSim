#include "SettingsScene.h"
#include "SimulationScene.h"

#include "Button.h"
#include "Label.h"
#include "TextBox.h"
#include "Dropdown.h"

#include "QuadTree.h"
#include "Grid.h"
#include "LineSweep.h"
#include "IntValidator.h"
#include "FloatValidator.h"

void SettingsScene::setup()
{
	start_button.set_on_action([this]()
	{
		// Do not start if there are errors with the currently entered settings.
		if (handle_errors())
		{
			return;
		}

		generate_settings();

		if (return_scene == this)
		{
			return_scene = new SimulationScene(settings, gen_partitioning());
		}
	});

	exit_button.set_on_action([this]() { return_scene = nullptr; });

	start_button.set_min_width(BUTTON_MIN_WIDTH);
	exit_button.set_min_width(BUTTON_MIN_WIDTH);

	num_planets_input.set_prompt_text("Number of random planets to generate");
	num_systems_input.set_prompt_text("Number of random systems to generate");

	partitioning_dropdown.add_choice("None");
	partitioning_dropdown.add_choice("Quad tree");
	partitioning_dropdown.add_choice("Grid");
	partitioning_dropdown.add_choice("Line sweep");

	partitioning_dropdown.set_on_selection([this](const std::string& selection)
	{
		if (selection == "Quad tree")
		{
			gui.hide(grid_nodes_per_row_input);
			gui.hide(grid_label);
			gui.show(quadtree_max_depth_input);
			gui.show(quad_max_bodies_input);
			gui.show(quad_bodies_label);
			gui.show(quad_depth_label);
		}
		else if (selection == "Grid")
		{
			gui.show(grid_nodes_per_row_input);
			gui.show(grid_label);
			gui.hide(quadtree_max_depth_input);
			gui.hide(quad_max_bodies_input);
			gui.hide(quad_bodies_label);
			gui.hide(quad_depth_label);
		}
		else if (selection == "Line sweep")
		{
			gui.hide(grid_nodes_per_row_input);
			gui.hide(grid_label);
			gui.hide(quadtree_max_depth_input);
			gui.hide(quad_max_bodies_input);
			gui.hide(quad_bodies_label);
			gui.hide(quad_depth_label);
		}
		else
		{
			gui.hide(grid_nodes_per_row_input);
			gui.hide(grid_label);
			gui.hide(quadtree_max_depth_input);
			gui.hide(quad_max_bodies_input);
			gui.hide(quad_bodies_label);
			gui.hide(quad_depth_label);
		}
	});

	partitioning_dropdown.set_selected(0);

	gui.hide(approximation_slider);
	gui.hide(approximation_label);
	gui.hide(approximation_description);


	approximate_gravity_checkbox.set_desc_font_size(10);
	approximate_gravity_checkbox.set_on_click([this](bool checked)
	{
		if (checked)
		{
			gui.show(approximation_slider);
			gui.show(approximation_label);
			gui.show(approximation_description);

		}
		else
		{
			gui.hide(approximation_slider);
			gui.hide(approximation_label);
			gui.hide(approximation_description);
		}
	});

	background_color = SKYBLUE;

	error_msg.set_color(RED);

	// Setting input validators.
	capacity_input.set_validator(std::make_unique<IntValidator>(1));
	start_size_input.set_validator(std::make_unique<FloatValidator>(0.1f));
	max_size_input.set_validator(std::make_unique<FloatValidator>(0.1f));
	num_planets_input.set_validator(std::make_unique<IntValidator>());
	num_systems_input.set_validator(std::make_unique<IntValidator>());
	grav_const_input.set_validator(std::make_unique<FloatValidator>(0.1f));
	sys_mass_ratio_input.set_validator(std::make_unique<FloatValidator>(0.1f));
	sys_min_planets_input.set_validator(std::make_unique<IntValidator>(1));
	sys_max_planets_input.set_validator(std::make_unique<IntValidator>(1));
	sys_min_dist_input.set_validator(std::make_unique<FloatValidator>(0.1f));
	sys_max_dist_input.set_validator(std::make_unique<FloatValidator>(0.1f));
	sys_moon_chance_input.set_validator(std::make_unique<FloatValidator>());
	sys_retrograde_input.set_validator(std::make_unique<FloatValidator>());

	quad_max_bodies_input.set_validator(std::make_unique<IntValidator>(1));
	quadtree_max_depth_input.set_validator(std::make_unique<IntValidator>(0));
	grid_nodes_per_row_input.set_validator(std::make_unique<IntValidator>(1));

	read_settings_to_gui();
}

SettingsScene::SettingsScene()
{
	setup();
}

SettingsScene::SettingsScene(const UniverseSettings& settings)
	: settings(settings)
{
	setup();
}


bool SettingsScene::handle_errors()
{
	// Return true if error msg was set to a non-empty string, else false.
	std::string error_text = scan_semantic_errors();
	if (error_text.empty()) {
		return false;
	}
	else
	{
		error_msg.set_text(error_text);
		error_msg.center_on(BUTTON_X + BUTTON_MIN_WIDTH);
		return true;
	}
}

std::string SettingsScene::scan_semantic_errors() const
{
	if (start_size_input.get_float() > max_size_input.get_float()) {
		return "Universe start size cannot be greater than its maximum size.";
	}
	else if (sys_min_planets_input.get_int() > sys_max_planets_input.get_int()) {
		return "System min planets cannot be greater than system max planets.";
	}
	else if (sys_min_dist_input.get_float() > sys_max_dist_input.get_float()) {
		return "Satellite min dist cannot be greater than satellite max dist.";
	}

	return "";
}

void SettingsScene::generate_settings()
{
	settings.universe_capacity = capacity_input.get_int();
	settings.universe_size_start = start_size_input.get_float();
	settings.universe_size_max = max_size_input.get_float();
	settings.num_rand_planets = num_planets_input.get_int();
	settings.num_rand_systems = num_systems_input.get_int();

	settings.grav_const = grav_const_input.get_double();
	settings.use_gravity_approximation = approximate_gravity_checkbox.is_checked();
	settings.grav_approximation_value = approximation_slider.get_val();

	settings.system_mass_ratio = sys_mass_ratio_input.get_float();

	settings.system_min_planets = sys_min_planets_input.get_int();
	settings.system_max_planets = sys_max_planets_input.get_int();
	settings.satellite_min_dist = sys_min_dist_input.get_float();
	settings.satellite_max_dist = sys_max_dist_input.get_float();
	settings.moon_chance = sys_moon_chance_input.get_double();
	settings.retrograde_chance = sys_retrograde_input.get_double();
}

void SettingsScene::read_settings_to_gui()
{
	constexpr int rounding = 3; // round floating point digits.
	capacity_input.set_text(std::to_string(settings.universe_capacity));
	start_size_input.set_text(std::to_string(static_cast<int>(settings.universe_size_start)));
	max_size_input.set_text(std::to_string(static_cast<int>(settings.universe_size_max)));
	num_planets_input.set_text(std::to_string(settings.num_rand_planets));
	num_systems_input.set_text(std::to_string(settings.num_rand_systems));

	grav_const_input.set_text(std::to_string(settings.grav_const).substr(0, rounding + 1));

	sys_mass_ratio_input.set_text(std::to_string(settings.system_mass_ratio).substr(0, rounding + 1));
	sys_min_planets_input.set_text(std::to_string(settings.system_min_planets));
	sys_max_planets_input.set_text(std::to_string(settings.system_max_planets));
	sys_min_dist_input.set_text(std::to_string(settings.satellite_min_dist).substr(0, rounding + 1));
	sys_max_dist_input.set_text(std::to_string(settings.satellite_max_dist).substr(0, rounding + 1));
	sys_moon_chance_input.set_text(std::to_string(settings.moon_chance).substr(0, rounding + 1));
	sys_retrograde_input.set_text(std::to_string(settings.retrograde_chance).substr(0, rounding + 1));

	if (settings.use_gravity_approximation)
	{
		approximate_gravity_checkbox.click();
		approximation_slider.set_val(settings.grav_approximation_value);
	}
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

		return std::make_unique<QuadTree>(settings.universe_size_max, bodies_per_quad, max_depth);
	}
	else if (name_method == "Grid") {
		int nodes_per_row = std::stoi(grid_nodes_per_row_input.get_text());
		return std::make_unique<Grid>(settings.universe_size_max, nodes_per_row);
	}
	else if (name_method == "Line sweep") {
		return std::make_unique<LineSweep>();
	}

	return nullptr;
}
