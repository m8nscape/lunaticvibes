#include "difficultytable.h"

#include <climits>
#include <unordered_map>

std::vector<std::string> DifficultyTable::getLevelList() const
{
	std::vector<std::string> levelList;
	levelList.reserve(entries.size());
	for (const auto& entry : entries)
	{
		levelList.push_back(entry.first);
	}

	std::sort(levelList.begin(), levelList.end(),
		std::bind(&DifficultyTable::compareByLevelOrder, this, std::placeholders::_1, std::placeholders::_2));

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

bool DifficultyTable::compareByLevelOrder(const std::string& first, const std::string& second) const
{
	auto getPosition = [](const std::unordered_map<std::string, int>& map, const std::string& key) -> int {
		if (const auto keyIt = map.find(key); keyIt != map.cend()) {
			return keyIt->second;
		}

		// If it's a numeric key, return it's value.
		try {
			return std::stoi(key);
		}
		catch (...)
		{
			// Non-numeric key, ignore.
		}

		return INT_MAX;
	};

	const int firstPos = getPosition(_levelOrder, first);
	const int secondPos = getPosition(_levelOrder, second);

	return firstPos < secondPos;
}
