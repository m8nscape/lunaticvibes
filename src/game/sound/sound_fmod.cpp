#include "common/log.h"
#include "sound_fmod_callback.h"
#include "sound_fmod.h"
#include "fmod_errors.h"
#include <cstdlib>

#include "common/utils.h"
#include "config/config_mgr.h"

SoundDriverFMOD::SoundDriverFMOD()
{
    initRet = FMOD::System_Create(&fmodSystem);      // Create the main system object.
    if (initRet != FMOD_OK)
    {
        LOG_ERROR << "Create FMOD System Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
        return;
    }

    fmodSystem->setDSPBufferSize(
        ConfigMgr::get('A', cfg::A_BUFLEN, 128),
        ConfigMgr::get('A', cfg::A_BUFCOUNT, 2)
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

    volume[SampleChannel::MASTER] = 1.0f;
    volume[SampleChannel::KEY] = 1.0f;
    volume[SampleChannel::BGM] = 1.0f;

    for (int c = 0; c < 3; ++c)
    {
        for (int i = 0; i < 4; ++i)
        {
            // create dummy dsp
            fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSP[c][i]);
            DSP[c][i]->setBypass(true);
        }
        keySamplesChannelGroup->addDSP(c * 2 + 0, DSP[c][0]);
        etcSamplesChannelGroup->addDSP(c * 2 + 0, DSP[c][1]);
        keySamplesChannelGroup->addDSP(c * 2 + 1, DSP[c][2]);
        etcSamplesChannelGroup->addDSP(c * 2 + 1, DSP[c][3]);
    }

    // create PITCHSHIFT dsp
    fmodSystem->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &PitchShiftFilter[0]);
    fmodSystem->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &PitchShiftFilter[1]);
    PitchShiftFilter[0]->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 512.f);
    PitchShiftFilter[1]->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 512.f);
    keySamplesChannelGroup->addDSP(6, PitchShiftFilter[0]);
    etcSamplesChannelGroup->addDSP(6, PitchShiftFilter[1]);

    // create MULTIBAND_EQ dsp
    for (int i = 0; i < 2; ++i)
    {
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &EQFilter[i][0]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &EQFilter[i][1]);

        EQFilter[i][0]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][0]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_B_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][0]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_C_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][0]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_D_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][0]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_E_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][0]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, 62.5f);
        EQFilter[i][0]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_B_FREQUENCY, 160.f);
        EQFilter[i][0]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_C_FREQUENCY, 400.f);
        EQFilter[i][0]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_D_FREQUENCY, 1000.f);
        EQFilter[i][0]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_E_FREQUENCY, 2500.f);

        EQFilter[i][1]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][1]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_B_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][1]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_C_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][1]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_D_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[i][1]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, 1000.f);
        EQFilter[i][1]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_B_FREQUENCY, 2500.f);
        EQFilter[i][1]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_C_FREQUENCY, 6250.f);
        EQFilter[i][1]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_D_FREQUENCY, 16000.f);
    }
    keySamplesChannelGroup->addDSP(7, EQFilter[0][0]);
    keySamplesChannelGroup->addDSP(8, EQFilter[0][1]);
    etcSamplesChannelGroup->addDSP(7, EQFilter[1][0]);
    etcSamplesChannelGroup->addDSP(8, EQFilter[1][1]);

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
        case FMOD_OUTPUTTYPE_WASAPI: LOG_INFO << "[FMOD] Output Type: WASAPI"; break;
        case FMOD_OUTPUTTYPE_ASIO: LOG_INFO << "[FMOD] Output Type: ASIO"; break;
        case FMOD_OUTPUTTYPE_PULSEAUDIO: LOG_INFO << "[FMOD] Output Type: PULSEAUDIO"; break;
        case FMOD_OUTPUTTYPE_ALSA: LOG_INFO << "[FMOD] Output Type: ALSA"; break;
        case FMOD_OUTPUTTYPE_COREAUDIO: LOG_INFO << "[FMOD] Output Type: COREAUDIO"; break;
        case FMOD_OUTPUTTYPE_AUDIOTRACK: LOG_INFO << "[FMOD] Output Type: AUDIOTRACK"; break;
        case FMOD_OUTPUTTYPE_OPENSL: LOG_INFO << "[FMOD] Output Type: OPENSL"; break;
        case FMOD_OUTPUTTYPE_AUDIOOUT: LOG_INFO << "[FMOD] Output Type: AUDIOOUT"; break;
        case FMOD_OUTPUTTYPE_AUDIO3D: LOG_INFO << "[FMOD] Output Type: AUDIO3D"; break;
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

