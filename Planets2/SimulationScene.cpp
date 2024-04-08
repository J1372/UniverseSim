#include "SimulationScene.h"
#include "Body.h"
#include <string>

#include "CameraState.h"
#include "FreeCamera.h"
#include "AdvCamera.h"

#include "SettingsScene.h"
#include "Physics.h"
#include "InteractionState.h"
#include "DefaultInteraction.h"
#include <raymath.h>
#include "RenderUtil.h"
#include "Removal.h"
#include "Orbit.h"

// enables using suffixes for seconds, milliseconds, etc.
using namespace std::chrono_literals;

SimulationScene::SimulationScene(const SettingsState& settings, std::unique_ptr<SpatialPartitioning>&& partitioning)
	: universe(settings.universe, std::move(partitioning)), settings_state(settings)
{
	camera_state = std::make_unique<FreeCamera>(starting_config);
	interaction_state = std::make_unique<DefaultInteraction>();

	on_screen_bodies.reserve(universe.get_settings().universe_capacity);

	gui.hide(help_message);
	gui.hide(tick_info_label);

	help_message.set_text(default_help_text + std::string{ interaction_state->get_help_text() });
	interaction_title.set_text(interaction_state->get_name());
	reposition_elements(GetScreenWidth(), GetScreenHeight());

	prompt_time = std::chrono::system_clock::now();

	listener = universe.removal_event().add_observer([this](const Removal& e)
	{
		int removed_id = e.removed;
		if (removed_id == orbit_central)
		{
			orbit_central = -1;
			orbit_projections.clear();
		}
		else
		{
			auto it = std::ranges::find_if(orbit_projections, [removed_id](const auto& p) { return p.first == removed_id; });

			if (it != orbit_projections.end())
			{
				std::swap(*it, orbit_projections.back());
				orbit_projections.pop_back();
			}
		}
	});

}

void SimulationScene::process_input()
{
	// Currently, if an InteractionState acts on a key/mouse input, it doesn't consume it
	// to prevent SimScene or camera state from acting on it.
	// So there can be input conflicts

	// Basic user interaction handling (non-camera).
	InteractionState* next_interaction_state = interaction_state->process_input(*camera_state, universe);

	// State change.
	if (next_interaction_state != interaction_state.get()) {
		interaction_state.reset(next_interaction_state);

		// Update ui elements to show interaction state specific text.
		help_message.set_text(default_help_text + std::string{ interaction_state->get_help_text() });
		interaction_title.set_text(interaction_state->get_name());
	}

	if (IsKeyPressed(KEY_O))
	{
		Vector2 screen_pos = GetMousePosition();
		Vector2 world_pos = GetScreenToWorld2D(screen_pos, camera_state->get_raylib_camera());

		if (const Body* overlapped = universe.get_body(world_pos))
		{
			int overlapped_id = overlapped->get_id();
			if (orbit_central == -1)
			{
				orbit_central = overlapped_id;
			}
			else if (overlapped_id == orbit_central)
			{
				orbit_central = -1;
				orbit_projections.clear();
			}
			else
			{
				auto it = std::ranges::find_if(orbit_projections, [overlapped_id](const auto& p) { return p.first == overlapped_id; });
				if (it == orbit_projections.end())
				{
					const Body& central = *universe.get_body(orbit_central);
					constexpr int samples = 129;

					orbit_projections.emplace_back(overlapped_id, OrbitProjection{ central, *overlapped,(float)universe.get_settings().grav_const, samples });
				}
				else
				{
					std::swap(*it, orbit_projections.back());
					orbit_projections.pop_back();
				}
			}
		}
	}


	// Toggles
	if (IsKeyPressed(KEY_B)) {
		should_render_partitioning = !should_render_partitioning;
	}

	if (IsKeyPressed(KEY_N))
	{
		should_render_debug_text = !should_render_debug_text;
	}

	if (IsKeyPressed(KEY_M)) {
		gui.toggle_visibility(tick_info_label);
	}

	if (IsKeyPressed(KEY_F))
	{
		should_render_forces = !should_render_forces;
	}

	if (IsKeyPressed(KEY_E))
	{
		should_render_extended_orbits = !should_render_extended_orbits;
	}

	if (IsKeyPressed(KEY_V))
	{
		should_render_velocities = !should_render_velocities;
		if (should_render_velocities)
		{
			orbit_rel_vel_color = GREEN;
		}
		else
		{
			orbit_rel_vel_color = SKYBLUE;
		}
	}

	if (IsKeyPressed(KEY_H)) {
		gui.toggle_visibility(help_message);
	}

	if (IsKeyPressed(KEY_SPACE)) {
		running = !running;
	}

	// Exiting to settings

	if (IsKeyPressed(KEY_ESCAPE)) {
		if (return_scene == this)
		{
			return_scene = new SettingsScene(settings_state);
		}
	}
	
}

