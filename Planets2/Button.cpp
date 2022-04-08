#include "Button.h"

void Button::render() const {

	DrawRectangleRec(rect, background_color);
	DrawRectangleLinesEx(rect, edge_width, edge_color);

	int start_x = rect.x + rect.width * (width_padding / 2);
	int start_y = rect.y + rect.height / 2;

	DrawText(text.c_str(), start_x, start_y, font_size, BLACK);
}