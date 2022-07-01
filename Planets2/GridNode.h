#pragma once
#include <vector>
#include "raylib.h"

class Body;
struct Collision;

class DebugInfo;


// A node in a grid. Stores its dimensions and pointers to bodies, whose centers are inside the node's dimensions.
class GridNode
{

	// Node's unique id.
	const int id;

	// Node's area.
	const Rectangle dimensions;

	// Bodies whose centers are inside the node's dimensions.
	std::vector<Body*> bodies;

public:

	GridNode() = default;
	GridNode(float x, float y, float node_size, int id);

	// Adds body to the node.
	void add(Body& body);

	// Removes body from the node.
	void rem(const Body& body);

	// Clears the node.
	void clear();

	// Checks if any body in the node overlaps with the point, and if so, returns it.
	// Else, returns nullptr.
	Body* find_body(Vector2 point) const;

	// Returns this nodes's dimensions.
	Rectangle get_representation() const;

	// Attaches text related to this node to info.
	void get_info(const Body& body, DebugInfo& info) const;

	// Scans for collisions between each body in this with each other and adds any to the collisions vector.
	// Returns number of collision checks performed.
	int get_collisions(std::vector<Collision>& collisions) const;

};

