#pragma once

class Body;

// Represents an event where a Body is being removed.
struct Removal {

	Body* removed;
	Body* absorbed_by;
	
	// Pointer to the last body in the universe's body list. Will be swapped with removed after event processed.
	const Body* last = nullptr; 
};
