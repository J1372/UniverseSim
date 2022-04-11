#pragma once
#include "Scene.h"

#include "TextBox.h"
#include "Button.h"

class SettingsScene : public Scene
{
	Button start_button { "Start", 500, 700 };
	Button exit_button{ "Exit", 600, 700 };

	TextBox num_planets_input { 300, 400, 500 };
	TextBox num_systems_input { 300, 500, 500 };

	TextBox* active_textbox = nullptr;

	void process_input();

	void render() const;

	Scene* return_scene = this;

public:
	SettingsScene(int width, int height);
	

	Scene* update();

};

