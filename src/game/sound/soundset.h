#pragma once
#include "common/types.h"

enum class eSoundSetType
{
	UNDEF,
	LR2,

};

class vSoundSet
{
protected:
	eSoundSetType _type;
public:
	virtual eSoundSetType type() const { return _type; }

public:
	vSoundSet() : _type(eSoundSetType::UNDEF) {}
	virtual ~vSoundSet() = default;

public:
	std::string name;
	std::string maker;

public:
	virtual Path getPathBGMSelect() const;
	virtual Path getPathBGMDecide() const;

	virtual Path getPathSoundOpenFolder() const { return Path(); }
	virtual Path getPathSoundCloseFolder() const { return Path(); }
	virtual Path getPathSoundOpenPanel() const { return Path(); }
	virtual Path getPathSoundClosePanel() const { return Path(); }
	virtual Path getPathSoundOptionChange() const { return Path(); }
	virtual Path getPathSoundDifficultyChange() const { return Path(); }

	virtual Path getPathSoundScreenshot() const { return Path(); }

	virtual Path getPathBGMResultClear() const { return Path(); }
	virtual Path getPathBGMResultFailed() const { return Path(); }
	virtual Path getPathSoundFailed() const { return Path(); }
	virtual Path getPathSoundLandmine() const { return Path(); }
	virtual Path getPathSoundScratch() const { return Path(); }

	virtual Path getPathBGMCourseClear() const { return Path(); }
	virtual Path getPathBGMCourseFailed() const { return Path(); }
};