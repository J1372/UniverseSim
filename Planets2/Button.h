#pragma once
#include <string>
#include <raylib.h>
#include <functional>
#include "UIElement.h"
class Scene;

class Button : public UIElement
{
	std::string text;
	int font_size;
	int text_length_pixels;
	float width_padding = 0.1;

	Rectangle rect;

	Color background_color = GRAY;
	Color edge_color = DARKGRAY;


	bool clickable = true;
	static constexpr int edge_width = 10;


	std::function<void(void)> callback = nullptr;

public:

	Button(const std::string& text, float x, float y) :
		text(text),
		font_size(12),
		text_length_pixels(MeasureText(text.c_str(), font_size)),
		rect{ x, y, text_length_pixels * (1+width_padding), 50}
	{}

	Button(const std::string &text, float x, float y, int font_size) :
		text(text),
		font_size(font_size),
		text_length_pixels(MeasureText(text.c_str(), font_size)),
		rect{ x, y, text_length_pixels * (1 + width_padding), 50 }
	{}



	void set_on_action(std::function<void(void)> on_action) { callback = on_action; }
	void click() { if (clickable) callback(); }

	void set_clickable(bool to_set) { clickable = to_set; }

	bool contains_point(Vector2 point) const {
		return point.x >= rect.x and point.x < rect.x + rect.width
			and point.y >= rect.y and rect.y < rect.y + rect.height;
	}

	void render() const;
};


