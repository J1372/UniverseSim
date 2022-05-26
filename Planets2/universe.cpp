#include "Universe.h"
#include <malloc.h>
#include <numbers>
#include <iostream>
#include "my_random.h"
#include "raylib.h"
#include "Physics.h"
#include <algorithm>

#include <vector>

Universe::Universe()
{
	partitioning_method = std::make_unique<QuadTree>(2 * settings.universe_size_max);
	generate_universe();
	//std::cout << std::thread::hardware_concurrency();
}

Universe::Universe(const UniverseSettings& to_set)
{
	settings = to_set;
	partitioning_method = std::make_unique<QuadTree>(2 * settings.universe_size_max);
	generate_universe();
}


Universe::Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning_method) : partitioning_method(std::move(partitioning_method)) {
	settings = to_set;
	generate_universe();
}

void Universe::generate_universe()
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
	return generated_bodies < INT_MAX;
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

void Universe::update_pos()
{
	for (int i = 0; i < active_bodies.size(); i++) {
		Body& body = *active_bodies[i];

		body.pos_update(settings.universe_size_max);
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

void Universe::handle_collision(Collision collision, std::vector<int>& to_remove)
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
	bool already_removed = std::any_of(to_remove.begin(), to_remove.end(), [&bigger, &smaller](int id) { return id == bigger.id or id == smaller.id; });

	if (already_removed) {
		return;
	}

	bigger.absorb(smaller);
	smaller.notify_being_removed(&bigger);

	// smaller.notify could remove from partitioning if partitioning makes itself an observer of that body.
	// but that would involve the partitioning systems managing their observer statuses of their bodies when they move.
	// and it is not costly to just remove from root on collision, since collision is relatively rare.

	to_remove.push_back(smaller.id);

	partitioning_method->rem_body(smaller);
}

void Universe::handle_collisions()
{
	std::vector<Collision> collisions = partitioning_method->get_collisions();
	std::vector<int> to_remove;
	to_remove.reserve(active_bodies.size());

	for (const Collision& collision : collisions) {
		handle_collision(collision, to_remove);
	}

	// Sort the ids.
	std::sort(to_remove.begin(), to_remove.end());

	// Comparison predicate for std::lower_bound.
	auto predicate = [](const std::unique_ptr<Body>& ptr, int id) {
		return ptr->id < id;
	};

	// Both active_bodies and to_remove are sorted by id in increasing order.
	// We can loop backwards through to_remove and do binary search on active_bodies to efficiently remove all bodies with ids in to_remove.
	for (auto it = to_remove.rbegin(); it != to_remove.rend(); it++) {
		int id = *it;

		// Find iterator->body to remove using binary search.
		auto remove_it = std::lower_bound(active_bodies.begin(), active_bodies.end(), id, predicate);

		active_bodies.erase(remove_it);
	}
}
void Universe::update()
{
	handle_gravity(); // do grav pulls (update acceleration)
	update_pos(); // update velocities and positions 
	partitioning_method->update();
	handle_collisions();

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
	long system_mass = Rand::num(1, settings.RAND_MASS) * 5000; // rand_mass is max planet mass of random planet

	int num_planets = Rand::num(settings.system_min_planets, settings.system_max_planets);

	float remaining_mass = 1 - settings.SYSTEM_STAR_MASS_RATIO;

	



	float star_x = Rand::num(-settings.universe_size_start, settings.universe_size_start);
	float star_y = Rand::num(-settings.universe_size_start, settings.universe_size_start);
	long star_mass = settings.SYSTEM_STAR_MASS_RATIO * system_mass;

	Body& star = create_body(star_x, star_y, star_mass);
	
	std::vector<float> mass_ratios = gen_rand_portions(num_planets);

	for (int i = 0; i < num_planets; ++i) {
		Body& planet = create_satellite(star, Rand::real(), mass_ratios[i] * system_mass);
		if (Rand::real() < .1f) {
			create_satellite(planet, Rand::real(), .1 * planet.mass);
		}
		// rand num moons (distribution based on mass maybe)
	}

	return star;
}

Body& Universe::create_satellite(const Body& orbiting, float ecc, long mass)
{
	// create a satellite in range of 1.5-10 sat_dist size distance from orbited body.
	// 1 sat_dist = orbiting.size + my_size
	// 1 size away == touch edge of orbiting body
	// if sat_dist < 1, immediate collision.

	// Performs no checks on whether its initial position collides with another satellite of the body.
	// maybe perform the check in the create_system call

	float sat_dist = Rand::real() * (settings.SATELLITE_MAX_DIST - settings.SATELLITE_MIN_DIST) + settings.SATELLITE_MIN_DIST;

	return create_body(sat_dist, orbiting, ecc, settings.grav_const, mass);

}

Body& Universe::create_rand_satellite(const Body& orbiting)
{
	//long mass = std::max((rand() % orbiting.mass) / 2, 1l);
	long mass = Rand::num(1, (int)(orbiting.mass / 2)); // randi is 32-bit.

	float ecc = Rand::real(); // needs to be random in between 0 and 0.99. cannot be 1 unless use different calculation.

	return create_satellite(orbiting, ecc, mass);
}

Body* Universe::get_body(Vector2 point) const
{
	if (has_partitioning()) {
		return partitioning_method->find_body(point);
	}
	else {
		auto it = std::find_if(active_bodies.begin(), active_bodies.end(), [point](const std::unique_ptr<Body>& body) {return body->contains_point(point); });
		if (it != active_bodies.end()) {
			return &**it; // Returns address of a dereference of an iterator to unique_ptr to Body. (Body*)
		}
		else {
			return nullptr;
		}
	}
	return nullptr;
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
