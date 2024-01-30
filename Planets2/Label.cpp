#include "Label.h"

Label::Label(std::string_view text, float x, float y, int font_size)
	: text(text), pos{ x, y }, font_size(font_size)
{}

Label::Label(std::string_view text, float x, float y, int font_size, Color color)
	: text(text), pos{ x, y }, font_size(font_size), color(color)
{}

void Label::set_pos(Vector2 to_set)
{
	pos = to_set;
}

void Label::set_text(std::string_view to_set)
{
	text = to_set;
}

std::string_view Label::get_text() const
{
	return text;
}

void Label::set_color(Color to_set)
{
	color = to_set;
}

void Label::center_on(int center_x)
{
	int text_length = MeasureText(text.c_str(), font_size);

	pos.x = center_x - text_length / 2;
}

void Label::render() const
{
	DrawText(text.c_str(), pos.x, pos.y, font_size, color);
}
