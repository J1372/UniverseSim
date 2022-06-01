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

float Universe::get_rand_sat_dist() const
{
	return Rand::real() * (settings.SATELLITE_MAX_DIST - settings.SATELLITE_MIN_DIST) + settings.SATELLITE_MIN_DIST;
}

Universe::Universe()
{
	partitioning_method = std::make_unique<QuadTree>(settings.universe_size_max, 10, 10);
	create_universe();
	//std::cout << std::thread::hardware_concurrency();
}

Universe::Universe(const UniverseSettings& to_set) : settings(to_set),
dimensions{ -settings.universe_size_max / 2.0f, -settings.universe_size_max / 2.0f, settings.universe_size_max , settings.universe_size_max }
{
	partitioning_method = std::make_unique<QuadTree>(settings.universe_size_max, 10, 10);
	create_universe();
}

void Universe::add_body(std::unique_ptr<Body>&& body_ptr)
{
	if (!can_create_body()) {
		return;
	}

	Body& body = *body_ptr;
	body.id = generated_bodies++;

	active_bodies.emplace_back(std::move(body_ptr));

	if (has_partitioning()) {
		partitioning_method->add_body(body);
	}
}

void Universe::add_bodies(std::vector<std::unique_ptr<Body>>& bodies)
{
	int prev_size = active_bodies.size();

	int remaining_space = settings.UNIVERSE_CAPACITY - prev_size;

	// Enforce that adding bodies doesnt go over capacity.
	int adding = std::min(static_cast<int>(bodies.size()), remaining_space);

	auto end_it = bodies.begin() + adding;


	for (auto it = bodies.begin(); it != end_it; it++) {
		Body& body = **it;
		body.id = generated_bodies++;
	}

	active_bodies.insert(active_bodies.end(), std::make_move_iterator(bodies.begin()), std::make_move_iterator(end_it));
	bodies.clear();


	if (has_partitioning()) {
		auto it = std::next(active_bodies.begin(), prev_size);

		while (it != active_bodies.end()) {
			Body& body = **it;
			partitioning_method->add_body(body);
			it++;
		}
	}


}

std::vector<std::unique_ptr<Body>>::iterator Universe::get_iterator(int id)
{
	auto predicate = [](const std::unique_ptr<Body>& ptr, int id) {
		return ptr->id < id;
	};

	// Find iterator->body by id using binary search.
	return std::lower_bound(active_bodies.begin(), active_bodies.end(), id, predicate);
}


Universe::Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning) : settings(to_set), 
	partitioning_method(std::move(partitioning)),
	dimensions{ -settings.universe_size_max / 2.0f, -settings.universe_size_max / 2.0f, settings.universe_size_max , settings.universe_size_max }
{
	
	create_universe();
}

void Universe::create_universe()
{
	// TODO make physics settings changeable while running. Keep universe settings const while running.
	//		->move reserve/make unique out into constructor / run-once-at-start method.

	active_bodies.reserve(settings.UNIVERSE_CAPACITY);

	for (int i = 0; i < settings.num_rand_planets; ++i) {
		create_rand_body();
	}

	for (int i = 0; i < settings.num_rand_systems; ++i) {
		create_rand_system();
	}
}

bool Universe::can_create_body() const
{
	return generated_bodies < settings.UNIVERSE_CAPACITY;
	// interesting. no mass lost but bodies def decrease as absorption happens.
	// time -> denser universe. but at a point no new objects ccreated.
	// 
	// would like an efficient way to reuse ids, or just use a long.
	// 
}

void Universe::handle_gravity()
{
	for (int i = 0; i < active_bodies.size() - 1; i++) {
		Body& body1 = *active_bodies[i];
		for (int j = i + 1; j < active_bodies.size(); j++) {
			Body& body2 = *active_bodies[j];

			grav_pull(body1, body2);

		}
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
	handle_gravity(); // do grav pulls (update acceleration)
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
	long star_mass = settings.SYSTEM_STAR_MASS_RATIO * system_mass;
	//float remaining_mass = 1 - settings.SYSTEM_STAR_MASS_RATIO; // unused

	Body& star = *system.emplace_back(std::make_unique<Body>(x, y, star_mass));

	std::vector<float> mass_ratios = gen_rand_portions(num_planets);

	for (int i = 0; i < num_planets; ++i) {
		float ecc = Rand::real();
		// need to times mass ratio by remaining_mass, not system_mass.
		Body& planet = *system.emplace_back(std::make_unique<Body>(get_rand_sat_dist(), star, ecc, settings.grav_const, mass_ratios[i] * system_mass));


		float moon_roll = Rand::real();
		if (moon_roll < .1f) {
			ecc = Rand::real();
			// Can have this eat into planet's mass instead of just adding mass (currently actual mass > system_mass with moon generation).
			system.emplace_back(std::make_unique<Body>(get_rand_sat_dist(), planet, ecc, settings.grav_const, .1f * planet.mass));
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

void Universe::grav_pull(Body& body1, Body& body2) const
{
	// minor optimization, don't call dist_body. calc it from distv_body
	// 3 mass scalar makes very cool
	double force = (settings.grav_const * body1.mass * body2.mass) / std::pow(body1.dist_body(body2), 2);
	// this is the net force


	std::array<float, 2> dist_v = body1.distv_body(body2);

	float theta = atan2(dist_v[0], dist_v[1]); // radians
	// float dtheta = (double)theta * 180 / std::numbers::pi; // degrees

	// F = ma

	// calc net (x, y) force vectors for both. are they the same / reversed?

	std::array<float, 2> force_vectors { (float)(force * sin(theta)), (float)(force * cos(theta)) };

	body1.grav_pull(force_vectors);

	force_vectors[0] = -force_vectors[0];
	force_vectors[1] = -force_vectors[1];

	body2.grav_pull(force_vectors);


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
