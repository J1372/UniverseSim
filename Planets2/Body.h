#ifndef BODY_H
#define BODY_H

#include <raylib.h>
#include "PlanetType.h"

struct Orbit;

// A body in the universe.
class Body {

	static constexpr PlanetType TYPES[] =
	{
		{ RAYWHITE, 1, -1},
		{ RAYWHITE, 10, 0 },
		{ LIGHTGRAY, 20, 1000 },
		{ SKYBLUE, 40, 2000 },
		{ BLUE, 70, 4000 },
		{ DARKBLUE, 100, 6000 },
		{ GOLD, 300, 9000 },
		{ ORANGE, 600, 70000 },
		{ DARKGRAY, 1500, 150000 },
		{ SKYBLUE, 1, std::numeric_limits<long>::max() }
	};

	// Body's unique id.
	int id = -1;

	// Universe position vector.
	Vector2 position{};

	// Velocity vector.
	Vector2 velocity{};

	// Net force vector acting on this body this tick.
	Vector2 force{};

	// The radius of the body.
	float radius = 0.0f;

	// The body's mass.
	long mass = 0l;

	// A pointer to this body's current planetary type.
	const PlanetType* type = &TYPES[1]; // Starts as asteroid, updates on construction.

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

	// Returns this body's velocity vector.
	void set_vel(Vector2 to_set);

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

	// Returns this body's velocity vector, relative to another body.
	Vector2 vel_relative(const Body& other) const;

	// Returns this body's velocity vector, relative to another velocity.
	Vector2 vel_relative(Vector2 relative_to) const;

	// Returns this body's radius.
	float get_radius() const;

	// Returns this body's mass.
	long get_mass() const;

	// Returns current forces acting on the body.
	Vector2 get_forces() const;

	// Returns this body's color.
	Color color() const;

	void reset_forces();


	// Moves the body to be at the given point in the orbit.
	// Sets body's velocity such that its path is that of the given orbit.
	void set_orbit(const Orbit& orbit, float point);

	// Returns true if this body's mass is greater than or equal to the mass of the other body.
	bool can_eat(const Body& other) const;

	// Adds the mass of the other body to this body, and updates this body's type if necessary.
	// Uses other body's momentum to simulate an impact force on this body, changing its velocity.
	void absorb(const Body& other);

	// Can have a partial absorb method as well.

	// Checks the body to see if it should downgrade or upgrade its type.
	void upgrade_update();

	// Updates the body's position, using its current velocity, and resets its acceleration to 0.
	void pos_update();

	// Applies a force to the body.
	void apply_force(Vector2 to_apply);

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

	// Sets the body's mass to the given value, and checks for type changes.
	void set_mass(long to_set);

	// Changes the body's mass by the given value, and checks for type changes.
	void change_mass(long to_change);

	// Returns a bounding box for the body.
	Rectangle get_bounding_box() const;

	// Returns the body's mass moment vector (x * mass, y * mass).
	Vector2 get_mass_moment() const;

	Vector2 force_applied_by(const Body& other) const;
	Vector2 force_applied_by(Vector2 point, long point_mass) const;

	bool collided_with(const Body& other) const;
	bool in_rect(Rectangle rect) const;
	bool intersects_rect(Rectangle rect) const;

	// Returns true if this body's id is equal to the other body's id, else false.
	bool operator==(const Body& other) const;

	// Returns true if this body's id is not equal to the other body's id, else false.
	bool operator!=(const Body& other) const;

};

#endif
