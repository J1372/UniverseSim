#pragma once
#include "Scene.h"
#include <raylib.h>

#include "universe.h"
class Body;

class SimulationScene : public Scene
{

	Universe universe;
	Camera2D camera;

	bool running = false;
	// Can only be true if universe.has_partitioning() is true.
	bool should_render_partitioning = false;
	bool should_render_debug_text = false;

	std::vector<Body*> on_screen_bodies;

	void zoom_in() { camera.zoom *= 2; }

	void zoom_out() { camera.zoom /= 2; }

	void process_input();

	// Updates the list of bodies that are on screen
	void update_on_screen_bodies();


	void attach_debug_info() const;
	void attach_partitioning_debug_info() const;

	void clear_debug_text();

	/* 
	Draws debug text alongside every body.
	*/
	void draw_debug_text(int font_size, int spacing) const;
	bool on_screen(const Body& body) const;
	void render_bodies() const;
	void render() const;

	Scene* return_scene = this;

public:

	SimulationScene(int width, int height, UniverseSettings settings) : Scene(width, height), universe{settings}
	{
		camera.offset = { static_cast<float>(screen_width) / 2, static_cast<float>(screen_height) / 2 };
		camera.target = { 0, 0 };
		camera.rotation = 0;
		camera.zoom = 1.0f;

		on_screen_bodies.reserve(universe.get_num_bodies());
	}

	void resize(int width, int height);

	Scene* update();



};

