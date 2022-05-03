#pragma once
#include "GuiScene.h"
#include "UniverseSettings.h"

#include "TextBox.h"
#include "Button.h"
#include "Label.h"


class SettingsScene : public GuiScene
{
	UniverseSettings settings{};


	static constexpr int BUTTON_Y = 800;

	Button start_button = gui.add<Button>(&start_button, "Start", 400, BUTTON_Y);
	Button exit_button = gui.add<Button>(&exit_button, "Exit", 500, BUTTON_Y);

	static constexpr int TEXTBOX_WIDTH = 400;
	static constexpr int LABEL_OFFSET = TEXTBOX_WIDTH + 50;

	static constexpr int COLUMN_Y = 200;

	// Universe generation settings column
	static constexpr int UNIVERSE_START_X = 200;

	Label universe_header = gui.add<Label>(&universe_header, "Universe Generation", UNIVERSE_START_X, COLUMN_Y, 12);

	TextBox start_size_input = gui.add<TextBox>(&start_size_input, "1000", UNIVERSE_START_X, COLUMN_Y + 100, TEXTBOX_WIDTH);
	TextBox num_planets_input = gui.add<TextBox>(&num_planets_input, "0", UNIVERSE_START_X, COLUMN_Y + 200, TEXTBOX_WIDTH);
	TextBox num_systems_input = gui.add<TextBox>(&num_systems_input, "1", UNIVERSE_START_X, COLUMN_Y + 300, TEXTBOX_WIDTH);
	


	Label start_size_label = gui.add<Label>(&start_size_label, "Universe starting size", UNIVERSE_START_X + LABEL_OFFSET, 320, 12);
	Label num_planets_label = gui.add<Label>(&num_planets_label, "Num planets", UNIVERSE_START_X + LABEL_OFFSET, 420, 12);
	Label num_systems_label = gui.add<Label>(&num_systems_label, "Num systems", UNIVERSE_START_X + LABEL_OFFSET, 520, 12);

	// Physics settings column
	static constexpr int PHYSICS_START_X = UNIVERSE_START_X + LABEL_OFFSET + 200;
	Label physics_header = gui.add<Label>(&physics_header, "Physics", PHYSICS_START_X, COLUMN_Y, 12);
	TextBox grav_const_input = gui.add<TextBox>(&grav_const_input, "0.75", PHYSICS_START_X, COLUMN_Y + 100, TEXTBOX_WIDTH);

	Label grav_const_label = gui.add<Label>(&grav_const_label, "Grav const", PHYSICS_START_X + LABEL_OFFSET, 320, 12);


	// System generation settings column

	

	void generate_settings();

public:
	SettingsScene(int width, int height);
};

