#include "TextBox.h"
void TextBox::click() {
	int mouse_x_in_box = GetMousePosition().x - rect.x;

	// not perfect, but "functional"
	// Better to do percentage on the text width, not textbox width
	double mouse_pct = mouse_x_in_box / rect.width;
	cursor_pos = mouse_pct * entered_text.size();

	active = true;
}

void TextBox::render() const {
	DrawRectangleRec(rect, background_color);
	DrawRectangleLinesEx(rect, edge_width, edge_color);

	int start_x = rect.x + rect.width * (width_padding / 2);
	int start_y = rect.y + rect.height / 2;

	DrawText(entered_text.c_str(), start_x, start_y, font_size, WHITE);
	
	if (active) {
		int cursor_line_x = MeasureText(entered_text.substr(0, cursor_pos).c_str(), font_size);
		constexpr int CURSOR_LINE_PADDING_HEIGHT = 0.2;
		int cursor_line_y = rect.y + rect.height * CURSOR_LINE_PADDING_HEIGHT;
		int cursor_line_y_end = rect.y + rect.height * (1 - CURSOR_LINE_PADDING_HEIGHT);

		DrawLine(cursor_line_x, cursor_line_y, cursor_line_x + 5, cursor_line_y_end, WHITE);
	}
}

void TextBox::send_keypress(int key)
{
	constexpr int BACKSPACE_CODE = 8;

	if (key == BACKSPACE_CODE) {
		if (entered_text.empty())
			return;

		entered_text.erase(cursor_pos);
		cursor_pos--;
		return;
	}

	// if key is invalid.
	if (key < 32 or key > 126) {
		return;
	}


	entered_text += static_cast<char>(key);
}
