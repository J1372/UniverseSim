#pragma once
#include "GuiScene.h"
#include "UniverseSettings.h"

class Button;
class Label;
class TextBox;


class SettingsScene : public GuiScene
{
	UniverseSettings settings;

	static constexpr int BUTTON_X = 400;
	static constexpr int BUTTON_Y = 800;

	Button& start_button = gui.add<Button>("Start", BUTTON_X, BUTTON_Y);
	Button& exit_button = gui.add<Button>("Exit", BUTTON_X + 100, BUTTON_Y);


	static constexpr int TEXTBOX_WIDTH = 400;
	static constexpr int LABEL_OFFSET = TEXTBOX_WIDTH + 50;

	static constexpr int COLUMN_Y = 100;



	// Universe generation settings column
	static constexpr int UNIVERSE_START_X = 50;

	Label& universe_header = gui.add<Label>("Universe Generation", UNIVERSE_START_X + TEXTBOX_WIDTH / 3, COLUMN_Y, 12);

	TextBox& start_size_input = gui.add<TextBox>(UNIVERSE_START_X, COLUMN_Y + 100, TEXTBOX_WIDTH);
	TextBox& num_planets_input = gui.add<TextBox>(UNIVERSE_START_X, COLUMN_Y + 200, TEXTBOX_WIDTH);
	TextBox& num_systems_input = gui.add<TextBox>(UNIVERSE_START_X, COLUMN_Y + 300, TEXTBOX_WIDTH);
	


	Label& start_size_label = gui.add<Label>("Universe starting size", UNIVERSE_START_X + LABEL_OFFSET, COLUMN_Y + 120, 12);
	Label& num_planets_label = gui.add<Label>("Num planets", UNIVERSE_START_X + LABEL_OFFSET, COLUMN_Y + 220, 12);
	Label& num_systems_label = gui.add<Label>("Num systems", UNIVERSE_START_X + LABEL_OFFSET, COLUMN_Y + 320, 12);



	// Physics settings column
	static constexpr int PHYSICS_START_X = UNIVERSE_START_X + LABEL_OFFSET + 200;
	Label& physics_header = gui.add<Label>("Physics", PHYSICS_START_X + TEXTBOX_WIDTH / 3, COLUMN_Y, 12);

	TextBox& grav_const_input = gui.add<TextBox>(PHYSICS_START_X, COLUMN_Y + 100, TEXTBOX_WIDTH);
	
	Label& grav_const_label = gui.add<Label>("Grav const", PHYSICS_START_X + LABEL_OFFSET, COLUMN_Y + 120, 12);




	// System generation settings column
	static constexpr int SYSTEMS_START_X = PHYSICS_START_X + LABEL_OFFSET + 200;
	Label& systems_header = gui.add<Label>("System Generation", SYSTEMS_START_X + TEXTBOX_WIDTH / 3, COLUMN_Y, 12);


	TextBox& sys_min_planets_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 100, TEXTBOX_WIDTH);
	TextBox& sys_max_planets_input = gui.add<TextBox>(SYSTEMS_START_X, COLUMN_Y + 200, TEXTBOX_WIDTH);

	Label& sys_min_planets_label = gui.add<Label>("Min planets", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 120, 12);
	Label& sys_max_planets_label = gui.add<Label>("Max planets", SYSTEMS_START_X + LABEL_OFFSET, COLUMN_Y + 220, 12);

	void init_default(); // register event handling and other gui setup.
	void generate_settings();

	void read_settings_to_gui(); // set gui elements to reflect the current universe settings.

public:

	SettingsScene(int width, int height);
	SettingsScene(int width, int height, UniverseSettings& settings);


};

