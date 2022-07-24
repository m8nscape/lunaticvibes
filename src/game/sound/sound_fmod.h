#pragma once

#include <array>
#include <string>
#include <thread>
#include "sound_driver.h"
#include "fmod.hpp"
#include "common/types.h"

// This game uses FMOD Low Level API to play sounds as we don't use FMOD Studio,

class SoundDriverFMOD: public SoundDriver
{
    friend class SoundMgr;

private:
	FMOD::System *fmodSystem = nullptr;
	int initRet;

protected:
	std::shared_ptr<FMOD::ChannelGroup> keySamplesChannelGroup;
	std::shared_ptr<FMOD::ChannelGroup> etcSamplesChannelGroup;
	std::map<SampleChannel, float> volume;
	FMOD::DSP* DSP[3][4];

public:
	SoundDriverFMOD();
	virtual ~SoundDriverFMOD();

public:
	virtual std::vector<std::pair<int, std::string>> getDeviceList(bool asio = false);

private:
    bool bLoading = false;
    std::thread tLoadSampleThread;
    void loadSampleThread();

public:
    int setAsyncIO(bool async = true);

public:
	virtual int loadKeySample(const Path& path, size_t index);
	virtual void playKeySample(size_t count, size_t index[]);
	virtual void stopKeySamples();
	virtual void freeKeySamples();
	virtual void update();

public:
	virtual int loadSample(const Path& path, size_t index, bool isStream = false, bool loop = false);
	virtual void playSample(size_t index);
	virtual void stopSamples();
	virtual void freeSamples();
	int getChannelsPlaying();

public:
	virtual void setVolume(SampleChannel ch, float v);
	virtual void setDSP(DSPType type, int index, SampleChannel ch, float p1, float p2);
};
