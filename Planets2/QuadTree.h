#pragma once
#include <unordered_map>
#include <memory>
#include "Body.h"

class QuadTree {

	static constexpr int MAX_BODIES = 10;

	std::unordered_map<int, Body*> quad_bodies;


	std::unique_ptr<QuadTree> UL = nullptr;
	std::unique_ptr<QuadTree> UR = nullptr;
	std::unique_ptr<QuadTree> LL = nullptr;
	std::unique_ptr<QuadTree> LR = nullptr;

public:
	QuadTree* parent = nullptr;

	float x;
	float y;
	float end_x;
	float end_y;


	QuadTree(float x, float y, float end_x, float end_y) : x(x), y(y), end_x(end_x), end_y(end_y)
	{
		//quad_bodies.reserve(10 * MAX_BODIES); // I think it's already 100 by default
	}

	void perform_collision_check(std::vector<int>&);

	void handle_collision(std::unordered_map<int, Body*>::iterator& it, std::vector<int>&);

	void add_body(Body&);
	bool rem_body(const Body&);
	bool is_leaf() const { return UL == nullptr; }

	bool in_bounds(int check_x, int check_y) const {
		return check_x >= x && check_x < end_x&&
			check_y >= y && check_y < end_y;
	}

	void get_quads(const QuadTree* (&)[4]) const;

	float get_width() const { return end_x - x; }
	float get_height() const { return end_y - y; }

	void update_pos();

private:

	int get_total_size() const;


	bool is_root() const { return parent == nullptr; }

	// leaf node methods (doesn't really make sense for parent nodes)
	bool is_empty() const { return quad_bodies.size() == 0; }
	bool has_room() const { return quad_bodies.size() < MAX_BODIES; }
	bool is_full() const { return quad_bodies.size() >= MAX_BODIES; }


	void add_to_child(Body&);
	void rem_from_child(const Body&);

	void concatenate();
	void split();

	bool reinsert(Body&);

	// root only method !!!
	void expand(Body&);

};