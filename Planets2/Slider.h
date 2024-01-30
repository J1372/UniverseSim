#pragma once
#include "UIElement.h"

#include "raylib.h"
#include "Circle.h"
#include <string>

class Slider : public UIElement
{

	Circle slider_node;
	Rectangle rail;
	static constexpr float RAIL_HEIGHT = 10;
	static constexpr float NODE_RADIUS = RAIL_HEIGHT;

	Color rail_color = GRAY;
	Color slider_color = DARKGRAY;

	float cur_val;
	float min_val;
	float max_val;
	std::string cur_val_str; // cached, updated on val change instead of recalc on render.


	float range() const;

	// Returns a clamped percentage of an x as it relates to the rail.
	float get_percentage(float x);

	// translates a point on the rail (represented by a percentage range [0, 1]) to a value.
	float translate_percent(float percentage) const;

	void move_slider();

public:

	Slider(float x, float y, float width, float min_val, float max_val);

	float get_val() const;
	void set_val(float val);

	void click() override;
	void notify_drag() override;
	bool contains_point(Vector2 point) const override;
	void render() const override;

};

