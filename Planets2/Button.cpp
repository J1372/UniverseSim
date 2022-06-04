#include "Button.h"

void Button::init()
{
	text_length_pixels = MeasureText(text.c_str(), font_size);
	rect.width = get_default_width();
}

void Button::render() const {

	DrawRectangleRec(rect, background_color);
	DrawRectangleLinesEx(rect, edge_width, edge_color);

	int start_x = rect.x + rect.width / 2 - text_length_pixels/2;
	int start_y = rect.y + rect.height / 3;

	DrawText(text.c_str(), start_x, start_y, font_size, BLACK);
}

bool Button::send_keypress(int key_code)
{
	return false;
}
