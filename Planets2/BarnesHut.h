#pragma once

#include "raylib.h"
#include <memory>
#include <functional>
#include <span>

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

	Rectangle dimensions;

	// References to the parent and owning pointers to the node's 4 potential children.
	// For this quad tree implementation in particular (Maximum 1 body per node, AND rebuilt on tick), 
	// should look into object pooling.
	BarnesHut* parent = nullptr; // We never recurse upwards
	std::unique_ptr<BarnesHut> UL = nullptr;
	std::unique_ptr<BarnesHut> UR = nullptr;
	std::unique_ptr<BarnesHut> LL = nullptr;
	std::unique_ptr<BarnesHut> LR = nullptr;

	// In Barnes-Hut, a quad can only have 0 or 1 bodies.
	Body* node_body = nullptr;

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

	// Handles calculations for updating this node's mass sum and center of mass when a new body is added.
	void update_mass_add(const Body& to_add);


public:

	static void set_approximation(float to_set);

	BarnesHut(float size, float approximation_value); // can have max depth just in case

	// internal constructor. still needs to be public for make_unique.
	BarnesHut(float x, float y, float size);

	void set_size(float size);

	// Will add a body to the most appropriate quad node. Potentially splits that node if it reached its capacity.
	void add_body(Body& to_add);

	// Adds the body to the correct child quad.
	void add_to_child(Body& body);

	bool is_leaf() const { return UL == nullptr; } // Non-leaf nodes always have all 4 quads.

	void handle_gravity(Body& body, float grav_const) const;

	// I'm not sure whether it'd be better to rebuild every frame or try to use reinsertion.
	void update(std::span<const std::unique_ptr<Body>> bodies);

	// Returns true if the body's center is inside the quad's dimensions.
	bool contains(Body& body) const;
	
	bool is_root() const { return parent == nullptr; }

	bool is_empty() const { return !node_body; }
	bool has_room() const { return !node_body; }
	bool is_full() const { return node_body != nullptr; }

	// Creates 4 new child nodes. The current body is moved into the child node that contains it.
	void split();

	// Frees 4 child nodes.
	void concatenate();

	/*
	Returns the first (and hopefully only) child quad where the predicate is true, or nullptr if predicate false in all four child nodes.
	Checks in order: UL, UR, LL, LR.
	*/
	BarnesHut* get_quad(std::function<bool(const BarnesHut&)> predicate) const;
};

