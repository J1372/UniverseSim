#pragma once
#include <vector>
#include <memory>

class SettingsScene;
class SimulationScene;


class Scene
{

public:

	static SettingsScene settings_scene;
	static SimulationScene simulation_scene;

	static void init_scenes();

	virtual Scene* update() = 0;

	virtual void resize(int width, int height)
	{}

	virtual ~Scene() = default;
};

