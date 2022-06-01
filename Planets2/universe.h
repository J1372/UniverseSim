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

	int tick = 0;
	int num_collision_checks = 0;
	int num_collision_checks_tick = 0;
	int generated_bodies = 0;

	// O(n)
	void handle_collisions(std::vector<Collision>& collisions);
	// O(1)
	void handle_collision(Collision collision, std::vector<Removal>& to_remove);
	void handle_removal(Removal removal);

	// O(n^2)
	void handle_gravity();

	// O(n)
	void update_pos();

	std::vector<float> gen_rand_portions(int num_slots) const;

	// O(n^2)
	std::vector<Collision> get_collisions_no_partitioning();

	bool in_bounds(Vector2 point) const;

	std::vector<std::unique_ptr<Body>>::iterator get_iterator(int id);

	void rem_body(Body& body, Body& removed_by);

	float get_rand_sat_dist() const;

public:

	Universe();

	Universe(const UniverseSettings& to_set);
	Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning);

	void set_settings(const UniverseSettings& to_set);
	void set_partitioning(std::unique_ptr<SpatialPartitioning>&& partitioning);


	bool can_create_body() const;

	template <class... ArgTypes>
	Body& create_body(ArgTypes&&... args)
	{
		int id = generated_bodies++;

		Body& body = *active_bodies.emplace_back(std::make_unique<Body>(std::forward<ArgTypes>(args)...));
		body.id = id;

		if (has_partitioning()) {
			partitioning_method->add_body(body);
		}

		return body;
	}

	std::vector<std::unique_ptr<Body>> generate_rand_system(float x, float y);

	// Command to add the body to the universe.
	void add_body(std::unique_ptr<Body>&& body_ptr);

	// Transfers all bodies from the vector and then clears the vector.
	void add_bodies(std::vector<std::unique_ptr<Body>>& bodies);


	// Creates a new universe, using the current settings.
	void create_universe();

	// Returns a random orbit
	Orbit gen_rand_orbit(const Body& orbited, const Body& orbiter, float retrograde_chance) const;


	Body& create_rand_body();
	Body& create_rand_system();


	bool has_partitioning() const { return partitioning_method.get() != nullptr; }
	const SpatialPartitioning* get_partitioning() const { return partitioning_method.get(); }
	UniverseSettings& get_settings() { return settings; }

	void rem_body(int id);
	void rem_body(Body& body);

	// Returns the body at the coordinate, or nullptr if not found.
	Body* get_body(Vector2 point) const;
	// Returns the body associated with the id, or nullptr if not found.
	Body* get_body(int id) const;

	const std::vector<std::unique_ptr<Body>>& get_bodies() const { return active_bodies; }
	int get_num_bodies() const { return active_bodies.size(); }

	void update();

	void grav_pull(Body& body1, Body& body2) const;

	int get_num_collision_checks() const { return num_collision_checks; }
	int get_num_collision_checks_tick() const { return num_collision_checks_tick; }
	int get_tick() const { return tick; }

};
