#pragma once
#include <string>
#include <vector>
#include <memory>
#include <span>

class CameraState;
class Universe;
class Body;

class DefaultInteraction;
class PlanetCreation;
class SystemCreation;

class InteractionState
{

public:

	// Interaction state implementations
	// Since these are static, they are initialized before Raylib runs its init.
	// This means raylib functions wont work correctly in their constructors.
	// Need to call their inits later, after Raylib has initialized itself and the window.
	static DefaultInteraction default_interaction;
	static PlanetCreation planet_interaction;
	static SystemCreation system_interaction;

	virtual InteractionState* process_input(const CameraState& camera_state, Universe& universe) = 0;

	virtual std::string get_name() const = 0;
	virtual std::string get_help_text() const = 0;

	virtual std::span<const std::unique_ptr<Body>> get_creating_bodies() const = 0;

	virtual ~InteractionState() = default;

};

