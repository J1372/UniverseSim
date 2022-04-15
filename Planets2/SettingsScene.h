#pragma once
#include "GuiScene.h"

#include "TextBox.h"
#include "Button.h"
#include "Label.h"

class SettingsScene : public GuiScene
{

	static constexpr int BUTTON_Y = 700;

	Button start_button = gui.add<Button>(&start_button, "Start", 500, BUTTON_Y);
	Button exit_button = gui.add<Button>(&exit_button, "Exit", 600, BUTTON_Y);

	static constexpr int TEXTBOX_START_X = 300;
	static constexpr int TEXTBOX_WIDTH = 500;


	TextBox num_planets_input = gui.add<TextBox>(&num_planets_input, TEXTBOX_START_X, 400, TEXTBOX_WIDTH);
	TextBox num_systems_input = gui.add<TextBox>(&num_systems_input, TEXTBOX_START_X, 500, TEXTBOX_WIDTH);

	static constexpr int LABEL_X = TEXTBOX_START_X + TEXTBOX_WIDTH + 100;

	Label num_planets_label = gui.add<Label>(&num_planets_label, "Num planets", LABEL_X, 420, 12);
	Label num_systems_label = gui.add<Label>(&num_systems_label, "Num systems", LABEL_X, 520, 12);

public:
	SettingsScene(int width, int height);
};

