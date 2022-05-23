#pragma once
#include <vector>

struct Rectangle;
struct Vector2;
class Body;
struct Collision;

class SpatialPartitioning
{
public:

	virtual void add_body(Body& body) = 0;
	virtual void rem_body(const Body& body) = 0;
	virtual void update() = 0; // Update after moving all bodies.

	virtual Body* find_body(Vector2 point) const = 0;

	virtual std::vector<Rectangle> get_representation() const = 0;
	virtual void attach_debug_text(Body& body) const = 0;

	// Scans for and returns a vector of Collision events.
	virtual std::vector<Collision> get_collisions() const = 0;

	virtual ~SpatialPartitioning() = default;
};

