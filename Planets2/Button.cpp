#include "Button.h"

void Button::render() const {

	DrawRectangleRec(rect, background_color);
	DrawRectangleLinesEx(rect, edge_width, edge_color);

	int start_x = rect.x + rect.width / 4;
	int start_y = rect.y + rect.height / 3;

	DrawText(text.c_str(), start_x, start_y, font_size, BLACK);
}

bool Button::send_keypress(int key_code)
{
	return false;
}
