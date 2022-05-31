#include "Body.h"
#include <cmath>
#include <iostream>
#include "my_random.h"
#include <numbers>
#include "Removal.h"
#include "Physics.h"



void Body::do_wraparound(float wraparound_val)
{
	float reset_val = 2 * wraparound_val;

	if (x > wraparound_val) {
		x = -reset_val + x;
	}
	else if (x < -wraparound_val) {
		x = reset_val - x;
	}

	if (y > wraparound_val) {
		y = -reset_val + y;
	}
	else if (y < -wraparound_val) {
		y = reset_val - y;
	}
}

Body::Body(float sat_dist, const Body& orbiting, float ecc, float grav_const, long mass)
{
	this->mass = std::max(1l, mass);

	upgrade_update();

	radius = std::max(((float)mass) / type->density, 1.0f);



	/*
	*
	* ecc = 0.0		Circular
	* ecc < 1.0		Elliptical
	* ecc = 1.0		Parabolic
	* ecc > 1.0		Hyperbolic
	*
	*
	*/
	// find random (x,y) pair using ecc.
	// e = (Ra-Rp)/(Ra+Rp)
	// find apoapsis
	// 


	float periapsis = sat_dist * (orbiting.radius + radius);
	float periapsis_angle = (Rand::real() * 2) * std::numbers::pi; // angle from orbiting body where periapsis is.
	float periapsis_pos[2] = { periapsis * std::cos(periapsis_angle), periapsis * std::sin(periapsis_angle) };
	// for some reason x = sin and y = cos;
	// this should be valid x and y for pos and negative. hope so :)

	float apoapsis = periapsis * (1 + ecc) / (1 - ecc); // apoapsis on opposite end. pi radians degrees.
	float semi_major_axis = (periapsis + apoapsis) / 2;

	// this is a random degree from periapsis;
	// for now, every planet starts at their periapsis. the periapsis is random.
	// in the future, I'd like every planet to start at a random point on there orbit.

	/*float true_anomaly = (randf() * 2) * std::numbers::pi;
	float dist_at_point = semi_major_axis * (1 - std::pow(ecc, 2)) / (1 + ecc * std::cos(true_anomaly));*/

	// find x and y using dist_at_point and true_anomaly
	// 
	// int x = dist_at_point
	// ecc >= 1.0 has possibility of divide by 0.

	x = periapsis_pos[0] + orbiting.x;
	y = periapsis_pos[1] + orbiting.y;

	// currently sets velocity for if they're at periapsis with no implementation for random point in there orbit.

	//periapsis_angle at periapsis_v[0][1];
	// all velocity at periapsis and apoapsis is tangental to body. 90 degrees = pi/2
	float num = (1 + ecc) * grav_const * (orbiting.mass + mass); // orbiting.mass at least, possibly + mass
	float den = (1 - ecc) * semi_major_axis;
	float velocity_periapsis = std::sqrt(num / den);

	//float v_angle = periapsis_angle;
	float velocity_angle = 2 * std::numbers::pi - periapsis_angle; // flip on y-axis

	// Relative velocity of the body around the body it is orbiting.
	float relative_velocity[2] = { velocity_periapsis * sin(velocity_angle), velocity_periapsis * cos(velocity_angle) };

	constexpr double RETROGRADE_CHANCE = 0.12;
	bool retrograde_roll = Rand::real() < RETROGRADE_CHANCE;

	if (retrograde_roll) {
		relative_velocity[0] = -relative_velocity[0];
		relative_velocity[1] = -relative_velocity[1];
	}

	// Final velocity of the body is its relative velocity added to the velocity of the body it is orbiting.
	vel_x = relative_velocity[0] + orbiting.vel_x;
	vel_y = relative_velocity[1] + orbiting.vel_y;

#ifdef MY_DEBUG
	std::cout << "Periapsis: " << periapsis << '\n';
	std::cout << "Per_theta: " << periapsis_angle * 57.2958 << '\n';
	std::cout << "Per_x    : " << periapsis_v[0] << '\n';
	std::cout << "Per_y    : " << periapsis_v[1] << "\n\n";
	std::cout << "Vel_p    : " << v_per << '\n';
	std::cout << "Vel_theta: " << v_angle * 57.2958 << '\n';
	std::cout << "Vel_x    : " << vel_v[0] << '\n';
	std::cout << "Vel_y    : " << vel_v[1] << "\n\n";

	std::cout << "Semi-maj : " << semi_major_axis << '\n';
	std::cout << "Apoapsis : " << apoapsis << '\n';
	std::cout << "Sat_dist : " << sat_dist << "\n\n";
#endif

}

