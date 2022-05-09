#pragma once
#include <unordered_map>
#include <memory>
#include "Body.h"
#include "SpatialPartitioning.h"
#include <functional>

class QuadTree : public SpatialPartitioning {

	static int quads_generated;
	int quad_id;

public:

	const float x;
	const float y;
	const float end_x;
	const float end_y;

	QuadTree(float size) : x(-size), y(-size), end_x(size), end_y(size), quad_id(quads_generated++)
	{}

	QuadTree(float x, float y, float end_x, float end_y) : x(x), y(y), end_x(end_x), end_y(end_y), quad_id(quads_generated++)
	{}

	void collision_check(std::vector<Body*>& to_remove);


	void add_body(Body& body);
	bool rem_body(const Body& body);
	bool is_leaf() const { return UL == nullptr; } // Non-leaf nodes always have all 4 quads.

	bool in_bounds(int check_x, int check_y) const {
		return check_x >= x && check_x < end_x&&
			check_y >= y && check_y < end_y;
	}

	Body* find_body(Vector2 point);

	const std::array<QuadTree*, 4> get_quads() const;

	float get_width() const { return end_x - x + 1; }
	float get_height() const { return end_y - y + 1; }

	void update();
	void draw_debug(const Camera2D& camera) const;
	void attach_debug_text(Body& body) const;

	~QuadTree() = default;

	//void update_removed(const std::vector<int> &indices_removed);


private:

	

	static constexpr int MAX_BODIES = 10;

	std::vector<Body*> quad_bodies;
	int cur_size = 0; // The number of bodies in this quad and its children.c

	QuadTree* parent = nullptr;
	std::unique_ptr<QuadTree> UL = nullptr;
	std::unique_ptr<QuadTree> UR = nullptr;
	std::unique_ptr<QuadTree> LL = nullptr;
	std::unique_ptr<QuadTree> LR = nullptr;

	/*
	* Checks for, then handles any collision between the body referenced by the first iterator,
	* with all the bodies from the second iterator to the end iterator of quad2's quad_bodies.
	* 
	* The first iterator is in quad1's quad_bodies.
	* The second iterator is in quad2's quad_bodies.
	* 
	* quad1 can reference the same QuadTree as quad2.
	* 
	* If the first iterator's body collides with and absorbs a body, decrements quad2's size and removes the iterator from its quad_bodies.
	* If the first iterator's body collides with and is absorbed by a body, decrements quad1's size and removes the iterator from its quad_bodies.
	* 
	* 
	* 
	* Returns true and advances the first iterator if its body was absorbed by another body.
	*/
	bool handle_collision(std::vector<Body*>::iterator& it, std::vector<Body*>::iterator&& it2,
		QuadTree& quad1, QuadTree& quad2, std::vector<Body*>& to_remove);


	bool handle_collision_child(std::vector<Body*>::iterator& it, QuadTree& original_quad, std::vector<Body*>& to_remove);

	bool is_root() const { return parent == nullptr; }

	int width() const { return end_x - x + 1; }
	int height() const { return end_y - y + 1; }


	// leaf node methods (doesn't really make sense for parent nodes)
	bool is_empty() const { return cur_size == 0; }
	bool has_room() const { return cur_size < MAX_BODIES; }
	bool is_full() const { return cur_size >= MAX_BODIES; }

	bool contains_point(Vector2 point) const;
	bool contains_fully(const Body& body) const;
	bool contains_partially(const Body& body) const;

	// Removes a body from the node, and returns an iterator to the next body.
	std::vector<Body*>::iterator rem_body(std::vector<Body*>::iterator it);

	// If a body is removed from a child node (due to collision), notifies all relevant parent nodes to update their sizes. Unused.
	void notify_child_removed();
	void move_to_parent(Body& body); // Similar to parent->add_body, but doesn't increase its size.
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

	// Returns all child quads where the predicate is true.
	std::vector<QuadTree*> get_quads(std::function<bool(const QuadTree&)> predicate) const;

	void concatenate();
	void split();

	void reinsert(Body& body);

	// Returns the smallest quad that contains the entire body.
	const QuadTree& find_quad(const Body& body) const;

};
