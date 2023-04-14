#include "Body.h"
#include "Removal.h"
#include "Physics.h"
#include <raymath.h>

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
	position = Vector2Add(position, movement);
}

void Body::change_vel(Vector2 acceleration)
{
	velocity = Vector2Add(velocity, acceleration);
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

Vector2 Body::get_forces() const
{
	return force;
}

Color Body::color() const
{
	return type->get_color();
}

Vector2 Body::distv(const Body& other) const
{
	return Vector2Subtract(other.position, position);
}

float Body::dist(const Body& other) const
{
	return std::sqrt(dist_squared(other));
}

float Body::dist_squared(const Body& other) const
{
	Vector2 vector = distv(other);
	float c_squared = std::pow(vector.x, 2) + std::pow(vector.y, 2);
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
	position = Vector2Add(orbit.orbited.position, relative_pos);

	// Relative velocity of this satellite around the body it is orbiting.
	Vector2 relative_velocity = orbit.vel_at(point);

	// Final velocity of the body is its relative velocity added to the velocity of the body it is orbiting.
	velocity = Vector2Add(orbit.orbited.velocity, relative_velocity);

}

bool Body::can_eat(const Body& other) const
{
	return mass >= other.mass;
}

void Body::absorb(const Body& other)
{
	// Completely inelastic collision.
	Vector2 final_momentum = Vector2Add(get_momentum(), other.get_momentum());
	long combined_mass = mass + other.mass;

	// Calculate the final velocity of the combined mass.
	Vector2 final_velocity = { final_momentum.x / combined_mass,
		final_momentum.y / combined_mass };

	velocity = final_velocity;
	set_mass(combined_mass);

}


void Body::upgrade_update()
{
	int change = type->get_change(mass, type + 1);

	while (change != 0)
	{
		type = type + change;
		change = type->get_change(mass, type + 1);
	}

	// After updating type, recalculate body radius.
	radius = type->get_radius(mass);
}

void Body::pos_update()
{
	Vector2 acceleration{
		force.x / mass,
		force.y / mass,
	};

	velocity = Vector2Add(velocity, acceleration);
	position = Vector2Add(position, velocity);

}

void Body::reset_forces()
{
	// set forces on this body to 0 for next tick.
	force = { 0,0 };
}

void Body::grav_pull(Vector2 pull)
{
	force = Vector2Add(force, pull);
}

Vector2 Body::get_momentum() const
{
	return Physics::moment(velocity, mass);
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

Rectangle Body::get_bounding_box() const
{
	return { left(), top(), diameter(), diameter() };
}

Vector2 Body::get_mass_moment() const
{
	return Physics::moment(position, mass);
}

bool Body::operator==(const Body& other) const
{
	return id == other.id;
}

bool Body::operator!=(const Body& other) const
{
	return id != other.id;
}
