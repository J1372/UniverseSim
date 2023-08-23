#pragma once
#include <raylib.h>

struct Rectangle;
struct Circle;

namespace Physics {

	// Returns true if a point is in a circle.
	bool point_in_circle(Vector2 point, float circle_x, float circle_y, float radius);
	// Returns true if a point is in a circle.
	bool point_in_circle(Vector2 point, Circle circle);

	// Returns true if a point is inside a rectangle.
	bool point_in_rect(Vector2 point, Rectangle rect);

	// Returns true if a circle intersects another circle.
	bool circles_intersect(Circle c1, Circle c2);

	// Returns true if a circle is completely inside a rectangle.
	bool circle_inside_rect(Circle circle, Rectangle rect);

	// Returns true if a circle intersects a rectangle.
	bool circle_intersects_rect(Circle circle, Rectangle rect);

	// Returns the scalar distance between two points.
	float dist(Vector2 point1, Vector2 point2);

	// Returns the (scalar distance)^2 between two points.
	// Can be used to avoid sqrt call in places where you don't need the actual distance.
	float dist_squared(Vector2 point1, Vector2 point2);

	// Returns the distance vector between two points in terms of (x,y) directions.
	Vector2 distv(Vector2 point1, Vector2 point2);

	// Returns the scalar net force applied by the second point mass to the first point mass.
	float net_force(Vector2 p1, long m1, Vector2 p2, long m2, float grav_const);

	// Returns the force vector applied by the second point mass to the first point mass.
	Vector2 grav_force(Vector2 p1, long m1, Vector2 p2, long m2, float grav_const);

	// Returns the moment of a vector and a given quantity.
	template <typename T>
	Vector2 moment(Vector2 vector, T quantity)
	{
		return { vector.x * quantity, vector.y * quantity };
	}

}
