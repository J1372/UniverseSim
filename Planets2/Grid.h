#pragma once
#include "SpatialPartitioning.h"
#include "GridNode.h"

struct Rectangle;
struct Collision;

class Grid : public SpatialPartitioning
{

	std::vector<GridNode> nodes;
	std::vector<Body*> bodies;

	const int grid_size; // total size of the grid
	const int node_size; // size of each node in the grid.
	const int nodes_per_row; // number of nodes in a row of the grid.

	int get_index(int pos) const;


	GridNode& get_node(int x, float y);
	const GridNode& get_node(int x, float y) const;

	// Returns a list of all grid nodes that the body overlaps with.
	std::vector<GridNode*> get_all_nodes(const Body& body);

public:

	Grid(int grid_size, int nodes_per_row);

	// Updates bodies to be in the correct grid.
	void update();

	void add_body(Body& body);
	void rem_body(const Body& body) override;

	Body* find_body(Vector2 point) const override;

	std::vector<Rectangle> get_representation() const override;
	void attach_debug_text(Body& body) const override;

	std::vector<Collision> get_collisions() override;
};

