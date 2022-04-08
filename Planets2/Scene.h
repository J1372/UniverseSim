#pragma once
#include <vector>
#include <memory>

class Scene
{
	//std::vector<std::unique_ptr<GuiElement>> elements;

	int screen_width;
	int screen_height;

public:

	virtual Scene* update() = 0;
	virtual void resize(int width, int height) = 0;
};

