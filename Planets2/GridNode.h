#pragma once
#include <vector>

class Body;

class GridNode
{
	const float x;
	const float y;
	const float end_x;
	const float end_y;

	std::vector<Body*> quad_bodies;

public:

	GridNode() = default;
	GridNode(float x, float y, float node_size) : x(x), y(y), end_x(x + node_size - 1), end_y(y + node_size - 1)
	{}

	void add(Body& body);
	void update();

	void collision_check(std::vector<Body*>& to_remove);

	bool in_bounds(float x, float y) const;

	void draw_debug() const;
};

