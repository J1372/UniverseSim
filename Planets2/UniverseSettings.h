#pragma once

// Some settings will be able to be set by user at runtime. 
struct UniverseSettings {

	// Max number bodies and area.
	static constexpr int UNIVERSE_CAPACITY = 1000; // Maximum number of bodies to be active at one time.
	int universe_size_start = 1000; // initialize all starting bodies in this area. 
	float universe_size_max = 100000; // bodies will wraparound if going past this area.

	// physics
	//static constexpr double GRAV_CONST = 1;
	static constexpr long RAND_MASS = 100; // The maximum amount of mass to allocate to a body created with create_rand_body.
	double grav_const = 0.75;

	// create_system settings.
	int system_min_planets = 100; // Minimum number of planets to generate in a system.
	int system_max_planets = 300; // Roughly, maximum number of planets to generate in a system. May be more, since some planets will also have satellites.

	static constexpr float SYSTEM_STAR_MASS_RATIO = .95f; // How much mass (%) of the system will be allocated to the star.  
	static constexpr float SATELLITE_MIN_DIST = 1.1F; // The minimum distance a body's periapsis can be from its orbiting body at start.
	static constexpr float SATELLITE_MAX_DIST = 40;   // The maximum distance a body's periapsis can be from its orbiting body at start.


	// Start generation settings
	int num_rand_planets;
	int num_rand_systems;
};