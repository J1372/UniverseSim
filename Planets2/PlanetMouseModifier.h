#pragma once

class Body;
class AdvCamera;
class Universe;

// Handles mouse input state for modifying a planet.
class PlanetMouseModifier
{
	// When dragging mouse, determines whether to change the modifying body's mass or velocity.
	enum class Modifying
	{
		NONE,
		MASS,
		VELOCITY
	};

	// The current modification being done by a drag.
	Modifying modify_mode = Modifying::NONE;

public:

	// If mouse drags inner radius, changes body velocity.
	// If mouse drags outer radius, changes body mass.
	// Returns true if body was modified.
	bool process_input(Body& modifying, const AdvCamera& camera, const Universe& universe);

};

