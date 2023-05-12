#include "common/pch.h"
#include "difficultytable.h"

namespace lunaticvibes
{

std::vector<std::string> DifficultyTable::getLevelList() const
{
	std::vector<std::pair<int, std::string>> sortOrder;
	for (auto& e : entries)
	{
		int level = 0;
		try {
			level = std::stoi(e.first);
		}
		catch (...)
		{
			level = INT_MAX;
		}
		sortOrder.push_back(std::make_pair(level, e.first));
	}
	std::sort(sortOrder.begin(), sortOrder.end());

	std::vector<std::string> levelList;
	for (auto& e : sortOrder)
	{
		levelList.push_back(e.second);
	}
	return levelList;
}

std::vector<std::shared_ptr<EntryBase>> DifficultyTable::getEntryList(const std::string& level)
{
	if (entries.find(level) == entries.end())
	{
		return {};
	}
	return entries.at(level);
}

}
