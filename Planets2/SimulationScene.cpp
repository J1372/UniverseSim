#include "SimulationScene.h"
#include "universe.h"
#include "Body.h"
#include <string>

#include "CameraState.h"
#include "FreeCamera.h"
#include "AdvCamera.h"

#include "SettingsScene.h"
#include "Physics.h"
#include "InteractionState.h"
#include "DefaultInteraction.h"
#include <iostream>

SimulationScene::SimulationScene(int width, int height, UniverseSettings settings, std::unique_ptr<SpatialPartitioning>&& partitioning) : 
	Scene(width, height), universe{ settings, std::move(partitioning) }, camera_state(&CameraState::free_camera),
	interaction_state(&InteractionState::default_interaction)
{
	CameraState::init_cameras(starting_config);

	on_screen_bodies.reserve(universe.get_num_bodies());
}

void SimulationScene::process_input()
{
	// Basic user interaction handling (non-camera).
	interaction_state = interaction_state->process_input(*camera_state, universe);

	// Toggles
	if (IsKeyPressed(KEY_V)) {
		if (universe.has_partitioning()) {
			should_render_partitioning = !should_render_partitioning;
		}
	}

	
	if (IsKeyPressed(KEY_N)) {
		should_render_tick_info = !should_render_tick_info;
	}

	if (IsKeyPressed(KEY_H)) {
		should_render_help_text = !should_render_help_text;
	}

	if (IsKeyPressed(KEY_B)) {
		should_render_debug_text = !should_render_debug_text;
	}

	if (IsKeyPressed(KEY_SPACE)) {
		running = !running;
	}

	// Exiting to settings

	if (IsKeyPressed(KEY_ESCAPE)) {
		return_scene = new SettingsScene(screen_width, screen_height, universe.get_settings());
	}

	
}

void SimulationScene::update_on_screen_bodies()
{
	on_screen_bodies.clear();

	const std::vector<std::unique_ptr<Body>>& bodies = universe.get_bodies();

	for (const auto& body_ptr : bodies) {
		Body& body = *body_ptr;

		if (on_screen(body)) {
			on_screen_bodies.push_back(&body); // this shouldnt work o.o get_bodies returns a const vector.
		}
	}
}

void SimulationScene::attach_debug_info() const
{
	for (Body* body_ptr : on_screen_bodies) {
		Body& body = *body_ptr;

		body.add_debug_text("ID: " + std::to_string(body.id));
		body.add_debug_text("X: " + std::to_string(body.x));
		body.add_debug_text("Y: " + std::to_string(body.y));
		body.add_debug_text("Vel(x): " + std::to_string(body.vel_x));
		body.add_debug_text("Vel(y): " + std::to_string(body.vel_y));
		body.add_debug_text("Mass: " + std::to_string(body.mass));
	}
}

void SimulationScene::attach_partitioning_debug_info() const
{
	// Safe to dereference since this method only called if should_render_partitioning, which is only true if the universe has a partitioning method.
	const SpatialPartitioning& partitioning = *universe.get_partitioning();

	for (Body* body_ptr : on_screen_bodies) {
		Body& body = *body_ptr;
		partitioning.attach_debug_text(body);
	}
}

void SimulationScene::clear_debug_text()
{
	for (Body* body : on_screen_bodies) {
		body->clear_debug_text();
	}
}


void SimulationScene::draw_debug_text(int font_size, int spacing) const {
	for (const Body* body_ptr : on_screen_bodies) {
		const Body& body = *body_ptr;
		Color planet_color = body.type->color;

		const std::string& debug_texts = body.get_debug_text();
		int text_x = body.x + body.radius + 20;
		int text_y = body.y + body.radius + 20;

		DrawText(debug_texts.c_str(), text_x, text_y, font_size, planet_color);

	}
	
}


