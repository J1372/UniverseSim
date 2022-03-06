#ifndef BODY_H
#define BODY_H

#include <raylib.h>
#include <cmath>


enum class Type : int {
	ASTEROID = 0,
	PLANET,
	SUN
};

// TypeExt in future will serve as a prototype instead of a reference to in Body
// to speed up performance (no dereferencing to get color.
// to add customizability to types (different colors / textures).
// We aren't bottlenecked by memory, just speed.
struct TypeExt {
	int level; // in type array list sorted by min_mass

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
	static constexpr TypeExt ASTEROID_TYPE = { 0, 10, 0, RAYWHITE };
	static constexpr TypeExt PLANET_TYPE = { 1, 14, 1000, SKYBLUE };
	static constexpr TypeExt SUN_TYPE = { 2, 50, 6000, GOLD };
	static constexpr TypeExt BLACK_HOLE_TYPE = { 3, 1000, 90000, DARKGRAY };
	static constexpr TypeExt END_TYPE = { -1, -1, LONG_MAX, SKYBLUE };

	static constexpr TypeExt TYPES[] =
	{
		ASTEROID_TYPE,
		PLANET_TYPE,
		SUN_TYPE,
		BLACK_HOLE_TYPE,
		END_TYPE
	};

	/*static constexpr int TYPE_MASSES[] { 0, 500, 2000, LONG_MAX };
	static constexpr Color TYPE_COLORS[]{ RAYWHITE, SKYBLUE, GOLD, GRAY };
	// LONG_MAX is an int.

	static const int hi = 0;*/


public:
	int id;
	//int cur_type = 0;

	float x;
	float y;
	float vel_x = 0;
	float vel_y = 0;
	float acc_x = 0;
	float acc_y = 0;
	//float acceleration;

	float radius; // in units. same as distance.
	const TypeExt* type;
	long mass; // in kg
	//TypeExt *type;  // either makes render faster or slower due to cache/indirection but idk??

	// mass/size increase -> camera increases #units shown (zoom out). pixel/unit decrease.

	//Color color;
	//const struct Texture2D* texture;

	Body(int id, int x, int y, long mass) : id(id), x(x), y(y), mass(std::max(1l, mass))
	{
		type = &ASTEROID_TYPE;
		//radius = (float)mass;

		//texture = nullptr;

		upgrade_update();

		radius = std::max(((float)mass) / type->density, 1.0f);

	};

	Body(int id, float sat_dist, float ecc, const Body& orbiting, float grav_const, long mass);

	void distv_body(const Body&, float(&)[2]) const;

	float dist_body(const Body&) const;

	bool can_eat(const Body&) const;

	void absorb(Body&);

	// void impact (struct Body * const, struct Body * const); partial absorb

	void upgrade_update();

	void pos_update();

	bool check_col(const Body&) const;

	void grav_pull(const float(&)[2]);

	void get_momentum(float(&)[2]);

	// grav_pull more efficient to do to both at same time. but that handled by universe not body.
	// could have Body.grav_pull just take a force. and universe calls both bodies

};

#endif