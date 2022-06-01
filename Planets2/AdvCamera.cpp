#include "AdvCamera.h"
#include <utility>


void AdvCamera::recalculate_speed_target()
{
	target_speed = 5 * target_speed_multiplier / camera.zoom;
}

AdvCamera::AdvCamera(Vector2& offset, Vector2& target) : target_speed_multiplier(8), offset_bounds{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) }
{
	camera.offset = offset;
	camera.target = target;
	camera.zoom = 1.0f;
	camera.rotation = 0.0f;
	recalculate_speed_target();
}

AdvCamera::AdvCamera(Vector2& offset, Vector2& target, int target_speed_multiplier) : target_speed_multiplier(target_speed_multiplier), offset_bounds{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) }
{
	camera.offset = offset;
	camera.target = target;
	camera.zoom = 1.0f;
	camera.rotation = 0.0f;
	recalculate_speed_target();
}

AdvCamera::AdvCamera(Vector2&& offset, Vector2&& target) : target_speed_multiplier(8), offset_bounds{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) }
{
	camera.offset = std::move(offset);
	camera.target = std::move(target);
	camera.zoom = 1.0f;
	camera.rotation = 0.0f;
	recalculate_speed_target();
}

AdvCamera::AdvCamera(Vector2&& offset, Vector2&& target, int target_speed_multiplier) : target_speed_multiplier(target_speed_multiplier), offset_bounds{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) }
{
	camera.offset = std::move(offset);
	camera.target = std::move(target);
	camera.zoom = 1.0f;
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

void AdvCamera::set_zoom(float level)
{
	camera.zoom = level;
	recalculate_speed_target();
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
	camera.offset.x += vec.x;
	camera.offset.y += vec.y;
}

void AdvCamera::set_target(Vector2 vec)
{
	camera.target = vec;
}

void AdvCamera::set_offset(Vector2 vec)
{
	camera.offset = vec;
}

void AdvCamera::set_offset_bounds(float min_x, float min_y, float width, float height)
{
	offset_bounds.x = min_x;
	offset_bounds.y = min_y;
	offset_bounds.width = width;
	offset_bounds.height = height;
}