void SimulationScene::render() const
{
	if (should_render_partitioning) {
		// Safe to dereference this since if should_render_partitioning == true then universe has partitioning.
		const SpatialPartitioning& partitioning = *universe.get_partitioning();
		std::vector<Rectangle> rep = partitioning.get_representation();

		for (const Rectangle& rect : rep) {
			DrawRectangleLinesEx(rect, 50, WHITE);
		}
	}

	// Need to render creation bodies differently (always print their specs)
	std::span<const std::unique_ptr<Body>> creating = interaction_state->get_creating_bodies();

	if (!creating.empty()) {
		render_creating_bodies(creating);
	}

	render_bodies();

	if (should_render_debug_text) {
		attach_debug_info();

		if (should_render_partitioning) {
			attach_partitioning_debug_info();
		}

		constexpr int font_size = 25;
		constexpr int spacing = 20;
		draw_debug_text(font_size, spacing);
	}

	
}

bool SimulationScene::on_screen(const Body& body) const
{
	const Camera2D& camera = camera_state->get_raylib_camera();
	Vector2 leftmost = GetWorldToScreen2D({ body.x - body.radius, body.y }, camera);
	Vector2 rightmost = GetWorldToScreen2D({ body.x + body.radius, body.y }, camera);

	Vector2 lowest = GetWorldToScreen2D({ body.x, body.y + body.radius }, camera);
	Vector2 highest = GetWorldToScreen2D({ body.x, body.y - body.radius }, camera);

	// can optimize : screen_pos.x >= -body.radius && screen_pos.y >= -body.radius

	return rightmost.x >= 0 and lowest.y >= 0 and leftmost.x < GetScreenWidth() and highest.y < GetScreenHeight();
}

void SimulationScene::render_bodies() const
{
	for (const Body* body_ptr : on_screen_bodies) {
		const Body& body = *body_ptr;

		Color planet_color = body.type->color;
		DrawCircle(body.x, body.y, body.radius, planet_color);
	}
}

void SimulationScene::render_creating_bodies(std::span<const std::unique_ptr<Body>> bodies) const
{
	for (const std::unique_ptr<Body>& body_ptr : bodies) {
		const Body& body = *body_ptr;
		Color planet_color = body.type->color;
		DrawCircle(body.x, body.y, body.radius, planet_color);

		constexpr int font_size = 25;
		int text_x = body.x + body.radius + 20;
		int text_y = body.y + body.radius + 20;
		std::string info_text { "X: " + std::to_string(body.x) + "\n" };

		info_text += "Y: " + std::to_string(body.y) + "\n";
		info_text += "Vel(x): " + std::to_string(body.vel_x) + "\n";
		info_text += "Vel(y): " + std::to_string(body.vel_y) + "\n";
		info_text += "Mass: " + std::to_string(body.mass) + "\n";

		DrawText(info_text.c_str(), text_x, text_y, font_size, planet_color);
	}
}

void SimulationScene::resize(int width, int height)
{
	screen_width = width;
	screen_height = height;

	//camera_state->resize(static_cast<float>(screen_width), static_cast<float>(screen_height));
}

Scene* SimulationScene::update()
{
	process_input();

	if (running) {
		universe.update();
	}

	camera_state = camera_state->update(universe);

	update_on_screen_bodies();

	BeginDrawing();
		ClearBackground(BLACK);

			BeginMode2D(camera_state->get_raylib_camera());
				render();
			EndMode2D();

		// render creation body.
		// if want creation to be anchored to target vector instead of screen, just add to on_screen_bodies. if on_screen.

		DrawFPS(50, 50);

		std::string num_bodies_str = "Number bodies: " + std::to_string(universe.get_num_bodies());
		DrawText(num_bodies_str.c_str(), 50, 70, 20, RAYWHITE);

		if (should_render_tick_info) {
			std::string tick_info = "Tick " + std::to_string(universe.get_tick()) + "\n";
			tick_info += "Collision checks (tick) : " + std::to_string(universe.get_num_collision_checks_tick()) + "\n";
			tick_info += "Collision checks (total): " + std::to_string(universe.get_num_collision_checks());
			DrawText(tick_info.c_str(), 50, 95, 20, RAYWHITE);
		}

		if (should_render_help_text) {
			DrawText(help_text.c_str(), 1400, 100, 20, RAYWHITE);
		}

		std::string interaction_title = interaction_state->get_name();
		DrawText(interaction_title.c_str(), 50, 900, 20, RAYWHITE);

	EndDrawing();

	clear_debug_text();

	return return_scene;
}
