#include "Physics.h"
#include "Collision.h"

#include "Body.h"
#include "Circle.h"

bool Physics::have_collided(const Body& body1, const Body& body2)
{
	float c_squared = std::pow(body2.x - body1.x, 2) + std::pow(body2.y - body1.y, 2);
	return c_squared < std::pow((body2.radius + body1.radius), 2);
}

bool Physics::point_in_circle(Vector2 point, float circle_x, float circle_y, float radius)
{
	float dist_x = point.x - circle_x;
	float dist_y = point.y - circle_y;

	float dist_squared = std::pow(dist_x, 2) + std::pow(dist_y, 2);
	float dist = std::sqrt(dist_squared); // can compare to radius^2 instead.

	return dist <= radius;
}

bool Physics::point_in_circle(Vector2 point, Circle circle)
{
	float dist_x = point.x - circle.center.x;
	float dist_y = point.y - circle.center.y;

	float dist_squared = std::pow(dist_x, 2) + std::pow(dist_y, 2);
	float dist = std::sqrt(dist_squared); // can compare to radius^2 instead.

	return dist <= circle.radius;
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
	float dist_x = std::abs(body.x - rect.x - rect.width / 2);
	float dist_y = std::abs(body.y - rect.y - rect.height / 2);

	if (dist_x > (rect.width / 2 + body.radius)) { return false; }
	if (dist_y > (rect.height / 2 + body.radius)) { return false; }

	if (dist_x <= (rect.width / 2)) { return true; }
	if (dist_y <= (rect.height / 2)) { return true; }

	int corner_dist = std::pow((dist_x - rect.width / 2), 2) + std::pow((dist_y - rect.height / 2), 2);

	return corner_dist <= std::pow(body.radius, 2);
}

bool Physics::circle_intersects_rect(Circle circle, Rectangle rect)
{
	float dist_x = std::abs(circle.center.x - rect.x - rect.width / 2);
	float dist_y = std::abs(circle.center.y - rect.y - rect.height / 2);

	if (dist_x > (rect.width / 2 + circle.radius)) { return false; }
	if (dist_y > (rect.height / 2 + circle.radius)) { return false; }

	if (dist_x <= (rect.width / 2)) { return true; }
	if (dist_y <= (rect.height / 2)) { return true; }

	int corner_dist = std::pow((dist_x - rect.width / 2), 2) + std::pow((dist_y - rect.height / 2), 2);

	return corner_dist <= std::pow(circle.radius, 2);
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
	float dist = body1.dist(body2);
	if (dist == 0) {
		return 0.0f;
	}

	return (grav_const * body1.mass * body2.mass) / std::pow(dist, 2);
}

float Physics::net_force(const Body& body, Vector2 center_mass, long point_mass, float grav_const)
{
	float dist = Physics::dist({ body.x, body.y }, center_mass);
	if (dist == 0) {
		return 0.0f;
	}

	return (grav_const * body.mass * point_mass) / std::pow(dist, 2);
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

	std::array<float, 2> dist_vector = Physics::distv({ body.x, body.y }, center_mass);

	float theta = atan2(dist_vector[0], dist_vector[1]); // radians

	std::array<float, 2> force_vectors{ (float)(force * sin(theta)), (float)(force * cos(theta)) };

	body.grav_pull(force_vectors);

}
