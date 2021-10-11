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

    static void setDSP(DSPType type, SampleChannel ch, int p1, int p2);
    static void updateDSP();

    static void setFrequencyFactor(double freq);
    static void setPitch(double pitch);
};