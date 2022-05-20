#pragma once

class Body;

// Represents an event where a Body is being removed.
struct Removal {

	Body& removed;
	Body* absorbed_by;

};
