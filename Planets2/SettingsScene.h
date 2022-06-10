#pragma once
#include "GuiScene.h"
#include "UniverseSettings.h"

class Button;
class Label;
class TextBox;
class Dropdown;
class SpatialPartitioning;

// need actual includes now to use gui.add<T> in static class. 
#include "Button.h"
#include "Dropdown.h"
#include "Label.h"
#include "Textbox.h"
#include "Slider.h"
#include "CheckBox.h"

// A settings scene where the user can define the parameters for the simulation universe.
class SettingsScene : public GuiScene
{

	// Currently no GUI containers / composites, so some setup code looks redundant.

	// The settings to be used in the simulation.
	UniverseSettings settings;

	// Start and exit buttons
	static constexpr float BUTTON_X = 860.0f;
	static constexpr float BUTTON_Y = 920.0f;
	static constexpr float BUTTON_MIN_WIDTH = 100.0f;

	Button& start_button = gui.add<Button>("Start", BUTTON_X, BUTTON_Y);
	Button& exit_button = gui.add<Button>("Exit", BUTTON_X + 100, BUTTON_Y);


	static constexpr float TEXTBOX_WIDTH = 400.0f;

	// Start x of label from the beginning of a textbox.
	static constexpr float LABEL_OFFSET = TEXTBOX_WIDTH + 25;

	// Begin columns at this y coordinate.
	static constexpr float COLUMN_Y = 50.0f;

	// Universe generation settings column
	static constexpr float UNIVERSE_START_X = 50.0f;

	Label& universe_header = gui.add<Label>("Universe Generation", UNIVERSE_START_X + TEXTBOX_WIDTH / 3, COLUMN_Y, 12);

	TextBox& capacity_input = gui.add<TextBox>(UNIVERSE_START_X, COLUMN_Y + 100, TEXTBOX_WIDTH);
	TextBox& start_size_input = gui.add<TextBox>(UNIVERSE_START_X, COLUMN_Y + 200, TEXTBOX_WIDTH);
	TextBox& max_size_input = gui.add<TextBox>(UNIVERSE_START_X, COLUMN_Y + 300, TEXTBOX_WIDTH);
	TextBox& num_planets_input = gui.add<TextBox>(UNIVERSE_START_X, COLUMN_Y + 400, TEXTBOX_WIDTH);
	TextBox& num_systems_input = gui.add<TextBox>(UNIVERSE_START_X, COLUMN_Y + 500, TEXTBOX_WIDTH);
	


	Label& capacity_label = gui.add<Label>("Universe capacity", UNIVERSE_START_X + LABEL_OFFSET, COLUMN_Y + 120, 12);
	Label& start_size_label = gui.add<Label>("Universe starting size", UNIVERSE_START_X + LABEL_OFFSET, COLUMN_Y + 220, 12);
	Label& max_size_label = gui.add<Label>("Universe maximum size", UNIVERSE_START_X + LABEL_OFFSET, COLUMN_Y + 320, 12);
	Label& num_planets_label = gui.add<Label>("Num planets", UNIVERSE_START_X + LABEL_OFFSET, COLUMN_Y + 420, 12);
	Label& num_systems_label = gui.add<Label>("Num systems", UNIVERSE_START_X + LABEL_OFFSET, COLUMN_Y + 520, 12);

	// Physics settings column
	static constexpr float PHYSICS_START_X = UNIVERSE_START_X + LABEL_OFFSET + 200;
	Label& physics_header = gui.add<Label>("Physics", PHYSICS_START_X + TEXTBOX_WIDTH / 3, COLUMN_Y, 12);

	TextBox& grav_const_input = gui.add<TextBox>(PHYSICS_START_X, COLUMN_Y + 100, TEXTBOX_WIDTH);

	Label& grav_const_label = gui.add<Label>("Grav const", PHYSICS_START_X + LABEL_OFFSET, COLUMN_Y + 120, 12);


	// Barnes hut gravity settings
	static constexpr float SLIDER_WIDTH = TEXTBOX_WIDTH;
	CheckBox& approximate_gravity_checkbox = gui.add<CheckBox>("Use an approximation for gravity calculations to increase performance", PHYSICS_START_X, COLUMN_Y + 200, 20.0f);
	Slider& approximation_slider = gui.add<Slider>(PHYSICS_START_X, COLUMN_Y + 300, SLIDER_WIDTH, 0.0f, 1.0f);
	Label& approximation_label = gui.add<Label>("Approximation value", PHYSICS_START_X + LABEL_OFFSET, COLUMN_Y + 300, 12);
	Label& approximation_description = gui.add<Label>("Increasing this value improves performance\nbut decreases accuracy",
		PHYSICS_START_X, COLUMN_Y + 350, 20);

