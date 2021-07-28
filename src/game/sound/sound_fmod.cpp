#include "common/log.h"
#include "sound_fmod_callback.h"
#include "sound_fmod.h"
#include "fmod_errors.h"
#include <cstdlib>

#include "common/utils.h"

SoundDriverFMOD::SoundDriverFMOD()
{
    initRet = FMOD::System_Create(&fmodSystem);      // Create the main system object.
    if (initRet != FMOD_OK)
    {
        LOG_ERROR << "Create FMOD System Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
        return;
    }

    fmodSystem->setDSPBufferSize(
        128, 2
        // conf().audio.get<unsigned>(aud_bufLen),
        // conf().audio.get<int>(aud_bufCount)
    );

    initRet = fmodSystem->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (initRet != FMOD_OK)
    {
        LOG_ERROR << "FMOD System Initialize Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
        return;
    }

    FMOD::ChannelGroup* pk = nullptr;
    initRet = fmodSystem->createChannelGroup("KEY_SAMPLES", &pk);
    if (initRet != FMOD_OK)
    {
        LOG_ERROR << "Create Key channel group Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
        return;
    }
    keySamplesChannelGroup = std::shared_ptr<FMOD::ChannelGroup>(pk, [](FMOD::ChannelGroup* p) { p->release(); });

    FMOD::ChannelGroup* pe = nullptr;
    initRet = fmodSystem->createChannelGroup("SYS_SAMPLES", &pe);
    if (initRet != FMOD_OK)
    {
        LOG_ERROR << "Create sys channel group Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
        return;
    }
    etcSamplesChannelGroup = std::shared_ptr<FMOD::ChannelGroup>(pe, [](FMOD::ChannelGroup* p) { p->release(); });


    if (initRet == FMOD_OK)
    {
        LOG_DEBUG << "FMOD System Initialize Finished.";

        FMOD_OUTPUTTYPE outputtype;
        fmodSystem->getOutput(&outputtype);
        switch (outputtype)
        {
        case FMOD_OUTPUTTYPE_AUTODETECT: LOG_INFO << "[FMOD] Output Type: AUTODETECT"; break;
        case FMOD_OUTPUTTYPE_UNKNOWN: LOG_INFO << "[FMOD] Output Type: UNKNOWN"; break;
        case FMOD_OUTPUTTYPE_NOSOUND: LOG_INFO << "[FMOD] Output Type: NOSOUND"; break;
        case FMOD_OUTPUTTYPE_WAVWRITER: LOG_INFO << "[FMOD] Output Type: WAVWRITER"; break;
        case FMOD_OUTPUTTYPE_NOSOUND_NRT: LOG_INFO << "[FMOD] Output Type: NOSOUND_NRT"; break;
        case FMOD_OUTPUTTYPE_WAVWRITER_NRT: LOG_INFO << "[FMOD] Output Type: WAVWRITER_NRT"; break;
        case FMOD_OUTPUTTYPE_DSOUND: LOG_INFO << "[FMOD] Output Type: DSOUND"; break;
        case FMOD_OUTPUTTYPE_WINMM: LOG_INFO << "[FMOD] Output Type: WINMM"; break;
        case FMOD_OUTPUTTYPE_WASAPI: LOG_INFO << "[FMOD] Output Type: WASAPI"; break;
        case FMOD_OUTPUTTYPE_ASIO: LOG_INFO << "[FMOD] Output Type: ASIO"; break;
        case FMOD_OUTPUTTYPE_PULSEAUDIO: LOG_INFO << "[FMOD] Output Type: PULSEAUDIO"; break;
        case FMOD_OUTPUTTYPE_ALSA: LOG_INFO << "[FMOD] Output Type: ALSA"; break;
        case FMOD_OUTPUTTYPE_COREAUDIO: LOG_INFO << "[FMOD] Output Type: COREAUDIO"; break;
        case FMOD_OUTPUTTYPE_XAUDIO: LOG_INFO << "[FMOD] Output Type: XAUDIO"; break;
        case FMOD_OUTPUTTYPE_PS3: LOG_INFO << "[FMOD] Output Type: PS3"; break;
        case FMOD_OUTPUTTYPE_AUDIOTRACK: LOG_INFO << "[FMOD] Output Type: AUDIOTRACK"; break;
        case FMOD_OUTPUTTYPE_OPENSL: LOG_INFO << "[FMOD] Output Type: OPENSL"; break;
        case FMOD_OUTPUTTYPE_WIIU: LOG_INFO << "[FMOD] Output Type: WIIU"; break;
        case FMOD_OUTPUTTYPE_AUDIOOUT: LOG_INFO << "[FMOD] Output Type: AUDIOOUT"; break;
        case FMOD_OUTPUTTYPE_AUDIO3D: LOG_INFO << "[FMOD] Output Type: AUDIO3D"; break;
        case FMOD_OUTPUTTYPE_ATMOS: LOG_INFO << "[FMOD] Output Type: ATMOS"; break;
        case FMOD_OUTPUTTYPE_WEBAUDIO: LOG_INFO << "[FMOD] Output Type: WEBAUDIO"; break;
        case FMOD_OUTPUTTYPE_NNAUDIO: LOG_INFO << "[FMOD] Output Type: NNAUDIO"; break;
        case FMOD_OUTPUTTYPE_MAX: LOG_INFO << "[FMOD] Output Type: MAX"; break;
        default: LOG_INFO << "[FMOD] Output Type: ???";
        }
        LOG_INFO;

        int driverId;
        fmodSystem->getDriver(&driverId);
        char name[256];
        int systemRate;
        int speakerLanes;
        fmodSystem->getDriverInfo(driverId, name, 255, 0, &systemRate, 0, &speakerLanes);
        LOG_INFO << "[FMOD] Device Name: " << name;
        LOG_INFO << "[FMOD] Device Sample Rate: " << systemRate;
        LOG_INFO << "[FMOD] Device Channels: " << speakerLanes;

        unsigned bufferLen;
        int buffers;
        fmodSystem->getDSPBufferSize(&bufferLen, &buffers);
        LOG_INFO << "[FMOD] DSP Buffers: " << bufferLen << ", " << buffers;

        //setAsyncIO();
    }
}