std::array<float, 2> Body::distv_body(const Body& other) const
{
	return { other.x - x , other.y - y };
}

float Body::dist_body(const Body& other) const
{
	float c_squared = std::pow(std::abs(other.x - x), 2) + std::pow(std::abs(other.y - y), 2);
	return std::sqrt(c_squared);
}

bool Body::contains_point(Vector2 point) const
{
	return Physics::point_in_circle(point, x, y, radius);
}

std::pair<Body*, Body*> Body::get_sorted_pair(Body& body1, Body& body2)
{
	if (body1.mass >= body2.mass) {
		return std::make_pair<Body*, Body*>(&body1, &body2);
	}
	else {
		return std::make_pair<Body*, Body*>(&body2, &body1);
	}
}

bool Body::can_eat(const Body& other) const
{
	return mass >= other.mass;
}

void Body::absorb(const Body& other)
{
	std::array<float, 2> mom = get_momentum();
	std::array<float, 2> other_mom = other.get_momentum();
	std::array<float, 2> combined_mom;

	combined_mom[0] = mom[0] + other_mom[0];
	combined_mom[1] = mom[1] + other_mom[1];


	long combined_mass = mass + other.mass;
	float d_vel_x = combined_mom[0] / ((double)combined_mass); // vf , not delta
	float d_vel_y = combined_mom[1] / ((double)combined_mass);

	vel_x = d_vel_x;
	vel_y = d_vel_y;


	// upgrade its type if it meets mass requirements.
	set_mass(combined_mass);

	/*float force_x = other.mass / -other.vel_x;
	float force_y = other.mass / -other.vel_y;

	float acc_x = force_x / mass; // == (prev_mass + other.mass), we have its mass now
	float acc_y = force_y / mass;

	vel_x += acc_x;
	vel_y += acc_y;*/
	//vel_x += .3*other.vel_x; // this should be a portion of the force that other smacks u with.
	//vel_y += .3*other.vel_y;
}


void Body::upgrade_update()
{
	while (mass >= TYPES[type_level + 1].min_mass) {

		type_level++;
	}

	type = &TYPES[type_level];

	// After updating type, recalculate body radius.
	radius = std::max(((float)mass) / type->density, 1.0f);
}

void Body::pos_update(float wraparound_val)
{
	vel_x += acc_x;
	vel_y += acc_y;

	x += vel_x;
	y += vel_y;

	acc_x = 0; // set accelerations to 0 for next tick
	acc_y = 0;

	do_wraparound(wraparound_val);

}

void Body::grav_pull(std::array<float, 2> force_vector)
{

	// calc net acceleration for both vectors.
	float d_acc_x = force_vector[0] / mass;
	float d_acc_y = force_vector[1] / mass;

	acc_x += d_acc_x;
	acc_y += d_acc_y;

}

std::array<float, 2> Body::get_momentum() const
{
	return { mass * vel_x , mass * vel_y };
}

void Body::add_debug_text(const std::string&& text)
{
	debug_info += text + "\n";
}

const std::string& Body::get_debug_text() const
{
	return debug_info;
}

void Body::clear_debug_text()
{
	debug_info.clear();
}

void Body::set_mass(long to_set)
{
	mass = std::max(1l, to_set);
	upgrade_update();
}

void Body::change_mass(long to_change)
{
	mass = std::max(1l, mass + to_change);
	upgrade_update();
}

void Body::notify_being_removed(Body* absorbed_by)
{
	Removal removal { *this, absorbed_by };
	on_removal_observers.notify_all(removal);
}

void Body::notify_being_removed(Removal removal)
{
	on_removal_observers.notify_all(removal);
}

Event<Removal>& Body::removal_event()
{
	return on_removal_observers;
}

Rectangle Body::get_bounding_box() const
{
	return { left(), top(), diameter(), diameter() };
}

bool Body::operator==(const Body& other) const
{
	return id == other.id;
}

bool Body::operator!=(const Body& other) const
{
	return id != other.id;
}
