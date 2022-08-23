#include "FloatValidator.h"
#include <raylib.h>
#include <iostream>

FloatValidator::FloatValidator(double min)
	: min(min)
{}

FloatValidator::FloatValidator(double min, double max)
	: min(min), max(max)
{}

bool FloatValidator::on_press(std::string_view curr_text, int character, int insert_at)
{
	if (character == KEY_BACKSPACE)
	{
		if (insert_at > 0)
		{
			char deleted_char = curr_text[insert_at - 1];
			if (deleted_char == '.')
			{
				has_decimal = false;
			}
		}

		return true;
	}

	if (std::isdigit(character))
	{
		return true;
	}

	if (character == '-' and insert_at == 0)
	{
		if (curr_text.empty())
		{
			return true;
		}

		return curr_text[0] != '-';
	}

	if (character == '.' and !has_decimal)
	{
		has_decimal = true;
		return true;
	}

	return false;
}

void FloatValidator::on_exit(std::string& to_check)
{
	if (to_check.empty())
	{
		to_check = std::to_string(min);
		return;
	}
	else if (to_check.size() == 1 and to_check[0] == '-')
	{
		// user only input a minus sign with no number after it.
		to_check = std::to_string(min);
		return;
	}

	double val = std::stod(to_check);

	if (val > max)
	{
		to_check = std::to_string(max);
	}
	else if (val < min)
	{
		to_check = std::to_string(min);
	}
}
