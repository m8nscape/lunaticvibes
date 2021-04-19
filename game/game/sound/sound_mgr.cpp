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

int SoundMgr::loadKeySample(std::string path, size_t index)
{
    if (!_inst._initialized) return 1;
    return _inst.driver->loadKeySample(path, index);
}
void SoundMgr::playKeySample(size_t count, size_t index[])
{
    if (!_inst._initialized) return;
    return _inst.driver->playKeySample(count, index);
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
int SoundMgr::loadSample(std::string path, size_t index, bool isStream, bool loop)
{
    if (!_inst._initialized) return 1;
    return _inst.driver->loadSample(path, index, isStream, loop); 
}
void SoundMgr::playSample(size_t index)
{
    if (!_inst._initialized) return;
    return _inst.driver->playSample(index);
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
