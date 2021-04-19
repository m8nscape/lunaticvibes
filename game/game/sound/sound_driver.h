#pragma once
#include <array>
#include <string>
#include "fmod.hpp"

typedef std::size_t size_t;

class SoundDriver
{
    friend class SoundMgr;

public:
    SoundDriver() = default;
    virtual ~SoundDriver() = default;
    static const size_t KEYSAMPLES = 36 * 36 + 1;
    static const size_t ETCSAMPLES = 64; 

protected:
    std::array<FMOD::Sound*, KEYSAMPLES> keySamples{};  // Sound samples of key sound
    std::array<FMOD::Sound*, ETCSAMPLES> etcSamples{};  // Sound samples of BGM, effect, etc

public:
    virtual int loadKeySample(std::string path, size_t index) = 0;
    virtual void playKeySample(size_t count, size_t index[]) = 0;
    virtual void stopKeySamples() = 0;
    virtual void freeKeySamples() = 0;
    virtual int loadSample(std::string path, size_t index, bool isStream = false, bool loop = false) = 0;
    virtual void playSample(size_t index) = 0;
    virtual void stopSamples() = 0;
    virtual void freeSamples() = 0;
    virtual void update() = 0;
};