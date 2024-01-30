#include "Button.h"

float Button::get_default_width() const
{
	return text_length_pixels * (1 + width_padding);
}

Button::Button(std::string_view text_view, float x, float y) :
	text(text_view),
	font_size(12),
	text_length_pixels(MeasureText(text.c_str(), font_size)),
	rect{ x, y, get_default_width(), 50 }
{}

Button::Button(std::string_view text_view, float x, float y, int font_size) :
	text(text_view),
	font_size(font_size),
	text_length_pixels(MeasureText(text.c_str(), font_size)),
	rect{ x, y, get_default_width(), 50 }
{}

void Button::set_min_width(float min_width)
{
	rect.width = std::max(min_width, get_default_width());
}

void Button::set_on_action(std::function<void(void)> on_action)
{
	callback = on_action;
}

bool Button::contains_point(Vector2 point) const
{
	return CheckCollisionPointRec(point, rect);
}

void Button::render() const {

	DrawRectangleRec(rect, background_color);
	DrawRectangleLinesEx(rect, edge_width, edge_color);

	int start_x = rect.x + rect.width / 2 - text_length_pixels/2;
	int start_y = rect.y + rect.height / 3;

	DrawText(text.c_str(), start_x, start_y, font_size, BLACK);
}

void Button::click()
{
	if (callback) {
		callback();
	}
}

bool Button::send_keypress(int key_code)
{
	return false;
}
