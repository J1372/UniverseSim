#pragma once
#include <vector>

struct Rectangle;
struct Vector2;
class Body;
struct Collision;

// An interface for spatial partitionings thatt can be used for collision detection.
class SpatialPartitioning
{
protected:

	// Number of collision checks that were performed last tick.
	int num_collision_checks_tick = 0;

public:

	// Adds a body to be checked for collision.
	virtual void add_body(Body& body) = 0;

	// Removes a body from the partitioning.
	virtual void rem_body(const Body& body) = 0;

	// Updates the partitioning to reflect new positions.
	virtual void update() = 0;

	// Tries to find and return the body that overlaps with the point, returns nullptr if none found.
	virtual Body* find_body(Vector2 point) const = 0;

	// Returns a representation of the partitioning method.
	virtual std::vector<Rectangle> get_representation() const = 0;

	// Attaches debug information related to the partitioning method to the body.
	virtual void attach_debug_text(Body& body) const = 0;

	// Scans for and returns a vector of Collision events.
	virtual std::vector<Collision> get_collisions() = 0;

	// Returns the number of collision checks performed after previous call to get_collisions.
	int get_collision_checks_this_tick() const { return num_collision_checks_tick; }

	virtual ~SpatialPartitioning() = default;
};

