#pragma once
#include <vector>

class Camera2D;
class Body;

class SpatialPartitioning
{
public:

	virtual void add_body(Body& body) = 0;
	virtual void collision_check(std::vector<Body*>& to_remove) = 0;
	virtual void update() = 0; // Update after moving all bodies.
	virtual void draw_debug(const Camera2D& camera) const = 0;
	virtual void attach_debug_text(Body& body) const = 0;

	virtual ~SpatialPartitioning() = default;
};

