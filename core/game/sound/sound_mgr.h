#pragma once
#include "sound_driver.h"
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
    static int loadKeySample(std::string path, size_t index);
    static void playKeySample(size_t count, size_t index[]);
    static void freeKeySamples();
    static int loadSample(std::string path, size_t index, bool isStream = false, bool loop = false);
    static void playSample(size_t index);
    static void freeSamples();
    static void update();
};