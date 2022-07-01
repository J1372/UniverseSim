#pragma once
#include "SpatialPartitioning.h"
#include "GridNode.h"

struct Rectangle;
struct Collision;

// A basic grid partitioning method for collision detection.
class Grid : public SpatialPartitioning
{

	// A vector of all nodes in the grid.
	std::vector<GridNode> nodes;

	// A vector of all bodies in the grid.
	std::vector<Body*> bodies;

	const float grid_size; // Total size of the grid
	const float node_size; // Size of each node in the grid.
	const int nodes_per_row; // Number of nodes in a row of the grid.

	// Returns the x or y grid node index of a given coordinate position.
	int get_index(int pos) const;

	// Returns a reference to the grid node at a given position.
	GridNode& get_node(Vector2 pos);

	// Returns a const reference to the grid node at a given position.
	const GridNode& get_node(Vector2 pos) const;

	// Returns a list of all grid nodes that the body overlaps with.
	std::vector<GridNode*> get_all_nodes(const Body& body);

public:

	// Constructs a grid of the given size with (nodes_per_row^2) nodes.
	Grid(float grid_size, int nodes_per_row);

	// Updates bodies to be in the correct grid node.
	void update();

	// Adds body to the grid.
	void add_body(Body& body);

	// Removes body from the grid.
	void rem_body(const Body& body) override;

	// Tries to find and return a pointer to the body that overlaps with the point.
	// If none found, returns nullptr.
	Body* find_body(Vector2 point) const override;

	// Returns a representation of the grid.
	std::vector<Rectangle> get_representation() const override;

	// Attaches grid-specific debug information about the body to info.
	void get_info(const Body& body, DebugInfo& info) const override;

	// Detects and returns a vector of all collision events between bodies in this grid.
	std::vector<Collision> get_collisions() override;
};
