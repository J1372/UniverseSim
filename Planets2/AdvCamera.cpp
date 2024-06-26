#include "AdvCamera.h"
#include <utility>
#include <algorithm>
#include "Body.h"


void AdvCamera::recalculate_speed_target()
{
	target_speed = 5 * target_speed_multiplier / camera.zoom;
}

AdvCamera::AdvCamera(Vector2 offset, Vector2 target) : target_speed_multiplier(8), offset_bounds{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) }
{
	camera.offset = offset;
	camera.target = target;
	camera.zoom = 1.0f;
	camera.rotation = 0.0f;
	recalculate_speed_target();
}

void AdvCamera::zoom_in()
{
	camera.zoom *= 2;
	recalculate_speed_target();
}

void AdvCamera::zoom_out()
{ 
	camera.zoom /= 2;
	recalculate_speed_target();
}

void AdvCamera::set_zoom(float level)
{
	camera.zoom = level;
	recalculate_speed_target();
}

void AdvCamera::increase_speed_target()
{
	target_speed_multiplier++;
	recalculate_speed_target();
}

void AdvCamera::decrease_speed_target()
{
	if (target_speed_multiplier > 1) {
		target_speed_multiplier--;
		recalculate_speed_target();
	}
}

void AdvCamera::increase_speed_offset()
{
	offset_speed += OFFSET_INCREMENT;
}

void AdvCamera::decrease_speed_offset()
{
	// Prevent movement from going dropping to zero or going negative.
	if (offset_speed > OFFSET_INCREMENT) {
		offset_speed -= OFFSET_INCREMENT;
	}
}

float AdvCamera::get_zoom() const
{
	return camera.zoom;
}

bool AdvCamera::in_view(const Body& body) const
{
	Vector2 pos = body.pos();

	Vector2 leftmost = GetWorldToScreen2D({ body.left(), pos.y }, camera);
	Vector2 rightmost = GetWorldToScreen2D({ body.right(), pos.y }, camera);

	float screen_radius = (rightmost.x - leftmost.x) / 2.0f;
	float center_x = leftmost.x + screen_radius;

	Vector2 lowest { center_x, leftmost.y + screen_radius };
	Vector2 highest { center_x, leftmost.y - screen_radius };

	// can optimize : screen_pos.x >= -body.radius && screen_pos.y >= -body.radius

	return rightmost.x >= 0 and lowest.y >= 0 and leftmost.x < GetScreenWidth() and highest.y < GetScreenHeight();
}

bool AdvCamera::in_view(Rectangle rect) const
{
	Vector2 UL = GetWorldToScreen2D({ rect.x, rect.y }, camera);
	Vector2 LR = GetWorldToScreen2D({ rect.x + rect.width, rect.y + rect.height }, camera);
	float width = LR.x - UL.x;
	float height = LR.y - UL.y;

	return UL.x < GetScreenWidth() and UL.y < GetScreenHeight()
		and LR.x >= 0 and LR.y >= 0;
}

Rectangle AdvCamera::get_view() const
{
	Vector2 UL = GetScreenToWorld2D({ 0.0f, 0.0f }, camera);
	Vector2 LR = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, camera);
	float width = LR.x - UL.x;
	float height = LR.y - UL.y;

	return { UL.x, UL.y, width, height };
}

const Camera2D& AdvCamera::get_raylib_camera() const
{
	return camera;
}

void AdvCamera::move_target(Direction dir)
{
	switch (dir) {
	case Direction::LEFT:
		camera.target.x -= target_speed;
		break;
	case Direction::UP:
		camera.target.y -= target_speed;
		break;
	case Direction::RIGHT:
		camera.target.x += target_speed;
		break;
	case Direction::DOWN:
		camera.target.y += target_speed;
		break;
	}
}

void AdvCamera::move_offset(Direction dir)
{
	switch (dir) {
	case Direction::LEFT:
		camera.offset.x = std::max(camera.offset.x - offset_speed, offset_bounds.x);
		break;
	case Direction::UP:
		camera.offset.y = std::max(camera.offset.y - offset_speed, offset_bounds.y);
		break;
	case Direction::RIGHT:
		camera.offset.x = std::min(camera.offset.x + offset_speed, offset_bounds.width);
		break;
	case Direction::DOWN:
		camera.offset.y = std::min(camera.offset.y + offset_speed, offset_bounds.height);
		break;
	}
}

void AdvCamera::move_target(Vector2 vec)
{
	camera.target.x += vec.x;
	camera.target.y += vec.y;
}

void AdvCamera::move_offset(Vector2 vec)
{
	Vector2 new_pos{ camera.offset.x + vec.x , camera.offset.y + vec.y };
	set_offset(new_pos);
}

void AdvCamera::set_target(Vector2 vec)
{
	camera.target = vec;
}

void AdvCamera::set_offset(Vector2 vec)
{
	camera.offset.x = std::clamp(vec.x, offset_bounds.x, offset_bounds.width);
	camera.offset.y = std::clamp(vec.y, offset_bounds.y, offset_bounds.height);
}

void AdvCamera::set_offset_bounds(float min_x, float min_y, float width, float height)
{
	offset_bounds.x = min_x;
	offset_bounds.y = min_y;
	offset_bounds.width = width;
	offset_bounds.height = height;

	// If the current offset is now out of bounds, will move it back.
	set_offset(camera.offset);
}
