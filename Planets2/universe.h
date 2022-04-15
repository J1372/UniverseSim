#pragma once
#include "Body.h"
#include <unordered_map>
#include <memory>
#include "QuadTree.h"
#include <limits>
#include <iostream>
#include "UniverseSettings.h"

class SpatialPartitioning;

//const double GRAV_CONST = 0.00000000006743
//const double GRAV_CONST = 1;

class Universe {
	static constexpr int UNIVERSE_CAPACITY = 1000;
	static constexpr int UNIVERSE_SIZE_START = 1000;
	static constexpr float UNIVERSE_SIZE_MAX = 100000; // delete if go out of bounds or wraparound.
	static constexpr long RAND_MASS = 100;
	static constexpr double GRAV_CONST = 0.75;
	static constexpr int MASS_SCALING = 1; // used to be 3 but need to see how to incorporate that with create_system orbits.

	UniverseSettings settings{};

	std::unique_ptr<SpatialPartitioning> partitioning_method = std::make_unique<QuadTree>(UNIVERSE_SIZE_MAX);

	std::vector<std::unique_ptr<Body>> active_bodies;

	int generated_bodies = 0;

	// O(n^2)
	void handle_collisions();

	// O(1)
	bool handle_collision(int it);

	// O(n^2)
	void handle_gravity();

	// O(n)
	void update_pos();

	std::vector<float> gen_rand_portions(int num_slots) const;

public:

	Universe() {
		// maybe useful with default settings
		active_bodies.reserve(UNIVERSE_CAPACITY);

		//std::cout << std::thread::hardware_concurrency();
	};

	Universe(const UniverseSettings &to_set) {
		settings = to_set;

		active_bodies.reserve(settings.UNIVERSE_CAPACITY);
		generate_universe();
		
	};


	// Generates a new universe, using the current settings.
	void generate_universe();


	bool can_create_body() const;

	Body& create_body(float x, float y, long mass);
	Body& create_body(float sat_dist, const Body& orbiting, float ecc, long mass);
	Body& create_satellite(const Body& orbiting, float ecc, long mass);

	Body& create_rand_body();
	Body& create_rand_system();
	Body& create_rand_satellite(const Body& orbiting);

	const SpatialPartitioning* get_partitioning() const { return partitioning_method.get(); }

	const std::vector<std::unique_ptr<Body>>& get_bodies() const { return active_bodies; }
	int get_num_bodies() const { return active_bodies.size(); }

	void update();

	void grav_pull(Body& body1, Body& body2) const;

};
