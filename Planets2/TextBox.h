#pragma once

#include "UIElement.h"
#include <string>
#include <raylib.h>
#include <functional>

class TextBox : public UIElement
{
	std::string entered_text;
	int cursor_pos = 0;

	int font_size;
	int text_length_pixels;
	double width_padding = 0.1;

	Rectangle rect;

	Color background_color = GRAY;
	Color edge_color = DARKGRAY;

	bool editable = false;
	bool active = false;
	static constexpr int edge_width = 10;

	std::function<void(std::string& text)> callback = nullptr;

public:

	void click();

	bool contains_point(Vector2 point) const {
		return point.x >= rect.x and point.x < rect.x + rect.width
			and point.y >= rect.y and rect.y < rect.y + rect.height;
	}

	void render() = 0;

	void send_keypress(int key);

	std::string get_text() const { return entered_text; }

	void set_active(bool to_set) { active = to_set; }
};

