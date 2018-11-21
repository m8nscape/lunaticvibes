#include "sound_mgr.h"
#include "sound_fmod.h"

SoundMgr SoundMgr::_inst;

int SoundMgr::initFMOD()
{
    if (!_initialized)
    {
        _inst.driver = std::make_unique<SoundDriverFMOD>();
        auto ret = ((SoundDriverFMOD*)_inst.driver.get())->initRet;
        if (ret == FMOD_OK)
        {
            _initialized = true;
            return 0;
        }
    }
    return 1;
}

int SoundMgr::loadKeySample(std::string path, size_t index)
{
    if (!_initialized) return 1;
    return _inst.driver->loadKeySample(path, index);
}
void SoundMgr::playKeySample(size_t count, size_t index[])
{
    if (!_initialized) return;
    return _inst.driver->playKeySample(count, index);
}
void SoundMgr::freeKeySamples()
{
    if (!_initialized) return;
    return _inst.driver->freeKeySamples();
}
int SoundMgr::loadSample(std::string path, size_t index, bool isStream, bool loop)
{
    if (!_initialized) return 1;
    return _inst.driver->loadSample(path, index, isStream, loop); }
void SoundMgr::playSample(size_t index)
{
    if (!_initialized) return;
    return _inst.driver->playSample(index);
}
void SoundMgr::freeSamples()
{
    if (!_initialized) return;
    return _inst.driver->freeSamples();
}