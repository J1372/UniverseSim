#include "DebugInfo.h"

DebugInfo::DebugInfo(const std::string& text)
{
	info = text;
}

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
