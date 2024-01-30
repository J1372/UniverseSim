#pragma once
#include "Scene.h"

#include "Universe.h"
#include "AdvCamera.h"
#include <span>
#include <chrono>
#include "Label.h"
#include "TextBox.h"
#include "DebugInfo.h"

#include "GuiComponentList.h"
#include "SettingsState.h"

class CameraState;
class Body;
class InteractionState;

// This scene handles user interaction during the simulation and rendering of the simulation universe.
class SimulationScene : public Scene
{

	// The universe being simulated.
	Universe universe;

	// Copy of settings state, to restore settings scene when transition.
	SettingsState settings_state;

	// A starting configuration for the cameras.
	AdvCamera starting_config { Vector2{0,0}, Vector2{0,0} };

	// The current camera state.
	std::unique_ptr<CameraState> camera_state;

	// The current user interaction state.
	std::unique_ptr<InteractionState> interaction_state;

	// User-toggleable variables.
	
	// Whether the simulation is currently running.
	bool running = false;

	GuiComponentList gui;

	// Whether to render the tick number and collision statistics.
	Label& tick_info_label = gui.add<Label>("_", 50.0f, 95.0f, 20, RAYWHITE);
	Label& num_bodies_label = gui.add<Label>("_", 50.0f, 70.0f, 20, RAYWHITE);

	// Whether to render a representation of the collision partitioning method.
	// Can only be set to true if universe.has_partitioning() is true.
	bool should_render_partitioning = false;

	// Whether to render and update the debug text of bodies that are on screen.
	bool should_render_debug_text = false;

	bool should_render_forces = false;
	bool should_render_velocities = false;

	// Default help text.
	const std::string default_help_text = "[H] to close help text\n";

	// Time when help prompt text was first displayed.
	std::chrono::system_clock::time_point prompt_time;

	// A vector of pointers to all bodies that are currently on screen.
	std::vector<Body*> on_screen_bodies;

	// Information about all on screen bodies.
	std::vector<DebugInfo> body_info;

	// Labels to draw on the screen.
	// These rely on screen size and are repositioned when window resized and on scene enter.
	// If add other interactive gui elements to sim, should use a GuiComponentList to handle its update and render.
	Label& help_prompt = gui.add<Label>( "Press [H] to open the help menu", 0.0f, 0.0f, 20, WHITE );
	Label& interaction_title = gui.add<Label>( "_", 0.0f, 0.0f, 20, RAYWHITE );
	Label& help_message = gui.add<Label>( default_help_text, 0.0f, 0.0f, 20, RAYWHITE);

	// Handles all user input.
	void process_input();

	// Updates the list of bodies that are on screen
	void update_on_screen_bodies();

	// Attaches standard debug info to bodies that are on screen.
	void attach_debug_info();

	// Renders the body.
	void render_body(const Body& body) const;

	// Draws debug text alongside every body.
	void render_debug_text() const;

	// Draws text near the given body.
	void render_near_body(const Body& body, const char* text) const;

	// Returns true if a body is at least partially on screen, else false.
	bool on_screen(const Body& body) const;

	// Returns true if a rectangle is at least partially on screen, else false.
	bool on_screen(Rectangle rect) const;

	// Renders all bodies that the user is creating.
	void render_creating_bodies(std::span<const Body> bodies) const;

	// Handles rendering of the universe.
	void render_universe() const;

	// Handles rendering of universe's collision detection partitioning method.
	void render_partitioning() const;

	// Handles rendering of debug information.
	void render_debug() const;

	// Handles rendering of any information that has a simple screen position, as opposed to a place in the universe.
	void render_screen_info();

	void render_forces() const;
	void render_velocities() const;
	void render_velocity(const Body& body) const;
	void render_body_vector(const Body& body, Vector2 vec, float dist_scale, float thick_scale, Color color) const;

	// Moves text elements that adjust to screen size to new positions.
	void reposition_elements(int screen_width, int screen_height);

	// Scene to return at the end of update.
	Scene* return_scene = this;

public:

	SimulationScene(const SettingsState& settings, std::unique_ptr<SpatialPartitioning>&& partitioning);

	// Handles all user input, updates and renders the universe, and then renders any additional scene items.
	Scene* update() override;
	
	// Adjusts any elements that rely on screensize.
	void notify_resize(int width, int height) override;

};
