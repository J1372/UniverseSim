#pragma once
#include "Event.h"

class Body;
struct Collision;

namespace Physics {

	// Returns a reference to the collision events observer pool.
	Event<Collision>& collision_event();

	// Returns true if two bodies are colliding, else false.
	bool have_collided(const Body& body1, const Body& body2);

	// Checks if two bodies have collided, and notifies all collision observers if they have.
	void handle_collision(Body& body1, Body& body2);
}
