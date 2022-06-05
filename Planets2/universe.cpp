#include "Universe.h"
#include <malloc.h>
#include <numbers>
#include <iostream>
#include "my_random.h"
#include "raylib.h"
#include "Physics.h"
#include <algorithm>

#include <vector>
#include "Removal.h"

#include "Orbit.h"

float Universe::get_rand_sat_dist() const
{
	return Rand::real() * (settings.satellite_max_dist - settings.satellite_min_dist) + settings.satellite_min_dist;
}

Universe::Universe() : barnes_quad { settings.universe_size_max, .5f }
{
	partitioning_method = std::make_unique<QuadTree>(settings.universe_size_max, 10, 10);
	//std::cout << std::thread::hardware_concurrency();
}

Universe::Universe(const UniverseSettings& to_set) : settings(to_set),
	dimensions{ -settings.universe_size_max / 2.0f, -settings.universe_size_max / 2.0f, settings.universe_size_max , settings.universe_size_max },
	barnes_quad{ settings.universe_size_max, 200000.0f }
{
	partitioning_method = std::make_unique<QuadTree>(settings.universe_size_max, 10, 10);
	create_universe();
}

Universe::Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning) : settings(to_set),
	partitioning_method(std::move(partitioning)),
	dimensions{ -settings.universe_size_max / 2.0f, -settings.universe_size_max / 2.0f, settings.universe_size_max , settings.universe_size_max },
	barnes_quad{ settings.universe_size_max, 10.0f }
{
	create_universe();
}

void Universe::add_body(std::unique_ptr<Body>&& body_ptr)
{
	if (!can_create_body()) {
		return;
	}

	Body& body = *body_ptr;

	if (!in_bounds({ body.x, body.y })) {
		return;
	}

	body.id = generated_bodies++;

	active_bodies.emplace_back(std::move(body_ptr));

	if (has_partitioning()) {
		partitioning_method->add_body(body);
	}
}

void Universe::add_bodies(std::vector<std::unique_ptr<Body>>& bodies)
{
	int prev_size = active_bodies.size();

	int remaining_space = settings.universe_capacity - prev_size;

	// Enforce that adding bodies doesnt go over capacity.
	int adding = std::min(static_cast<int>(bodies.size()), remaining_space);

	auto it = bodies.begin();
	auto end_it = bodies.begin() + adding;
	while (it != end_it and can_create_body()) {
		std::unique_ptr<Body>& body = *it;
		add_body(std::move(body));
		it++;
	}

	bodies.clear();

}

std::vector<std::unique_ptr<Body>>::iterator Universe::get_iterator(int id)
{
	auto predicate = [](const std::unique_ptr<Body>& ptr, int id) {
		return ptr->id < id;
	};

	// Find iterator->body by id using binary search.
	return std::lower_bound(active_bodies.begin(), active_bodies.end(), id, predicate);
}

void Universe::create_universe()
{
	// TODO make physics settings changeable while running. Keep universe settings const while running.
	//		->move reserve/make unique out into constructor / run-once-at-start method.
	dimensions = { -settings.universe_size_max / 2.0f,
		-settings.universe_size_max / 2.0f,
		settings.universe_size_max ,
		settings.universe_size_max };

	tick = 0;
	num_collision_checks = 0;
	num_collision_checks_tick = 0;
	generated_bodies = 0;

	barnes_quad.set_size(settings.universe_size_max);
	BarnesHut::set_approximation(settings.grav_approximation_value);

	active_bodies.clear();
	active_bodies.reserve(settings.universe_capacity);

	for (int i = 0; i < settings.num_rand_planets; ++i) {
		create_rand_body();
	}

	for (int i = 0; i < settings.num_rand_systems; ++i) {
		create_rand_system();
	}
}

bool Universe::can_create_body() const
{
	return active_bodies.size() < settings.universe_capacity;
}

void Universe::set_settings(const UniverseSettings& to_set)
{
	settings = to_set;
}

void Universe::set_partitioning(std::unique_ptr<SpatialPartitioning>&& partitioning)
{
	partitioning_method = std::move(partitioning);
}

void Universe::handle_gravity()
{
	if (active_bodies.empty()) {
		return;
	}

	for (int i = 0; i < active_bodies.size() - 1; i++) {
		Body& body1 = *active_bodies[i];

		for (int j = i + 1; j < active_bodies.size(); j++) {
			Body& body2 = *active_bodies[j];

			Physics::grav_pull(body1, body2, settings.grav_const);

		}
	}
}

