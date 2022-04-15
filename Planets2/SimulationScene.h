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

	void zoom_in() { camera.zoom *= 2; }

	void zoom_out() { camera.zoom /= 2; }

	void process_input();

	bool on_screen(const Body& body) const;
	void render_system() const;
	void render() const;

public:

	SimulationScene(int width, int height, UniverseSettings settings) : Scene(width, height), universe{settings}
	{
		camera.offset = { static_cast<float>(screen_width) / 2, static_cast<float>(screen_height) / 2 };
		camera.target = { 0, 0 };
		camera.rotation = 0;
		camera.zoom = 1.0f;
	}

	void resize(int width, int height);

	Scene* update();



};

