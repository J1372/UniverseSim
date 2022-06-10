#include "Slider.h"
#include <algorithm>

float Slider::range() const
{
	return max_val - min_val;
}

float Slider::get_percentage(float x)
{
	float on_slider_x = x - rail.x;
	float x_percentage = on_slider_x / rail.width;

	return x_percentage;
}

float Slider::translate_percent(float percentage) const
{
	return percentage * range() + min_val;
}

void Slider::move_slider()
{
	float percent_point = (cur_val - min_val) / range();
	slider_node.center.x = rail.x + percent_point * rail.width;
}

Slider::Slider(float x, float y, float width, float min_val, float max_val) :
	rail { x, y, width, RAIL_HEIGHT }, slider_node{ x, y + RAIL_HEIGHT / 2, NODE_RADIUS },
	min_val(min_val), max_val(max_val)
{
	set_val(min_val);
}

float Slider::get_val() const
{
	return cur_val;
}

void Slider::set_val(float val)
{
	cur_val = std::min(val, max_val);
	cur_val = std::max(cur_val, min_val);
	cur_val_str = std::to_string(cur_val);
}

void Slider::click()
{
	float percent_point = get_percentage(GetMouseX());
	set_val(translate_percent(percent_point));
	move_slider();
}

void Slider::notify_drag()
{
	float percent_point = get_percentage(GetMouseX());
	set_val(translate_percent(percent_point));
	move_slider();
}

bool Slider::contains_point(Vector2 point) const
{
	// return true if on rail or if in circle node
	bool on_rail = CheckCollisionPointRec(point, rail);
	bool on_slider = CheckCollisionPointCircle(point, slider_node.center, slider_node.radius);

	return on_rail or on_slider;
}

void Slider::render() const
{
	DrawRectangleRec(rail, rail_color);
	DrawCircleV(slider_node.center, slider_node.radius, slider_color);
	DrawText(cur_val_str.c_str(), slider_node.center.x - 30, slider_node.center.y - 30, 20, BLACK);
}
