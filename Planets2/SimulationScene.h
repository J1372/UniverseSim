#pragma once
#include "Scene.h"

#include "universe.h"
#include "AdvCamera.h"
#include <span>

class CameraState;
class Body;
class InteractionState;

// This scene handles user interaction during the simulation and rendering of the simulation universe.
class SimulationScene : public Scene
{
	// The universe being simulated.
	Universe universe;

	// A starting configuration for the cameras.
	AdvCamera starting_config { Vector2{0,0}, Vector2{0,0} };

	// The current camera state.
	CameraState* camera_state;

	// The current user interaction state.
	InteractionState* interaction_state;

	// User-toggleable variables.
	
	// Whether the simulation is currently running.
	bool running = false;

	// Whether to render the tick number and collision statistics.
	bool should_render_tick_info = false;

	// Whether to render a representation of the collision partitioning method.
	// Can only be set to true if universe.has_partitioning() is true.
	bool should_render_partitioning = false;

	// Whether to render and update the debug text of bodies that are on screen.
	bool should_render_debug_text = false;

	// Whether to render help text for the user.
	bool should_render_help_text = false;

	// Default help text.
	const std::string default_help_text = "[H] to close help text\n";

	// The current help text to be shown if the user has toggled on its display.
	// This is the default help text, along with any state specific help text.
	std::string current_help_text = default_help_text;

	// A vector of pointers to all bodies that are currently on screen.
	std::vector<Body*> on_screen_bodies;

	// Handles all user input.
	void process_input();

	// Updates the list of bodies that are on screen
	void update_on_screen_bodies();

	// Attaches standard debug info to bodies that are on screen.
	void attach_debug_info() const;

	// Attaches partitioning debug info to bodies that are on screen.
	void attach_partitioning_debug_info() const;

	// Clears debug info of all on screen bodies.
	void clear_debug_text();

	// Draws debug text alongside every body.
	void draw_debug_text(int font_size, int spacing) const;

	// Returns true if a body is at least partially on screen, else false.
	bool on_screen(const Body& body) const;

	// Renders all on screen bodies.
	void render_bodies() const;

	// Renders all bodies that the user is creating.
	void render_creating_bodies(std::span<const std::unique_ptr<Body>> bodies) const;

	// Handles rendering of the universe.
	void render() const;

	// Scene to return at the end of update.
	Scene* return_scene = this;

public:

	// Initialization to be called after Raylib's init.
	void init();

	// Enters the scene, creates a new universe after passing it the given settings and partitioning.
	void enter(UniverseSettings settings, std::unique_ptr<SpatialPartitioning>&& partitioning);

	// Handles all user input, updates and renders the universe, and then renders any additional scene items.
	Scene* update();

};
