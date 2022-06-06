#ifndef BODY_H
#define BODY_H

#include <raylib.h>
#include <cmath>
#include <array>
#include <vector>
#include <string>

#include "Event.h"

struct Orbit;
struct Removal;

// Represents a type of planetary body.
struct TypeExt {
	int density; // Density of this type.
	long min_mass; // Bodies below this mass cannot be this type.
	Color color; // Color to use for the body.
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
	static constexpr TypeExt ASTEROID_TYPE = { 10, 0, RAYWHITE };
	static constexpr TypeExt PLANET_TYPE = { 14, 1000, SKYBLUE };
	static constexpr TypeExt SUN_TYPE = { 50, 6000, GOLD };
	static constexpr TypeExt BLACK_HOLE_TYPE = { 1000, 90000, DARKGRAY };
	static constexpr TypeExt END_TYPE = { -1, LONG_MAX, SKYBLUE }; // A null type.

	static constexpr TypeExt TYPES[] =
	{
		ASTEROID_TYPE,
		PLANET_TYPE,
		SUN_TYPE,
		BLACK_HOLE_TYPE,
		END_TYPE
	};

	// The current body's type index in TYPES.
	int type_level = 0;

	// Represents debug info that has been attached to the body.
	std::string debug_info;

	// Observers to notify when this body is being removed.
	Event<Removal> on_removal_observers;


	void do_wraparound(float wraparound_val);


public:

	// Returns a pair of body pointers, where the first has more mass than the second.
	static std::pair<Body*, Body*> get_sorted_pair(Body& body1, Body& body2);

	int id = -1;

	float x = 0.0;
	float y = 0.0;
	float vel_x = 0;
	float vel_y = 0;
	float acc_x = 0;
	float acc_y = 0;

	float radius = 0.0; // in units. same as distance.
	long mass = 0l; // in kg

	// A pointer to this body's current planetary type.
	const TypeExt* type;

	Body() = default;

	Body(float x, float y, long mass);

	// satellite constructor
	Body(long mass, const Orbit& orbit);

	// Adjusts body's position and velocity such that its path is that of the given orbit.
	// Body starts at the orbit's periapsis.
	// later: float can give [0,1) of point on orbit.
	void set_orbit(const Orbit& orbit);

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
	void pos_update(float wraparound_val);

	// Using the given forces, adjusts this body's acceleration.
	void grav_pull(std::array<float, 2> force_vector);

	// Returns this body's momentum in x and y vectors.
	std::array<float, 2> get_momentum() const;

	// Returns the distance in (x,y) vectors to the other body.
	std::array<float, 2> distv_body(const Body& other) const;

	// Returns the scalar distance between this body to the other body.
	float dist_body(const Body& other) const;

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
	void add_debug_text(const std::string&& text);

	// Returns the body's debug text.
	const std::string& get_debug_text() const;

	// Clears body's ddebug text.
	void clear_debug_text();

	// Sets the body's mass to the given value, and checks for type changes.
	void set_mass(long to_set);

	// Changes the body's mass by the given value, and checks for type changes.
	void change_mass(long to_change);

	// Notifies the body that it is being removed.
	void notify_being_removed(Body* absorbed_by);

	// Notifies the body that it is being removed.
	void notify_being_removed(Removal removal);

	// Returns the body's observer list for its removal.
	Event<Removal>& removal_event();

	// Returns a bounding box for the body.
	Rectangle get_bounding_box() const;

	// Returns the body's mass moment vectors (x * mass, y * mass).
	Vector2 get_mass_moment() const;

	// Returns true if this body's id is equal to the other body's id, else false.
	bool operator==(const Body& other) const;

	// Returns true if this body's id is not equal to the other body's id, else false.
	bool operator!=(const Body& other) const;

};

#endif
