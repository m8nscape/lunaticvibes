#pragma once
#include "sound_driver.h"
#include "sound_sample.h"
#include <memory>

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
    static std::vector<std::pair<int, std::string>> getDeviceList(bool asio = false);

public:
    static int loadKeySample(const Path& path, size_t sample);
    static void playKeySample(size_t count, size_t* samples);
    static void stopKeySamples();
    static void freeKeySamples();
    static int loadSample(const Path& path, eSoundSample sample, bool isStream = false, bool loop = false);
    static void playSample(eSoundSample sample);
    static void stopSamples();
    static void freeSamples();
    static void update();

    static void setVolume(SampleChannel ch, float v);

    static void setDSP(DSPType type, int index, SampleChannel ch, float p1, float p2);
    static void updateDSP();

    static void setFreqFactor(double f);
    static void setSpeed(double speed);
    static void setPitch(double pitch);

    static void setEQ(EQFreq freq, int gain);
};