#pragma once
#include "TextValidator.h"

// Enforces that input text is a float.
// Allows numbers and up to one decimal point.
// Allows an optional minus sign at the front of the input.
class FloatValidator : public TextValidator
{

	double min = std::numeric_limits<double>::min(); // minimum positive double.
	double max = std::numeric_limits<double>::max();

	bool has_decimal = false;

public:

	FloatValidator() = default;
	FloatValidator(double min);
	FloatValidator(double min, double max);

	// Returns false if inserting the given character at the given position would result in an invalid float, else true.
	bool on_press(std::string_view curr_text, int character, int insert_at) override;

	// Enforces that the input integer is within the minimum and maximum range.
	void on_exit(std::string& to_check) override;
};

