#pragma once

class Body;
struct Vector2;

struct Orbit
{
	// The body being orbited.
	const Body& orbited;

	// Closest point of satellite to orbited body.
	float periapsis = 0;

	// Angle in radians of satellite's periapsis relative to orbited's center.
	float periapsis_angle = 0;


	/*
	*
	* ecc = 0.0		Circular
	* ecc < 1.0		Elliptical
	* ecc = 1.0		Parabolic
	* ecc > 1.0		Hyperbolic
	*
	*/

	// Relation between orbit's highest and lowest point (apoapsis and periapsis).
	// e = (Ra-Rp)/(Ra+Rp)
	float eccentricity = 0;

	// The universe's gravitational constant.
	float grav_const = 0;

	// If false, satellite orbital movement is reversed.
	bool prograde = true;

	// sets periapsis of orbit to units of sat_dists (orbited.radius + orbiter.radius).
	// sat_dist < 1, immediate collision
	// sat_dist = 1, touching.
	// sat_dist > 1, no collision
	void set_periapsis(const Body& orbiter, float sat_dist);

	// Returns the periapsis point of the satellite relative to the orbited body.
	Vector2 periapsis_point() const;

	// The farthest distance of the satellite from the orbited body in the orbit.
	float apoapsis() const;
	
	// Returns the orbit's semi major axis (periapsis + apoapsis) / 2
	float semi_major_axis() const;


	// Relative velocity of orbiter around orbited at orbiter's periapsis.
	float velocity_periapsis(const Body& orbiter) const;

	// Relative velocity vector of orbiter around orbiting at orbiter's periapsis.
	Vector2 velocity_periapsis_vector(const Body& orbiter) const;

	// Relative velocity vector of orbiter around orbiting at orbiter's periapsis.
	float velocity_periapsis_angle() const;

	// Relative velocity of orbiter around orbiting at a point in its orbit.
	// point = 0.0, periapsis.
	// point = 0.5, apoapsis.
	// point = 1.0, periapsis.
	//float vel_at(float point);

	//float orbital_period() const;

	Orbit(const Body& orbited) : orbited(orbited)
	{}

};
