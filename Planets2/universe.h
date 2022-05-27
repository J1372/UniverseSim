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

	Rectangle dimensions;

	int generated_bodies = 0;

	// O(n)
	void handle_collisions(std::vector<Collision>& collisions);
	// O(1)
	void handle_collision(Collision collision, std::vector<int>& to_remove);

	// O(n^2)
	void handle_gravity();

	// O(n)
	void update_pos();

	std::vector<float> gen_rand_portions(int num_slots) const;

	// O(n^2)
	std::vector<Collision> get_collisions_no_partitioning() const;

	bool in_bounds(Vector2 point) const;

	std::vector<std::unique_ptr<Body>>::iterator get_iterator(int id);

	void rem_body(Body& body, Body& removed_by);

public:

	Universe();

	Universe(const UniverseSettings& to_set);
	Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning);

	// Generates a new universe, using the current settings.
	void generate_universe();


	bool can_create_body() const;

	template <class... ArgTypes>
	Body& create_body(ArgTypes&&... args)
	{
		int id = generated_bodies++;

		active_bodies.emplace_back(std::make_unique<Body>(id, std::forward<ArgTypes>(args)...));

		Body& body = *active_bodies[active_bodies.size() - 1];

		if (has_partitioning()) {
			partitioning_method->add_body(body);
		}

		return body;
	}

	// should enforce mass <= orbiting.mass
	Body& create_satellite(const Body& orbiting, float ecc, long mass);

	Body& create_rand_body();
	Body& create_rand_system();
	Body& create_rand_satellite(const Body& orbiting);


	bool has_partitioning() const { return partitioning_method.get() != nullptr; }
	const SpatialPartitioning* get_partitioning() const { return partitioning_method.get(); }
	UniverseSettings& get_settings() { return settings; }

	void rem_body(int id);
	void rem_body(Body& body);

	// Returns the body at the coordinate, or nullptr if not found.
	Body* get_body(Vector2 point) const;

	const std::vector<std::unique_ptr<Body>>& get_bodies() const { return active_bodies; }
	int get_num_bodies() const { return active_bodies.size(); }

	void update();

	void grav_pull(Body& body1, Body& body2) const;

};