void SimulationScene::update_orbit_projections()
{
	const Body& central = *universe.get_body(orbit_central);
	for (auto& [sat_id, op] : orbit_projections)
	{
		const Body& satellite = *universe.get_body(sat_id);
		op.update(central, satellite, (float)universe.get_settings().grav_const);
	}
}

void SimulationScene::attach_debug_info()
{
	auto attach_body_info = [](const Body& body, DebugInfo& info)
	{
		Vector2 pos = body.pos();
		Vector2 vel = body.vel();

		info.add("ID: " + std::to_string(body.get_id()));
		info.add("Pos(x): " + std::to_string(pos.x));
		info.add("Pos(y): " + std::to_string(pos.y));
		info.add("Vel(x): " + std::to_string(vel.x));
		info.add("Vel(y): " + std::to_string(vel.y));
		info.add("Mass: " + std::to_string(body.get_mass()));
	};

	if (should_render_partitioning) {
		const SpatialPartitioning& partitioning = universe.get_partitioning();

		for (int i = 0; i < on_screen_bodies.size(); ++i) {
			const Body& body = *on_screen_bodies[i];
			DebugInfo& info = body_info[i];

			attach_body_info(body, info);
			partitioning.get_info(body, info);

		}
	}
	else {
		for (int i = 0; i < on_screen_bodies.size(); ++i) {
			const Body& body = *on_screen_bodies[i];
			DebugInfo& info = body_info[i];

			attach_body_info(body, info);
		}
	}
}

void SimulationScene::render_debug_text() const {
	for (int i = 0; i < on_screen_bodies.size(); ++i) {
		const Body& body = *on_screen_bodies[i];
		const DebugInfo& info = body_info[i];

		RenderUtil::render_near_body(body, info.c_str());
	}
}

void SimulationScene::render_screen_info()
{
	// Draw fps approximation in the upper left corner.
	DrawFPS(50, 50);

	// Draw number of bodies in the universe below fps display.
	std::string num_bodies_str = "Bodies: " + std::to_string(universe.get_num_bodies());
	num_bodies_label.set_text(num_bodies_str);

	// Render the tick and collision statistics below the number bodies display.
	if (tick_info_label.is_visible()) {
		std::string tick_info = "Tick " + std::to_string(universe.get_tick()) + "\n";
		tick_info += "Collision checks (tick) : " + std::to_string(universe.get_num_collision_checks_tick()) + "\n";
		tick_info += "Collision checks (total): " + std::to_string(universe.get_num_collision_checks());
		
		tick_info_label.set_text(tick_info);
	}

	// Render the help prompt if it has not been disabled.
	// Disable the help prompt if it has been rendered to the user long enough.
	if (help_prompt.is_visible()) {
		// Get time elapsed since prompt was first displayed.
		auto cur_time = std::chrono::system_clock::now();
		std::chrono::duration elapsed_seconds = cur_time - prompt_time;

		// If more than a certain amount of seconds have passed, disable the prompt.
		if (elapsed_seconds > 5s) {
			gui.hide(help_prompt);
		}

	}

	gui.render();
}

void SimulationScene::render_partitioning() const
{
	const SpatialPartitioning& partitioning = universe.get_partitioning();
	std::vector<Rectangle> rep = partitioning.get_representation();

	// When zoomed out, there is often a visual glitch, especially when line thickness is lowered.
	// I'm not sure how to fix it.
	for (Rectangle rect : rep) {
		if (camera_state->in_view(rect)) {
			DrawRectangleLinesEx(rect, 30, WHITE);
		}
	}
}

