#pragma once
#include "soundset.h"
#include "game/skin/skin.h"

class SoundSetLR2 : public vSoundSet
{
public:
	SoundSetLR2();
	SoundSetLR2(Path p);
	virtual ~SoundSetLR2() = default;
	void loadCSV(Path p);
	bool parseHeader(const std::vector<StringContent>& tokens);
	bool parseBody(const std::vector<StringContent>& tokens);

private:
	struct CustomFile
	{
		StringContent title;
		size_t value;
		std::vector<StringContent> label;

		// file
		StringContent filepath;
		std::vector<Path> pathList;
		size_t defIdx;
	};
	std::vector<CustomFile> customfiles;
	std::vector<size_t> customizeRandom;

protected:
	Path filePath;
	Path thumbnailPath;

	unsigned csvLineNumber = 0;          // line parsing index

	std::map<std::string, Path> soundFilePath;
	bool loadPath(const std::string& key, const std::string& rawpath);

public:
	virtual Path getPathBGMSelect() const;
	virtual Path getPathBGMDecide() const;

	virtual Path getPathSoundOpenFolder() const;
	virtual Path getPathSoundCloseFolder() const;
	virtual Path getPathSoundOpenPanel() const;
	virtual Path getPathSoundClosePanel() const;
	virtual Path getPathSoundOptionChange() const;
	virtual Path getPathSoundDifficultyChange() const;

	virtual Path getPathSoundScreenshot() const;

	virtual Path getPathBGMResultClear() const;
	virtual Path getPathBGMResultFailed() const;
	virtual Path getPathSoundFailed() const;
	virtual Path getPathSoundLandmine() const;
	virtual Path getPathSoundScratch() const;

	virtual Path getPathBGMCourseClear() const;
	virtual Path getPathBGMCourseFailed() const;

public:
	virtual size_t getCustomizeOptionCount() const;
	virtual SkinBase::CustomizeOption getCustomizeOptionInfo(size_t idx) const;
	StringPath getFilePath() const;

};