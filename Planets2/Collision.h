#pragma once

#include <utility>

class Body;

// Represents a collision event between two bodies.
struct Collision {

	Body& bigger;
	Body& smaller;

	Collision(std::pair<Body*, Body*> collided);

};