	// System generation settings column
	static constexpr float SYSTEMS_START_X = PHYSICS_START_X + LABEL_OFFSET + 200;
	Label& systems_header = gui.add<Label>("System Generation", SYSTEMS_START_X + TEXTBOX_WIDTH / 3, COLUMN_Y, 12);


	TextBox& sys_mass_ratio_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 100, TEXTBOX_WIDTH);
	TextBox& sys_min_planets_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 200, TEXTBOX_WIDTH);
	TextBox& sys_max_planets_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 300, TEXTBOX_WIDTH);
	TextBox& sys_min_dist_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 400, TEXTBOX_WIDTH);
	TextBox& sys_max_dist_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 500, TEXTBOX_WIDTH);
	TextBox& sys_moon_chance_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 600, TEXTBOX_WIDTH);
	TextBox& sys_retrograde_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 700, TEXTBOX_WIDTH);


	Label& sys_mass_ratio_label = gui.add<Label>("Star mass ratio", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 120, 12);
	Label& sys_min_planets_label = gui.add<Label>("Min planets", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 220, 12);
	Label& sys_max_planets_label = gui.add<Label>("Max planets", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 320, 12);
	Label& sys_min_dist_label = gui.add<Label>("Satellite min dist", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 420, 12);
	Label& sys_max_dist_label = gui.add<Label>("Satellite max dist", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 520, 12);
	Label& sys_moon_chance_label = gui.add<Label>("Moon chance", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 620, 12);
	Label& sys_retrograde_label = gui.add<Label>("Retrograde chance", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 720, 12);





	// Partitioning selection and specific settings.
	static constexpr float PARTITIONING_X = UNIVERSE_START_X;
	static constexpr float PARAM_X = PARTITIONING_X + 200;

	static constexpr float PARTITIONING_Y = 750.0f;
	Dropdown& partitioning_dropdown = gui.add<Dropdown>(PARTITIONING_X, PARTITIONING_Y, 12);
	Label& partitioning_label = gui.add<Label>("Collision spatial partitioning", PARTITIONING_X, PARTITIONING_Y - 50, 12);

	// Quadtree settings.
	TextBox& quad_max_bodies_input = gui.add<TextBox>("10", PARAM_X, PARTITIONING_Y, TEXTBOX_WIDTH);
	TextBox& quadtree_max_depth_input = gui.add<TextBox>("10", PARAM_X + LABEL_OFFSET, PARTITIONING_Y, TEXTBOX_WIDTH);
	Label& quad_bodies_label = gui.add<Label>("Max bodies before split", PARAM_X, PARTITIONING_Y - 50, 12);
	Label& quad_depth_label = gui.add<Label>("Max depth", PARAM_X + LABEL_OFFSET, PARTITIONING_Y - 50, 12);

	// Grid settings
	TextBox& grid_nodes_per_row_input = gui.add<TextBox>("10", PARAM_X, PARTITIONING_Y, TEXTBOX_WIDTH);
	Label& grid_label = gui.add<Label>("Nodes per row", PARAM_X, PARTITIONING_Y - 50, 12);

	Label& error_msg = gui.add<Label>("", BUTTON_X, BUTTON_Y - 30, 20);


	// Generates settings from the gui elements.
	void generate_settings();

	// Set gui elements to reflect the current universe settings.
	void read_settings_to_gui(); 

	// Creates and returns the selected partitioning method.
	std::unique_ptr<SpatialPartitioning> gen_partitioning();

	// Handles any user input errors by setting the error message.
	// Returns true if there was an error, else false.
	bool handle_errors();

	// Scans general input for errors related to not being a number and returns any error message.
	std::string scan_nan_errors() const;

	// Scans general input for semantic errors and returns any error message.
	std::string scan_other_errors() const;

	// Scans partitioning input for any errors and returns any error message.
	std::string scan_partitioning_errors() const;

public:

	// Initialization after Raylib init.
	// Register event handling and other gui setup.
	void init();

	// Enters the settings scene.
	void enter();

};

