#pragma once
#include "GuiScene.h"

#include "TextBox.h"
#include "Button.h"

class SettingsScene : public GuiScene
{
	Button start_button = gui.add<Button>("Start", 500, 700);
	Button exit_button = gui.add<Button>("Exit", 600, 700);

	TextBox num_planets_input = gui.add<TextBox>(300, 400, 500);
	TextBox num_systems_input = gui.add<TextBox>(300, 500, 500);

public:
	SettingsScene(int width, int height);
};

