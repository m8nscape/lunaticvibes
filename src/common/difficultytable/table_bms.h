#pragma once
#include "difficultytable.h"

class DifficultyTableBMS: public DifficultyTable
{
public:
	DifficultyTableBMS() = default;
	virtual ~DifficultyTableBMS() = default;

protected:
	std::string symbol;

public:
	virtual void updateFromUrl(std::function<void(UpdateResult)> finishedCallback) override;

	virtual bool loadFromFile() override;

	Path getFolderPath() const;

	std::string getSymbol() const;
};