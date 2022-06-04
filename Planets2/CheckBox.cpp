#include "CheckBox.h"

CheckBox::CheckBox(const std::string& description, float x, float y, float size) :
	description(description), rect{x, y, size, size}
{
}

bool CheckBox::is_checked() const
{
	return checked;
}

void CheckBox::click()
{
	checked = !checked;

	if (checked) {
		check_color = GREEN;
	}
	else {
		check_color = RED;
	}

	if (callback) {
		callback(checked);
	}
}

bool CheckBox::contains_point(Vector2 point) const
{
	return CheckCollisionPointRec(point, rect);
}

void CheckBox::render() const
{
	DrawRectangle(rect.x, rect.y, rect.width, rect.height, background_color);

	DrawText(description.c_str(), rect.x + rect.width + 10, rect.y + .1 * rect.height, 20, BLACK);

	float start_x = rect.x + .5 * padding * rect.width;
	float start_y = rect.y + .5 * padding * rect.height;
	float width = (1 - padding) * rect.width;
	float height = (1 - padding) * rect.height;

	DrawRectangle(start_x, start_y, width, height, check_color);
}

void CheckBox::set_on_click(std::function<void(bool)> action)
{
	callback = action;
}
