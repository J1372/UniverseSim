#pragma once
#include "UniverseSettings.h"

// Universe settings as well as other settings needed to restore settings scene.
struct SettingsState
{
	UniverseSettings universe;

	std::string partitioning_selected = "None";

	struct
	{
		int max_bodies = 10;
		int max_depth = 10;
	} quadtree;

	struct
	{
		int nodes_per_row = 10;
		int quad_max_depth = 10;
	} grid;

};
