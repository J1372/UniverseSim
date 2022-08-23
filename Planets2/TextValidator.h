#pragma once
#include <string>

// An input validator to guarantee that text entered into a TextBox is valid.
class TextValidator
{
public:

	// Checks if a character should be allowed to be inserted at a point in the current input.
	// Returns true if it should be allowed, else false.
	virtual bool on_press(std::string_view curr_text, int character, int insert_at) = 0;

	// Checks the entire string and potentially modifies it to enforce any semantic restraints.
	virtual void on_exit(std::string& to_check) = 0;

	virtual ~TextValidator() = default;
};
