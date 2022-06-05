#include "Label.h"

Label::Label(const std::string& text, int x, int y, int font_size) : text(text), x(x), y(y), font_size(font_size)
{}

void Label::render() const
{
	DrawText(text.c_str(), x, y, font_size, color);
}
