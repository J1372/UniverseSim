#include "SatelliteCreation.h"
#include "Physics.h"
#include "CameraState.h"
#include "Universe.h"

#include "SystemCreation.h"
#include "DefaultInteraction.h"
#include "PlanetCreation.h"
#include <raymath.h>
#include "RenderUtil.h"
#include "DebugInfo.h"
#include "PlanetMouseModifier.h"
#include "Removal.h"

SatelliteCreation::SatelliteCreation(const Body& parent, const Body& creating, Universe& universe)
	: creating(creating), orbit_projection{ parent, creating, static_cast<float>(universe.get_settings().grav_const), 129}
{
	listener = universe.removal_event().add_observer([this, &universe](Removal e) 
	{
		if (e.removed == parent_id)
		{
			if (e.was_absorbed())
			{
				const Body* absorber = universe.get_body(e.absorbed_by);
				update_orbit(*absorber, static_cast<float>(universe.get_settings().grav_const), universe.get_tick());
			}
			else
			{
				parent_id = -1;
			}
		}
	});

	update_orbit(parent, static_cast<float>(universe.get_settings().grav_const), universe.get_tick());
}

void SatelliteCreation::update_orbit(float grav_const, int tick_stamp)
{
	orbit_projection.update(grav_const);
	prev_projection_tick = tick_stamp;
}


void SatelliteCreation::update_orbit(const Body& orbiting, float grav_const, int tick_stamp)
{
	parent_id = orbiting.get_id();
	creating.set_vel(orbiting.vel());
	orbit_projection.update(orbiting, creating, grav_const);
	prev_projection_tick = tick_stamp;
}

InteractionState* SatelliteCreation::process_input(const CameraState& camera_state, Universe& universe)
{
	if (parent_id == -1) return new DefaultInteraction;

	const Body& parent = *universe.get_body(parent_id);

	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());
	int cur_tick = universe.get_tick();
	float grav_const = static_cast<float>(universe.get_settings().grav_const);

	if (satellite_modifier.process_input(creating, camera_state.get_camera(), universe))
	{
		update_orbit(grav_const, cur_tick);
	}
	else if (IsKeyPressed(KEY_R))
	{
		const Body* mouse_overlapped = universe.get_body(universe_point);

		if (mouse_overlapped)
		{
			update_orbit(*mouse_overlapped, grav_const, cur_tick);
		}
	}
	else if (IsKeyPressed(KEY_ENTER))
	{
		// Add user body to universe, and end creation mode. 
		universe.add_body(std::move(creating));
		return new DefaultInteraction;
	}
	else if (IsKeyPressed(KEY_ONE))
	{
		return new DefaultInteraction;
	}
	else if (IsKeyPressed(KEY_TWO))
	{
		return new PlanetCreation{ universe_point };
	}
	else if (IsKeyPressed(KEY_THREE))
	{
		return new SystemCreation{ universe_point, universe };
	}
	else
	{
		// Handle fine tuning inputs.
		constexpr float change_magnitude = 0.01f;

		float vel_scale = 1.0f;
		if (IsKeyDown(KEY_LEFT_BRACKET))
		{
			vel_scale -= change_magnitude;
		}
		else if (IsKeyDown(KEY_RIGHT_BRACKET))
		{
			vel_scale += change_magnitude;
		}

		float vel_rot = 0.0f;
		if (IsKeyDown(KEY_SEMICOLON))
		{
			vel_rot -= change_magnitude;
		}
		else if (IsKeyDown(KEY_APOSTROPHE))
		{
			vel_rot += change_magnitude;
		}

		if (vel_scale != 1.0f or vel_rot != 0.0f)
		{
			// Apply transformations to satellite's relative velocity.
			Vector2 new_rel_vel = Vector2Rotate(Vector2Scale(creating.vel_relative(parent), vel_scale), vel_rot);
			creating.set_vel(Vector2Add(parent.vel(), new_rel_vel));
			update_orbit(grav_const, cur_tick);
		}
		else if (cur_tick != prev_projection_tick)
		{
			update_orbit(grav_const, cur_tick);
		}
	}

	return this;
}

std::string_view SatelliteCreation::get_name() const
{
	return "Satellite creator";
}

std::string_view SatelliteCreation::get_help_text() const
{
	return
		"Drag center of planet to change velocity\n"
		"Drag edge of planet to change mass\n"
		"[Left Bracket] to scale velocity down\n"
		"[Right Bracket] to scale velocity up\n"
		"[Semicolon] to rotate velocity counterclockwise\n"
		"[Apostrophe] to rotate velocity clockwise\n"
		"[Enter] to add to universe and return\n"
		"[R] to create satellite (pointed to by mouse)\n"
		"[1] to go to default mode\n"
		"[2] to go to planet creator\n"
		"[3] to go to system generator\n";
}

void SatelliteCreation::render_world(const AdvCamera& camera, const Universe& universe)
{
	orbit_projection.render_orbit(PURPLE);
	orbit_projection.render_connecting_line(RED);

	constexpr Color connecting_line_color = BEIGE;
	const Orbit& orbit = orbit_projection.get_orbit();
	const Body& central = *orbit.orbited;

	std::string per_rep = "Periapsis: " + std::to_string(orbit.periapsis);
	std::string apo_rep = "Apoapsis: " + std::to_string(orbit.apoapsis());
	orbit_projection.render_orbit_node_connected(per_rep, 0.0f, BLUE, connecting_line_color);
	orbit_projection.render_orbit_node_connected(apo_rep, 0.5f, RED, connecting_line_color);
	orbit_projection.render_orbit_node_connected("Asc:", 0.25f, GRAY, connecting_line_color);
	orbit_projection.render_orbit_node_connected("Desc:", 0.75f, GRAY, connecting_line_color);
	orbit_projection.render_relative_velocity(GREEN);

	if (camera.in_view(creating))
	{
		Vector2 pos = central.distv(creating);
		Vector2 vel = creating.vel_relative(central);

		const std::string notice_suffix = " (relative)";
		DebugInfo info{ "Pos(x): " + std::to_string(pos.x) + notice_suffix };
		info.add("Pos(y): " + std::to_string(pos.y) + notice_suffix);
		info.add("Vel(x): " + std::to_string(vel.x) + notice_suffix);
		info.add("Vel(y): " + std::to_string(vel.y) + notice_suffix);
		info.add("Mass: " + std::to_string(creating.get_mass()));
		info.add(per_rep);
		info.add(apo_rep);
		info.add("Eccentricity: " + std::to_string(orbit.eccentricity));
		info.add("Periapsis angle:    " + std::to_string(orbit.periapsis_angle));

		RenderUtil::render_body(creating);
		RenderUtil::render_near_body(creating, info.c_str());
	}
}
