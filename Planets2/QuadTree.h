#pragma once
#include <unordered_map>
#include <memory>
#include "Body.h"
#include "SpatialPartitioning.h"
#include "raylib.h"
#include <functional>

class QuadTree : public SpatialPartitioning {
public:

	const float x;
	const float y;
	const float end_x;
	const float end_y;

	QuadTree(float size);
	QuadTree(float x, float y, float end_x, float end_y);



	void add_body(Body& body);
	void rem_body(const Body& body);
	bool is_leaf() const { return UL == nullptr; } // Non-leaf nodes always have all 4 quads.

	bool in_bounds(int check_x, int check_y) const {
		return check_x >= x && check_x < end_x&&
			check_y >= y && check_y < end_y;
	}

	Body* find_body(Vector2 point) const;

	const std::array<QuadTree*, 4> get_quads() const;

	float get_width() const { return end_x - x + 1; }
	float get_height() const { return end_y - y + 1; }

	void update();

	// Returns a representation of the boundaries of the quad tree and all of its child nodes.
	std::vector<Rectangle> get_representation() const;
	void attach_debug_text(Body& body) const;

	// Performs a collision check, and returns all collision events.
	std::vector<Collision> get_collisions() const;

	~QuadTree() = default;

	//void update_removed(const std::vector<int> &indices_removed);

private:

	static int quads_generated;
	int quad_id;

	static constexpr int MAX_BODIES = 10;

	std::vector<Body*> quad_bodies;
	int cur_size = 0; // The number of bodies in this quad and its children.


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


	// leaf node methods (doesn't really make sense for parent nodes)
	bool is_empty() const { return cur_size == 0; }
	bool has_room() const { return cur_size < MAX_BODIES; }
	bool is_full() const { return cur_size >= MAX_BODIES; }

	bool contains_point(Vector2 point) const;
	bool contains_fully(const Body& body) const;
	bool contains_partially(const Body& body) const;

	// If a body is removed from a child node (due to collision), notifies all relevant parent nodes to update their sizes. Unused.
	void notify_child_removed();
	void move_to_child(std::vector<Body*>::iterator& it); // Moves to child without increasing our current size.
	bool in_more_than_one_child(Body& body);
	void selective_add(Body& new_body); // Chooses whether to add body to current quad or a child quad.
	void add_to_child(Body& body);
	//void rem_from_child(const Body& body);

	/*
	Returns the first (and hopefully only) child quad where the predicate is true, or nullptr if predicate false in all four child nodes.
	Checks in order: UL, UR, LL, LR.
	*/
	QuadTree* get_quad(std::function<bool(const QuadTree&)> predicate) const;

	// Returns all child quads (of UL, UR, LL, LR)  where the predicate is true.
	std::vector<QuadTree*> get_quads(std::function<bool(const QuadTree&)> predicate) const;

	// Returns all child quads (searches entire depth) where the predicate is true.
	std::vector<QuadTree*> get_all_quads(std::function<bool(const QuadTree&)> predicate) const;

	void concatenate();
	void split();

	void reinsert(Body& body);

	// Returns the smallest quad that contains the entire body.
	QuadTree& find_quad(const Body& body);
	// Returns the smallest quad that contains the entire body.
	const QuadTree& find_quad(const Body& body) const;


	void get_representation_internal(std::vector<Rectangle>& rep) const;

};
