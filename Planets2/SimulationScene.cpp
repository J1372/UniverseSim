#include "SimulationScene.h"
#include "universe.h"
#include "Body.h"
#include <string>

#include "CameraState.h"
#include "FreeCamera.h"
#include "AdvCamera.h"

#include "SettingsScene.h"
#include "Physics.h"

SimulationScene::SimulationScene(int width, int height, UniverseSettings settings, std::unique_ptr<SpatialPartitioning>&& partitioning) : 
	Scene(width, height), universe{ settings, std::move(partitioning) }, camera_state(&CameraState::free_camera)
{
	CameraState::init_cameras(starting_config);

	on_screen_bodies.reserve(universe.get_num_bodies());
}

void SimulationScene::process_input()
{
	// Basic user interaction handling (non-camera).
	if (in_creation_mode()) {
		handle_creation_interaction();
	}
	else {
		handle_default_interaction();
	}

	// Toggles
	if (IsKeyPressed(KEY_V)) {
		if (universe.has_partitioning()) {
			should_render_partitioning = !should_render_partitioning;
		}
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

void SimulationScene::handle_default_interaction()
{
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 screen_point = GetMousePosition();
		Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state->get_raylib_camera());

		if (IsKeyDown(KEY_LEFT_CONTROL)) {
			// Ctrl-click = try to delete body.

			Body* body = universe.get_body(universe_point);

			if (body) {
				universe.rem_body(*body);
			}

		}
		else if (IsKeyDown(KEY_LEFT_SHIFT)) {
			// Shift-click = start user body creation.

			creating = std::make_unique<Body>();

			creating->x = universe_point.x;
			creating->y = universe_point.y;

			creating->set_mass(100); // Could set default to middle of Body::ASTEROID_TYPE.


		}
	}
}

void SimulationScene::handle_creation_interaction()
{
	// When dragging mouse, determines whether to change the creation body's mass or velocity.
	enum class Modifying {
		NONE,
		MASS,
		VELOCITY
	};

	static Modifying modify_mode = Modifying::NONE;

	Vector2 screen_point = GetMousePosition();
	Vector2 universe_point = GetScreenToWorld2D(screen_point, camera_state->get_raylib_camera());

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

		if (IsKeyDown(KEY_LEFT_CONTROL)) {
			// Ctrl-click = try to delete body.

			Body* body = universe.get_body(universe_point);

			if (body) {
				universe.rem_body(*body);
			}
		}
		else if (IsKeyDown(KEY_LEFT_SHIFT)) {
			// Shift-click = add current creation, start new user body creation.

			universe.add_body(std::move(creating));

			creating = std::make_unique<Body>();

			creating->x = universe_point.x;
			creating->y = universe_point.y;

			// handles upgrade_check and updating radius
			creating->set_mass(100);


		}
		else {
			// select modifying mode based on where user clicked.
			if (Physics::point_in_circle(universe_point, creating->x, creating->y, creating->radius * .7)) {
				// Clicked on the inside section of the body.
				modify_mode = Modifying::VELOCITY;

			}
			else if (creating->contains_point(universe_point)) {
				// Clicked on the outside section of the body.
				modify_mode = Modifying::MASS;

			}
			else {
				// User did not click on the body they are creating.
				modify_mode = Modifying::NONE;
			}
		}
	}
	else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		// Mouse is being dragged

		Vector2 movement = GetMouseDelta();
		// Can compare difference universe point (0,0) to (movement.x, movement.y)
		// So zooming out would increase rate of change of i.e mass.
		// Currently flat amount based on movement on screen.

		if (modify_mode == Modifying::MASS) {
			// User is altering body's mass.

			// This determines whether the user dragged the mouse towards the body.
			float towards_body_x = creating->x < universe_point.x ? -movement.x : movement.x;
			float towards_body_y = creating->y < universe_point.y ? -movement.y : movement.y;
			float towards_body_delta = towards_body_x + towards_body_y;

			// Increase mass if dragged towards, else decrease.
			creating->change_mass(5 * -towards_body_delta);

		}
		else if (modify_mode == Modifying::VELOCITY) {
			// User is altering body's velocity.

			// Increase velocity in opposite direction of the mouse drag.
			creating->vel_x -= movement.x/2;
			creating->vel_y -= movement.y/2;
		}
	}
	else if (IsKeyPressed(KEY_ENTER)) {
		// Add user body to universe, and end creation mode. 
		universe.add_body(std::move(creating));

		creating = nullptr;
	}
}

bool SimulationScene::in_creation_mode() const
{
	return creating.get() != nullptr;
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

	// Need to render creation body ddifferently (always print its specs)
	if (in_creation_mode()) {
		render_creation_body();
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

void SimulationScene::render_creation_body() const {
	const Body& body = *creating;
	Color planet_color = body.type->color;
	DrawCircle(body.x, body.y, body.radius, planet_color);

	constexpr int font_size = 25;
	int text_x = body.x + body.radius + 20;
	int text_y = body.y + body.radius + 20;
	std::string info_text { "X: " + std::to_string(body.x) + "\n"};

	info_text += "Y: " + std::to_string(body.y) + "\n";
	info_text += "Vel(x): " + std::to_string(body.vel_x) + "\n";
	info_text += "Vel(y): " + std::to_string(body.vel_y) + "\n";
	info_text += "Mass: " + std::to_string(body.mass) + "\n";

	DrawText(info_text.c_str(), text_x, text_y, font_size, planet_color);
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

		if (should_render_help_text) {
			DrawText(help_text.c_str(), 1400, 100, 20, RAYWHITE);
		}

	EndDrawing();

	clear_debug_text();

	return return_scene;
}
