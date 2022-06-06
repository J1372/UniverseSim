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

	// Returns true if a point is in a circle.
	bool point_in_circle(Vector2 point, float circle_x, float circle_y, float radius);
	// Returns true if a point is in a circle.
	bool point_in_circle(Vector2 point, Circle circle);

	// Returns true if a point is inside a rectangle.
	bool point_in_rect(Vector2 point, Rectangle rect);

	// Returns true if a body is completely inside a rectangle.
	bool body_inside_rect(const Body& body, Rectangle rect);
	// Returns true if a circle is completely inside a rectangle.
	bool circle_inside_rect(Circle circle, Rectangle rect);

	// Returns true if a body intersects a rectangle.
	bool body_intersects_rect(const Body& body, Rectangle rect);
	// Returns true if a circle intersects a rectangle.
	bool circle_intersects_rect(Circle circle, Rectangle rect);

	// Returns the scalar distance between two points.
	float dist(Vector2 point1, Vector2 point2);

	// Returns the vector distance between two points in terms of (x,y) directions.
	std::array<float, 2> distv(Vector2 point1, Vector2 point2);

	// Returns the scalar net force applied by each body to each other.
	float net_force(const Body& body1, const Body& body2, float grav_const);

	// Returns the scalar net force applied by a point mass to a body.
	float net_force(const Body& body, Vector2 center_mass, long point_mass, float grav_const);

	// Applies a gravitational force on each body to each other.
	void grav_pull(Body& body1, Body& body2, float grav_const);

	// Applies a gravitational force from the point mass to the body.
	void grav_pull(Body& body, Vector2 center_mass, long point_mass, float grav_const);

}
