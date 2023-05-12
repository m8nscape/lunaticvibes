#pragma once
#include "difficultytable.h"

namespace lunaticvibes
{

class DifficultyTableBMS : public DifficultyTable
{
public:
	DifficultyTableBMS() = default;
	virtual ~DifficultyTableBMS() = default;

protected:
	std::string symbol;
	std::string data_url;

public:
	virtual void updateFromUrl(std::function<void(UpdateResult)> finishedCallback) override;

	virtual bool loadFromFile() override;

	Path getFolderPath() const;

	std::string getSymbol() const;

	void parseHeader(const std::string& content);
	void parseBody(const std::string& content);
};

}