std::vector<std::pair<int, std::string>> SoundDriverFMOD::getDeviceList(bool asio)
{
    FMOD::System* f = nullptr;
    int numDrivers = 0;
    if (FMOD_OK == FMOD::System_Create(&f))
    {
        std::vector<std::pair<int, std::string>> res;

        char name[512];
        FMOD_GUID guid;
        int systemrate;
        FMOD_SPEAKERMODE speakermode;
        int speakermodechannels;

        // Auto
        f->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
        f->getNumDrivers(&numDrivers);
        for (int i = 0; i < numDrivers; ++i)
        {
            if (FMOD_OK == f->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels))
            {
                res.push_back(std::make_pair(i, std::string(name)));
            }
        }

        if (asio)
        {
#ifdef WIN32
            // ASIO
            f->setOutput(FMOD_OUTPUTTYPE_ASIO);
            f->getNumDrivers(&numDrivers); // this call may throw 0xc0000008 when asio device is in use. Also affected by /EHsc flag automatically added by Ninja
            for (int i = 0; i < numDrivers; ++i)
            {
                if (FMOD_OK == f->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels))
                {
                    std::string namefmt = "[ASIO] "s + name;
                    res.push_back(std::make_pair(i, namefmt));
                }
            }
#endif
        }

        f->release();
        return res;
    }

    return {};
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

