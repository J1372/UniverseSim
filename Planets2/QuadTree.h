#pragma once
#include <unordered_map>
#include <memory>
#include "Body.h"

class QuadTree {

	static constexpr int MAX_BODIES = 10;

	std::vector<Body*> quad_bodies;


	std::unique_ptr<QuadTree> UL = nullptr;
	std::unique_ptr<QuadTree> UR = nullptr;
	std::unique_ptr<QuadTree> LL = nullptr;
	std::unique_ptr<QuadTree> LR = nullptr;

public:
	QuadTree* parent = nullptr;

	const float x;
	const float y;
	const float end_x;
	const float end_y;


	QuadTree(float x, float y, float end_x, float end_y) : x(x), y(y), end_x(end_x), end_y(end_y)
	{
		//quad_bodies.reserve(10 * MAX_BODIES);
	}

	void perform_collision_check(std::vector<Body*>& to_remove);


	void add_body(Body& body);
	bool rem_body(const Body& body);
	bool is_leaf() const { return UL == nullptr; } // Non-leaf nodes always have all 4 quads.

	bool in_bounds(int check_x, int check_y) const {
		return check_x >= x && check_x < end_x&&
			check_y >= y && check_y < end_y;
	}

	const std::array<QuadTree*, 4> get_quads() const;

	float get_width() const { return end_x - x; }
	float get_height() const { return end_y - y; }

	void update_pos();

	void update_removed(const std::vector<int> &indices_removed);

private:

	void handle_collision(std::vector<Body*>::iterator& it, std::vector<Body*>& to_remove);

	bool is_root() const { return parent == nullptr; }

	// leaf node methods (doesn't really make sense for parent nodes)
	bool is_empty() const { return quad_bodies.size() == 0; }
	bool has_room() const { return quad_bodies.size() < MAX_BODIES; }
	bool is_full() const { return quad_bodies.size() >= MAX_BODIES; }


	void add_to_child(Body& body);
	void rem_from_child(const Body& body);

	void concatenate();
	void split();

	void reinsert(Body& body);

	// root only method !!!
	void expand(Body& out_of_bounds_body);

};
