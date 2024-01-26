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


SimulationScene::SimulationScene(const SettingsState& settings, std::unique_ptr<SpatialPartitioning>&& partitioning)
	: universe(settings.universe, std::move(partitioning)), settings_state(settings)
{
	camera_state = std::make_unique<FreeCamera>(starting_config);
	interaction_state = std::make_unique<DefaultInteraction>();

	on_screen_bodies.reserve(universe.get_settings().universe_capacity);

	gui.hide(help_message);
	gui.hide(tick_info_label);

	current_help_text = default_help_text + interaction_state->get_help_text();
	help_message.set_text(current_help_text);
	interaction_title.set_text(interaction_state->get_name());
	reposition_elements(GetScreenWidth(), GetScreenHeight());

	prompt_time = std::chrono::system_clock::now();
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
		current_help_text = default_help_text + interaction_state->get_help_text();
		help_message.set_text(current_help_text);


		interaction_title.set_text(interaction_state->get_name());
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

	if (IsKeyPressed(KEY_V))
	{
		should_render_velocities = !should_render_velocities;
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

void SimulationScene::update_on_screen_bodies()
{
	on_screen_bodies.clear();

	std::span<Body> bodies = universe.get_bodies();

	for (Body& body : bodies) {
		if (on_screen(body)) {
			on_screen_bodies.push_back(&body);
		}
	}
}

void SimulationScene::attach_debug_info()
{
	auto attach_body_info = [](const Body& body, DebugInfo& info)
	{
		Vector2 pos = body.pos();
		Vector2 vel = body.vel();

		info.add("ID: " + std::to_string(body.get_id()));
		info.add("X: " + std::to_string(pos.x));
		info.add("Y: " + std::to_string(pos.y));
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

		render_near_body(body, info.get());

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
		std::chrono::duration<double> elapsed_seconds = cur_time - prompt_time;

		// If more than a certain amount of seconds have passed, disable the prompt.
		if (elapsed_seconds.count() > 5.0) {
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
		if (on_screen(rect)) {
			DrawRectangleLinesEx(rect, 30, WHITE);
		}
	}
}

void SimulationScene::render_universe() const
{
	// Render all bodies in the universe that are on screen.
	for (const Body* body_ptr : on_screen_bodies) {
		render_body(*body_ptr);
	}
}

bool SimulationScene::on_screen(const Body& body) const
{
	// Return true if a body is at least partially on screen.
	
	const Camera2D& camera = camera_state->get_raylib_camera();
	Vector2 pos = body.pos();

	Vector2 leftmost = GetWorldToScreen2D({ body.left(), pos.y}, camera);
	Vector2 rightmost = GetWorldToScreen2D({ body.right(), pos.y }, camera);

	Vector2 lowest = GetWorldToScreen2D({ pos.x, body.bottom() }, camera);
	Vector2 highest = GetWorldToScreen2D({ pos.x, body.top() }, camera);

	// can optimize : screen_pos.x >= -body.radius && screen_pos.y >= -body.radius

	return rightmost.x >= 0 and lowest.y >= 0 and leftmost.x < GetScreenWidth() and highest.y < GetScreenHeight();
}

bool SimulationScene::on_screen(Rectangle rect) const
{
	const Camera2D& camera = camera_state->get_raylib_camera();
	
	Vector2 UL = GetWorldToScreen2D({ rect.x, rect.y }, camera);
	Vector2 LR = GetWorldToScreen2D({ rect.x + rect.width, rect.y + rect.height }, camera);
	float width = LR.x - UL.x;
	float height = LR.y - UL.y;

	return UL.x < GetScreenWidth() and UL.y < GetScreenHeight()
		and LR.x >= 0 and LR.y >= 0;
}

void SimulationScene::render_body(const Body& body) const
{
	Vector2 pos = body.pos();
	Color planet_color = body.color();

	DrawCircle(pos.x, pos.y, body.get_radius(), planet_color);
}

void SimulationScene::render_creating_bodies(std::span<const Body> bodies) const
{
	for (const Body& body : bodies) {
		if (on_screen(body))
		{
			render_body(body);

			Vector2 pos = body.pos();
			Vector2 vel = body.vel();
			DebugInfo info{ "X: " + std::to_string(pos.x) };

			info.add("Y: " + std::to_string(pos.y));
			info.add("Vel(x): " + std::to_string(vel.x));
			info.add("Vel(y): " + std::to_string(vel.y));
			info.add("Mass: " + std::to_string(body.get_mass()));

			render_near_body(body, info.get());
			render_velocity(body);
		}
	}
}

void SimulationScene::render_near_body(const Body& body, const std::string& text) const
{
	Vector2 pos = body.pos();
	float radius = body.get_radius();


	// Calculate font size with minor scaling based on body's radius.
	constexpr float FONT_START = 25;
	constexpr float FONT_SCALE = 0.05;
	int font_size = FONT_START + FONT_SCALE * radius;

	int text_x = pos.x + radius + 20;
	int text_y = pos.y + radius + 20;
	DrawText(text.c_str(), text_x, text_y, font_size, body.color());
}

void SimulationScene::render_forces() const
{
	for (const Body* body : on_screen_bodies)
	{
		float dist_scale = 50.0f;
		float thick_scale = std::max(3.0f, body->diameter() / 20);
		render_body_vector(*body, body->get_forces(), dist_scale, thick_scale, RED);
	}
}

void SimulationScene::render_velocities() const
{
	for (const Body* body : on_screen_bodies)
	{
		render_velocity(*body);
	}
}

void SimulationScene::render_velocity(const Body& body) const
{
	float dist_scale = 50.0f;
	float thick_scale = std::max(3.0f, body.diameter() / 20);
	render_body_vector(body, body.vel(), dist_scale, thick_scale, SKYBLUE);
}

void SimulationScene::render_body_vector(const Body& body, Vector2 vec, float dist_scale, float thick_scale, Color color) const
{
	Vector2 dir = Vector2Normalize(vec);
	Vector2 start_pos = Vector2Add(body.pos(), Vector2Scale(dir, (body.get_radius() - 0.5f)));
	Vector2 end_pos = Vector2Add(start_pos, Vector2Scale(dir, dist_scale));
	DrawLineEx(start_pos, end_pos, thick_scale, color);
}

Scene* SimulationScene::update()
{
	process_input();

	if (running) {
		universe.update();
	}

	// Handle input related to the camera.
	CameraState* next_camera_state = camera_state->update(universe);
	if (camera_state.get() != next_camera_state)
	{
		camera_state.reset(next_camera_state);
	}

	// Camera may have moved, or universe updated.
	// Need to get a new list of bodies that are on screen.
	update_on_screen_bodies();

	BeginDrawing();
		ClearBackground(BLACK);

			BeginMode2D(camera_state->get_raylib_camera());
				// Begin rendering anything that should take the camera, universe position, into account.
				
				if (should_render_partitioning) {
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
				
				// Need to render user creation bodies differently (always print their specs)
				// Not relevant to every interaction state, and not every state wants their planets to have info rendered as well.
				// In the future, can allow states to queue render commands to sim scene in their update maybe,
				// instead of explicitly asking the state.
				std::span<const Body> creating = interaction_state->get_creating_bodies();

				// Render bodies that the user is creating.
				render_creating_bodies(creating);
				

				// Render the actual bodies in the universe.
				render_universe();

				// Render all relevant debug text.
				if (should_render_debug_text) {
					body_info.resize(on_screen_bodies.size());

					attach_debug_info();
					render_debug_text();

					body_info.clear();
				}


			EndMode2D();

		// Render any other information that should be overlaid onto the screen.
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
