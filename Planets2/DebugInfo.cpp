#include "DebugInfo.h"

DebugInfo::DebugInfo(std::string_view text)
	: info(text)
{}

void DebugInfo::add(std::string_view to_add)
{
	if (!info.empty())
	{
		info += "\n";
	}

	info += to_add;
}

std::string_view DebugInfo::get() const
{
	return info;
}

void DebugInfo::clear()
{
	info.clear();
}

const char* DebugInfo::c_str() const
{
	return info.c_str();
}
