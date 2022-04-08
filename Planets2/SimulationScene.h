#pragma once
#include "Scene.h"
class Universe;

class SimulationScene : public Scene
{

	const Universe& universe;

public:

	SimulationScene(const Universe& universe) : universe(universe) {}


};

