#include "SimulationScene.h"
#include "universe.h"
#include "Body.h"
#include "QuadTree.h"
#include <string>

#include "SettingsScene.h"

void SimulationScene::process_input()
{
	static int multiplier = 8;
	static float cam_speed = 5 * multiplier / camera.zoom;

	if (IsKeyPressed(KEY_V)) {
		if (universe.has_partitioning()) {
			should_render_partitioning = !should_render_partitioning;
		}
	}

	if (IsKeyPressed(KEY_B)) {
		should_render_debug_text = !should_render_debug_text;
	}

	// Camera movement keys

	if (IsKeyDown(KEY_W)) {
		camera.target.y -= cam_speed;
	}
	if (IsKeyDown(KEY_A)) {
		camera.target.x -= cam_speed;
	}
	if (IsKeyDown(KEY_S)) {
		camera.target.y += cam_speed;
	}
	if (IsKeyDown(KEY_D)) {
		camera.target.x += cam_speed;
	}

	if (IsKeyPressed(KEY_SPACE)) {
		running = !running;
	}

	if (IsKeyPressed(KEY_MINUS)) {
		multiplier = std::max(1, multiplier - 1);
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_EQUAL)) {
		multiplier++;
		cam_speed = 5 * multiplier / camera.zoom;
	}

	// Camera zoom for keys and mousewheel

	float wheel_move = GetMouseWheelMove();

	if (wheel_move < 0 or IsKeyPressed(KEY_COMMA)) {
		zoom_out();
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (wheel_move > 0 or IsKeyPressed(KEY_PERIOD)) {
		zoom_in();
		cam_speed = 5 * multiplier / camera.zoom;
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
	const SpatialPartitioning& partitioning = universe.get_partitioning();

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

		const std::vector<std::string> debug_texts = body.get_debug_text();
		int text_x = body.x + body.radius + 20;
		int text_y = body.y + body.radius + 20;

		int cur_spacing = 0;

		for (const std::string& text : debug_texts) {
			DrawText(text.c_str(), text_x, text_y + cur_spacing, font_size, planet_color);
			cur_spacing += spacing;
		}
	}
	
}


void SimulationScene::render() const
{
	if (should_render_partitioning) {
		const SpatialPartitioning& partitioning = universe.get_partitioning();
		partitioning.draw_debug(camera);
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

void SimulationScene::resize(int width, int height)
{
	screen_width = width;
	screen_height = height;

	camera.offset = { static_cast<float>(screen_width) / 2, static_cast<float>(screen_height) / 2 };
}

Scene* SimulationScene::update()
{
	process_input();

	if (running) {
		universe.update();
	}

	update_on_screen_bodies();

	BeginDrawing();
		ClearBackground(BLACK); // maybe better to have before beginmode2d?

			BeginMode2D(camera);
				render();
			EndMode2D();

		DrawFPS(50, 50);

		std::string num_bodies_str = "Number bodies: " + std::to_string(universe.get_num_bodies());

		DrawText(num_bodies_str.c_str(), 50, 70, 20, RAYWHITE);

	EndDrawing();

	clear_debug_text();

	return return_scene;
}
