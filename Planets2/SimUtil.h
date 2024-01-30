#pragma once
#include <optional>
#include <string_view>

namespace SimUtil
{
	// Convert a string view to an int.
	std::optional<int> svtoi(std::string_view sv);
};


