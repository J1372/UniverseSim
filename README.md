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

### Releases
Runnable binaries of the project for Windows 32 bit and 64 bit can be found under [releases](https://github.com/Xxthedarkmatterturtlepasta003xX/UniverseSim/releases/tag/v1.0.0).

### Building project
The project uses C++20 features.

In order to build the project from source code, you must download [Raylib v4.0](https://github.com/raysan5/raylib/releases/tag/4.0.0), then include and link it with your project.


In code, this project is comprised of several different parts.

## Gui
The default Raylib library does not come with standard gui elements, so I made minimalistic versions for use in scenes.
- UIElement - This is the base class for all gui elements.
- GuiComponentList - This is a container of gui elements that can be used in any scene.
- Button - A rectangular button that performs a function on click.
- Dropdown - A list of choices that can be selected from. Expands on click.
- Label - A text label.
- TextBox - A box that can be clicked on and text entered into.
- Slider - A slider to set a value between a min and max value.

## Scenes
- Scene - The base interface for every scene.
- GuiScene - A specialization of Scene that implements its render and update methods.
This type of scene should be used if the scene only has gui elements and nothing else.
- SettingsScene - Displays the main settings gui.
- SimulationScene - Runs the simulation and handles user input during simulation.

### Cameras
The simulation scene delegates some camera policy to a camera state machine.
- AdvCamera - A wrapper around the Raylib Camera2D struct that supports methods for zooming, moving offset/target in a direction, and setting and updating offset/target velocities.
- CameraState - Base class in camera state pattern.
- FreeCamera - A camera with unrestricted movement that is fully controlled by the user.
- AnchoredCamera - A camera whose target is kept focused on a planetary body. The user can still pan the camera around, but this camera state prevents the focused body from going off screen.

### Interaction states
The simulation scene uses a state machine for general user interaction with the universe.
- InteractionState - The base interface for interaction states.
- SystemCreation - User generates and previews planetary systems, potentially adding them to the universe.
- PlanetCreation - User customizes the velocity and mass of a body to be added to the universe.
- DefaultInteraction - The default user interaction with the universe. Can delete planets.

## Simulation model
- Universe - Holds settings and a collection of planetary bodies. Updates the model, optionally delegating collision checking to a partitioning method, and handles Collision events.
- UniverseSettings - Struct for settings that are used to parameterize universe generation and physics simulation.
- Body - A planetary body in the universe.
- Physics - Namespace for common physics related methods.
- BarnesHut - An implementation of the Barnes-Hut gravity approximation algorithm.
- Orbit - A description of an orbit around a planetary body.

### Partitionings
Based on user selected settings, the universe update tick may delegate collision checking and finding a body based on a point to a partitioning method.
- SpatialPartitioning - This is the base interface for all partitioning implementations.
- QuadTree - A quad tree implementation. Can be parameterized based on number of bodies per quad and maximum quad tree subdivision depth.
- Grid - A basic grid-based partitioning method. Can be parameterized based on nodes per row.
- LineSweep - An implementation of the line sweep algorithm.

### Events
The simulation model has some event structs in order to represent events and hold their relevant information.
- Event\<EventClass\> - An observer list for an event of EventClass that can register and unregister callback methods that take a parameter of EventClass.
- Removal - A removal event that holds a reference to the body that is going to be removed and an optional reference to a body that is the cause of its removal.
- Collision - A collision event that holds a reference to the more massive planetary body and the smaller planetary body.


## Other
- Circle - a representation of a circle with radius around a point.
- my_random - Namespace for random number generation.
- View - Entrypoint to the program. Initializes Raylib and updates the active scene.
