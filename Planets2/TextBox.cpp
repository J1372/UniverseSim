#include "TextBox.h"
#include <algorithm>
#include "TextValidator.h"

int TextBox::get_start_x_text() const
{
	return rect.x + rect.width * (width_padding / 2);
}

TextBox::TextBox(float x, float y, float width) : rect{ x, y, width, 50 }
{}

TextBox::TextBox(const std::string& start_text, float x, float y, float width) : rect{x, y, width, 50}
{
	set_text(start_text);
}


void TextBox::click() {
	double mouse_x_in_box = GetMouseX() - get_start_x_text();

	double mouse_pct = mouse_x_in_box / MeasureText(entered_text.c_str(), font_size);

	int len_text = entered_text.size();
	cursor_pos = std::clamp(static_cast<int>(mouse_pct * len_text), 0, len_text);

}

bool TextBox::contains_point(Vector2 point) const
{
	return CheckCollisionPointRec(point, rect);
}

void TextBox::render() const {
	DrawRectangleRec(rect, background_color);
	DrawRectangleLinesEx(rect, edge_width, edge_color);

	int start_x = get_start_x_text();
	int top_y = rect.y + edge_width; // y coord of the bottom of the top edge.
	int full_width = rect.height - 2 * edge_width; // distance from bottom of top edge to top of bottom edge.
	int start_y = top_y + full_width / 3;

	if (entered_text.empty()) {
		DrawText(prompt_text.c_str(), start_x, start_y, font_size, LIGHTGRAY);
	}
	else {
		DrawText(entered_text.c_str(), start_x, start_y, font_size, WHITE);
	}

	
	if (active) {
		constexpr int CURSOR_LINE_PADDING = 3;
		constexpr double CURSOR_LINE_PADDING_HEIGHT = 0.2;
		int cursor_line_x = start_x + MeasureText(entered_text.substr(0, cursor_pos).c_str(), font_size) + CURSOR_LINE_PADDING;
		int cursor_line_y = top_y + full_width * CURSOR_LINE_PADDING_HEIGHT;
		int cursor_line_y_end = top_y + full_width * (1 - CURSOR_LINE_PADDING_HEIGHT);

		DrawLine(cursor_line_x, cursor_line_y, cursor_line_x, cursor_line_y_end, WHITE);
	}
}

bool TextBox::send_keypress(int key_code)
{
	if (validator and !validator->on_press(entered_text, key_code, cursor_pos))
	{
		return false;
	}

	if (key_code == KEY_BACKSPACE) {
		if (cursor_pos > 0) {
			cursor_pos--;
			entered_text.erase(cursor_pos, 1);
		}

		return true;
	}

	// if key is invalid.
	if (key_code < 32 or key_code > 126) {
		return false;
	}

	char letter = static_cast<char>(key_code);
	int combined_length = MeasureText(entered_text.c_str(), font_size) 
		+ MeasureText(std::to_string(letter).c_str(), font_size);

	// Don't allow entered text to go outside the textbox.
	if (combined_length + (get_start_x_text() - rect.x) < rect.width - edge_width) {
		entered_text.insert(cursor_pos, 1, letter);
		cursor_pos++;
	}

	return true;
}

void TextBox::set_text(std::string_view to_set)
{
	for (char character : to_set)
	{
		send_keypress(character);
	}
}

const std::string& TextBox::get_text() const
{
	return entered_text;
}

int TextBox::get_int() const
{
	return std::stoi(entered_text);
}

float TextBox::get_float() const
{
	return std::stof(entered_text);
}

double TextBox::get_double() const
{
	return std::stod(entered_text);
}

void TextBox::set_prompt_text(const std::string& text)
{
	prompt_text = text;
}

void TextBox::set_validator(std::unique_ptr<TextValidator>&& to_set)
{
	validator = std::move(to_set);
}

void TextBox::on_deactivation()
{
	if (validator)
	{
		validator->on_exit(entered_text);
	}
}