void SimulationScene::render_universe() const
{
	// Render all bodies in the universe that are on screen.
	for (const Body* body_ptr : on_screen_bodies) {
		RenderUtil::render_body(*body_ptr);
	}
}

void SimulationScene::render_forces() const
{
	for (const Body* body : on_screen_bodies)
	{
		RenderUtil::render_force(*body);
	}
}

void SimulationScene::render_velocities() const
{
	for (const Body* body : on_screen_bodies)
	{
		RenderUtil::render_velocity(*body);
	}
}

void SimulationScene::render_orbit_projections()
{
	for (auto& [_, op] : orbit_projections)
	{
		op.render_orbit(PURPLE);

		if (should_render_extended_orbits)
		{
			constexpr Color connecting_line_color = BEIGE;
			op.render_orbit_node_connected("Periapsis:", 0.0f, BLUE, connecting_line_color);
			op.render_orbit_node_connected("Apoapsis:", 0.5f, RED, connecting_line_color);
			op.render_orbit_node_connected("Asc:", 0.25f, GRAY, connecting_line_color);
			op.render_orbit_node_connected("Desc:", 0.75f, GRAY, connecting_line_color);
		}
		else
		{
			op.render_orbit_node("Periapsis:", 0.0f, BLUE);
			op.render_orbit_node("Apoapsis:", 0.5f, RED);
		}

		op.render_connecting_line(RED);
		op.render_relative_velocity(orbit_rel_vel_color);
	}
}

Scene* SimulationScene::update()
{
	process_input();

	if (running)
	{
		universe.update();
		if (orbit_central != -1)
		{
			update_orbit_projections();
		}
	}

	// Handle input related to the camera.
	CameraState* next_camera_state = camera_state->update(universe);
	if (camera_state.get() != next_camera_state)
	{
		camera_state.reset(next_camera_state);
	}

	// Camera may have moved, or universe updated.
	// Need to get a new list of bodies that are on screen.
	on_screen_bodies = universe.get_bodies_in(camera_state->get_view());

	BeginDrawing();
		ClearBackground(BLACK);

			BeginMode2D(camera_state->get_raylib_camera());
				// Begin rendering anything that should take the camera, universe position, into account.
				
				if (should_render_partitioning)
				{
					render_partitioning();
				}

				if (should_render_forces)
				{
					render_forces();
				}

				if (should_render_velocities)
				{
					render_velocities();
				}

				// Render the actual bodies in the universe.
				render_universe();

				// Render all relevant debug text.
				if (should_render_debug_text)
				{
					body_info.resize(on_screen_bodies.size());

					attach_debug_info();
					render_debug_text();

					body_info.clear();
				}

				if (orbit_central != -1)
				{
					render_orbit_projections();
				}

				// Allow interaction state to render any state-specific world elements.
				interaction_state->render_world(camera_state->get_camera(), universe);

			EndMode2D();

		// Render any other information that should be overlaid onto the screen.
		//   ( Anything that has a screen position, not a world position. )
		render_screen_info();

	EndDrawing();

	return return_scene;
}

void SimulationScene::notify_resize(int width, int height)
{
	reposition_elements(width, height);
	camera_state->notify_resize(width, height);
}

void SimulationScene::reposition_elements(int screen_width, int screen_height)
{
	// Center prompt on center of the screen
	Vector2 prompt_pos = { 0.0f, GetScreenHeight() / 2 };
	help_prompt.set_pos(prompt_pos);
	help_prompt.center_on(GetScreenWidth() / 2);

	// Keep help message display on the upper right side of the screen.
	Vector2 help_pos = { 0.73 * GetScreenWidth(), 100 };
	help_message.set_pos(help_pos);

	// Keep interaction title display on the lower left side of the screen.
	Vector2 title_pos = { 50, 0.89 * GetScreenHeight() };
	interaction_title.set_pos(title_pos);
}
