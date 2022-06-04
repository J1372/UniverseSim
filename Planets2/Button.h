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
	float width_padding = 1;

	Rectangle rect;

	Color background_color = GRAY;
	Color edge_color = DARKGRAY;

	static constexpr int edge_width = 5;


	std::function<void(void)> callback = nullptr;

	float get_default_width() const {
		return text_length_pixels * (1 + width_padding);
	}

public:

	Button(const std::string& text, float x, float y) :
		text(text),
		font_size(12),
		text_length_pixels(MeasureText(text.c_str(), font_size)),
		rect{ x, y, get_default_width(), 50}
	{}

	Button(const std::string &text, float x, float y, int font_size) :
		text(text),
		font_size(font_size),
		text_length_pixels(MeasureText(text.c_str(), font_size)),
		rect{ x, y, get_default_width(), 50}
	{}

	// Late initialization for Buttons that are in a static object. Raylib's MeasureText won't work well with static objects.
	void init();

	void set_min_width(float min_width) {
		rect.width = std::max(min_width, get_default_width());
	}

	void set_on_action(std::function<void(void)> on_action) { callback = on_action; }

	bool contains_point(Vector2 point) const {
		return point.x >= rect.x and point.x < rect.x + rect.width
			and point.y >= rect.y and point.y < rect.y + rect.height;
	}

	void render() const;

	void click() { if (callback) callback(); }
	bool send_keypress(int key_code);
};


