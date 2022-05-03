#pragma once
#include "GuiScene.h"
#include "UniverseSettings.h"

#include "TextBox.h"
#include "Button.h"
#include "Label.h"


class SettingsScene : public GuiScene
{
	UniverseSettings settings{};


	static constexpr int BUTTON_Y = 700;

	Button start_button = gui.add<Button>(&start_button, "Start", 500, BUTTON_Y);
	Button exit_button = gui.add<Button>(&exit_button, "Exit", 600, BUTTON_Y);

	static constexpr int TEXTBOX_START_X = 300;
	static constexpr int TEXTBOX_WIDTH = 500;


	TextBox start_size_input = gui.add<TextBox>(&start_size_input, "1000", TEXTBOX_START_X, 300, TEXTBOX_WIDTH);
	TextBox num_planets_input = gui.add<TextBox>(&num_planets_input, "0", TEXTBOX_START_X, 400, TEXTBOX_WIDTH);
	TextBox num_systems_input = gui.add<TextBox>(&num_systems_input, "1", TEXTBOX_START_X, 500, TEXTBOX_WIDTH);
	TextBox grav_const_input = gui.add<TextBox>(&grav_const_input, "0.75", TEXTBOX_START_X, 600, TEXTBOX_WIDTH);

	static constexpr int LABEL_X = TEXTBOX_START_X + TEXTBOX_WIDTH + 50;

	Label start_size_label = gui.add<Label>(&start_size_label, "Universe starting size", LABEL_X, 320, 12);
	Label num_planets_label = gui.add<Label>(&num_planets_label, "Num planets", LABEL_X, 420, 12);
	Label num_systems_label = gui.add<Label>(&num_systems_label, "Num systems", LABEL_X, 520, 12);
	Label grav_const_label = gui.add<Label>(&grav_const_label, "Grav const", LABEL_X, 620, 12);

	void generate_settings();

public:
	SettingsScene(int width, int height);
};

