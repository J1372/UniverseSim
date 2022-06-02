#include "Body.h"
#include <cmath>
#include <iostream>
#include "my_random.h"
#include <numbers>
#include "Removal.h"
#include "Physics.h"

#include "Orbit.h"


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

Body::Body(float x, float y, long mass) : x(x), y(y), mass(std::max(1l, mass))
{
	upgrade_update();
}

Body::Body(long mass, const Orbit& orbit) :
	mass(std::max(1l, mass))
{
	upgrade_update();
	set_orbit(orbit);

}

std::array<float, 2> Body::distv_body(const Body& other) const
{
	return { other.x - x , other.y - y };
}

float Body::dist_body(const Body& other) const
{
	// can remove abs
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

void Body::set_orbit(const Orbit& orbit)
{
	// Currently all satellites start at periapsis.
	Vector2 relative_pos = orbit.periapsis_point();
	x = orbit.orbited.x + relative_pos.x;
	y = orbit.orbited.y + relative_pos.y;

	// Relative velocity of this satellite around the body it is orbiting.
	Vector2 relative_velocity = orbit.velocity_periapsis_vector(*this); // might not need to send *this if use orbital period.

	// Final velocity of the body is its relative velocity added to the velocity of the body it is orbiting.
	vel_x = orbit.orbited.vel_x + relative_velocity.x;
	vel_y = orbit.orbited.vel_y + relative_velocity.y;
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

Vector2 Body::get_mass_moment() const
{
	return { mass * x, mass * y };
}

bool Body::operator==(const Body& other) const
{
	return id == other.id;
}

bool Body::operator!=(const Body& other) const
{
	return id != other.id;
}
