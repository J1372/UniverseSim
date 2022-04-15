#include "Label.h"

void Label::render() const
{
	DrawText(text.c_str(), x, y, font_size, color);
}
