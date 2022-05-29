#pragma once
#include "Event.h"

class Body;
struct Collision;
struct Rectangle;
struct Vector2;

namespace Physics {

	// Returns a reference to the collision events observer pool.
	Event<Collision>& collision_event();

	// Returns true if two bodies are colliding, else false.
	bool have_collided(const Body& body1, const Body& body2);

	// Checks if two bodies have collided, and notifies all collision observers if they have.
	void handle_collision(Body& body1, Body& body2);

	bool point_in_circle(Vector2 point, float circle_x, float circle_y, float radius);

	// Returns true if a point is inside a rectangle.
	bool point_in_rect(Vector2 point, Rectangle rect);

	// Returns true if a body is completely inside a rectangle.
	bool body_inside_rect(const Body& body, Rectangle rect);

	// Returns true if a body intersects a rectangle.
	bool body_intersects_rect(const Body& body, Rectangle rect);
}
