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
	int text_length_pixels = 0;
	float width_padding = 0.1;

	Rectangle rect;

	Color background_color = BLACK;
	Color edge_color = DARKGRAY;

	bool editable = false;
	static constexpr int edge_width = 10;

	// Returns the x coord of where to begin drawing the currently entered text.
	int get_start_x_text() const { return rect.x + rect.width * (width_padding / 2); }

public:

	TextBox(float x, float y, float width) : rect{ x, y, width, 50 } {}
	TextBox(const std::string& start_text, float x, float y, float width) : entered_text(start_text), rect{ x, y, width, 50 } {}

	void click();

	bool contains_point(Vector2 point) const {
		return point.x >= rect.x and point.x < rect.x + rect.width
			and point.y >= rect.y and point.y < rect.y + rect.height;
	}

	void render() const;

	bool send_keypress(int key_code);

	void set_text(std::string& to_set) { entered_text = to_set; }
	void set_text(std::string&& to_set) { entered_text = to_set; }
	std::string get_text() const { return entered_text; }

	void set_prompt_text(const std::string& text) { prompt_text = text; }
};
