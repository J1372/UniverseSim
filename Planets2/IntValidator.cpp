#include "IntValidator.h"
#include <raylib.h>


IntValidator::IntValidator(int min)
	: min(min)
{}

IntValidator::IntValidator(int min, int max)
	: min(min), max(max)
{}

bool IntValidator::on_press(std::string_view curr_text, int character, int insert_at)
{
	if (character == KEY_BACKSPACE)
	{
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

	return false;
}

void IntValidator::on_exit(std::string& to_check)
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

	int val = std::stoi(to_check);

	if (val > max)
	{
		to_check = std::to_string(max);
	}
	else if (val < min)
	{
		to_check = std::to_string(min);
	}

}