int SoundDriverFMOD::loadKeySample(const Path& spath, size_t index)
{
    if (spath.empty()) return 0;
    
    std::string path = spath.u8string();

	FMOD_RESULT r = FMOD_ERR_FILE_NOTFOUND;
	if (fs::exists(spath) && fs::is_regular_file(spath))
		r = fmodSystem->createSound(path.c_str(), FMOD_LOOP_OFF | FMOD_UNIQUE, 0, &keySamples[index]);

    // Also find ogg with the same filename
    if (r == FMOD_ERR_FILE_NOTFOUND && strEqual(spath.extension().string(), ".wav", true))
        r = fmodSystem->createSound(path.replace(path.length() - 4, 4, ".ogg").c_str(), FMOD_LOOP_OFF | FMOD_UNIQUE, 0, &keySamples[index]);

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

int SoundDriverFMOD::loadSample(const Path& spath, size_t index, bool isStream, bool loop)
{
    if (spath.empty()) return 0;
    
    if (etcSamples[index] != nullptr)
        etcSamples[index]->release();

    std::string path = spath.u8string();

    int flag = FMOD_DEFAULT;
    flag |= isStream ? FMOD_CREATESTREAM : FMOD_CREATESAMPLE;
    flag |= loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

    FMOD_RESULT r = FMOD_ERR_FILE_NOTFOUND;
    if (fs::exists(spath) && fs::is_regular_file(spath))
        r = fmodSystem->createSound(path.c_str(), flag, 0, &etcSamples[index]);

    // Also find ogg with the same filename
    if (r == FMOD_ERR_FILE_NOTFOUND && strEqual(spath.extension().string(), ".wav", true))
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

void SoundDriverFMOD::setVolume(SampleChannel ch, float v)
{
    volume[ch] = v;

    switch (ch)
    {
    case SampleChannel::MASTER:
        keySamplesChannelGroup->setVolume(volume[SampleChannel::MASTER] * volume[SampleChannel::KEY]);
        etcSamplesChannelGroup->setVolume(volume[SampleChannel::MASTER] * volume[SampleChannel::BGM]);
        break;
    case SampleChannel::KEY:
        keySamplesChannelGroup->setVolume(volume[SampleChannel::MASTER] * volume[SampleChannel::KEY]);
        break;
    case SampleChannel::BGM:
        etcSamplesChannelGroup->setVolume(volume[SampleChannel::MASTER] * volume[SampleChannel::BGM]);
        break;
    }
}

void SoundDriverFMOD::setDSP(DSPType type, int dspIndex, SampleChannel ch, float p1, float p2)
{
    FMOD_DSP_TYPE fmodType = FMOD_DSP_TYPE_UNKNOWN;
    switch (type)
	{
	case DSPType::REVERB:     fmodType = FMOD_DSP_TYPE_SFXREVERB; break;
	case DSPType::DELAY:      fmodType = FMOD_DSP_TYPE_ECHO; break;
	case DSPType::LOWPASS:    fmodType = FMOD_DSP_TYPE_LOWPASS; break;
	case DSPType::HIGHPASS:   fmodType = FMOD_DSP_TYPE_HIGHPASS; break;
	case DSPType::FLANGER:    fmodType = FMOD_DSP_TYPE_FLANGE; break;
	case DSPType::CHORUS:     fmodType = FMOD_DSP_TYPE_CHORUS; break;
	case DSPType::DISTORTION: fmodType = FMOD_DSP_TYPE_DISTORTION; break;
    }

    if (fmodType == FMOD_DSP_TYPE_UNKNOWN)
    {
        for (int c = 0; c < 4; ++c)
        {
            DSP[dspIndex][c]->setBypass(true);
        }
    }
    else
    {
        auto updateDSP = [&](std::shared_ptr<FMOD::ChannelGroup>& cg, FMOD::DSP*& dsp)
        {
            FMOD_DSP_TYPE typeOld = FMOD_DSP_TYPE_UNKNOWN;
            int dspi = 4;
            if (cg->getDSPIndex(dsp, &dspi) != FMOD_OK || dsp->getType(&typeOld) != FMOD_OK || typeOld != fmodType)
            {
                cg->removeDSP(dsp);
                dsp->release();
                fmodSystem->createDSPByType(fmodType, &dsp);
                cg->addDSP(dspi, dsp);
            }
            if (dsp != nullptr)
            {
                // How DSP tweaked by LR2 is unknown. These parameters are just assumptions.
                switch (fmodType)
                {
                case FMOD_DSP_TYPE_SFXREVERB:
                    dsp->setParameterFloat(FMOD_DSP_SFXREVERB_EARLYLATEMIX, float(p1 * 100.0));
                    dsp->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, float(p2 * (20.0 - (-10.0)) + (-10.0)));
                    break;

                case FMOD_DSP_TYPE_ECHO:
                    dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, float(p1 * (100.0 - 20.0) + 20.0));
                    dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, float(p2 * 50.0));
                    break;

                case FMOD_DSP_TYPE_LOWPASS:
                    dsp->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, float(p1 * (22000.0 - 500.0) + 500.0));
                    dsp->setParameterFloat(FMOD_DSP_LOWPASS_RESONANCE, float(p2 * (10.0 - 1.0) + 1.0));
                    break;

                case FMOD_DSP_TYPE_HIGHPASS:
                    dsp->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, float(p1 * (5050.0 - 500.0) + 500.0));
                    dsp->setParameterFloat(FMOD_DSP_HIGHPASS_RESONANCE, float(p2 * (10.0 - 1.0) + 1.0));
                    break;

                case FMOD_DSP_TYPE_FLANGE:
                    dsp->setParameterFloat(FMOD_DSP_FLANGE_DEPTH, float(p1 * (1.0 - 0.01) + 0.01));
                    dsp->setParameterFloat(FMOD_DSP_FLANGE_RATE, float(p2 * 20.0));
                    break;

                case FMOD_DSP_TYPE_CHORUS:
                    dsp->setParameterFloat(FMOD_DSP_CHORUS_RATE, float(p1 * 20.0));
                    dsp->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, float(p2 * 100.0));
                    break;

                case FMOD_DSP_TYPE_DISTORTION:
                    dsp->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, float(p1 * 1.0));
                    break;
                }
                dsp->setBypass(false);
            }
        };

        switch (ch)
        {
        case SampleChannel::MASTER:
            DSP[dspIndex][2]->setBypass(true);
            DSP[dspIndex][3]->setBypass(true);
            updateDSP(keySamplesChannelGroup, DSP[dspIndex][0]);
            updateDSP(etcSamplesChannelGroup, DSP[dspIndex][1]);
            break;
        case SampleChannel::KEY:
            DSP[dspIndex][0]->setBypass(true);
            DSP[dspIndex][1]->setBypass(true);
            DSP[dspIndex][3]->setBypass(true);
            updateDSP(keySamplesChannelGroup, DSP[dspIndex][2]);
            break;
        case SampleChannel::BGM:
            DSP[dspIndex][0]->setBypass(true);
            DSP[dspIndex][1]->setBypass(true);
            DSP[dspIndex][2]->setBypass(true);
            updateDSP(etcSamplesChannelGroup, DSP[dspIndex][3]);
            break;
        }
    }
}

