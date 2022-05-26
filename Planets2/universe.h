#pragma once
#include "Body.h"
#include <unordered_map>
#include <memory>
#include "QuadTree.h"
#include <limits>
#include "UniverseSettings.h"
#include "Collision.h"

#include <functional>

struct Vector2;
class SpatialPartitioning;

//const double GRAV_CONST = 0.00000000006743
//const double GRAV_CONST = 1;

class Universe {

	UniverseSettings settings{};

	std::unique_ptr<SpatialPartitioning> partitioning_method;

	std::vector<std::unique_ptr<Body>> active_bodies;

	int generated_bodies = 0;

	// O(n^2)
	//void handle_collisions();

	// O(1)
	//bool handle_collision(int it);

	// O(n^2)
	void handle_gravity();

	// O(n)
	void update_pos();

	std::vector<float> gen_rand_portions(int num_slots) const;

	void handle_collisions();
	void handle_collision(Collision collision, std::vector<int>& to_remove);

public:

	Universe();

	Universe(const UniverseSettings& to_set);
	Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning_method);

	// Generates a new universe, using the current settings.
	void generate_universe();


	bool can_create_body() const;

	template <class... ArgTypes>
	Body& create_body(ArgTypes&&... args)
	{
		int id = generated_bodies++;

		active_bodies.emplace_back(std::make_unique<Body>(id, std::forward<ArgTypes>(args)...));

		Body& body = *active_bodies[active_bodies.size() - 1];

		partitioning_method->add_body(body);

		return body;
	}

	Body& create_satellite(const Body& orbiting, float ecc, long mass);

	Body& create_rand_body();
	Body& create_rand_system();
	Body& create_rand_satellite(const Body& orbiting);


	bool has_partitioning() const { return partitioning_method.get() != nullptr; }
	const SpatialPartitioning* get_partitioning() const { return partitioning_method.get(); }
	UniverseSettings& get_settings() { return settings; }

	// Returns the body at the coordinate, or nullptr if not found.
	Body* get_body(Vector2 point) const;

	const std::vector<std::unique_ptr<Body>>& get_bodies() const { return active_bodies; }
	int get_num_bodies() const { return active_bodies.size(); }

	void update();

	void grav_pull(Body& body1, Body& body2) const;

};
