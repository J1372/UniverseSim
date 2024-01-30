#pragma once

#include "UIElement.h"
#include <raylib.h>
#include <functional>

class CheckBox : public UIElement
{
	std::string description;
	int font_size = 20;

	Rectangle rect;

	Color background_color = BLACK;
	Color check_color = RED;


	static constexpr float padding = .3f;

	bool checked = false;


	std::function<void(bool)> callback = nullptr;

public:

	CheckBox(std::string_view description, float x, float y, float size);


	bool is_checked() const;

	void click() override;
	bool contains_point(Vector2 point) const override;
	void render() const override;

	void set_desc_font_size(int to_set);
	void set_on_click(std::function<void(bool)> action);

};
