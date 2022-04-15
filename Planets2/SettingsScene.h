#pragma once
#include "GuiScene.h"

#include "TextBox.h"
#include "Button.h"
#include "Label.h"

class SettingsScene : public GuiScene
{
	Button start_button = gui.add<Button>(&start_button, "Start", 500, 700);
	Button exit_button = gui.add<Button>(&exit_button, "Exit", 600, 700);

	TextBox num_planets_input = gui.add<TextBox>(&num_planets_input, 300, 400, 500);
	TextBox num_systems_input = gui.add<TextBox>(&num_systems_input, 300, 500, 500);

	Label num_planets_label = gui.add<Label>(&num_planets_label, "Num planets", 900, 420, 12);
	Label num_systems_label = gui.add<Label>(&num_systems_label, "Num systems", 900, 520, 12);

public:
	SettingsScene(int width, int height);
};

