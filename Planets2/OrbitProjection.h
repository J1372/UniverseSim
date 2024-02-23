#pragma once

#include "Orbit.h"
#include <vector>
#include <string>
#include <raylib.h>

// Helper class for maintaining orbit projection cache and rendering orbital info.
class OrbitProjection
{
	Orbit orbit;

	// Never null.
	const Body* satellite;

	// Cached satellite absolute positions.
	// Allows rendering orbit path without recalculating every point when unchanged.
	std::vector<Vector2> positions;

	void update_projections();

public:

	OrbitProjection(const Body& parent, const Body& satellite, float grav_const, int num_projections);

	void update(float grav_const);
	void update(const Body& parent, const Body& satellite, float grav_const);

	void render_relative_velocity(Color color) const;
	void render_orbit(Color color);
	void render_orbit_node(const std::string& text, float point, Color color) const;
	void render_orbit_node_connected(const std::string& text, float point, Color node_color, Color line_color) const;
	void render_connecting_line(Color color) const;
	
	//void render_semi_major_axis(Color color) const;
	//void render_semi_minor_axis(Color color) const;

	const Orbit& get_orbit() const;
	const Body& get_satellite() const;

};
