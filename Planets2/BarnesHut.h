#pragma once

#include "raylib.h"
#include <memory>
#include <span>
#include "QuadChildren.h"
#include "DynamicPool.h"

class Body;

// A specialized quad tree used in the Barnes Hut algorithm for approximating gravity simulation.
class BarnesHut
{
	// Used in determining whether a body is sufficiently far from a node's center of mass.
	// A higher approximation will result in less accuracy.
	static float approximation_value;

	static DynamicPool<QuadChildren<BarnesHut>> quad_pool;

	// A quad can only have 0 or 1 body.
	// If a quad has a body, it cannot be a parent.
	// Each non-empty quad is a leaf.

	// The center of mass of all bodies in and below this node.
	Vector2 center_of_mass {0,0};

	// The mass of all bodies in and below this node.
	long mass_sum = 0l;

	// Area of this quad node.
	Rectangle dimensions{};

	// The node's 4 potential children.
	//std::unique_ptr<QuadChildren<BarnesHut>, PoolDeleter<QuadChildren<BarnesHut>>>
	//std::unique_ptr<T, PoolDeleter<T>>
	PoolPtr<QuadChildren<BarnesHut>> children {quad_pool};
	// For this quad tree implementation in particular (Maximum 1 body per node, AND rebuilt on tick), 
	// should look into object pooling.

	// Returns the width of this quad divided by the distance from its center of mass to the body.
	float dist_ratio(const Body& body) const;

	// Returns whether a body is so far away from the node's center of mass,
	// that calculating the node's grav pull on the body should stop early.
	bool sufficiently_far(const Body& body) const;

	// Handles calculations for updating this node's mass sum and center of mass when a new point mass is added.
	void update_mass_add(Vector2 center, long mass);


public:

	// Sets the approximation value to use in the Barnes Hut algorithm. Increasing approximation will improve performance and decrease accuracy.
	static void set_approximation(float to_set);

	BarnesHut() = default;

	BarnesHut(float size, float approximation_value); // can have max depth just in case

	// internal constructor. still needs to be public for make_unique.
	BarnesHut(float x, float y, float size);

	// Resizes this node's dimensions.
	void set_size(float size);

	// Will add a body to the most appropriate quad node. Potentially splits that node if it reached its capacity.
	void add_body(Body& to_add);

	// Will add a point mass to the most appropriate quad node. Potentially splits that node if it reached its capacity.
	void add_body(Vector2 center, long mass);

	// Adds the point mass to the correct child quad.
	void add_to_child(Vector2 center, long mass);

	// Returns true if this is a leaf node.
	bool is_leaf() const;
	
	// Uses Barnes-Hut approximation to calculate net gravitational force applied to the body.
	void handle_gravity(Body& body, float grav_const) const;

	// Rebuilds the quadtree used for Barnes-Hut approximation.
	void update(std::span<const std::unique_ptr<Body>> bodies);

	// Returns true if a point is inside the quad's dimensions.
	bool contains(Vector2 point) const;

	// Returns true if the node has no body in it.
	bool is_empty() const;

	// Returns true if the node has a body in it.
	bool is_full() const;

	// Creates 4 new child nodes. The current body is moved into the child node that contains it.
	void split();

	// Frees 4 child nodes.
	void concatenate();

};
