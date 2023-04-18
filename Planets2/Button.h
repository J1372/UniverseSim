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

	float get_default_width() const;

public:

	Button(const std::string& text, float x, float y);

	Button(const std::string& text, float x, float y, int font_size);

	// Sets width of background to be at least min_width.
	void set_min_width(float min_width);

	// Sets function to call when button clicked.
	void set_on_action(std::function<void(void)> on_action);

	bool contains_point(Vector2 point) const;

	void render() const;

	void click();
	bool send_keypress(int key_code);
};
