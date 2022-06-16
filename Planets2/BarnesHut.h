#pragma once

#include "raylib.h"
#include <memory>
#include <functional>
#include <span>
#include <array>

class Body;

// A specialized quad tree used in the Barnes Hut algorithm for approximating gravity simulation.
class BarnesHut
{
	// Used in determining whether a body is sufficiently far from a node's center of mass.
	// A higher approximation will result in less accuracy.
	static float approximation_value;

	// A quad can only have 0 or 1 body.
	// If a quad has a body, it cannot be a parent.
	// Each non-empty quad is a leaf.

	Rectangle dimensions{};

	// The node's 4 potential children.
	std::unique_ptr<std::array<BarnesHut, 4>> children = nullptr;
	// For this quad tree implementation in particular (Maximum 1 body per node, AND rebuilt on tick), 
	// should look into object pooling.


	// The center of mass of all bodies in and below this node.
	Vector2 center_of_mass {0,0};

	// The mass of all bodies in and below this node.
	long mass_sum = 0l;

	// Returns the width of this quad divided by the distance from its center of mass to the body.
	float dist_ratio(const Body& body) const;

	// Returns whether a body is so far away from the node's center of mass,
	// that calculating the node's grav pull on the body should stop early.
	bool sufficiently_far(const Body& body) const;

	Vector2 calc_force(const Body& body) const;

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

	// Returns the upper left child quad node.
	BarnesHut& UL() const { return (*children)[0]; }
	// Returns the upper right child quad node.
	BarnesHut& UR() const { return (*children)[1]; }
	// Returns the lower left child quad node.
	BarnesHut& LL() const { return (*children)[2]; }
	// Returns the lower right child quad node.
	BarnesHut& LR() const { return (*children)[3]; }

	/*
	Returns the first (and hopefully only) child quad where the predicate is true, or nullptr if predicate false in all four child nodes.
	Checks in order: UL, UR, LL, LR.
	*/
	template<auto bool_func, class... ArgTypes>
	BarnesHut* get_quad(ArgTypes&&... args) const
	{
		// Assert provided bool_func returns a bool when called on a BarnesHut with args... parameter types.
		static_assert(std::is_invocable_r_v<bool, decltype(bool_func), BarnesHut&&, ArgTypes&&...>,
			"Given function must return a bool when called on a BarnesHut with the given parameters.");
		
		for (BarnesHut& child : *children) {
			if (std::invoke(bool_func, child, args...)) {
				return &child;
			}
		}

		return nullptr;
	}

};
