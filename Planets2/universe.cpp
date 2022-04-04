#include "Universe.h"
#include <malloc.h>
#include <numbers>
#include <iostream>
#include "rand_float.h"

bool Universe::can_create_body() const
{
	return generated_bodies < INT_MAX;
	// interesting. no mass lost but bodies def decrease as absorption happens.
	// time -> denser universe. but at a point no new objects ccreated.
	// 
	// would like an efficient way to reuse ids, or just use a long.
	// 
}

void Universe::handle_collisions()
{
	//std::vector<int> to_remove;
	//to_remove.reserve(cur_bodies / 2);

	//root.perform_collision_check(to_remove);

	// hope map.erase calls its destructor but idk.
	//for (int i = 0; i < to_remove.size(); ++i) {
		//int id = to_remove[i];
		//body_map.erase(id);
	//}
	
	int index = 0;
	//bool curr_eaten = false;
	while (index < active_bodies.size()) {
		bool removed = handle_collision(index);
		if (!removed) {
			index++;
		}
	}
	
}

bool Universe::handle_collision(int it)
{
	Body& body1 = active_bodies[it];
	int it2 = it + 1;
	while (it2 < active_bodies.size()) {
		////std::cout << "\t\tCollision check against " << it2->first << "\n\n";
		Body& body2 = active_bodies[it2];

		if (body1.check_col(body2)) { // there is a collision
			////std::cout << "\t\t\tCollision!" << '\n';
			////std::cout << "\t\t\t" << body1.x << ',' << body1.y << ',' << body1.radius << '\n';
			////std::cout << "\t\t\t" << body2.x << ',' << body2.y << ',' << body2.radius << '\n';

			if (body1.can_eat(body2)) { // it1 eats it2
				body1.absorb(body2);

				// better way of removing elements from vector exists, but do this for now.
				active_bodies.erase(active_bodies.begin() + it2); // move to next check
			}
			else { // it2 eats it1
				body2.absorb(body1);

				active_bodies.erase(active_bodies.begin() + it); // it1 no longer exists, no more checks on other it2s.
				return true;
			}
		}
		else { // no collision. move to next check.
			it2++;
		}

	}
	
	return false;
}

void Universe::handle_gravity()
{
	// maybe set all acc to 0
	// std::unordered_map<int, Body>::iterator
	for (int i = 0; i < active_bodies.size() - 1; i++) {
		Body& body1 = active_bodies[i];
		for (int j = i + 1; j < active_bodies.size(); j++) {
			Body& body2 = active_bodies[j];

			grav_pull(body1, body2);

		}
	}
}

void Universe::update_pos()
{
	for (int i = 0; i < active_bodies.size(); i++) {
		Body& body = active_bodies[i];

		body.pos_update();
	}
}

std::vector<float> Universe::gen_rand_portions(int num_slots) const
{
	std::vector<float> slots;
	slots.reserve(num_slots);

	float sum = 0.0f;
	for (int i = 0; i < num_slots; ++i) {
		float lot = randf();
		slots.emplace_back(lot);
		sum += lot;
	}

	// map from [0:1] of total sum.
	for (int i = 0; i < num_slots; ++i) {
		slots[i] /= sum;
	}

	return slots;
}

void Universe::update()
{
	// handle_collisions first because we don't want to update bodies that we are just going to destroy.
	static int tick = 0;
	//std::cout << "Update #" << tick << '\n';
	handle_collisions(); // remove collided objects. on collision, performs body type upgrade maybe.
	handle_gravity(); // do grav pulls (update acceleration)
	update_pos(); // update velocities and positions

	// update quadtree after update_pos()
	++tick;
}

Body& Universe::create_body(float x, float y, long mass)
{
	int id = generated_bodies;

	active_bodies.emplace_back( id, x, y, mass );

	++generated_bodies;

	Body& body = active_bodies[active_bodies.size()-1];

	//root.add_body(body);

	return body;
}

Body& Universe::create_body(float sat_dist, const Body& orbiting, float ecc, long mass)
{
	int id = generated_bodies;

	active_bodies.emplace_back( id, sat_dist, ecc, orbiting, GRAV_CONST, mass );

	++generated_bodies;

	Body& body = active_bodies[active_bodies.size() - 1];

	//root.add_body(body);

	return body;
}

Body& Universe::create_rand_body()
{
	int id = generated_bodies;
	float x = randi(-UNIVERSE_START_SIZE, UNIVERSE_START_SIZE); // might be more efficient to randf * start_size
	float y = randi(-UNIVERSE_START_SIZE, UNIVERSE_START_SIZE);
	long mass = randi(1, RAND_MASS);

	active_bodies.emplace_back(id, x, y, mass);

	++generated_bodies;

	Body& body = active_bodies[active_bodies.size() - 1];

	//root.add_body(body);

	return body;
}

