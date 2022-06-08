#include "Label.h"

Label::Label(const std::string& text, int x, int y, int font_size) : text(text), x(x), y(y), font_size(font_size)
{}

void Label::set_text(const std::string& to_set)
{
	text = to_set;
}

const std::string& Label::get_text() const
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

	x = center_x - text_length / 2;
}

void Label::render() const
{
	DrawText(text.c_str(), x, y, font_size, color);
}
