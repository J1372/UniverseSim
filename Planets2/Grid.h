#pragma once
#include "SpatialPartitioning.h"
#include "GridNode.h"

class Grid : public SpatialPartitioning
{

	std::vector<GridNode> nodes;

	int grid_size;
	int node_size;
	int row_width; // number of nodes in a row of a grid.

	GridNode& get_node(int x, float y);

public:

	Grid(int grid_size, int node_size);

	void add_body(Body& body);
	void collision_check(std::vector<Body*>& to_remove);
	void update();
	void draw_debug(const Camera2D& camera) const;
};

