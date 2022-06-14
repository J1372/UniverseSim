#include "Physics.h"
#include "Collision.h"

#include "Body.h"
#include "Circle.h"

bool Physics::have_collided(const Body& body1, const Body& body2)
{
	Vector2 pos1 = body1.pos();
	Vector2 pos2 = body2.pos();

	float dist_squared = Physics::dist_squared(pos1, pos2);
	float combined_radius = body1.get_radius() + body2.get_radius();
	return dist_squared < std::pow(combined_radius, 2);
}

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

bool Physics::body_inside_rect(const Body& body, Rectangle rect)
{
	return body.top() >= rect.y and body.bottom() <= rect.y + rect.height and
		body.left() >= rect.x and body.right() <= rect.x + rect.width;
}

bool Physics::circle_inside_rect(Circle circle, Rectangle rect)
{
	return circle.top() >= rect.y and circle.bottom() <= rect.y + rect.height and
		circle.left() >= rect.x and circle.right() <= rect.x + rect.width;
}

bool Physics::body_intersects_rect(const Body& body, Rectangle rect)
{
	Vector2 pos = body.pos();
	float radius = body.get_radius();

	float right = rect.x + rect.width;
	float bottom = rect.y + rect.height;

	float closest_x = std::clamp(pos.x, rect.x, right);
	float closest_y = std::clamp(pos.y, rect.y, bottom);

	float dist_squared = Physics::dist_squared(pos, { closest_x, closest_y });
	return dist_squared < (radius * radius);
}

bool Physics::circle_intersects_rect(Circle circle, Rectangle rect)
{
	float radius = circle.radius;

	float right = rect.x + rect.width;
	float bottom = rect.y + rect.height;

	float closest_x = std::clamp(circle.center.x, rect.x, right);
	float closest_y = std::clamp(circle.center.y, rect.y, bottom);

	float dist_squared = Physics::dist_squared(circle.center, { closest_x, closest_y });
	return dist_squared < (radius* radius);
}

float Physics::dist(Vector2 point1, Vector2 point2)
{
	float c_squared = std::pow(point2.x - point1.x, 2) + std::pow(point2.y - point1.y, 2);
	return std::sqrt(c_squared);
}

float Physics::dist_squared(Vector2 point1, Vector2 point2)
{
	float c_squared = std::pow(point2.x - point1.x, 2) + std::pow(point2.y - point1.y, 2);
	return c_squared;
}

std::array<float, 2> Physics::distv(Vector2 point1, Vector2 point2)
{
	return { point2.x - point1.x , point2.y - point1.y };
}

float Physics::net_force(const Body& body1, const Body& body2, float grav_const)
{
	float dist = body1.dist_squared(body2);
	
	// Dist >= 0. Could add a very small number to dist to prevent division by 0. and avoid if check.
	if (dist == 0) {
		// Avoid division by 0.
		return 0.0f;
	}

	return (grav_const * body1.get_mass() * body2.get_mass()) / dist;
}

float Physics::net_force(const Body& body, Vector2 center_mass, long point_mass, float grav_const)
{
	float dist = Physics::dist_squared(body.pos(), center_mass);
	
	// Dist >= 0. Could add a very small number to dist to prevent division by 0. and avoid if check.
	if (dist == 0) {
		// Avoid division by 0.
		return 0.0f;
	}

	return (grav_const * body.get_mass() * point_mass) / dist;
}

void Physics::grav_pull(Body& body1, Body& body2, float grav_const)
{	
	// can move this to Body, and have physics' just be net_force for center of masses.
	double force = Physics::net_force(body1, body2, grav_const);

	// may be able to optimize by computing force vectors directly.

	std::array<float, 2> dist_vector = body1.distv(body2);

	float theta = atan2(dist_vector[0], dist_vector[1]); // radians

	std::array<float, 2> force_vectors{ (float)(force * sin(theta)), (float)(force * cos(theta)) };

	body1.grav_pull(force_vectors);

	force_vectors[0] = -force_vectors[0];
	force_vectors[1] = -force_vectors[1];

	body2.grav_pull(force_vectors);

}

void Physics::grav_pull(Body& body, Vector2 center_mass, long point_mass, float grav_const)
{
	double force = Physics::net_force(body, center_mass, point_mass, grav_const);

	// may be able to optimize by computing force vectors directly.

	std::array<float, 2> dist_vector = Physics::distv(body.pos(), center_mass);

	float theta = atan2(dist_vector[0], dist_vector[1]); // radians

	std::array<float, 2> force_vectors{ (float)(force * sin(theta)), (float)(force * cos(theta)) };

	body.grav_pull(force_vectors);

}
