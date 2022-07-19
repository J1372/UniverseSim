#pragma once
#include <vector>
#include <string>


struct Rectangle;
struct Vector2;
class Body;
struct Collision;

class DebugInfo;

// An interface for spatial partitionings that can be used for collision detection.
// Supports implementations that rebuild on tick as well as those that update theirselves.
class SpatialPartitioning
{
protected:

	// Number of collision checks that were performed last tick.
	int num_collision_checks_tick = 0;

public:

	// Implementations that rebuild on tick must still implement add_body, rem_body.
	// If they do not, issues would occur when the simulation is paused.
	// examples:
	// 1. Representation not updating after creating a system until unpaused.
	// 2. find_body will not work correctly until unpaused.
	//		(user would be unable to attach camera to new body created while paused)

	// Adds a body to be checked for collision.
	virtual void add_body(Body& body) = 0;

	// Removes a body from the partitioning.
	virtual void rem_body(const Body& body) = 0;
	
	virtual void notify_move(const Body* from, Body* to) = 0;

	// Updates the partitioning to reflect new positions.
	virtual void update() = 0;

	// Tries to find and return the body that overlaps with the point, returns nullptr if none found.
	virtual Body* find_body(Vector2 point) const = 0;

	// Returns a representation of the partitioning method.
	virtual std::vector<Rectangle> get_representation() const = 0;

	// Adds information about the body as it relates to this partitioning method to info.
	virtual void get_info(const Body& body, DebugInfo& info) const = 0;

	// Scans for and returns a vector of Collision events.
	virtual std::vector<Collision> get_collisions() = 0;

	// Returns the number of collision checks performed after previous call to get_collisions.
	int get_collision_checks_this_tick() const { return num_collision_checks_tick; }

	virtual ~SpatialPartitioning() = default;
};

