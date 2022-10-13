#pragma once

#include <raylib.h>
#include <utility>

// Represents a type of planetary body.
class PlanetType {

	Color color; // Color to use for this body type.
	int density; // Density of this type.
	long min_mass; // Minimum mass required to be this type.

public:

	constexpr PlanetType(Color color, int density, long min_mass) :
		color(color), density(density), min_mass(min_mass)
	{}

	float get_radius(long mass) const
	{
		return std::max(((float)mass) / density, 1.0f);
	}

	Color get_color() const
	{
		return color;
	}

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