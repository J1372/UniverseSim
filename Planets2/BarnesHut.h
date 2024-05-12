#pragma once

#include "raylib.h"
#include <span>
#include "QuadChildren.h"

class Body;

class BarnesHutNode
{
	// Standard Barnes-Hut quadtree:
	// A node can only have 0 or 1 body.
	// If a node has a body, it cannot be a parent.
	// Each non-empty quad is a leaf.

	// Leaf nodes modified to hold more than 1 body
	//	to reduce depth of result tree.

	// Union between leaf node info and parent node info.
	// Could use variant.
	static constexpr int NODE_MAX_CAP = 8;
	union U
	{
		// The center of mass of all bodies in and below this node.
		struct
		{
			Vector2 center_of_mass;
			long mass_sum;
		} parent;

		struct
		{
			std::array<std::pair<Vector2, long>, NODE_MAX_CAP> point_masses;
			int num_bodies;

			std::span<const std::pair<Vector2, long>> data_to_span() const
			{
				return { point_masses.data(), static_cast<std::size_t>(num_bodies) };
			}
		} leaf;


		U() { memset(this, 0, sizeof(U)); }

	} leaf_or_parent;


	// Area of this quad node.
	Rectangle dimensions {};

	// The node's 4 potential children.
	std::unique_ptr<QuadChildren<BarnesHutNode>> children;

	// Returns the width of this quad divided by the distance from its center of mass to the body.
	float dist_ratio_sq(Vector2 point) const;

	// Returns whether a body is so far away from the node's center of mass,
	// that calculating the node's grav pull on the body should stop early.
	bool sufficiently_far(Vector2 point, float approximation_value_sq) const;

	// Handles calculations for updating this node's mass sum and center of mass when a new point mass is added.
	void update_mass_add(Vector2 center, long mass);

	// Will add a point mass to the most appropriate quad node.
	// Potentially splits that node if it reached its capacity.
	void add_body(Vector2 center, long mass);

	// Adds the point mass to the correct child quad.
	void add_to_child(Vector2 center, long mass);

	// Returns true if this is a leaf node.
	bool is_leaf() const;

	// Returns true if a point is inside the quad's dimensions.
	bool contains(Vector2 point) const;

	// Creates 4 new child nodes.
	// The current point mass is moved into the child node that contains it.
	void split();

	// Frees 4 child nodes.
	void concatenate();

public:

	BarnesHutNode() = default;
	BarnesHutNode(float x, float y, float size);

	// Uses Barnes-Hut approximation to calculate and return the gravitational force vector applied to the body.
	Vector2 force_applied_to(Vector2 point, long mass, float approximation_value_sq) const;

	// Rebuilds the quadtree used for Barnes-Hut approximation.
	void update(std::span<const Body> bodies);

};

// A specialized quad tree used in the Barnes Hut algorithm for approximating gravity simulation.
class BarnesHut
{
	BarnesHutNode root;

	// Used in determining whether a body is sufficiently far from a node's center of mass.
	// A higher approximation will result in less accuracy.
	float approximation_value;
	float approximation_value_squared; // cache to use for updates.

public:

	BarnesHut(float size, float approximation_value); // can have max depth just in case

	// Uses Barnes-Hut approximation to calculate and return the gravitational force vector applied to the body.
	Vector2 force_applied_to(const Body& body) const;

	// Rebuilds the quadtree used for Barnes-Hut approximation.
	void update(std::span<const Body> bodies)
	{
		root.update(bodies);
	}

};
