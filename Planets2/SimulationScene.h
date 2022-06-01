#pragma once
#include "Scene.h"

#include "universe.h"
#include "AdvCamera.h"
#include <span>

class CameraState;
class Body;
class InteractionState;


class SimulationScene : public Scene
{

	Universe universe;
	AdvCamera starting_config { Vector2{0,0}, Vector2{0,0} };
	CameraState* camera_state;
	InteractionState* interaction_state;

	bool running = false;
	bool should_render_tick_info = false;
	// Can only be true if universe.has_partitioning() is true.
	bool should_render_partitioning = false;
	bool should_render_debug_text = false;
	bool should_render_help_text = false;

	const std::string help_text =
		"[H] to close help text\n"
		"[V] to show partitioning representation\n"
		"[B] to show debug text info\n"
		"[N] to show tick # and collision stats\n"
		"[SPACE] to toggle pause\n"
		"[W] [A] [S] [D] to move the camera\n"
		"[-, +] to control the camera's speed\n"
		"[COMMA] or scroll down to zoom out\n"
		"[PERIOD] or scroll up to zoom in\n"
		"[ESCAPE] to go back to settings\n"
		"Right click to anchor camera to a planet\n";

	std::vector<Body*> on_screen_bodies;

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
	void render_creating_bodies(std::span<const std::unique_ptr<Body>> bodies) const;
	void render() const;

	Scene* return_scene = this;

public:

	SimulationScene(int width, int height, UniverseSettings settings, std::unique_ptr<SpatialPartitioning>&& partitioning);

	void resize(int width, int height);

	Scene* update();



};

