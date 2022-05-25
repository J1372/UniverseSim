#ifndef BODY_H
#define BODY_H

#include <raylib.h>
#include <cmath>
#include <array>
#include <vector>
#include <string>

#include "Event.h"

struct Removal;

struct TypeExt {
	int density;
	long min_mass;
	Color color; // list of colors (may be weighted). Pick one from prototype.
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
	static constexpr TypeExt END_TYPE = { -1, LONG_MAX, SKYBLUE };

	static constexpr TypeExt TYPES[] =
	{
		ASTEROID_TYPE,
		PLANET_TYPE,
		SUN_TYPE,
		BLACK_HOLE_TYPE,
		END_TYPE
	};

	int type_level = 0;
	std::string debug_info;

	// Observers to notify when this body is being removed.
	Event<Removal> on_removal_observers;


	void do_wraparound(float wraparound_val);


public:
	int id = -1;

	float x = 0.0;
	float y = 0.0;
	float vel_x = 0;
	float vel_y = 0;
	float acc_x = 0;
	float acc_y = 0;

	float radius = 0.0; // in units. same as distance.
	long mass = 0l; // in kg

	const TypeExt* type;

	Body() = default;

	Body(int id, float x, float y, long mass) : id(id), x(x), y(y), mass(std::max(1l, mass))
	{
		upgrade_update();

		radius = std::max(((float)mass) / type->density, 1.0f);

	};

	// satellite constructor
	Body(int id, float sat_dist, float ecc, const Body& orbiting, float grav_const, long mass);

	// Returns a pair of body pointers, where the first has more mass than the second.
	static std::pair<Body*, Body*> get_sorted_pair(Body& body1, Body& body2);

	bool can_eat(const Body& other) const;

	void absorb(const Body& other);

	// void impact (struct Body * const, struct Body * const); partial absorb

	void upgrade_update();

	void pos_update(float wraparound_val);

	void grav_pull(std::array<float, 2> force_vector);

	std::array<float, 2> get_momentum() const;

	std::array<float, 2> distv_body(const Body& other) const;
	float dist_body(const Body& other) const;

	bool contains_point(Vector2 point) const;

	float left() const { return x - radius; }
	float right() const { return x + radius; }
	float top() const { return y - radius; }
	float bottom() const { return y + radius; }


	void add_debug_text(const std::string&& text);
	const std::string& get_debug_text() const;
	void clear_debug_text();



	// Notifies the body that it is being removed.
	void notify_being_removed(Body* absorbed_by);

	Event<Removal>& removal_event();

	Rectangle get_bounding_box() const;

	bool operator==(const Body& other) const;


};

#endif
