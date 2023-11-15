#include "Physics.h"
#include "Circle.h"
#include <raymath.h>
#include <cmath>
#include <algorithm>

bool Physics::point_in_circle(Vector2 point, float circle_x, float circle_y, float radius)
{
	float dist_squared = Physics::dist_squared(point, { circle_x, circle_y });
	return dist_squared <= radius * radius;
}

bool Physics::point_in_circle(Vector2 point, Circle circle)
{
	float dist_squared = Physics::dist_squared(point, circle.center);
	return dist_squared <= circle.radius * circle.radius;
}

bool Physics::point_in_rect(Vector2 point, Rectangle rect)
{
	return point.x >= rect.x and point.x < rect.x + rect.width and
		point.y >= rect.y and point.y < rect.y + rect.height;
}

bool Physics::circles_intersect(Circle c1, Circle c2)
{
	float dist_squared = Physics::dist_squared(c1.center, c2.center);
	float combined_radius = c1.radius + c2.radius;
	return dist_squared < std::pow(combined_radius, 2);
}

bool Physics::circle_inside_rect(Circle circle, Rectangle rect)
{
	return circle.top() >= rect.y and circle.bottom() <= rect.y + rect.height and
		circle.left() >= rect.x and circle.right() <= rect.x + rect.width;
}

bool Physics::circle_intersects_rect(Circle circle, Rectangle rect)
{
	float radius = circle.radius;

	float right = rect.x + rect.width;
	float bottom = rect.y + rect.height;

	float closest_x = std::clamp(circle.center.x, rect.x, right);
	float closest_y = std::clamp(circle.center.y, rect.y, bottom);

	float dist_squared = Physics::dist_squared(circle.center, { closest_x, closest_y });
	return dist_squared < (radius * radius);
}

float Physics::dist(Vector2 point1, Vector2 point2)
{
	return std::sqrt(Physics::dist_squared(point1, point2));
}

float Physics::dist_squared(Vector2 point1, Vector2 point2)
{
	Vector2 vector = Physics::distv(point1, point2);
	float c_squared = std::pow(vector.x, 2) + std::pow(vector.y, 2);
	return c_squared;
}

Vector2 Physics::distv(Vector2 point1, Vector2 point2)
{
	return Vector2Subtract(point2, point1);
}

float Physics::net_force(Vector2 p1, long m1, Vector2 p2, long m2)
{
	float dist = Physics::dist_squared(p1, p2);

	// Dist >= 0. Could add a very small number to dist to prevent division by 0. and avoid if check.
	if (dist == 0)
	{
		// Avoid division by 0.
		return 0.0f;
	}

	return (static_cast<float>(m1) * m2) / dist;
}

Vector2 Physics::grav_force(Vector2 p1, long m1, Vector2 p2, long m2)
{
	float force = Physics::net_force(p1, m1, p2, m2);

	Vector2 dist = Physics::distv(p1, p2);
	float theta = atan2(dist.y, dist.x); // radians

	return { (float)(force * cos(theta)), (float)(force * sin(theta)) };
}
