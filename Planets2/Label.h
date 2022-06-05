#pragma once
#include "UIElement.h"
#include "raylib.h"
#include <string>

class Label : public UIElement
{

	std::string text;
	int x;
	int y;
	int font_size;
	Color color = BLACK;

public:

	Label(const std::string& text, int x, int y, int font_size);

	void render() const;
};

