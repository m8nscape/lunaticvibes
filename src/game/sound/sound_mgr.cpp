#include "sound_mgr.h"
#include "sound_fmod.h"
#include "sound_sample.h"

SoundMgr SoundMgr::_inst;

int SoundMgr::initFMOD()
{
    if (!_inst._initialized)
    {
        _inst.driver = std::make_unique<SoundDriverFMOD>();
        auto ret = ((SoundDriverFMOD*)_inst.driver.get())->initRet;
        if (ret == FMOD_OK)
        {
            _inst._initialized = true;
            loadLR2Bgm();
            loadLR2Sound();
            return 0;
        }
    }
    return 1;
}

std::vector<std::pair<int, std::string>> SoundMgr::getDeviceList(bool asio)
{
    if (!_inst._initialized) return {};
    return _inst.driver->getDeviceList(asio);
}

int SoundMgr::loadKeySample(const Path& path, size_t sample)
{
    if (!_inst._initialized) return 1;
    return _inst.driver->loadKeySample(path, sample);
}
void SoundMgr::playKeySample(size_t count, size_t* samples)
{
    if (!_inst._initialized) return;
    return _inst.driver->playKeySample(count, samples);
}
void SoundMgr::stopKeySamples()
{
    if (!_inst._initialized) return;
    return _inst.driver->stopKeySamples();
}
void SoundMgr::freeKeySamples()
{
    if (!_inst._initialized) return;
    return _inst.driver->freeKeySamples();
}
int SoundMgr::loadSample(const Path& path, eSoundSample sample, bool isStream, bool loop)
{
    if (!_inst._initialized) return 1;
    return _inst.driver->loadSample(path, static_cast<size_t>(sample), isStream, loop);
}
void SoundMgr::playSample(eSoundSample sample)
{
    if (!_inst._initialized) return;
    return _inst.driver->playSample(static_cast<size_t>(sample));
}
void SoundMgr::stopSamples()
{
    if (!_inst._initialized) return;
    return _inst.driver->stopSamples();
}
void SoundMgr::freeSamples()
{
    if (!_inst._initialized) return;
    return _inst.driver->freeSamples();
}

void SoundMgr::update()
{
    if (!_inst._initialized) return;
    return _inst.driver->update();
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
