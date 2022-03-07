#pragma once
#include "Body.h"
#include <unordered_map>
#include <memory>
#include "QuadTree.h"
#include <limits>


//const double GRAV_CONST = 0.00000000006743
//const double GRAV_CONST = 1;

class Universe {
	//static constexpr int UNIVERSE_CAPACITY = 2000;
	static constexpr double GRAV_CONST = 0.75;
	static constexpr int UNIVERSE_START_SIZE = 10000;
	static constexpr long RAND_MASS = 100;
	static constexpr int MASS_SCALING = 1; // used to be 3 but need to see how to incorporate that with create_system orbits.

	std::vector<Body> active_bodies;

	QuadTree root{ -std::numeric_limits<float>::max(),
		-std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max() };

	/*QuadTree root{ -4 * UNIVERSE_START_SIZE,
		-4 * UNIVERSE_START_SIZE,
		4 * UNIVERSE_START_SIZE,
		4 * UNIVERSE_START_SIZE };*/


		/*std::unique_ptr<QuadTree> root = std::make_unique<QuadTree>(-std::numeric_limits<float>::max(),
			-std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max());*/


	int generated_bodies = 0;

public:

	Universe() {
		active_bodies.reserve(1000);
	};

	bool can_create_body() const;

	Body& create_body(float, float, long);
	Body& create_body(float sat_dist, const Body& orbiting, float ecc, long mass);
	Body& create_satellite(const Body&, float, long);

	Body& create_rand_body();
	Body& create_rand_system();
	Body& create_rand_satellite(const Body&);

	const std::vector<Body>& get_bodies() const { return active_bodies; }

	void update();

	void grav_pull(Body& body1, Body& body2) const;

	const QuadTree* get_quad_root() const { return &root; }


	//~Universe() not needed because the map frees itself.

private:

	// O(n^2)
	void handle_collisions();

	// O(1)
	bool handle_collision(int it);

	// O(n^2)
	void handle_gravity();

	// O(n)
	void update_pos();

	std::vector<float> gen_rand_portions(int num_slots) const;

};
