#pragma once
#include "InteractionState.h"
#include <array>

#include "Orbit.h"
#include "Body.h"
#include "PlanetMouseModifier.h"

class SatelliteCreation : public InteractionState
{
	const Body* parent;
	Body creating;
	Orbit cur_orbit;

	static constexpr int samples = 129;
	static_assert(samples >= 3, "Number of samples must be more than 2 to at least sample periapsis, then apoapsis, then periapsis (will render semi-major axis).");
	static_assert(samples % 2 == 1, "Number of samples must be odd so that the apoapsis is always sampled (without additional logic or multiple loops).");
	static constexpr float sample_dist = 1.0f / (samples - 1);

	// Cached samples to use for drawing orbit ellipse.
	std::array<Vector2, samples> orbit_samples;

	// Timestamp of tick #, when orbit projection was calculated.
	int prev_projection_tick = -1;

	PlanetMouseModifier satellite_modifier;

	void update_orbit(float grav_const, int tick_stamp);
	void update_orbit(const Body& orbiting, float grav_const, int tick_stamp);

public:

	SatelliteCreation(const Body& parent, const Body& creating, double grav_const);

	// Processes all relevant state input for creating a body and returns the next interaction state.
	InteractionState* process_input(const CameraState& camera_state, Universe& universe) override;

	std::string_view get_name() const override;

	// Returns help text specific to the planet creation state.
	std::string_view get_help_text() const override;

	void render_world(const AdvCamera& camera, const Universe& universe) override;

};

