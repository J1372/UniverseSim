#pragma once

class Body;

// Represents an event where a Body is being removed.
struct Removal {

	/*
	* Since removal event processing may invalidate iterators, we use ids instead of pointers.
	* When processing events, this requires a map search to get a reference to the body with the matching id.
	* 
	* At least some issues with using pointers could be handled, but that would
	* add its own overhead.
	* Due to this, I'm not even sure if using pointers would be more performant.
	* 
	* Additionally: using pointers would definitely make the code more complex.
	* Additionally: removal events are relatively rare, so it should be fine to use the safer id method.
	*/


	// The id of the body being removed.
	const int removed;

	// The id of the body absorbing the removed body.
	const int absorbed_by = -1;

	// Returns true if the removed body was absorbed by another body.
	bool was_absorbed() const
	{
		return absorbed_by != -1;
	}

	// Returns true if the body was removed by some other means (like user interaction).
	bool was_deleted() const
	{
		return absorbed_by == -1;
	}



};
