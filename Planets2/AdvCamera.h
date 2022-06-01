#pragma once

#include "raylib.h"

enum class Direction {
	LEFT,
	UP,
	RIGHT,
	DOWN
};

// A wrapper around the Raylib Camera2D struct. Adding functionality for zooming and setting target and offset velocity.
class AdvCamera
{
	/*
	Camera2D has two separate Vector2's, offset and target.

	The target is the position in the world that the camera is focused around.
	It uses world coordinates.

	The offset is the position on screen that the target should be shown at.
	(0,0) = top left
	(screen_width, screen_height) = bottom right
	It uses flat pixel coordinates.


	As such, they can't use the same cam_speed most of the time.
	Zooming doesn't affect offset velocity, but it will affect target velocity.\

	Zooming is centered on the target.
	So, an offset of 0,0, placing the target at the upper left corner, will zoom in/out there.


	*/

	Camera2D camera;
	Rectangle offset_bounds;
	 
	int target_speed_multiplier;
	float target_speed;

	float offset_speed = 5;
	static constexpr int OFFSET_INCREMENT = 2;

	// Need to recalculate the target speed on zoom in and out.
	void recalculate_speed_target();

public:

	AdvCamera() = default;
	AdvCamera(Vector2& offset, Vector2& target);
	AdvCamera(Vector2& offset, Vector2& target, int target_speed_multiplier);


	AdvCamera(Vector2&& offset, Vector2&& target);
	AdvCamera(Vector2&& offset, Vector2&& target, int target_speed_multiplier);

	void move_target(Direction dir);
	void move_offset(Direction dir);

	void move_target(Vector2 vec);
	void move_offset(Vector2 vec);

	void set_target(Vector2 vec);
	void set_offset(Vector2 vec);

	void set_offset_bounds(float min_x, float min_y, float width, float height);


	void zoom_in();
	void zoom_out();


	// Increases the rate at which this camera will travel when move_target called with a direction.
	void increase_speed_target();
	// Decreases the rate at which this camera will travel when move_target called with a direction.
	void decrease_speed_target();

	// Increases the rate at which this camera will travel when move_offset called with a direction.
	void increase_speed_offset();
	// Decreases the rate at which this camera will travel when move_offset called with a direction.
	void decrease_speed_offset();

	void set_zoom(float level);

	float get_zoom() const { return camera.zoom; }


	// Returns the underlying raylib Camera2D struct of this camera.
	const Camera2D& get_raylib_camera() const { return camera; }

};

