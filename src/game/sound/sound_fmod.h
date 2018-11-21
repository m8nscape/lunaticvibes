#pragma once

#include <array>
#include <string>
#include <thread>
#include "sound_driver.h"
#include "fmod.hpp"

// This game uses FMOD Low Level API to play sounds as we don't use FMOD Studio,

class SoundDriverFMOD: public SoundDriver
{
    friend class SoundMgr;

private:
	FMOD::System *fmodSystem = nullptr;
	int initRet;

public:
	SoundDriverFMOD();
	virtual ~SoundDriverFMOD();

private:
    bool bLoading = false;
    std::thread tLoadSampleThread;
    void loadSampleThread();

public:
    int setAsyncIO(bool async = true);

public:
	virtual int loadKeySample(std::string path, size_t index);
	virtual void playKeySample(size_t count, size_t index[]);
	virtual void freeKeySamples();
	int update();

public:
	virtual int loadSample(std::string path, size_t index, bool isStream = false, bool loop = false);
	virtual void playSample(size_t index);
	virtual void freeSamples();
	int getChannelsPlaying();

};
