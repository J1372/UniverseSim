#pragma once
#include "SpatialPartitioning.h"
#include <span>

// Line sweep algorithm for collision detection.
class LineSweep : public SpatialPartitioning
{

	enum class SweepEvent {
		END, // Algorithm should end because all entry events processed.
		ENTRY, // Encountered a body's leftmost point.
		LEAVE // Encountered a body's rightmost point.
	};


	// Sorted in ascending order of body's leftmost x-coordinate.
	std::vector<Body*> entry_events;

	// Sorted in ascending order of body's rightmost x-coordinate.
	std::vector<Body*> leave_events;

	// Returns the next event in the sweep line algorithm.
	SweepEvent get_next_event(int entries_processed, int leaves_processed) const;

	// Scans for and adds any collision events between the entering body and the currently active bodies to the collisions vector.
	// Returns number of collision checks performed.
	int get_collisions(Body& entry, std::span<Body*> currently_active, std::vector<Collision>& collisions) const;

	// Sorts entry and leave events in ascending order.
	void sort_events();


	// Gets an iterator for the body's entry event.
	std::vector<Body*>::const_iterator get_entry_it(const Body& body) const;
	// Gets an iterator for the body's leave event.
	std::vector<Body*>::const_iterator get_leave_it(const Body& body) const;

	// predicates for STL algorithms
	static bool left_less_than_point(const Body* body, float entry_x);
	static bool left_less_than(const Body* body1, const Body* body2);

	static bool right_less_than_point(const Body* body, float leave_x);
	static bool right_less_than(const Body* body1, const Body* body2);


public:

	// Adds a body to be considered by the algorithm.
	void add_body(Body& body) override;

	// Adds all bodies to be considered by the algorithm.
	void add_bodies(std::span<Body*> bodies);

	// Removes the body from being considered by the algorithm.
	void rem_body(const Body& body) override;

	// Updates algorithm's entry and leave events.
	void update() override;

	// Returns a pointer to the body that contains the point. If none do, returns nullptr.
	Body* find_body(Vector2 point) const override;

	// Returns a visual representation of the algorithm's events.
	std::vector<Rectangle> get_representation() const override;

	// Attaches text indicating body's entry event index, leave event index to the body's debug text.
	void attach_debug_text(Body& body) const override;

	// Detects and returns a vector of all collision events.
	std::vector<Collision> get_collisions() override;

};
