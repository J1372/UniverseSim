#include "Body.h"
#include <cmath>
#include <iostream>
#include "my_random.h"
#include <numbers>
#include "Removal.h"
#include "Physics.h"

#include "Orbit.h"

Body::Body(float x, float y, long mass) : position{ x, y }, mass(std::max(1l, mass))
{
	upgrade_update();
}

Body::Body(long mass, const Orbit& orbit, float point) :
	mass(std::max(1l, mass))
{
	upgrade_update();
	set_orbit(orbit, point);

}

void Body::set_id(int to_set)
{
	id = to_set;
}

void Body::set_pos(Vector2 to_set)
{
	position = to_set;
}

void Body::change_pos(Vector2 movement)
{
	position.x += movement.x;
	position.y += movement.y;
}

void Body::change_vel(Vector2 acceleration)
{
	velocity.x += acceleration.x;
	velocity.y += acceleration.y;
}

int Body::get_id() const
{
	return id;
}

Vector2 Body::pos() const
{
	return position;
}

Vector2 Body::vel() const
{
	return velocity;
}

float Body::get_radius() const
{
	return radius;
}

long Body::get_mass() const
{
	return mass;
}

Color Body::color() const
{
	return type->color;
}

std::array<float, 2> Body::distv(const Body& other) const
{
	std::array<float, 2> vector = { other.position.x - position.x,
		other.position.y - position.y };
	return vector;
}

float Body::dist(const Body& other) const
{
	return std::sqrt(dist_squared(other));
}

float Body::dist_squared(const Body& other) const
{
	std::array<float, 2> vector = distv(other);
	float c_squared = std::pow(vector[0], 2) + std::pow(vector[1], 2);
	return c_squared;
}

bool Body::contains_point(Vector2 point) const
{
	return Physics::point_in_circle(point, position.x, position.y, radius);
}

float Body::left() const
{
	return position.x - radius;
}

float Body::right() const
{
	return position.x + radius;
}

float Body::top() const
{
	return position.y - radius;
}

float Body::bottom() const
{
	return position.y + radius;
}

float Body::diameter() const
{
	return 2 * radius;
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

void Body::set_orbit(const Orbit& orbit, float point)
{
	// Relative position of this satellite around the body it is orbiting.
	Vector2 relative_pos = orbit.pos_at(point);
	Vector2 central_pos = orbit.orbited.pos();
	position.x = central_pos.x + relative_pos.x;
	position.y = central_pos.y + relative_pos.y;

	// Relative velocity of this satellite around the body it is orbiting.
	Vector2 relative_velocity = orbit.vel_vec_at(point);

	// Final velocity of the body is its relative velocity added to the velocity of the body it is orbiting.
	velocity.x = orbit.orbited.velocity.x + relative_velocity.x;
	velocity.y = orbit.orbited.velocity.y + relative_velocity.y;

}

bool Body::can_eat(const Body& other) const
{
	return mass >= other.mass;
}

void Body::absorb(const Body& other)
{
	std::array<float, 2> mom = get_momentum();
	std::array<float, 2> other_mom = other.get_momentum();
	std::array<float, 2> combined_mom { mom[0] + other_mom[0],
		mom[1] + other_mom[1] };

	long combined_mass = mass + other.mass;

	// Calculate the final velocity of the combined mass.
	float vel_fx = combined_mom[0] / ((double)combined_mass);
	float vel_fy = combined_mom[1] / ((double)combined_mass);

	velocity.x = vel_fx;
	velocity.y = vel_fy;


	// upgrade its type if it meets mass requirements.
	set_mass(combined_mass);

}


void Body::upgrade_update()
{
	while (mass < TYPES[type_level].min_mass) {
		type_level--;
	}

	while (mass >= TYPES[type_level + 1].min_mass) {
		type_level++;
	}

	type = &TYPES[type_level];

	// After updating type, recalculate body radius.
	radius = std::max(((float)mass) / type->density, 1.0f);
}

void Body::pos_update()
{
	Vector2 acceleration{
		force.x / mass,
		force.y / mass,
	};

	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	position.x += velocity.x;
	position.y += velocity.y;

	force.x = 0; // set forces on this body to 0 for next tick.
	force.y = 0;

}

void Body::grav_pull(std::array<float, 2> force_vector)
{
	force.x += force_vector[0];
	force.y += force_vector[1];
}

std::array<float, 2> Body::get_momentum() const
{
	return { mass * velocity.x , mass * velocity.y };
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
	return { mass * position.x, mass * position.y };
}

bool Body::operator==(const Body& other) const
{
	return id == other.id;
}

bool Body::operator!=(const Body& other) const
{
	return id != other.id;
}
