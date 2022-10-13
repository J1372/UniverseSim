#pragma once
#include "TextValidator.h"

// Enforces that input text is an integer.
// Only allows numbers and an optional minus sign at the front of the input.
class IntValidator : public TextValidator
{

	int min = std::numeric_limits<int>::min();
	int max = std::numeric_limits<int>::max();

public:

	IntValidator() = default;
	IntValidator(int min);
	IntValidator(int min, int max);

	// Returns false if inserting the given character at the given position would result in an invalid integer, else true.
	bool on_press(std::string_view curr_text, int character, int insert_at) override;

	// Enforces that the input integer is within the minimum and maximum range.
	void on_exit(std::string& to_check) override;
};

