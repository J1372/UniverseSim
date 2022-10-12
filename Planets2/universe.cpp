#include "Universe.h"
#include "MyRandom.h"
#include "raylib.h"
#include "Physics.h"
#include <algorithm>

#include "Collision.h"
#include "Removal.h"

#include "Orbit.h"

Universe::Universe() : barnes_quad { settings.universe_size_max, .5f },
	dimensions{ -settings.universe_size_max / 2.0f, -settings.universe_size_max / 2.0f, settings.universe_size_max , settings.universe_size_max }
{
	partitioning_method = nullptr;
	//std::cout << std::thread::hardware_concurrency();
}

Universe::Universe(const UniverseSettings& to_set, std::unique_ptr<SpatialPartitioning>&& partitioning) : settings(to_set),
	partitioning_method(std::move(partitioning)),
	dimensions{ -settings.universe_size_max / 2.0f, -settings.universe_size_max / 2.0f, settings.universe_size_max , settings.universe_size_max },
	barnes_quad{ settings.universe_size_max, 10.0f }
{
	create_universe();
}

void Universe::add_body(Body&& body)
{
	if (!can_create_body()) {
		return;
	}

	if (!in_bounds(body.pos())) {
		return;
	}

	active_bodies.add(std::move(body));

	if (has_partitioning()) {
		partitioning_method->add_body(active_bodies.back());
	}


}

void Universe::add_bodies(std::vector<Body>&& bodies)
{
	int prev_size = active_bodies.size();

	int remaining_space = settings.universe_capacity - prev_size;

	// Enforce that adding bodies doesnt go over capacity.
	int adding = std::min(static_cast<int>(bodies.size()), remaining_space);

	auto it = bodies.begin();
	auto end_it = bodies.begin() + adding;
	while (it != end_it and can_create_body()) {
		Body& body = *it;
		add_body(std::move(body));
		it++;
	}

	bodies.clear();

}

