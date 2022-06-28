#ifndef BODY_H
#define BODY_H

#include <raylib.h>
#include <string>

#include "Event.h"

struct Orbit;
struct Removal;

// Represents a type of planetary body.
struct PlanetType {
	Color color; // Color to use for the body.
	int density; // Density of this type.
	long min_mass; // Minimum mass required to be this type.

	// Returns whether a body's type rank has changed due to change in mass.
	int get_change(long body_mass, const PlanetType* next_type) const
	{
		// If mass has met the minimum mass requirement of the next type, return increased.
		if (body_mass >= next_type->min_mass) {
			return 1;
		}

		// If mass is now lower than this type's minimum mass, return decreased.
		if (body_mass < min_mass) {
			return -1;
		}

		// Body stayed the same type.
		return 0;
	}
	

};


class Body {

	/*
	* Min mass progressions (from prev type)
	*
	* 1,
	* 1000,
	* 6,
	* 15
	*
	*/

	static constexpr PlanetType MIN_TYPE = { RAYWHITE, 1, -1};
	static constexpr PlanetType ASTEROID_TYPE = { RAYWHITE, 10, 0 };
	static constexpr PlanetType PLANET_TYPE = { SKYBLUE, 15, 1000};
	static constexpr PlanetType SUN_TYPE = { GOLD, 100, 6000 };
	static constexpr PlanetType BLACK_HOLE_TYPE = { DARKGRAY, 1200, 120000 };
	static constexpr PlanetType MAX_TYPE = { SKYBLUE, 1, std::numeric_limits<long>::max()};
	static constexpr PlanetType TYPES[] =
	{
		MIN_TYPE,
		ASTEROID_TYPE,
		PLANET_TYPE,
		SUN_TYPE,
		BLACK_HOLE_TYPE,
		MAX_TYPE
	};

	// Body's unique id.
	int id = -1;

	// Universe position vector.
	Vector2 position;

	// Velocity vector.
	Vector2 velocity;

	// Net force vector acting on this body this tick.
	Vector2 force;

	// The radius of the body.
	float radius = 0.0f;

	// The body's mass.
	long mass = 0l;

	// A pointer to this body's current planetary type.
	const PlanetType* type = &TYPES[1]; // Starts as asteroid, updates on construction.

	// Represents debug info that has been attached to the body.
	std::string debug_info;

public:

	// Returns a pair of body pointers, where the first has more mass than the second.
	static std::pair<Body*, Body*> get_sorted_pair(Body& body1, Body& body2);

	Body() = default;

	Body(float x, float y, long mass);

	// Satellite constructor.
	Body(long mass, const Orbit& orbit, float point);

	// Sets this body's id.
	void set_id(int to_set);

	// Sets this body's position.
	void set_pos(Vector2 to_set);

	// Changes this body's position by the given vector.
	void change_pos(Vector2 movement);

	// Changes this body's velocity by the given vector.
	void change_vel(Vector2 acceleration);

	// Returns this body's id.
	int get_id() const;

	// Returns this body's position vector.
	Vector2 pos() const;

	// Returns this body's velocity vector.
	Vector2 vel() const;

	// Returns this body's radius.
	float get_radius() const;

	// Returns this body's mass.
	long get_mass() const;

	// Returns this body's color.
	Color color() const;


	// Moves the body to be at the given point in the orbit.
	// Sets body's velocity such that its path is that of the given orbit.
	void set_orbit(const Orbit& orbit, float point);

	// Returns true if this body's mass is greater than or equal to the mass of the other body.
	bool can_eat(const Body& other) const;

	// Adds the mass of the other body to this body, and updates this body's type if necessary.
	// Uses other body's momentum to simulate an impact force on this body, changing its velocity.
	void absorb(const Body& other);

	// Can have a partial absorb method as well.

	// Checks the body to see if it should change its type.
	// If this body's mass no longer meets its type's minimum mass requirement, downgrades this body's type.
	// If this body's mass >= to the next type's minimum mass requirement, upgrades this body's type.
	void upgrade_update();

	// Updates the body's position, using its current velocity, and resets its acceleration to 0.
	void pos_update();

	// Using the given forces, adjusts this body's acceleration.
	void grav_pull(Vector2 pull);

	// Returns this body's momentum vector.
	Vector2 get_momentum() const;

	// Returns the vector distance to the other body.
	Vector2 distv(const Body& other) const;

	// Returns the scalar distance between this body and the other body.
	float dist(const Body& other) const;

	// Returns the scalar distance squared between this body and the other body.
	float dist_squared(const Body& other) const;

	// Returns true if the point overlaps with this body.
	bool contains_point(Vector2 point) const;

	// Returns the leftmost x-coordinate of this body.
	float left() const;

	// Returns the rightmost x-coordinate of this body.
	float right() const;

	// Returns the top y-coordinate of this body.
	float top() const;

	// Returns the bottom y-coordinate of this body.
	float bottom() const;

	// Returns this body's diameter.
	float diameter() const;

	// Adds the given debug text to the body.
	void add_debug_text(const std::string& text);

	// Returns the body's debug text.
	const std::string& get_debug_text() const;

	// Clears body's debug text.
	void clear_debug_text();

	// Sets the body's mass to the given value, and checks for type changes.
	void set_mass(long to_set);

	// Changes the body's mass by the given value, and checks for type changes.
	void change_mass(long to_change);

	// Returns a bounding box for the body.
	Rectangle get_bounding_box() const;

	// Returns the body's mass moment vector (x * mass, y * mass).
	Vector2 get_mass_moment() const;

	// Returns true if this body's id is equal to the other body's id, else false.
	bool operator==(const Body& other) const;

	// Returns true if this body's id is not equal to the other body's id, else false.
	bool operator!=(const Body& other) const;

};

#endif
