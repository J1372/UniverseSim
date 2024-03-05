# UniverseSim

![Image: orbit trajectories](https://github.com/J1372/UniverseSim/assets/95717261/e9613a2d-fd17-459d-b7eb-d4cbed3f2741)

A customizable and interactive n-body planetary physics simulator written in C++. Made using the C [Raylib](https://github.com/raysan5/raylib) library for window creation, graphics, and keyboard and mouse input.

This program simulates n-body physics between planetary bodies. This means that every body is exerting gravitational forces on every other body, as opposed to moving on a predefined orbit or only simulating forces between a main body and its satellites.

Settings for universe generation, physics, and partitioning methods can be customized at runtime using a gui.
The simulation is paused on startup, but this can be toggled with the spacebar.
Different information displays can be toggled by the user and the user may interact with the simulator in various ways listed in the Features section.
Additional commands and toggles can be found in the help menu by pressing the H key.

### Features:
- Universe generation:
  - Customizable random system generator, which generates a functional planetary system with satellites of varying distances and orbital eccentricities. Parameterized by user-specified:
    - Ratio of central body mass to system mass
    - Min and max number of planets
    - Min and max distance from central body
    - Moon chance
    - Retrograde chance
  - Multiple different planetary types with varying densities.

 - Display of simulation information:
    - Real-time display of orbital trajectories.
    - Toggleable display of planetary body information.
    - Toggleable display of collision statistics
      - Number of collision checks last tick.
      - Number of collision checks since simulation started.
    - Toggleable visual representation of partitioning methods during simulation.
      - If planetary body information display also enabled, will also display alongside the body extra information related to the partitioning method.

- Interaction during simulation:
  - Attaching the camera to follow a planet.
  - Creating custom planets, setting mass and velocity.
    - Creating satellites using relative velocity and orbital info and trajectory display.
  - Generating systems with initial velocities.
  - Deleting objects.



- Three different methods of spatial partitioning that can be selected at runtime for use in collision detection:
  - Quadtree
  - Grid
  - Line sweep
- Option to set value for gravitational accuracy to increase performance using the Barnes-Hut approximation algorithm.
- N-body physics simulation between planetary bodies.

## Video
Here are videos of various clips showing the simulator and some of its features (with timestamps in the descriptions):

[Video](https://youtu.be/7g5A06wBgi0)

[Older Video](https://youtu.be/eocuC2M2wu4)

## Releases
Runnable binaries of the project for Windows 32 bit and 64 bit can be found under [releases](https://github.com/J1372/UniverseSim/releases).

## Building the project
The project uses C++20 features.

In order to build the project from source code, you must download [Raylib v4.0](https://github.com/raysan5/raylib/releases/tag/4.0.0), then include and link it with your project.
