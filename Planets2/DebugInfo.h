#pragma once
#include <string>

// Handles building a string of information for visual representation.
// Different text information is separated by a delimiter.
class DebugInfo
{

	// Current info.
	std::string info;

	// const char delimiter; // could allow setting delimiter

public:

	DebugInfo() = default;
	DebugInfo(std::string_view text);

	// Adds the given text to info.
	void add(std::string_view text);

	// Returns the currently built debug information.
	std::string_view get() const;

	// Clears this debug information.
	void clear();
	
	// Returns the C-style string of the information.
	const char* c_str() const;

};
