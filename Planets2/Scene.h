#pragma once
#include <vector>
#include <memory>

class Scene
{

protected:
	int screen_width;
	int screen_height;

	Scene(int width, int height) : screen_width(width), screen_height(height) {}

public:

	virtual Scene* update() = 0;

	virtual void resize(int width, int height)
	{ 
		screen_width = width;
		screen_height = height;
	}

	virtual ~Scene() = default;
};

