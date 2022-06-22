#pragma once

#include "raylib.h"

enum class Direction {
	LEFT,
	UP,
	RIGHT,
	DOWN
};

// A wrapper around the Raylib Camera2D struct.
// Adds functionality for zooming and setting target and offset velocity, as well as moving in a direction.
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


	As such, they can't use the same camera speed most of the time.
	Zooming doesn't affect offset velocity, but it will affect target velocity.

	Zooming is centered on the target.
	So, an offset of 0,0, placing the target at the upper left corner, will zoom in/out there.

	*/

	// Underlying Raylib camera.
	Camera2D camera;

	// Offset will not be able to go outside these bounds.
	Rectangle offset_bounds;
	
	// The current multiplier used in calculating the camera's target movement speed.
	int target_speed_multiplier;

	// The camera's target movement speed, in world coordinates
	float target_speed;


	// The camera's offset movement speed, in screen coordinates
	float offset_speed = 5;

	// Amount to change offset when given an offset move command.
	static constexpr int OFFSET_INCREMENT = 2;

	// Recalculates the target movement speed using the currently set multiplier and zoom level.
	void recalculate_speed_target();

public:

	AdvCamera() = default;
	AdvCamera(Vector2 offset, Vector2 target);

	// Moves the target position in a given direction using target movement speed.
	void move_target(Direction dir);

	// Moves the offset position in a given direction using offset movement speed.
	void move_offset(Direction dir);

	// Moves the target position by the given (x, y) vectors.
	void move_target(Vector2 vec);

	// Moves the offset position by the given (x, y) vectors.
	void move_offset(Vector2 vec);

	// Sets the target position to the given (x, y) vectors.
	void set_target(Vector2 vec);

	// Sets the offset position to the given (x, y) vectors.
	void set_offset(Vector2 vec);

	// Sets the offset bounds.
	void set_offset_bounds(float min_x, float min_y, float width, float height);

	// Zooms the camera in and updates target movement speed.
	void zoom_in();
	// Zooms the camera out and updates target movement speed.
	void zoom_out();
	// Sets the zoom level and updates target movement speed
	void set_zoom(float level);


	// Increases the rate at which this camera will travel when move_target called with a direction.
	void increase_speed_target();
	// Decreases the rate at which this camera will travel when move_target called with a direction.
	void decrease_speed_target();

	// Increases the rate at which this camera will travel when move_offset called with a direction.
	void increase_speed_offset();
	// Decreases the rate at which this camera will travel when move_offset called with a direction.
	void decrease_speed_offset();

	// Returns the current zoom level.
	float get_zoom() const;

	// Returns a reference to the underlying raylib Camera2D struct of this camera.
	const Camera2D& get_raylib_camera() const;

};

