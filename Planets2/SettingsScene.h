#pragma once
#include "GuiScene.h"

#include "TextBox.h"
#include "Button.h"

class SettingsScene : public GuiScene
{
	Button start_button = add_element<Button>("Start", 500, 700);
	Button exit_button = add_element<Button>("Exit", 600, 700);

	TextBox num_planets_input = add_element<TextBox>(300, 400, 500);
	TextBox num_systems_input = add_element<TextBox>(300, 500, 500);

public:
	SettingsScene(int width, int height);
};

