#include "Physics.h"
#include "Collision.h"

#include "Body.h"
	
// Collision event observers to be called when a collision happens.
Event<Collision> on_collision_observers;

Event<Collision>& Physics::collision_event()
{
	return on_collision_observers;
}

bool Physics::have_collided(const Body& body1, const Body& body2)
{
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

bool Physics::body_intersects_rect(const Body& body, Rectangle rect)
{
	float dist_x = std::abs(body.x - rect.x - rect.width / 2);
	float dist_y = std::abs(body.y - rect.y - rect.width / 2);

	if (dist_x > (rect.width / 2 + body.radius)) { return false; }
	if (dist_y > (rect.height / 2 + body.radius)) { return false; }

	if (dist_x <= (rect.width / 2)) { return true; }
	if (dist_y <= (rect.height / 2)) { return true; }

	int corner_dist = std::pow((dist_x - rect.width / 2), 2) + std::pow((dist_y - rect.height / 2), 2);

	return corner_dist <= std::pow(body.radius, 2);
}