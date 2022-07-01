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
	DebugInfo(const std::string& text);

	// Adds the given text to info.
	void add(const std::string& text);

	// Returns the currently built debug information.
	const std::string& get() const;

	// Clears this debug information.
	void clear();

};
