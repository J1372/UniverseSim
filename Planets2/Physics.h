#pragma once
#include "Event.h"

class Body;
struct Collision;
struct Rectangle;
struct Vector2;
struct Circle;

namespace Physics {

	// Returns true if two bodies are colliding, else false.
	bool have_collided(const Body& body1, const Body& body2);

	bool point_in_circle(Vector2 point, float circle_x, float circle_y, float radius);
	bool point_in_circle(Vector2 point, Circle circle);

	// Returns true if a point is inside a rectangle.
	bool point_in_rect(Vector2 point, Rectangle rect);

	// Returns true if a body is completely inside a rectangle.
	bool body_inside_rect(const Body& body, Rectangle rect);
	bool circle_inside_rect(Circle circle, Rectangle rect);

	// Returns true if a body intersects a rectangle.
	bool body_intersects_rect(const Body& body, Rectangle rect);
	bool circle_intersects_rect(Circle circle, Rectangle rect);

	float dist(Vector2 point1, Vector2 point2);
	std::array<float, 2> distv(Vector2 point1, Vector2 point2);

	float net_force(const Body& body1, const Body& body2, float grav_const);
	float net_force(const Body& body, Vector2 center_mass, long point_mass, float grav_const);

	void grav_pull(Body& body1, Body& body2, float grav_const);
	void grav_pull(Body& body, Vector2 center_mass, long point_mass, float grav_const);

}
