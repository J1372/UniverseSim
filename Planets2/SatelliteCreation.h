#pragma once
#include "InteractionState.h"
#include <array>

#include "Orbit.h"
#include "Body.h"
#include "PlanetMouseModifier.h"
#include "Event.h"
#include "OrbitProjection.h"

struct Removal;

class SatelliteCreation : public InteractionState
{
	int parent_id;
	Body creating;
	EventHandle<Removal> listener;
	
	OrbitProjection orbit_projection;

	// Timestamp of tick #, when orbit projection was calculated.
	int prev_projection_tick = -1;

	PlanetMouseModifier satellite_modifier;

	void update_orbit(float grav_const, int tick_stamp);
	void update_orbit(const Body& orbiting, float grav_const, int tick_stamp);

public:

	SatelliteCreation(const Body& parent, const Body& creating, Universe& universe);

	// Processes all relevant state input for creating a body and returns the next interaction state.
	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	std::string_view get_name() const override;

	// Returns help text specific to the planet creation state.
	std::string_view get_help_text() const override;

	void render_world(const AdvCamera& camera, const Universe& universe) override;

};