Body& Universe::create_rand_system()
{
	// might be useful to enforce a minimum

	long system_mass = randi(1, RAND_MASS) * 5000; // rand_mass is max planet mass of random planet oops
	// * 100 applies to min and max
	// rand_mass*100 applies only to max

	//int num_planets = (rand() % 10) + 1; // at least 1 planet, up to 10.
	constexpr int MIN_PLANETS = 100; // 100 - 300 was
	constexpr int MAX_PLANETS = 300;

	int num_planets = randi(MIN_PLANETS, MAX_PLANETS); // at least 1 planet, up to 10.
	//std::cout << num_planets << '\n';
	constexpr float star_mass_ratio = .95f;
	constexpr float remaining_mass = 1 - star_mass_ratio;

	



	float star_x = randi(-UNIVERSE_START_SIZE, UNIVERSE_START_SIZE);
	float star_y = randi(-UNIVERSE_START_SIZE, UNIVERSE_START_SIZE);
	//std::cout << RAND_MAX << '\n';
	long star_mass = star_mass_ratio * system_mass;

	Body& star = create_body(star_x, star_y, star_mass);
	
	std::vector<float> mass_ratios = gen_rand_portions(num_planets);

	for (int i = 0; i < num_planets; ++i) {
		Body& planet = create_satellite(star, randf(), mass_ratios[i] * system_mass);
		if (randf() < .1f) {
			create_satellite(planet, randf(), .1 * planet.mass);
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

	// Performs no checks on whether its initial position collides with another satellite of the body.
	// maybe perform the check in the create_system call
	static int num_created = 1;
	

	constexpr float MIN_DIST = 1.1F;
	constexpr float MAX_DIST = 40;
	float sat_dist = randf() * (MAX_DIST - MIN_DIST) + MIN_DIST; // sat_dist in range 1.5 to 10.0


	Body& sat = create_body(sat_dist, orbiting, ecc, mass); // create body first to have it calculate its size for us for periapsis.

	num_created++;

	std::cout << num_created << '\n';

	return sat;

}

Body& Universe::create_rand_satellite(const Body& orbiting)
{
	// create a satellite in range of 1.5-10 sat_dist size distance from orbited body.
	// 1 sat_dist = orbiting.size + my_size
	// if sat_dist = 1, then edges touching.
	// if sat_dist < 1, immediate collision.

	// Performs no checks on whether its initial position collides with another satellite of the body.

	float sat_dist = randf() * 8.5f + 1.5f; // need to be float, this is int. and goes up to 10.5

	//long mass = std::max((rand() % orbiting.mass) / 2, 1l);
	long mass = randi(1, (int)(orbiting.mass / 2)); // randi is 32-bit oops

	float ecc = randf(); // needs to be random in between 0 and 0.99. cannot be 1 unless use different calculation.
	Body& sat = create_body(0, 0, mass);


	// Calculating position of satellite at its periapsis (closest point to the body it orbits).
	float periapsis = sat_dist * (orbiting.radius + sat.radius);
	float periapsis_angle = (randf() * 2) * std::numbers::pi; // angle from orbiting body where periapsis is.
	float periapsis_v[2] = { periapsis * std::sin(periapsis_angle), periapsis * std::cos(periapsis_angle) };

	sat.x = orbiting.x + periapsis_v[0];
	sat.y = orbiting.y + periapsis_v[1];



	// Calculating velocity of satellite at its periapsis.
	float apoapsis = periapsis * (1 + ecc) / (1 - ecc);
	float semi_major_axis = (periapsis + apoapsis) / 2;

	float numerator = (1 + ecc) * GRAV_CONST * mass;
	float denominator = (1 - ecc) * semi_major_axis;
	float periapsis_vel = std::sqrt(numerator / denominator);

	float vel_v[2] = { periapsis_vel * sin(periapsis_angle), periapsis_vel * cos(periapsis_angle) };

	sat.vel_x = vel_v[0];
	sat.vel_y = vel_v[1];


	return sat;
}

void Universe::grav_pull(Body& body1, Body& body2) const
{
	// minor optimization, don't call dist_body. calc it from distv_body
	// 3 mass scalar makes very cool
	double force = GRAV_CONST * (1 * body1.mass * body2.mass) / std::pow(body1.dist_body(body2), 2);
	// this is the net force

	//std::cout << "Grav pull: " << body1.id << ", " << body2.id << '\n';
	//std::cout << "\tForce - " << force << "\n";

	std::array<float, 2> dist_v = body1.distv_body(body2);

	//std::cout << "\tdist_x - " << dist_v[0] << "\n";
	//std::cout << "\tdist_y - " << dist_v[1] << "\n\n";

	float theta = atan2(dist_v[0], dist_v[1]); // radians
	//std::cout << "\trTheta - " << theta << '\n';
	// float dtheta = (double)theta * 180 / std::numbers::pi; // degrees

	//std::cout << "\tdTheta - " << dtheta << "\n\n";

	// F = ma

	// calc net (x, y) force vectors for both. are they the same / reversed?

	std::array<float, 2> force_vectors { (float)(force * sin(theta)), (float)(force * cos(theta)) };

	//std::cout << "\tForce_x = " << force_vectors[0] << '\n';
	//std::cout << "\tForce_y = " << force_vectors[1] << "\n\n";

	body1.grav_pull(force_vectors);

	force_vectors[0] = -force_vectors[0];
	force_vectors[1] = -force_vectors[1];

	body2.grav_pull(force_vectors);


}