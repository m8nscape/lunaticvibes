#include "difficultytable.h"

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
		sortOrder.emplace_back(level, e.first);
	}
	std::sort(sortOrder.begin(), sortOrder.end());

	std::vector<std::string> levelList;
	levelList.reserve(sortOrder.size());
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