void Universe::rem_from_partitioning(Body& to_remove)
{
	partitioning_method->rem_body(to_remove);

	const Body& moving = active_bodies.back();
	if (moving != to_remove) {
		// Bodies are removed by swapping with last element, then popping from the vector.
		// This means any pointer in partitioning to the last Body in the vector needs to 
		// be updated to point to the now removed body.
		partitioning_method->notify_move(&moving, &to_remove);
	}
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

	barnes_quad.set_size(settings.universe_size_max);
	BarnesHut::set_approximation(settings.grav_approximation_value);

	active_bodies.clear();
	active_bodies.reserve(settings.universe_capacity);

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

void Universe::handle_wraparound(Body& body)
{
	Vector2 pos = body.pos();
	float wraparound_val = settings.universe_size_max / 2;
	float reset_val = 2 * wraparound_val;

	if (pos.x > wraparound_val) {
		pos.x = -reset_val + pos.x;
	}
	else if (pos.x < -wraparound_val) {
		pos.x = reset_val + pos.x;
	}

	if (pos.y > wraparound_val) {
		pos.y = -reset_val + pos.y;
	}
	else if (pos.y < -wraparound_val) {
		pos.y = reset_val + pos.y;
	}

	body.set_pos(pos);
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

	// Changing this loop to use iterators had a big performance decrease,
	// which probably only affects the debug build, but is still annoying.
	for (int i = 0; i < active_bodies.size() - 1; i++) {
		Body& body1 = active_bodies[i];

		for (int j = i + 1; j < active_bodies.size(); j++) {
			Body& body2 = active_bodies[j];

			Physics::grav_pull(body1, body2, settings.grav_const);

		}
	}
}

void Universe::handle_gravity_approximation()
{
	for (int i = 0; i < active_bodies.size(); i++) {
		Body& body1 = active_bodies[i];
		barnes_quad.handle_gravity(body1, settings.grav_const);
	}
}

void Universe::handle_removal(Removal removal)
{
	removal.last = &active_bodies.back();
	on_removal_observers.notify_all(removal);
	Body* removed = removal.removed;
	Body* absorbed = removal.absorbed_by;

	if (absorbed) {
		if (has_partitioning()) {
			// Remove the body before absorption re-add after to deal with radius change.
			// A more efficient notify_radius_changed(Body, future_radius) can be a part of SpatialPartitioning,
			// but it would be messier. we would need to use its future radius, not previous radius.
			partitioning_method->rem_body(*absorbed);
			absorbed->absorb(*removed);
			partitioning_method->add_body(*absorbed);
		}
		else {
			absorbed->absorb(*removed);
		}


	}

	if (has_partitioning()) {
		rem_from_partitioning(*removed);
	}

	active_bodies.rem(*removed);
}

void Universe::update_pos()
{
	for (auto it = active_bodies.begin(); it != active_bodies.end(); it++) {
		Body& body = *it;

		body.pos_update();

		handle_wraparound(body);
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
		Body& body1 = *it1;
		for (auto it2 = it1 + 1; it2 != active_bodies.end(); it2++) {
			Body& body2 = *it2;

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
	bool already_removed = std::any_of(to_remove.begin(), to_remove.end(), [&bigger, &smaller](Removal removal) { return removal.removed == &bigger or removal.removed == &smaller; });

	if (already_removed) {
		return;
	}

	to_remove.emplace_back(&smaller, &bigger);

}

void Universe::handle_collisions(std::span<const Collision> collisions)
{
	std::vector<Removal> to_remove;
	to_remove.reserve(active_bodies.size());

	for (const Collision& collision : collisions) {
		handle_collision(collision, to_remove);
	}

	for (int i = 0; i < to_remove.size(); ++i) {
		Removal removal = to_remove[i];

		for (int j = i + 1; j < to_remove.size(); ++j) {
			Removal& to_update = to_remove[j];
			if (to_update.removed == &active_bodies.back()) {
				to_update.removed = removal.removed;
			}

			if (to_update.absorbed_by == removal.removed) {
				to_update.absorbed_by = removal.absorbed_by; // Could also just delete this event instead.
			}

			if (to_update.absorbed_by == &active_bodies.back()) { // can else if with first if
				to_update.absorbed_by = removal.removed;
			}
		}

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

void Universe::create_rand_system()
{
	float star_x = Rand::num(-settings.universe_size_start, settings.universe_size_start);
	float star_y = Rand::num(-settings.universe_size_start, settings.universe_size_start);
	std::vector<Body> system = generate_rand_system(star_x, star_y);

	add_bodies(std::move(system));
}

std::vector<Body> Universe::generate_rand_system(float x, float y)
{
	std::vector<Body> system;

	int num_planets = Rand::num(settings.system_min_planets, settings.system_max_planets);
	int approximate_num_moons = settings.moon_chance * num_planets;
	system.reserve(num_planets + approximate_num_moons);

	long system_mass = Rand::num(1, settings.RAND_MASS) * 5000; // rand_mass is max mass of random planet
	long star_mass = settings.system_mass_ratio * system_mass;
	long remaining_mass = system_mass * (1 - settings.system_mass_ratio);

	Body& star = system.emplace_back(x, y, star_mass);

	std::vector<float> mass_ratios = gen_rand_portions(num_planets);

	// Compress the maximum possible distance of a moon to its planet.
	float moon_max_dist = std::max(settings.satellite_min_dist, settings.satellite_max_dist / 10);

	for (int i = 0; i < num_planets; ++i) {
		long planet_mass = mass_ratios[i] * remaining_mass;

		Body& planet = system.emplace_back(0.0f, 0.0f, planet_mass);
		Orbit planet_orbit = gen_rand_orbit(star, planet);
		planet.set_orbit(planet_orbit, Rand::real());
		

		if (Rand::chance(settings.moon_chance)) {
			// Currently only one moon can be generated per planet.
			// Can have this eat into planet's mass instead of just adding mass (currently actual mass > system_mass with moon generation).
			long moon_mass = Rand::real(0.01f, 0.1f) * planet.get_mass();

			Body& moon = system.emplace_back(0.0f, 0.0f, moon_mass);
			Orbit moon_orbit = gen_rand_orbit(planet, moon);
			moon_orbit.set_periapsis(planet, Rand::real(settings.satellite_min_dist, moon_max_dist));
			moon.set_orbit(moon_orbit, Rand::real());
		}

	}

	return system;
}

Body* Universe::get_body(Vector2 point)
{
	if (!in_bounds(point)) {
		return nullptr;
	}

	if (has_partitioning()) {
		// Try to find the body using our partitioning method.
		return partitioning_method->find_body(point);
	}
	else {
		// Try to find the body by looping through all bodies.
		// Could use a template method or std::function in BodyList.
		auto it = std::find_if(active_bodies.begin(), active_bodies.end(), [point](const Body& body) {return body.contains_point(point); });
		if (it != active_bodies.end()) {
			Body& body = *it;
			return &body;
		}
		else {
			return nullptr;
		}
	}
	return nullptr;
}

Body* Universe::get_body(int search_id)
{
	int index = active_bodies.get_index(search_id);

	if (index != -1)
	{
		return &active_bodies[index];
	}
	else
	{
		return nullptr;
	}

}

Orbit Universe::gen_rand_orbit(const Body& orbited, const Body& orbiter) const
{
	Orbit orbit { orbited };

	orbit.set_periapsis(orbiter, Rand::real(settings.satellite_min_dist, settings.satellite_max_dist));
	orbit.grav_const = settings.grav_const;
	orbit.periapsis_angle = Rand::radian();
	orbit.eccentricity = Rand::real();
	orbit.prograde = Rand::chance(settings.retrograde_chance) ? false : true;

	return orbit;
}


void Universe::rem_body(Body& body)
{
	if (has_partitioning()) {
		rem_from_partitioning(body);
	}

	on_removal_observers.notify_all({ &body, nullptr, &active_bodies.back()});

	// Active bodies no longer needs to be sorted by id, so we can swap-pop.
	active_bodies.rem(body);
}

bool Universe::has_partitioning() const
{
	return partitioning_method.get() != nullptr;
}

const SpatialPartitioning* Universe::get_partitioning() const
{
	return partitioning_method.get();
}

UniverseSettings& Universe::get_settings()
{
	return settings;
}

std::span<const Body> Universe::get_bodies() const
{
	return active_bodies.span();
}

std::span<Body> Universe::get_bodies()
{
	return active_bodies;
}

int Universe::get_num_bodies() const
{
	return active_bodies.size();
}

int Universe::get_num_collision_checks() const
{
	return num_collision_checks;
}

int Universe::get_num_collision_checks_tick() const
{
	return num_collision_checks_tick;
}

int Universe::get_tick() const
{
	return tick;
}

Event<Removal>& Universe::removal_event()
{
	return on_removal_observers;
}