SoundDriverFMOD::~SoundDriverFMOD()
{
    bLoading = false;
    if (tLoadSampleThread.joinable())
        tLoadSampleThread.join();

    // release before system release
    keySamplesChannelGroup.reset();
    etcSamplesChannelGroup.reset();

    if (initRet == FMOD_OK && fmodSystem != nullptr)
        fmodSystem->release();
    LOG_DEBUG << "FMOD System released.";
}

int SoundDriverFMOD::setAsyncIO(bool async)
{
    if (async)
    {
        fmodSystem->setFileSystem(
            FmodCallbackFileOpen,
            FmodCallbackFileClose,
            nullptr,
            nullptr,
            FmodCallbackAsyncRead,
            FmodCallbackAsyncReadCancel,
            -1
        );
    }
    else
    {
        // Fallback
        fmodSystem->setFileSystem(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, -1);
    }
    return 0;
}

/*
FMOD_ASYNCREADINFO parameters: 
    void *      handle;     R
    unsigned    offset;     R
    unsigned    sizebytes;  R
    int         priority;   R
    void *      buffer;     RW
    unsigned    bytesread;  RW
    FMOD_RESULT result;     RW
    void *      userdata;
*/
void SoundDriverFMOD::loadSampleThread()
{
    while (bLoading)
    {
        if (!asyncSampleLoadQueue.empty())
        {
            LOCK_QUEUE;
            auto& info = asyncSampleLoadQueue.front();
            asyncSampleLoadQueue.pop();

            FILE* pFile = (FILE*)info->handle;
            info->buffer = std::malloc(info->bytesread * sizeof(char));
            if (info->buffer == NULL) return;
            size_t sizeread = fread((void*)info->buffer, sizeof(char), (unsigned)info->bytesread, pFile);
            if (sizeread < info->bytesread)
            {
                info->bytesread = (unsigned)sizeread;
                info->done(info, FMOD_ERR_FILE_EOF);
            }
            else
            {
                info->done(info, FMOD_OK);
            }
        }
    }
}

