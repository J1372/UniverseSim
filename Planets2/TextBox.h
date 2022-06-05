#pragma once

#include "UIElement.h"
#include <string>
#include <raylib.h>
#include <functional>

class TextBox : public UIElement
{
	std::string entered_text;
	std::string prompt_text; // Shown when entered_text is empty.
	int cursor_pos = 0;

	int font_size = 12;
	float width_padding = 0.1;

	Rectangle rect;

	Color background_color = BLACK;
	Color edge_color = DARKGRAY;

	static constexpr int edge_width = 10;

	// Returns the x coord of where to begin drawing the currently entered text.
	int get_start_x_text() const;

public:

	TextBox(float x, float y, float width);
	TextBox(const std::string& start_text, float x, float y, float width);

	void click();

	bool contains_point(Vector2 point) const;

	void render() const;

	bool send_keypress(int key_code);

	void set_text(std::string& to_set);
	void set_text(std::string&& to_set);
	const std::string& get_text() const;

	void set_prompt_text(const std::string& text);
};
