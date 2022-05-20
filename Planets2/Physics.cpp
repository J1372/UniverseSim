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
