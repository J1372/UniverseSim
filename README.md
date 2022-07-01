# UniverseSim

A highly customizable universe generator and n-body planetary physics simulator written in C++. Made using the C [Raylib](https://github.com/raysan5/raylib) library for window creation, graphics, and keyboard and mouse input.


### What it does
On startup, users use the gui to customize simulation settings for universe generation, physics, and partitioning methods.
Afterwards, a universe is generated according to these settings. The simulation is paused on startup, but this can be toggled with the spacebar.
The user can press the H key for help and to see a list of all toggles and commands.


### Features:
- N-body physics simulation between planetary bodies.
- A settings gui.
- Three different methods of spatial partitioning that can be selected at runtime for use in collision detection.
  - Quadtree
  - Grid
  - Line sweep
- Option to set value for gravitational accuracy to increase performance using the Barnes-Hut approximation algorithm.
- Generation of dynamic universes.
  - Multiple different planetary types with varying densities.
  - Generation of bodies with random masses and uniform random positions.
  - Customizable random system generator, which generates a functional planetary system with satellites of varying distances and orbital eccentricities. Parameterized by user-specified:
    - Ratio of central body mass to system mass
    - Min and max number of planets
    - Min and max distance from central body
    - Moon chance
    - Retrograde chance
 
- Interaction during simulation.
  - Attaching camera to follow a planet.
  - Creating a custom planet, setting mass and velocity.
  - Creating planetary systems with the mouse.

- Graphical display of interesting simulation data.
  - Toggleable display of collision statistics
    - Number of collision checks last tick.
    - Number of collision checks since simulation started.
  - Toggleable display of planetary bodies' information.
  - Toggleable visual representation of partitioning methods during simulation.
    - If planetary body information display also enabled, will also display alongside the body extra information related to the partitioning method.

## Video
Here is a video of various clips showing the simulator and some of its features (with timestamps in the description):

[Video](https://youtu.be/eocuC2M2wu4)

## Releases
Runnable binaries of the project for Windows 32 bit and 64 bit can be found under [releases](https://github.com/Xxthedarkmatterturtlepasta003xX/UniverseSim/releases/tag/v1.0.0).

## Building the project
The project uses C++20 features.

In order to build the project from source code, you must download [Raylib v4.0](https://github.com/raysan5/raylib/releases/tag/4.0.0), then include and link it with your project.
