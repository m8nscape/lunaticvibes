#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include "../entry/entry.h"

class DifficultyTable
{
public:
	DifficultyTable() = default;
	virtual ~DifficultyTable() = default;

protected:
	std::string name;
	std::string url;

	// level -> chart
	// level may be other than numbers. (e.g. ★???)
	std::map<std::string, std::vector<std::shared_ptr<EntryBase>>> entries;
	std::unordered_map<std::string, int> _levelOrder;

public:
	const std::string& getName() const { return name; }
	const std::string& getUrl() const { return url; }
	void setUrl(const std::string& u) { url = u; }

	enum class UpdateResult
	{
		OK,
		INTERNAL_ERROR,
		WEB_PATH_ERROR,
		WEB_CONNECT_ERR,
		WEB_TIMEOUT,
		WEB_PARSE_FAILED,
		HEADER_PATH_ERROR,
		HEADER_CONNECT_ERR,
		HEADER_TIMEOUT,
		HEADER_PARSE_FAILED,
		DATA_PATH_ERROR,
		DATA_CONNECT_ERR,
		DATA_TIMEOUT,
		DATA_PARSE_FAILED,
	};
	virtual void updateFromUrl(std::function<void(UpdateResult)> finishedCallback) = 0;

	virtual bool loadFromFile() = 0;

	std::vector<std::string> getLevelList() const;
	std::vector<std::shared_ptr<EntryBase>> getEntryList(const std::string& level);

private:
	[[nodiscard]] bool compareByLevelOrder(const std::string&, const std::string&) const;
};