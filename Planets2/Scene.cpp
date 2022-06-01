#include "Scene.h"
#include "SettingsScene.h"
#include "SimulationScene.h"

SettingsScene Scene::settings_scene;
SimulationScene Scene::simulation_scene;

void Scene::init_scenes()
{
	settings_scene.init();
	simulation_scene.init();
}