void SoundDriverFMOD::setFreqFactor(double f)
{
    keySamplesChannelGroup->setPitch(f);
    etcSamplesChannelGroup->setPitch(f);
    PitchShiftFilter[0]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 1.0f);
    PitchShiftFilter[1]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 1.0f);
    PitchShiftFilter[0]->setBypass(true);
    PitchShiftFilter[1]->setBypass(true);
}

void SoundDriverFMOD::setSpeed(double speed)
{
    double pitch = 1.0 / speed;
    keySamplesChannelGroup->setPitch(speed);
    etcSamplesChannelGroup->setPitch(speed);
    PitchShiftFilter[0]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
    PitchShiftFilter[1]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
    PitchShiftFilter[0]->setBypass(false);
    PitchShiftFilter[1]->setBypass(false);
}

void SoundDriverFMOD::setPitch(double pitch)
{
    keySamplesChannelGroup->setPitch(1.0);
    etcSamplesChannelGroup->setPitch(1.0);
    PitchShiftFilter[0]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
    PitchShiftFilter[1]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
    PitchShiftFilter[0]->setBypass(false);
    PitchShiftFilter[1]->setBypass(false);
}

void SoundDriverFMOD::setEQ(EQFreq freq, int gain)
{
    int i = 0, ch = 0;
    switch (freq)
    {
    case EQFreq::_62_5: i = 0; ch = FMOD_DSP_MULTIBAND_EQ_A_GAIN; break;
    case EQFreq::_160:  i = 0; ch = FMOD_DSP_MULTIBAND_EQ_B_GAIN; break;
    case EQFreq::_400:  i = 0; ch = FMOD_DSP_MULTIBAND_EQ_C_GAIN; break;
    case EQFreq::_1000: i = 0; ch = FMOD_DSP_MULTIBAND_EQ_D_GAIN; break;
    case EQFreq::_2500: i = 1; ch = FMOD_DSP_MULTIBAND_EQ_B_GAIN; break;
    case EQFreq::_6250: i = 1; ch = FMOD_DSP_MULTIBAND_EQ_C_GAIN; break;
    case EQFreq::_16k:  i = 1; ch = FMOD_DSP_MULTIBAND_EQ_D_GAIN; break;
    }
    EQFilter[0][i]->setParameterFloat(ch, (float)gain);
    EQFilter[1][i]->setParameterFloat(ch, (float)gain);
}