#include "common/pch.h"
#include "sound_mgr.h"
#include "sound_fmod.h"
#include "sound_sample.h"

namespace lunaticvibes
{

SoundMgr SoundMgr::_inst;

int SoundMgr::initFMOD()
{
    if (!_inst._initialized)
    {
        LOG_INFO << "[Sound] Initializing sound driver...";
        _inst.driver = std::make_unique<SoundDriverFMOD>();
        auto ret = ((SoundDriverFMOD*)_inst.driver.get())->initRet;
        if (ret == FMOD_OK)
        {
            _inst._initialized = true;
            LOG_INFO << "[Sound] Sound driver init finished.";

            loadLR2Sound();
            return 0;
        }
    }
    else
    {
        LOG_ERROR << "[Sound] Already initialized";
    }
    return -255;
}

std::vector<std::pair<int, std::string>> SoundMgr::getDeviceList()
{
    if (!_inst._initialized) return {};
    return _inst.driver->getDeviceList();
}

int SoundMgr::setDevice(size_t index)
{
    if (!_inst._initialized) return -255;
    int ret = _inst.driver->setDevice(index);
    if (ret != 0) _inst._initialized = false;
    return ret;
}

std::pair<int, int> SoundMgr::getDSPBufferSize()
{
    if (!_inst._initialized) return { 1024, 4 };
    return _inst.driver->getDSPBufferSize();
}

int SoundMgr::loadNoteSample(const Path& path, size_t sample)
{
    if (!_inst._initialized) return -255;
    return _inst.driver->loadNoteSample(path, sample);
}
void SoundMgr::playNoteSample(SoundChannelType ch, size_t count, size_t* samples)
{
    if (!_inst._initialized) return;
    return _inst.driver->playNoteSample(ch, count, samples);
}
void SoundMgr::stopNoteSamples()
{
    if (!_inst._initialized) return;
    return _inst.driver->stopNoteSamples();
}
void SoundMgr::freeNoteSamples()
{
    if (!_inst._initialized) return;
    return _inst.driver->freeNoteSamples();
}
long long SoundMgr::getNoteSampleLength(size_t sample)
{
    if (!_inst._initialized) return 0;
    return _inst.driver->getNoteSampleLength(sample);
}
int SoundMgr::loadSysSample(const Path& path, eSoundSample sample, bool isStream, bool loop)
{
    if (!_inst._initialized) return -255;
    return _inst.driver->loadSysSample(path, static_cast<size_t>(sample), isStream, loop);
}
void SoundMgr::playSysSample(SoundChannelType ch, eSoundSample sample)
{
    if (!_inst._initialized) return;
    return _inst.driver->playSysSample(ch, static_cast<size_t>(sample));
}
void SoundMgr::stopSysSamples()
{
    if (!_inst._initialized) return;
    return _inst.driver->stopSysSamples();
}
void SoundMgr::freeSysSamples()
{
    if (!_inst._initialized) return;
    return _inst.driver->freeSysSamples();
}

void SoundMgr::startUpdate()
{
    if (!_inst._initialized) return;
    return _inst.driver->loopStart();
}

void SoundMgr::stopUpdate()
{
    if (!_inst._initialized) return;
    return _inst.driver->loopEnd();
}

void SoundMgr::setSysVolume(float v, int gradientTime)
{
    if (!_inst._initialized) return;
    return _inst.driver->setSysVolume(v, gradientTime);
}

void SoundMgr::setNoteVolume(float v, int gradientTime)
{
    if (!_inst._initialized) return;
    return _inst.driver->setNoteVolume(v, gradientTime);
}

void SoundMgr::setVolume(SampleChannel ch, float v)
{
    if (!_inst._initialized) return;
    return _inst.driver->setVolume(ch, v);
}

void SoundMgr::setDSP(DSPType type, int index, SampleChannel ch, float p1, float p2)
{
    if (!_inst._initialized) return;
    return _inst.driver->setDSP(type, index, ch, p1, p2);
}

void SoundMgr::updateDSP()
{

}

void SoundMgr::setFreqFactor(double f)
{
    if (!_inst._initialized) return;
    return _inst.driver->setFreqFactor(f);
}

void SoundMgr::setSpeed(double speed)
{
    if (!_inst._initialized) return;
    return _inst.driver->setSpeed(speed);
}

void SoundMgr::setPitch(double pitch)
{
    if (!_inst._initialized) return;
    return _inst.driver->setPitch(pitch);
}

void SoundMgr::setEQ(EQFreq freq, int gain)
{
    if (!_inst._initialized) return;
    return _inst.driver->setEQ(freq, gain);
}

}