void Universe::handle_gravity_approximation()
{
	if (active_bodies.empty()) {
		return;
	}
	
	for (int i = 0; i < active_bodies.size(); i++) {
		Body& body1 = *active_bodies[i];
		barnes_quad.handle_gravity(body1, settings.grav_const);
	}
}

void Universe::handle_removal(Removal removal)
{
	Body& removed = removal.removed;
	Body* absorbed = removal.absorbed_by;

	if (has_partitioning()) {
		partitioning_method->rem_body(removed);
	}

	if (absorbed) {
		if (has_partitioning()) {
			// Remove the body before absorption re-add after to deal with radius change.
			// A more efficient notify_radius_changed(Body, future_radius) can be a part of SpatialPartitioning,
			// but it would be messier. we would need to use its future radius, not previous radius.
			partitioning_method->rem_body(*absorbed);
			absorbed->absorb(removed);
			partitioning_method->add_body(*absorbed);
		}
		else {
			absorbed->absorb(removed);
		}


	}

	removed.notify_being_removed(removal);

	auto remove_it = get_iterator(removed.id);
	active_bodies.erase(remove_it);
}

void Universe::update_pos()
{
	for (int i = 0; i < active_bodies.size(); i++) {
		Body& body = *active_bodies[i];

		body.pos_update(settings.universe_size_max / 2);
	}
}

std::vector<float> Universe::gen_rand_portions(int num_slots) const
{
	std::vector<float> slots;
	slots.reserve(num_slots);

	float sum = 0.0f;
	for (int i = 0; i < num_slots; ++i) {
		float lot = Rand::real();
		slots.emplace_back(lot);
		sum += lot;
	}

	// map from [0:1] of total sum.
	for (int i = 0; i < num_slots; ++i) {
		slots[i] /= sum;
	}

	return slots;
}

std::vector<Collision> Universe::get_collisions_no_partitioning()
{
	num_collision_checks_tick = 0;

	std::vector<Collision> collisions;

	if (active_bodies.empty()) {
		return collisions;
	}

	collisions.reserve(active_bodies.size()); // could reserve on start, and on create_body resize it (after done handling).

	for (auto it1 = active_bodies.begin(); it1 != active_bodies.end() - 1; it1++) {
		Body& body1 = **it1;
		for (auto it2 = it1 + 1; it2 != active_bodies.end(); it2++) {
			Body& body2 = **it2;

			num_collision_checks_tick++;

			if (Physics::have_collided(body1, body2)) {
				collisions.emplace_back(Body::get_sorted_pair(body1, body2));
			}

		}
	}

	return collisions;
}

bool Universe::in_bounds(Vector2 point) const
{
	return Physics::point_in_rect(point, dimensions);
}

void Universe::handle_collision(Collision collision, std::vector<Removal>& to_remove)
{
	/*
	* A simple handling of collisions. The bigger object completely absorbs the smaller object.
	* 
	* However, since this method is in charge of what happens when a collision occurs (spatial partitionings and physics delegate that decision making to here),
	* it should be easy to add more complex and customizable collision mechanics later if wanted.
	*
	*/

	Body& bigger = collision.bigger;
	Body& smaller = collision.smaller;

	// need to check if bodies have already been removed. Can add flag to Body or keep a map.
	// or just loop through the presumably small to_remove vector

	// already_removed == true if one of the bodies in this collision event are already set to be removed.
	bool already_removed = std::any_of(to_remove.begin(), to_remove.end(), [&bigger, &smaller](Removal removal) { return removal.removed == bigger or removal.removed == smaller; });

	if (already_removed) {
		return;
	}

	to_remove.emplace_back(smaller, &bigger);

	// smaller.notify could remove from partitioning if partitioning makes itself an observer of that body.
	// but that would involve the partitioning systems managing their observer statuses of their bodies when they move.
	// and it is not costly to just remove from root on collision, since collision is relatively rare.

}

void Universe::handle_collisions(std::vector<Collision>& collisions)
{
	std::vector<Removal> to_remove;
	to_remove.reserve(active_bodies.size());

	for (const Collision& collision : collisions) {
		handle_collision(collision, to_remove);
	}

	for (Removal removal : to_remove) {
		handle_removal(removal);
	}
}

void Universe::update()
{
	// do grav pulls (update acceleration)
	if (settings.use_gravity_approximation) {
		barnes_quad.update(active_bodies);
		handle_gravity_approximation();
	}
	else {
		handle_gravity(); 
	}

	update_pos(); // update velocities and positions

	std::vector<Collision> collisions;

	if (has_partitioning()) {
		partitioning_method->update();
		collisions = partitioning_method->get_collisions();
		num_collision_checks_tick = partitioning_method->get_collision_checks_this_tick();
	}
	else {
		collisions = get_collisions_no_partitioning();
	}

	num_collision_checks += num_collision_checks_tick;

	handle_collisions(collisions);

	tick++;
}

