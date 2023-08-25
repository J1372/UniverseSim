#pragma once
#include "SpatialPartitioning.h"
#include <vector>

class Body;

class NullPartitioning : public SpatialPartitioning
{

	std::vector<Body*> bodies;

	// Inherited via SpatialPartitioning
	std::vector<Collision> get_collisions_impl() override;

	void add_body(Body& body) override;
	void rem_body(const Body& body) override;
	void notify_move(const Body* from, Body* to) override;
	void update() override;
	Body* find_body(Vector2 point) const override;
	std::vector<Rectangle> get_representation() const override;
	void get_info(const Body& body, DebugInfo& info) const override;
};

