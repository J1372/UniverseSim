#pragma once
#include <vector>
#include <memory>

class SettingsScene;
class SimulationScene;


class Scene
{

public:

	// Scene list.

	static SettingsScene settings_scene;
	static SimulationScene simulation_scene;

	// Initializes all scenes after Raylib has been initialized.
	static void init_scenes();

	// Updates the scene, and returns a pointer to the scene to transition to, which may be nullptr or the same scene.
	virtual Scene* update() = 0;

	// Notifies the scene that the window has been resized.
	virtual void resize(int width, int height)
	{}

	virtual ~Scene() = default;
};

