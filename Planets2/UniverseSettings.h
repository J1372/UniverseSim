#pragma once

// Some settings will be able to be set by user at runtime. 
struct UniverseSettings {

	// Max number bodies and area.
	int universe_capacity = 1000; // Maximum number of bodies to be active at one time.
	float universe_size_start = 1000; // initialize all starting bodies in this area. 
	float universe_size_max = 100000; // bodies will wraparound if going past this area.

	// physics
	static constexpr long RAND_MASS = 100; // The maximum amount of mass to allocate to a body created with create_rand_body.
	double grav_const = 1.0;

	// If more methods added, this should be removed and a gravityhandler interface should be added.
	bool use_gravity_approximation = false; 
	float grav_approximation_value = 0.0f;

	// create_system settings.
	int system_min_planets = 100; // Minimum number of planets to generate in a system.
	int system_max_planets = 300; // Roughly, maximum number of planets to generate in a system. May be more, since some planets will also have satellites.

	float system_mass_ratio = .95f; // How much mass (%) of the system will be allocated to the star.  
	float satellite_min_dist = 1.1F; // The minimum distance a body's periapsis can be from its orbiting body at start.
	float satellite_max_dist = 40;   // The maximum distance a body's periapsis can be from its orbiting body at start.

	// Chance a planet will have its own satellite
	double moon_chance = 0.1;

	// Chance a satellite's orbit will be retrograde.
	double retrograde_chance = 0.12;

	// Start generation settings
	int num_rand_planets = 0;
	int num_rand_systems = 1;
};