#pragma once

#include "UIElement.h"
#include <string>
#include <raylib.h>
#include <functional>

class TextBox : public UIElement
{
	std::string entered_text;

	int font_size;
	int text_length_pixels;
	double width_padding = 0.1;

	Rectangle rect;

	Color background_color = GRAY;
	Color edge_color = DARKGRAY;

	bool editable = false;
	static constexpr int edge_width = 10;

	std::function<void(std::string& text)> callback = nullptr;

public:

};

