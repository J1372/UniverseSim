#pragma once
#include "UIElement.h"
#include "raylib.h"
#include <string>

// A label to display basic standalone text with a font size and color in a gui.
class Label : public UIElement
{
	// The text to be displayed.
	std::string text;

	// Starting position of the text.
	Vector2 pos;

	// Font size of text
	int font_size;

	// Text's display color.
	Color color = BLACK;

public:

	Label(const std::string& text, float x, float y, int font_size);

	// Sets the label's position.
	void set_pos(Vector2 pos);

	// Sets the label's text.
	void set_text(const std::string& to_set);

	// Gets the label's text.
	const std::string& get_text() const;

	// Sets color of the label's text.
	void set_color(Color to_set);

	// Moves the label's x-coordinate such that it is centered around the given x-coordinate.
	void center_on(int center_x);

	// Draws the label.
	void render() const;
};

