#pragma once
#include <memory>
#include "SpatialPartitioning.h"
#include "raylib.h"
#include "QuadChildren.h"
#include "DynamicPool.h"

class Body;

// A QuadTree to be used for detecting collisions between bodies.
class QuadTree : public SpatialPartitioning {

public:

	QuadTree() = default;

	QuadTree(float size, int max_bodies_per_quad, int max_depth);

	// internal constructor. still needs to be public for make_unique.
	QuadTree(float x, float y, float size, int depth);

	// Will add a body to the most appropriate quad node. Potentially splits that node if it reached its capacity.
	void add_body(Body& body);

	// Will remove the body from the quad node that it is in. Potentially concatenates that node or its parent.
	void rem_body(const Body& body);

	// Returns whether the current quad is a leaf or not.
	bool is_leaf() const;

	// Returns a pointer to the body that overlaps with the point. Returns nullptr if no body found.
	Body* find_body(Vector2 point) const;

	// Returns an array of all 4 child node pointers (UL, UR, LL, LR).
	std::span<const QuadTree, 4> get_quads() const;

	// Checks all bodies and reinserts bodies into the most fitting node.
	// Handles node splitting due to movement.
	void update();

	// Returns a representation of the boundaries of the quad tree and all of its child nodes.
	std::vector<Rectangle> get_representation() const;

	// Finds the correct quad, then attaches the Quad id and number of bodies in the quad to the body's debug text.
	void attach_debug_text(Body& body) const;

	// Performs a collision check, and returns all collision events.
	std::vector<Collision> get_collisions();


private:

	// When number of bodies in a quad reaches this number, try to subdivide.
	// Actual number of bodies may be higher than this, if subdividing would exceed max_depth.
	static int max_bodies_per_quad;

	// Number of possible subdivisions to be made. Root depth is 0, so 1 == only one subdivision from root.
	static int max_depth;

	// Total number of quads generated since root quad created.
	static int quads_generated;

	static DynamicPool<QuadChildren<QuadTree>> quad_pool;

	// The quad's unique id.
	int quad_id;

	// The current quad's depth from the root.
	int depth;

	// The dimensions of the quad node.
	Rectangle dimensions;

	// List of body pointers to bodies that are fully contained in this quad.
	// They do not overlap with any other non-child quad.
	std::vector<Body*> quad_bodies;

	// The number of bodies in this quad and all its children (max depth).
	int cur_size = 0;

	// The node's parent node.
	QuadTree* parent = nullptr;

	// The node's 4 potential children.
	PoolPtr<QuadChildren<QuadTree>> children {quad_pool};

	// Adds the body to the appropriate quad, updating node sizes along the way.
	// Returns a pointer to the node the body was added to.
	QuadTree* add_internal(Body& body);
	
	// Checks all bodies and reinserts bodies into the most fitting node.
	void update_internal();

	// Will assume body is in the quad that this method was called on.
	void rem_body_internal(const Body& body);

	// Adds all detected collisions to collisions vector.
	int get_collisions(std::vector<Collision>& collisions) const;

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
	bool has_room() const;

	// Returns true if quad being added to would cause a split.
	bool is_full() const;

	// Returns true if the quad is not over capacity.
	bool should_concatenate() const;

	// Returns true if the quad is over capacity.
	bool should_split() const;

	// Returns true if quad's depth is the maximum depth limit, else false.
	bool reached_depth_limit() const;

	// Returns true if the point is inside the quad's dimensions.
	bool contains_point(Vector2 point) const;

	// Returns true if the body is fully inside the quad's dimensions.
	bool contains_fully(const Body& body) const;

	// Returns true if at least part of the body is inside the quad's dimensions.
	bool contains_partially(const Body& body) const;

	// If a body is removed from a child node (due to collision), notifies all relevant parent nodes to update their sizes.
	void notify_child_removed();

	// Moves body referenced by iterator to child node without increasing the current node's size.
	// Returns the next iterator.
	std::vector<Body*>::iterator move_to_child(std::vector<Body*>::iterator it);

	// Moves body referenced by iterator upwards.
	// Returns the next iterator.
	std::vector<Body*>::iterator move_up(std::vector<Body*>::iterator it);

	// Returns true if a body overlaps over more than one child nodes.
	bool in_more_than_one_child(const Body& body) const;

	// Chooses whether to add body to current quad or a child quad.
	// Returns a pointer to the actual node the body was added to.
	QuadTree* selective_add(Body& new_body);

	// Finds a child node that can contain the body, and adds the body somewhere in that node.
	// Returns a pointer to the actual node the body was added to.
	QuadTree* add_to_child(Body& body);

	// Handles possible concatenation in this quad and all relevant parent quads.
	void concat_check();

	// Moves all child nodes' bodies into this node, then resets their pointers to nullptr.
	void concatenate();

	// Creates 4 new child nodes. Moves all bodies that can completely fit inside a child node, into their respective child nodes.
	void split();

	// Reinserts a body from one node upwards into the smallest node that fully contains it.
	void reinsert(Body& body);

	// Returns the smallest quad that contains the entire body.
	QuadTree& find_quad(const Body& body);

	// Returns the smallest quad that contains the entire body.
	const QuadTree& find_quad(const Body& body) const;

	// Internal method used by get_representation.
	void get_representation_internal(std::vector<Rectangle>& rep) const;

	// Checks for quads that need to be split, and splits them.
	void split_check();

};