int SoundDriverFMOD::loadKeySample(std::string path, size_t index)
{
    if (path.empty()) return 0;
	FMOD_RESULT r = FMOD_ERR_FILE_NOTFOUND;
	if (fs::exists(path) && fs::is_regular_file(path))
		r = fmodSystem->createSound(path.c_str(), FMOD_UNIQUE, 0, &keySamples[index]);

    // Also find ogg with the same filename
    if (r == FMOD_ERR_FILE_NOTFOUND && path.length() > 4 && path.substr(path.length() - 4) == ".wav")
        r = fmodSystem->createSound(path.replace(path.length() - 4, 4, ".ogg").c_str(), FMOD_UNIQUE, 0, &keySamples[index]);

    if (r != FMOD_OK)
        LOG_DEBUG << "[FMOD] Loading Sample (" + path + ") Error: " << r << ", " << FMOD_ErrorString(r);

    return 1;
}

void SoundDriverFMOD::playKeySample(size_t count, size_t index[])
{
    for (size_t i = 0; i < count; i++)
    {
        FMOD_RESULT r = FMOD_OK;
        if (keySamples[index[i]] != nullptr)
            r = fmodSystem->playSound(keySamples[index[i]], &*keySamplesChannelGroup, false, 0);
        if (r != FMOD_OK)
            LOG_WARNING << "[FMOD] Playing Sample Error: " << r << ", " << FMOD_ErrorString(r);
    }
}

void SoundDriverFMOD::stopKeySamples()
{
    keySamplesChannelGroup->stop();
}

void SoundDriverFMOD::freeKeySamples()
{
    for (auto& s : keySamples)
        if (s != nullptr)
        {
            s->release();
            s = nullptr;
        }
}

int SoundDriverFMOD::loadSample(std::string path,size_t index, bool isStream, bool loop)
{
    if (etcSamples[index] != nullptr)
        etcSamples[index]->release();

    int flag = FMOD_DEFAULT;
    flag |= isStream ? FMOD_CREATESTREAM : FMOD_CREATESAMPLE;
    flag |= loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

    FMOD_RESULT r = FMOD_ERR_FILE_NOTFOUND;
    if (fs::exists(path) && fs::is_regular_file(path))
        r = fmodSystem->createSound(path.c_str(), flag, 0, &etcSamples[index]);

    // Also find ogg with the same filename
    if (r == FMOD_ERR_FILE_NOTFOUND && path.length() > 4 && path.substr(path.length() - 4) == ".wav")
        r = fmodSystem->createSound(path.replace(path.length() - 4, 4, ".ogg").c_str(), flag, 0, &etcSamples[index]);
    
    if (r != FMOD_OK)
        LOG_WARNING << "[FMOD] Loading Sample (" << path << ") Error: " << r << ", " << FMOD_ErrorString(r);
    return r;
}

void SoundDriverFMOD::playSample(size_t index)
{
    FMOD_RESULT r = FMOD_OK;
    if (etcSamples[index] != nullptr)
        r = fmodSystem->playSound(etcSamples[index], &*etcSamplesChannelGroup, false, 0);
    if (r != FMOD_OK)
        LOG_WARNING << "[FMOD] Playing Sample Error: " << r << ", " << FMOD_ErrorString(r);
}

void SoundDriverFMOD::stopSamples()
{
    etcSamplesChannelGroup->stop();
}

void SoundDriverFMOD::freeSamples()
{
    for (auto& s : etcSamples)
        if (s != nullptr)
        {
            s->release();
            s = nullptr;
        }
}

void SoundDriverFMOD::update()
{
    if (!fmodSystem) return;
    FMOD_RESULT r = fmodSystem->update();
    if (r != FMOD_OK)
        LOG_ERROR << "[FMOD] SoundDriverFMOD System Update Error: " << r << ", " << FMOD_ErrorString(r);
}

int SoundDriverFMOD::getChannelsPlaying()
{
    int c = 0;
    fmodSystem->getChannelsPlaying(&c);
    return c;
}

void SoundDriverFMOD::setDSP(DSPType type, SampleChannel ch, int p1, int p2)
{

}