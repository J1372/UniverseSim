#pragma once
#include <vector>
#include "raylib.h"

class Body;
struct Collision;


// A node in a grid. Stores its dimensions and pointers to bodies, whose centers are inside the node's dimensions.
class GridNode
{
	const int id;
	const Rectangle dimensions;


	std::vector<Body*> bodies;

public:

	GridNode() = default;
	GridNode(float x, float y, float node_size, int id);

	void add(Body& body);
	void rem(const Body& body);

	void clear();

	bool contains_point(Vector2 point) const;
	bool contains_partially(const Body& body) const;
	bool contains_fully(const Body& body) const;

	Body* find_body(Vector2 point) const;

	Rectangle get_representation() const;
	void attach_debug_text(Body& body) const;

	void get_collisions(std::vector<Collision>& collisions) const;

};