Body& Universe::create_rand_body()
{
	float x = Rand::num(-settings.universe_size_start, settings.universe_size_start);
	float y = Rand::num(-settings.universe_size_start, settings.universe_size_start);
	long mass = Rand::num(1, settings.RAND_MASS);

	return create_body(x, y, mass);
}

Body& Universe::create_rand_system()
{
	float star_x = Rand::num(-settings.universe_size_start, settings.universe_size_start);
	float star_y = Rand::num(-settings.universe_size_start, settings.universe_size_start);
	std::vector<std::unique_ptr<Body>> system = generate_rand_system(star_x, star_y);

	Body& star = *system[0];

	add_bodies(system);

	return star;
}

std::vector<std::unique_ptr<Body>> Universe::generate_rand_system(float x, float y)
{
	std::vector<std::unique_ptr<Body>> system;

	int num_planets = Rand::num(settings.system_min_planets, settings.system_max_planets);
	system.reserve(num_planets + 10);

	long system_mass = Rand::num(1, settings.RAND_MASS) * 5000; // rand_mass is max planet mass of random planet
	long star_mass = settings.system_mass_ratio * system_mass;
	long remaining_mass = system_mass * (1 - settings.system_mass_ratio);

	Body& star = *system.emplace_back(std::make_unique<Body>(x, y, star_mass));

	std::vector<float> mass_ratios = gen_rand_portions(num_planets);

	for (int i = 0; i < num_planets; ++i) {
		long planet_mass = mass_ratios[i] * remaining_mass;

		Body& planet = *system.emplace_back(std::make_unique<Body>(0, 0, planet_mass));
		Orbit planet_orbit = gen_rand_orbit(star, planet, settings.retrograde_chance);
		planet.set_orbit(planet_orbit);
		

		if (Rand::chance(settings.moon_chance)) {
			// Can have this eat into planet's mass instead of just adding mass (currently actual mass > system_mass with moon generation).
			long moon_mass = .1f * planet.mass;

			Body& moon = *system.emplace_back(std::make_unique<Body>(0, 0, moon_mass));
			Orbit moon_orbit = gen_rand_orbit(planet, moon, settings.retrograde_chance);
			moon.set_orbit(moon_orbit);
		}

		// rand num moons (distribution based on mass maybe)
	}

	return system;
}


Body* Universe::get_body(Vector2 point) const
{
	if (!in_bounds(point)) {
		return nullptr;
	}

	if (has_partitioning()) {
		return partitioning_method->find_body(point);
	}
	else {
		auto it = std::find_if(active_bodies.begin(), active_bodies.end(), [point](const std::unique_ptr<Body>& body) {return body->contains_point(point); });
		if (it != active_bodies.end()) {
			return it->get();
		}
		else {
			return nullptr;
		}
	}
	return nullptr;
}

Body* Universe::get_body(int id) const
{
	auto it = std::find_if(active_bodies.begin(), active_bodies.end(), [id](const std::unique_ptr<Body>& body) { return body->id == id; });

	if (it != active_bodies.end()) {
		return it->get();
	}
	else {
		return nullptr;
	}
}

void Universe::rem_body(int id)
{
	// Find iterator->body to remove.
	auto remove_it = get_iterator(id);

	Body& body = **remove_it;

	if (has_partitioning()) {
		partitioning_method->rem_body(body);
	}

	body.notify_being_removed(nullptr);

	active_bodies.erase(remove_it);
}

Orbit Universe::gen_rand_orbit(const Body& orbited, const Body& orbiter, float retrograde_chance) const
{
	Orbit orbit { orbited };

	orbit.set_periapsis(orbiter, get_rand_sat_dist());
	orbit.grav_const = settings.grav_const;
	orbit.periapsis_angle = Rand::radian();
	orbit.eccentricity = Rand::real();
	orbit.prograde = Rand::chance(retrograde_chance) ? false : true;

	return orbit;
}


void Universe::rem_body(Body& body)
{

	// Find iterator->body to remove.
	auto remove_it = get_iterator(body.id);

	if (has_partitioning()) {
		partitioning_method->rem_body(**remove_it);
	}

	body.notify_being_removed(nullptr);

	active_bodies.erase(remove_it);
}
