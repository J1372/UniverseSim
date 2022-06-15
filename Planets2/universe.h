#pragma once
#include <memory>
#include <vector>
#include "Body.h"
#include "UniverseSettings.h"
#include "BarnesHut.h"

#include "SpatialPartitioning.h"

struct Collision;
struct Vector2;

class Universe {

	// Settings that define universe generation, physics, system generation, etc.
	UniverseSettings settings{};

	// A possible partitioning method to be used in collision detection.
	std::unique_ptr<SpatialPartitioning> partitioning_method;

	// Gravity approximation method.
	BarnesHut barnes_quad;

	// Bodies that are being updated every tick.
	// Currently using vector of unique_ptrs for while developing partitioning methods.
	// Later will just be vector of Body.
	std::vector<std::unique_ptr<Body>> active_bodies;

	// Bounds of the universe. Bodies that go outside this rectangle wraparound.
	Rectangle dimensions;

	// Current update tick.
	int tick = 0;

	// Number of collision checks that have occurred.
	int num_collision_checks = 0;

	// Number of collision checks that occurred last tick.
	int num_collision_checks_tick = 0;

	// Total number of bodies that have evere been added to the universe.
	int generated_bodies = 0;

	// Handles all collision events.
	void handle_collisions(std::span<const Collision> collisions);

	// Handles a collision event. Adds any bodies to be removed to to_remove.
	void handle_collision(Collision collision, std::vector<Removal>& to_remove);

	// Handles a removal event.
	void handle_removal(Removal removal);

	// Updates all bodies' accelerations by applying forces to each body by each body.
	void handle_gravity();

	// Updates all bodies' accelerations by using Barnes-Hut to approximate gravitational forces.
	void handle_gravity_approximation();

	// Updates all bodies' positions accordint to their velocities and velocities according to their accelerations.
	void update_pos();

	// Handles body wraparound if body has gone out of bounds.
	void handle_wraparound(Body& body);
	
	// Generates random portions [0, 1] between num_slots objects. Sum of all portions == 1.
	std::vector<float> gen_rand_portions(int num_slots) const;

	// Detects and returns all collision events by comparing each body to every other body.
	std::vector<Collision> get_collisions_no_partitioning();

	// Returns true if a point is within the universe's area, else false.
	bool in_bounds(Vector2 point) const;

	// Returns an iterator to the body in active bodies that has the given id.
	std::vector<std::unique_ptr<Body>>::iterator get_iterator(int id);

public:

	Universe();

	Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning);

	void set_settings(const UniverseSettings& to_set);
	void set_partitioning(std::unique_ptr<SpatialPartitioning>&& partitioning);

	// Returns true if the universe is not already at capacity, else false.
	bool can_create_body() const;

	template <class... ArgTypes>
	// Creates a body in the universe by forwarding the arguments to a Body constructor and giving it an id.
	Body& create_body(ArgTypes&&... args)
	{
		int id = generated_bodies++;

		Body& body = *active_bodies.emplace_back(std::make_unique<Body>(std::forward<ArgTypes>(args)...));
		body.set_id(id);

		if (has_partitioning()) {
			partitioning_method->add_body(body);
		}

		return body;
	}

	// Generates a system where the central body is at (x,y), using the current settings.
	std::vector<std::unique_ptr<Body>> generate_rand_system(float x, float y);

	// Command to add the body to the universe.
	void add_body(std::unique_ptr<Body>&& body_ptr);

	// Transfers all bodies from the vector and then clears the vector.
	void add_bodies(std::vector<std::unique_ptr<Body>>& bodies);

	// Creates a new universe, using the current settings.
	void create_universe();

	// Returns a random orbit
	Orbit gen_rand_orbit(const Body& orbited, const Body& orbiter) const;

	// Creates a random body and returns a reference to it.
	Body& create_rand_body();

	// Creates a random system and returns a reference to the central body.
	Body& create_rand_system();

	// Returns true if there is a partitioning method that can be used.
	bool has_partitioning() const;

	// Returns a pointer to the partitioning method.
	const SpatialPartitioning* get_partitioning() const;

	// Returns the universe's current settings.
	UniverseSettings& get_settings();

	// Removes a body from the universe by its id.
	void rem_body(int id);

	// Removes a body from the universe.
	void rem_body(Body& body);

	// Returns the body at the coordinate, or nullptr if not found.
	Body* get_body(Vector2 point) const;

	// Returns the body associated with the id, or nullptr if not found.
	Body* get_body(int id) const;

	// Returns a reference to all bodies in the universe
	std::span<const std::unique_ptr<Body>> get_bodies() const;

	// Returns number of bodies currently in the universe.
	int get_num_bodies() const;

	// Updates the universe by 1 tick.
	void update();

	// Returns the total number of collision checks that occurred since the universe was created.
	int get_num_collision_checks() const;

	// Returns the total number of collision checks that occurred last tick.
	int get_num_collision_checks_tick() const;

	// Returns the current tick.
	int get_tick() const;

};
