#include "PlanetCreation.h"

#include "Universe.h"
#include "CameraState.h"
#include "Physics.h"

#include "SystemCreation.h"
#include "DefaultInteraction.h"
#include "RenderUtil.h"
#include "DebugInfo.h"
#include "SatelliteCreation.h"

Body PlanetCreation::create_default_body(Vector2 pos) const
{
	long mass = 200;
	return { pos.x, pos.y, mass };
}

PlanetCreation::PlanetCreation(Vector2 mouse_pos) // universe point of mouse param
	: creating(create_default_body(mouse_pos))
{}

InteractionState* PlanetCreation::process_input(const CameraState& camera_state, Universe& universe)
{
	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state.get_raylib_camera());


	if (creating_modifier.process_input(creating, camera_state.get_camera(), universe))
	{
		// do nothing,
	}
	else if (IsKeyPressed(KEY_R))
	{
		const Body* mouse_overlapped = universe.get_body(universe_point);

		if (mouse_overlapped)
		{
			return new SatelliteCreation{ *mouse_overlapped, creating, universe.get_settings().grav_const };
		}
	}
	else if (IsKeyPressed(KEY_ENTER)) {
		// Add user body to universe, and end creation mode. 
		universe.add_body(std::move(creating));
		return new DefaultInteraction;
	}
	else if (IsKeyPressed(KEY_ONE)) {
		return new DefaultInteraction;
	}
	else if (IsKeyPressed(KEY_THREE)) {
		return new SystemCreation{ universe_point, universe };
	}

	return this;
}

std::string_view PlanetCreation::get_name() const
{
	return "Planet creation";
}

std::string_view PlanetCreation::get_help_text() const
{
	return
		"Drag center of planet to change velocity\n"
		"Drag edge of planet to change mass\n"
		"Shift+click to add to universe\n"
		"[Enter] to add to universe and return\n"
		"[R] to go to satellite creator\n"
		"[1] to go to default mode\n"
		"[3] to go to system generator\n";

}
void PlanetCreation::render_world(const AdvCamera& camera, const Universe& universe)
{
	if (camera.in_view(creating))
	{
		RenderUtil::render_body(creating);

		Vector2 pos = creating.pos();
		Vector2 vel = creating.vel();
		DebugInfo info{ "Pos(x): " + std::to_string(pos.x) };

		info.add("Pos(y): " + std::to_string(pos.y));
		info.add("Vel(x): " + std::to_string(vel.x));
		info.add("Vel(y): " + std::to_string(vel.y));
		info.add("Mass: " + std::to_string(creating.get_mass()));

		RenderUtil::render_near_body(creating, info.c_str());
		RenderUtil::render_velocity(creating);
	}
}
