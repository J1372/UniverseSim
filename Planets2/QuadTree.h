#pragma once
#include <unordered_map>
#include <memory>
#include "Body.h"
#include "SpatialPartitioning.h"
#include "raylib.h"
#include <functional>

class QuadTree : public SpatialPartitioning {

public:

	QuadTree(float size);
	QuadTree(float x, float y, float end_x, float end_y);

	// Will add a body to the most appropriate quad node. Potentially splits that node if it reached its capacity.
	void add_body(Body& body);

	// Will remove the body from the quad node that it is in. Potentially concatenates that node or its parent.
	void rem_body(const Body& body);

	bool is_leaf() const { return UL == nullptr; } // Non-leaf nodes always have all 4 quads.

	// Returns a pointer to the body that overlaps with the point. Returns nullptr if no body found.
	Body* find_body(Vector2 point) const;

	// Returns an array of all 4 child node pointers (UL, UR, LL, LR).
	const std::array<QuadTree*, 4> get_quads() const;

	float get_width() const { return end_x - x + 1; }
	float get_height() const { return end_y - y + 1; }

	// Checks all bodies and reinserts bodies into the most fitting node.
	void update();

	// Returns a representation of the boundaries of the quad tree and all of its child nodes.
	std::vector<Rectangle> get_representation() const;

	// Finds the correct quad, then attaches the Quad id and number of bodies in the quad to the body's debug text.
	void attach_debug_text(Body& body) const;

	// Performs a collision check, and returns all collision events.
	std::vector<Collision> get_collisions() const;

	~QuadTree() = default;

private:

	static constexpr int MAX_BODIES = 10;
	static int quads_generated;
	int quad_id;

	const float x;
	const float y;
	const float end_x;
	const float end_y;

	std::vector<Body*> quad_bodies; // List of body pointers to bodies that are fully contained in this quad. They do not overlap with any other quad.
	int cur_size = 0; // The number of bodies in this quad and all its children (max depth).


	// References to the parent and owning pointers to the node's 4 potential children.
	QuadTree* parent = nullptr;
	std::unique_ptr<QuadTree> UL = nullptr;
	std::unique_ptr<QuadTree> UR = nullptr;
	std::unique_ptr<QuadTree> LL = nullptr;
	std::unique_ptr<QuadTree> LR = nullptr;

	Rectangle representation;

	// Will assume body is in the quad that this method was called on.
	void rem_body_internal(const Body& body);

	void get_collisions(std::vector<Collision>& collisions) const;

	// Performs a collision check between the body and all bodies starting at the given iterator until the end iterator.
	// If a collision is detected, adds a Collision event to the collision vector.
	void get_collisions_internal(Body& checking, std::vector<Body*>::const_iterator it, std::vector<Body*>::const_iterator end, std::vector<Collision>& collisions) const;

	void get_collisions_child(Body& checking, std::vector<Collision>& collisions) const;

	bool is_root() const { return parent == nullptr; }

	float width() const { return end_x - x + 1; }
	float height() const { return end_y - y + 1; }

	bool is_empty() const { return cur_size == 0; }
	bool has_room() const { return cur_size < MAX_BODIES; }
	bool is_full() const { return cur_size >= MAX_BODIES; }

	// Returns true if the point is inside the quad's dimensions.
	bool contains_point(Vector2 point) const;

	// Returns true if the body is fully inside the quad's dimensions.
	bool contains_fully(const Body& body) const;

	// Returns true if at least part of the body is inside the quad's dimensions.
	bool contains_partially(const Body& body) const;

	// If a body is removed from a child node (due to collision), notifies all relevant parent nodes to update their sizes.
	void notify_child_removed();
	// Moves body referenced by iterator to child without increasing the current node's size.
	void move_to_child(std::vector<Body*>::iterator& it);
	// Returns true if a body overlaps over more than one child nodes.
	bool in_more_than_one_child(const Body& body) const;
	// Chooses whether to add body to current quad or a child quad.
	void selective_add(Body& new_body);
	// If a child node can fully contain the body, calls add_body(body) on that node.
	void add_to_child(Body& body);

	/*
	Returns the first (and hopefully only) child quad where the predicate is true, or nullptr if predicate false in all four child nodes.
	Checks in order: UL, UR, LL, LR.
	*/
	QuadTree* get_quad(std::function<bool(const QuadTree&)> predicate) const;

	// Returns all child quads (of UL, UR, LL, LR)  where the predicate is true.
	std::vector<QuadTree*> get_quads(std::function<bool(const QuadTree&)> predicate) const;

	// Returns all child quads (searches entire depth) where the predicate is true.
	std::vector<QuadTree*> get_all_quads(std::function<bool(const QuadTree&)> predicate) const;

	// Moves all child nodes' bodies into this node, then resets their pointers to nullptr.
	void concatenate();
	// Creates 4 new child nodes. Moves all bodies that can completely fit inside a child node, into that node.
	void split();

	// Reinserts a body from one node upwards into the smallest node that fully contains it.
	void reinsert(Body& body);

	// Returns the smallest quad that contains the entire body.
	QuadTree& find_quad(const Body& body);
	// Returns the smallest quad that contains the entire body.
	const QuadTree& find_quad(const Body& body) const;


	void get_representation_internal(std::vector<Rectangle>& rep) const;

};
