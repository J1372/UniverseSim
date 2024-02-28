#pragma once
#include <memory>
#include "SpatialPartitioning.h"
#include "raylib.h"
#include "QuadChildren.h"
#include "DynamicPool.h"

class Body;

class QuadNode
{
	static DynamicPool<QuadChildren<QuadNode>> quad_pool;

	// The current quad's depth from the root.
	int depth = 0;

	// The dimensions of the quad node.
	Rectangle dimensions {};

	// List of body pointers to bodies that are fully contained in this quad.
	// They do not overlap with any other non-child quad.
	std::vector<Body*> quad_bodies;

	// The number of bodies in this quad and all its children (max depth).
	int cur_size = 0;

	// The node's parent node.
	QuadNode* parent = nullptr;

	// The node's 4 potential children.
	PoolPtr<QuadChildren<QuadNode>> children { quad_pool };

	// Checks all bodies and reinserts bodies into the most fitting node.
	void update_internal(int max_bodies, int max_depth);

	// Performs a collision check between the body and all bodies starting at the given iterator until the end iterator.
	// If a collision is detected, adds a Collision event to the collision vector.
	int get_collisions_internal(Body& checking, std::vector<Body*>::const_iterator it, std::vector<Body*>::const_iterator end, std::vector<Collision>& collisions) const;

	// Get collisions between a body and all bodies in relevant child nodes.
	int get_collisions_child(Body& checking, std::vector<Collision>& collisions) const;

	// Returns whether the quad is the root.
	bool is_root() const;

	// Returns whether any bodies are contained in the quad itself.
	bool is_empty() const;

	// Returns true if the quad can be added to without causing a split.
	bool has_room(int max_bodies) const;

	// Returns true if quad being added to would cause a split.
	bool is_full(int max_bodies) const;

	// Returns true if the quad is not over capacity.
	bool should_concatenate(int max_bodies) const;

	// Returns true if the point is inside the quad's dimensions.
	bool contains_point(Vector2 point) const;

	// Returns true if the body is fully inside the quad's dimensions.
	bool contains_fully(const Body& body) const;

	// Returns true if at least part of the body is inside the quad's dimensions.
	bool contains_partially(const Body& body) const;

	// Moves body referenced by iterator upwards.
	// Returns the next iterator.
	std::vector<Body*>::iterator move_up(std::vector<Body*>::iterator it);

	// Handles recursive splitting of a leaf.
	void leaf_split_check(int max_bodies, int max_depth);

	// Handles possible concatenation in this quad and all relevant parent quads.
	void concat_check(int max_bodies);

	// Moves all child nodes' bodies into this node, then resets their pointers to nullptr.
	void concatenate();

	// Creates 4 new child nodes.
	// Moves all bodies that can completely fit inside a child node, into their respective child nodes.
	// May split child nodes as well, if any reached max capacity.
	void split(int max_bodies, int max_depth);

	void add_no_split(Body& body);

	// Reinserts a body from one node upwards into the smallest node that fully contains it.
	void reinsert(Body& body);

	// Returns the smallest quad that contains the entire body.
	QuadNode& find_quad(const Body& body);

	// Returns the smallest quad that contains the entire body.
	const QuadNode& find_quad(const Body& body) const;

	// Internal method used by get_representation.
	void get_representation_internal(std::vector<Rectangle>& rep) const;

	// Checks for quads that need to be split, and splits them.
	void split_check(int max_bodies, int max_depth);

	// Returns whether the current quad is a leaf or not.
	bool is_leaf() const;

public:

	QuadNode() = default;
	// internal constructor. still needs to be public for make_unique.
	QuadNode(float x, float y, float size, QuadNode* parent, int depth, int max_bodies);

	// Will add a body to the most appropriate quad node. Potentially splits that node if it reached its capacity.
	void add_body(Body& body, int max_bodies_per_quad, int max_depth);

	// Will remove the body from the quad node that it is in. Potentially concatenates that node or its parent.
	void rem_body(const Body& body, int max_bodies_per_quad);

	void notify_move(const Body* from, Body* to);

	// Returns a pointer to the body that overlaps with the point. Returns nullptr if no body found.
	Body* find_body(Vector2 point) const;

	// Checks all bodies and reinserts bodies into the most fitting node.
	// Handles node splitting due to movement.
	void update(int max_bodies, int max_depth);

	// Adds all detected collisions to collisions vector.
	int get_collisions(std::vector<Collision>& collisions) const;

	// Returns a representation of the boundaries of the quad tree and all of its child nodes.
	std::vector<Rectangle> get_representation() const;

	// Attaches text related to the quadtree node which contains the body to info.  
	void get_info(const Body& body, DebugInfo& info) const;

};

// A QuadTree to be used for detecting collisions between bodies.
class QuadTree : public SpatialPartitioning
{
	// When number of bodies in a quad reaches this number, try to subdivide.
	// Actual number of bodies may be higher than this, if subdividing would exceed max_depth.
	int max_bodies_per_quad;

	// Number of possible subdivisions to be made. Root depth is 0, so 1 == only one subdivision from root.
	int max_depth;

	QuadNode root;

	// Performs a collision check, and returns all collision events.
	std::vector<Collision> get_collisions_impl() override;

public:

	QuadTree(float size, int max_bodies_per_quad, int max_depth);

	// Will add a body to the most appropriate quad node. Potentially splits that node if it reached its capacity.
	void add_body(Body& body) override
	{
		root.add_body(body, max_bodies_per_quad, max_depth);
	}

	// Will remove the body from the quad node that it is in. Potentially concatenates that node or its parent.
	void rem_body(const Body& body) override
	{
		root.rem_body(body, max_bodies_per_quad);
	}

	void notify_move(const Body* from, Body* to) override
	{
		root.notify_move(from, to);
	}

	// Returns a pointer to the body that overlaps with the point. Returns nullptr if no body found.
	Body* find_body(Vector2 point) const override
	{
		return root.find_body(point);
	}

	// Checks all bodies and reinserts bodies into the most fitting node.
	// Handles node splitting due to movement.
	void update() override
	{
		root.update(max_bodies_per_quad, max_depth);
	}

	// Returns a representation of the boundaries of the quad tree and all of its child nodes.
	std::vector<Rectangle> get_representation() const override
	{
		return root.get_representation();
	}

	// Attaches text related to the quadtree node which contains the body to info.  
	void get_info(const Body& body, DebugInfo& info) const override
	{
		root.get_info(body, info);
	}

};
