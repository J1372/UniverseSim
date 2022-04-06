#pragma once
#include <vector>
class Body;

class SpatialPartitioning
{
public:
	virtual void add_body(Body& body) = 0;
	virtual void collision_check(std::vector<Body> to_remove) = 0;
	virtual void update() = 0; // Update after moving all bodies.
};

