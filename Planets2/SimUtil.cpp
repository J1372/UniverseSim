#include "SimUtil.h"
#include <charconv>

std::optional<int> SimUtil::svtoi(std::string_view sv)
{
	int val;
	if (std::from_chars(sv.data(), sv.data() + sv.size(), val).ec == std::errc{})
	{
		return val;
	}
	else
	{
		return std::nullopt;
	}
}
