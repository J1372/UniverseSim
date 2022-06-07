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

	// Inits all states after Raylib's init.
	static void init_states();

	// Processes state-specific user input and returns the next interaction state.
	virtual InteractionState* process_input(const CameraState& camera_state, Universe& universe) = 0;

	// Returns the state's title.
	virtual std::string get_name() const = 0;

	// Returns state-specific help text.
	virtual std::string get_help_text() const = 0;

	// Returns a collection of bodies that are being created by the user.
	// Not all states use this, so it's probably better to append render commands to the Sim scene instead in 
	// process_input method.
	virtual std::span<const std::unique_ptr<Body>> get_creating_bodies() const = 0;

	virtual ~InteractionState() = default;

};

