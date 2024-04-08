#pragma once
#include <memory>
#include <vector>
#include "Body.h"
#include "UniverseSettings.h"
#include "BarnesHut.h"

#include "SpatialPartitioning.h"
#include "Event.h"
#include "BodyList.h"

struct Collision;
struct Vector2;
struct Removal;

class Universe {

	// Settings that define universe generation, physics, system generation, etc.
	UniverseSettings settings{};

	// A possible partitioning method to be used in collision detection.
	std::unique_ptr<SpatialPartitioning> partitioning_method;

	// Gravity approximation method.
	BarnesHut barnes_quad;

	// Bodies being updated every tick.
	BodyList active_bodies;

	// Bounds of the universe. Bodies that go outside this rectangle wraparound.
	Rectangle dimensions;

	// Current update tick.
	int tick = 0;

	// Number of collision checks that have occurred.
	int num_collision_checks = 0;

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

	// Returns true if a point is within the universe's area, else false.
	bool in_bounds(Vector2 point) const;

	void rem_from_partitioning(Body& to_remove);

	// Observers to notify when this body is being removed.
	Event<Removal> on_removal_observers;

public:

	Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning);

	// Returns true if the universe is not already at capacity, else false.
	bool can_create_body() const;

	// Generates a system where the central body is at (x,y), using the current settings.
	std::vector<Body> generate_rand_system(float x, float y);

	// Command to add the body to the universe.
	void add_body(Body&& body);

	// Transfers all bodies from the vector and then clears the vector.
	void add_bodies(std::vector<Body>&& bodies);

	// Returns a random orbit
	Orbit gen_rand_orbit(const Body& orbited, const Body& orbiter) const;

	// Creates a random system and adds it to the universe.
	void create_rand_system();

	// Returns a pointer to the partitioning method.
	const SpatialPartitioning& get_partitioning() const;

	// Returns the universe's current settings.
	const UniverseSettings& get_settings() const;

	// Removes a body from the universe.
	void rem_body(Body& body);

	// Returns the body at the coordinate, or nullptr if not found.
	Body* get_body(Vector2 point);

	// Returns the body whose id matches the given id, or nullptr if invalid id.
	Body* get_body(int search_id);

	// Returns a reference to all bodies in the universe
	std::span<const Body> get_bodies() const;

	// Returns a reference to all bodies in the universe
	std::span<Body> get_bodies();

	// Returns all bodies in area.
	std::vector<Body*> get_bodies_in(Rectangle area);

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

	// Returns the body's observer list for its removal.
	Event<Removal>& removal_event();
};
