#include "DebugInfo.h"

void DebugInfo::add(const std::string& to_add)
{
	if (!info.empty())
	{
		info += "\n";
	}

	info += to_add;
}

const std::string& DebugInfo::get() const
{
	return info;
}

void DebugInfo::clear()
{
	info.clear();
}
