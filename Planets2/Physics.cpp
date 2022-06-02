#include "Physics.h"
#include "Collision.h"

#include "Body.h"
#include "Circle.h"
	
// Collision event observers to be called when a collision happens.
Event<Collision> on_collision_observers;

Event<Collision>& Physics::collision_event()
{
	return on_collision_observers;
}

bool Physics::have_collided(const Body& body1, const Body& body2)
{
	// can remove abs
	float c_squared = std::pow(std::abs(body2.x - body1.x), 2) + std::pow(std::abs(body2.y - body1.y), 2);
	return c_squared < std::pow((body2.radius + body1.radius), 2);
}

void Physics::handle_collision(Body& body1, Body& body2)
{
	if (have_collided(body1, body2)) {
		Collision collision { Body::get_sorted_pair(body1, body2) };

		on_collision_observers.notify_all(collision);
	}
}

bool Physics::point_in_circle(Vector2 point, float circle_x, float circle_y, float radius)
{
	// Can remove absolute calls, since they are squared anyway.
	float dist_x = std::abs(point.x - circle_x);
	float dist_y = std::abs(point.y - circle_y);

	float dist_squared = std::pow(dist_x, 2) + std::pow(dist_y, 2);
	float dist = std::sqrt(dist_squared); // can compare to radius^2 instead.

	return dist <= radius;
}

bool Physics::point_in_circle(Vector2 point, Circle circle)
{
	// Can remove absolute calls, since they are squared anyway.
	float dist_x = std::abs(point.x - circle.center.x);
	float dist_y = std::abs(point.y - circle.center.y);

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
	// can remove abs
	float c_squared = std::pow(std::abs(point2.x - point1.x), 2) + std::pow(std::abs(point2.y - point1.y), 2);
	return std::sqrt(c_squared);
}

std::array<float, 2> Physics::distv(Vector2 point1, Vector2 point2)
{
	return { point2.x - point1.x , point2.y - point1.y };
}

float Physics::net_force(const Body& body1, const Body& body2, float grav_const)
{
	float dist = body1.dist_body(body2);
	return (grav_const * body1.mass * body2.mass) / std::pow(dist, 2);
}

float Physics::net_force(const Body& body, Vector2 center_mass, long point_mass, float grav_const)
{
	float dist = Physics::dist({ body.x, body.y }, center_mass);
	return (grav_const * body.mass * point_mass) / std::pow(dist, 2);
}

void Physics::grav_pull(Body& body1, Body& body2, float grav_const)
{	
	// can move this to Body, and have physics' just be net_force for center of masses.
	double force = Physics::net_force(body1, body2, grav_const);

	// may be able to optimize by computing force vectors directly.

	std::array<float, 2> dist_v = body1.distv_body(body2);

	float theta = atan2(dist_v[0], dist_v[1]); // radians

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

	std::array<float, 2> dist_v = Physics::distv({ body.x, body.y }, center_mass);

	float theta = atan2(dist_v[0], dist_v[1]); // radians

	std::array<float, 2> force_vectors{ (float)(force * sin(theta)), (float)(force * cos(theta)) };

	body.grav_pull(force_vectors);

}
