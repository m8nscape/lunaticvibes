#pragma once
#include <array>
#include <string>
#include "common/asynclooper.h"
#include "fmod.hpp"

typedef std::size_t size_t;

enum class DSPType
{
    OFF,
    REVERB,
    DELAY,
    LOWPASS,
    HIGHPASS,
    FLANGER,
    CHORUS,
    DISTORTION,
};

enum class EQFreq
{
    _62_5,
    _160,
    _400,
    _1000,
    _2500,
    _6250,
    _16k
};

enum class SampleChannel
{
    MASTER,
    KEY,
    BGM,
};

enum class SoundChannelType
{
    BGM_SYS,
    BGM_NOTE,
    KEY_SYS,
    KEY_LEFT,
    KEY_RIGHT,
    TYPE_COUNT,
};

class SoundDriver: public AsyncLooper
{
    friend class SoundMgr;

public:
    SoundDriver(std::function<void()> update) : AsyncLooper("Sound Driver", update, 1000, true) {}
    virtual ~SoundDriver() = default;

public:
    virtual std::vector<std::pair<int, std::string>> getDeviceList(bool asio = false) = 0;
    virtual int setDevice(size_t index, bool asio = false) = 0;

public:
    virtual int loadNoteSample(const Path& path, size_t index) = 0;
    virtual void playNoteSample(SoundChannelType ch, size_t count, size_t index[]) = 0;
    virtual void stopNoteSamples() = 0;
    virtual void freeNoteSamples() = 0;
    virtual int loadSysSample(const Path& path, size_t index, bool isStream = false, bool loop = false) = 0;
    virtual void playSysSample(SoundChannelType ch, size_t index) = 0;
    virtual void stopSysSamples() = 0;
    virtual void freeSysSamples() = 0;

public:
    virtual void setVolume(SampleChannel ch, float v) = 0;
    virtual void setDSP(DSPType type, int index, SampleChannel ch, float p1, float p2) = 0;
    virtual void setFreqFactor(double f) = 0;
    virtual void setSpeed(double speed) = 0;
    virtual void setPitch(double pitch) = 0;
    virtual void setEQ(EQFreq freq, int gain) = 0;
};
