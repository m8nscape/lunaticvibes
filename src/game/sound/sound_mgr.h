#pragma once
#include "sound_driver.h"
#include "sound_sample.h"

class SoundMgr
{
private:
    SoundMgr() = default;
    ~SoundMgr() = default;
    bool _initialized = false;
    static SoundMgr _inst;

private:
    std::unique_ptr<SoundDriver> driver;

public:
    static int initFMOD();
    static std::vector<std::pair<int, std::string>> getDeviceList();
    static int setDevice(size_t index);
    static std::pair<int, int> getDSPBufferSize();

public:
    static int loadNoteSample(const Path& path, size_t sample);
    static void playNoteSample(SoundChannelType ch, size_t count, size_t* samples);
    static void stopNoteSamples();
    static void freeNoteSamples();
    static long long getNoteSampleLength(size_t sample); // in ms
    static int loadSysSample(const Path& path, eSoundSample sample, bool isStream = false, bool loop = false);
    static void playSysSample(SoundChannelType ch, eSoundSample sample);
    static void stopSysSamples();
    static void freeSysSamples();
    static void startUpdate();
    static void stopUpdate();

    static void setSysVolume(float v, int gradientTime = 0);
    static void setNoteVolume(float v, int gradientTime = 0);
    static void setVolume(SampleChannel ch, float v);

    static void setDSP(DSPType type, int index, SampleChannel ch, float p1, float p2);
    static void updateDSP();

    static void setFreqFactor(double f);
    static void setSpeed(double speed);
    static void setPitch(double pitch);

    static void setEQ(EQFreq freq, int gain);
};