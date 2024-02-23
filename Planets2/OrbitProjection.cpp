#include "OrbitProjection.h"
#include "RenderUtil.h"
#include "Body.h"
#include "AdvCamera.h"
#include <raymath.h>
#include <cassert>

OrbitProjection::OrbitProjection(const Body& parent, const Body& satellite, float grav_const, int num_projections)
	: orbit{parent, satellite, grav_const }, satellite(&satellite), positions(num_projections)
{
	assert(num_projections > 2, "Number of samples must be more than 2 to at least sample periapsis, then apoapsis, then periapsis.");
	assert(num_projections % 2 == 1, "Number of samples must be odd so that the apoapsis is always sampled (without additional logic or multiple loops).");

	update_projections();
}

void OrbitProjection::update(float grav_const)
{
	orbit = { *orbit.orbited, *satellite, grav_const };
	update_projections();
}

void OrbitProjection::update(const Body& parent, const Body& satellite, float grav_const)
{
	this->satellite = &satellite;
	orbit = { parent, satellite, grav_const };
	update_projections();
}

void OrbitProjection::render_relative_velocity(Color color) const
{
	float thickness = std::max(3.0f, satellite->diameter() / 20.0f);
	RenderUtil::render_body_vector(*satellite, satellite->vel_relative(*orbit.orbited), 50.0f, thickness, color);
}

void OrbitProjection::render_orbit(Color color)
{
	DrawLineStrip(positions.data(), positions.size(), color);
}

void OrbitProjection::update_projections()
{
	float sample_dist = 1.0f / (positions.size() - 1);
	for (int i = 0; i < positions.size(); ++i)
	{
		positions[i] = Vector2Add(orbit.orbited->pos(), orbit.pos_at(sample_dist * i));
	}
}

void OrbitProjection::render_orbit_node(const std::string& text, float point, Color color) const
{
	constexpr float node_size = 5.0f;
	constexpr float text_offset = 20.0f;

	const Body& orbiting = *orbit.orbited;

	Vector2 rel_pos = orbit.pos_at(point);
	Vector2 pos = Vector2Add(orbiting.pos(), rel_pos);
	DrawCircleV(pos, node_size, color);
	DrawText(text.c_str(), pos.x + text_offset, pos.y + text_offset, 12, color);

	Vector2 vel = orbit.vel_at(point);
	std::string pos_rep = "Position: { " + std::to_string(rel_pos.x) + ", " + std::to_string(rel_pos.y) + " }";
	std::string vel_rep = "Velocity: { " + std::to_string(vel.x) + ", " + std::to_string(vel.y) + " }";
	DrawText(pos_rep.c_str(), pos.x + text_offset, pos.y + 2 * text_offset, 12, color);
	DrawText(vel_rep.c_str(), pos.x + text_offset, pos.y + 3 * text_offset, 12, color);

	DrawLineV(pos, Vector2Add(pos, Vector2Scale(Vector2Normalize(vel), 50.0f)), color);
}

void OrbitProjection::render_orbit_node_connected(const std::string& text, float point, Color node_color, Color line_color) const
{
	render_orbit_node(text, point, node_color);

	const Body& orbiting = *orbit.orbited;

	// Connecting lines may render over text from render_orbit_node.
	Vector2 pos = Vector2Add(orbiting.pos(), orbit.pos_at(point));
	DrawLineV(pos, orbiting.pos(), line_color);

	Vector2 ell_cen = Vector2Add(orbiting.pos(), orbit.get_ellipse_center());
	DrawLineV(ell_cen, pos, line_color);
}

const Orbit& OrbitProjection::get_orbit() const
{
	return orbit;
}

const Body& OrbitProjection::get_satellite() const
{
	return *satellite;
}

void OrbitProjection::render_connecting_line(Color color) const
{
	DrawLineV(orbit.orbited->pos(), satellite->pos(), color);
}
