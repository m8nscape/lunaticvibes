#include "common/log.h"
#include "sound_fmod_callback.h"
#include "sound_fmod.h"
#include "fmod_errors.h"
#include <cstdlib>

#include "common/utils.h"
#include "config/config_mgr.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

SoundDriverFMOD::SoundDriverFMOD(): SoundDriver(std::bind(&SoundDriverFMOD::update, this))
{
    // load device
    bool asio = false;
    int driver = -1;
#ifdef _WIN32
    if (ConfigMgr::get('A', cfg::A_MODE, cfg::A_MODE_AUTO) == cfg::A_MODE_ASIO)
    {
        asio = true;
    }
#endif
    auto devList = getDeviceList(asio);
    auto devName = ConfigMgr::get('A', cfg::A_DEVNAME, "");
    if (!devName.empty())
    {
        for (size_t i = 0; i < devList.size(); ++i)
        {
            if (devList[i].second == devName)
            {
                if (devList[i].first == -1)
                {
                    driver = findDriver(devList[i].second, asio);
                }
                else
                {
                    driver = devList[i].first;
                }
                break;
            }
        }
        if (driver < 0)
        {
            LOG_WARNING << "[FMOD] Driver not found: " << devName;
            asio = false;
        }
    }

    initRet = FMOD::System_Create(&fmodSystem);      // Create the main system object.
    if (initRet != FMOD_OK)
    {
        LOG_ERROR << "[FMOD] Create FMOD System Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
        return;
    }

    if (asio)
    {
        fmodSystem->setOutput(FMOD_OUTPUTTYPE_ASIO);
    }
    else
    {
        fmodSystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
    }

    // This is mandatory for a keysounded rhythm game. 64 is obviously not enough
    fmodSystem->setSoftwareChannels(512);

    if (driver > 0)
    {
#ifdef _WIN32
        [&]()
        {
            __try
            {
#endif
                initRet = fmodSystem->setDriver(driver);     // this call may throw 0xc0000008 when asio device is in use. It's ok to just continue
#ifdef _WIN32
            }
            __except (EXCEPTION_INVALID_HANDLE)
            {
                initRet = FMOD_ERR_INVALID_HANDLE;
            }
        }();
#endif
        if (initRet != FMOD_OK)
        {
            LOG_ERROR << "[FMOD] Set driver failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
            return;
        }
    }

    // set DSP size
    fmodSystem->setDSPBufferSize(
        ConfigMgr::get('A', cfg::A_BUFLEN, 128),
        ConfigMgr::get('A', cfg::A_BUFCOUNT, 2)
    );

#ifdef _WIN32
    [&]()
    {
        __try
        {
#endif
            initRet = fmodSystem->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD; same as above about SEH
#ifdef _WIN32
        }
        __except (EXCEPTION_INVALID_HANDLE)
        {
            initRet = FMOD_ERR_INVALID_HANDLE;
        }
    }();
#endif

    if (initRet != FMOD_OK)
    {
        LOG_ERROR << "[FMOD] FMOD System Initialize Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);

        LOG_ERROR << "[FMOD] Fallback to default device";
        fmodSystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
        fmodSystem->setDriver(0);

        initRet = fmodSystem->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
        if (initRet != FMOD_OK)
        {
            LOG_ERROR << "[FMOD] FMOD System Initialize Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
            return;
        }
    }

    if (initRet == FMOD_OK)
    {
        LOG_DEBUG << "[FMOD] FMOD System Initialize Finished.";

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

    volume[SampleChannel::MASTER] = 1.0f;
    volume[SampleChannel::KEY] = 1.0f;
    volume[SampleChannel::BGM] = 1.0f;

    createChannelGroups();
}

void SoundDriverFMOD::createChannelGroups()
{
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        FMOD::ChannelGroup* pcg = nullptr;
        char name[16];
        sprintf(name, "CHG_%d", (int)e);
        initRet = fmodSystem->createChannelGroup(name, &pcg);
        if (initRet != FMOD_OK)
        {
            LOG_ERROR << "Create channel group " << (int)e << " Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
            return;
        }
        channelGroup[e] = std::shared_ptr<FMOD::ChannelGroup>(pcg, [](FMOD::ChannelGroup* p) { p->release(); });
    }

    for (int c = 0; c < 3; ++c)
    {
        // create dummy dsp
        for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
        {
            fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPMaster[c][e]);
            DSPMaster[c][e]->setBypass(true);
        }
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPBgm[c][SoundChannelType::BGM_NOTE]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPBgm[c][SoundChannelType::BGM_SYS]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPKey[c][SoundChannelType::KEY_SYS]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPKey[c][SoundChannelType::KEY_LEFT]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPKey[c][SoundChannelType::KEY_RIGHT]);
        DSPBgm[c][SoundChannelType::BGM_NOTE]->setBypass(true);
        DSPBgm[c][SoundChannelType::BGM_SYS]->setBypass(true);
        DSPKey[c][SoundChannelType::KEY_SYS]->setBypass(true);
        DSPKey[c][SoundChannelType::KEY_LEFT]->setBypass(true);
        DSPKey[c][SoundChannelType::KEY_RIGHT]->setBypass(true);

        channelGroup[SoundChannelType::BGM_NOTE]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::BGM_NOTE]);
        channelGroup[SoundChannelType::BGM_NOTE]->addDSP(c * 2 + 1, DSPBgm[c][SoundChannelType::BGM_NOTE]);
        channelGroup[SoundChannelType::BGM_SYS]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::BGM_SYS]);
        channelGroup[SoundChannelType::BGM_SYS]->addDSP(c * 2 + 1, DSPBgm[c][SoundChannelType::BGM_SYS]);
        channelGroup[SoundChannelType::KEY_SYS]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::KEY_SYS]);
        channelGroup[SoundChannelType::KEY_SYS]->addDSP(c * 2 + 1, DSPKey[c][SoundChannelType::KEY_SYS]);
        channelGroup[SoundChannelType::KEY_LEFT]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::KEY_LEFT]);
        channelGroup[SoundChannelType::KEY_LEFT]->addDSP(c * 2 + 1, DSPKey[c][SoundChannelType::KEY_LEFT]);
        channelGroup[SoundChannelType::KEY_RIGHT]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::KEY_RIGHT]);
        channelGroup[SoundChannelType::KEY_RIGHT]->addDSP(c * 2 + 1, DSPKey[c][SoundChannelType::KEY_RIGHT]);
    }

    // create PITCHSHIFT dsp
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &PitchShiftFilter[e]);
        PitchShiftFilter[e]->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 512.f);
        channelGroup[e]->addDSP(6, PitchShiftFilter[e]);
    }

    // create MULTIBAND_EQ dsp
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &EQFilter[0][e]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &EQFilter[1][e]);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_B_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_C_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_D_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_E_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, 62.5f);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_B_FREQUENCY, 160.f);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_C_FREQUENCY, 400.f);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_D_FREQUENCY, 1000.f);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_E_FREQUENCY, 2500.f);
        EQFilter[1][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[1][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_B_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[1][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_C_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[1][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_D_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[1][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, 1000.f);
        EQFilter[1][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_B_FREQUENCY, 2500.f);
        EQFilter[1][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_C_FREQUENCY, 6250.f);
        EQFilter[1][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_D_FREQUENCY, 16000.f);
        channelGroup[e]->addDSP(7, EQFilter[0][e]);
        channelGroup[e]->addDSP(8, EQFilter[1][e]);
    }

    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        FMOD::ChannelGroup* pcg = nullptr;
        char name[16];
        sprintf(name, "CHG_%d", (int)e);
        initRet = fmodSystem->createChannelGroup(name, &pcg);
        if (initRet != FMOD_OK)
        {
            LOG_ERROR << "Create channel group " << (int)e << " Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
            return;
        }
        channelGroup[e] = std::shared_ptr<FMOD::ChannelGroup>(pcg, [](FMOD::ChannelGroup* p) { p->release(); });
    }

    for (int c = 0; c < 3; ++c)
    {
        // create dummy dsp
        for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
        {
            fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPMaster[c][e]);
            DSPMaster[c][e]->setBypass(true);
        }
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPBgm[c][SoundChannelType::BGM_NOTE]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPBgm[c][SoundChannelType::BGM_SYS]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPKey[c][SoundChannelType::KEY_SYS]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPKey[c][SoundChannelType::KEY_LEFT]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MIXER, &DSPKey[c][SoundChannelType::KEY_RIGHT]);
        DSPBgm[c][SoundChannelType::BGM_NOTE]->setBypass(true);
        DSPBgm[c][SoundChannelType::BGM_SYS]->setBypass(true);
        DSPKey[c][SoundChannelType::KEY_SYS]->setBypass(true);
        DSPKey[c][SoundChannelType::KEY_LEFT]->setBypass(true);
        DSPKey[c][SoundChannelType::KEY_RIGHT]->setBypass(true);

        channelGroup[SoundChannelType::BGM_NOTE]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::BGM_NOTE]);
        channelGroup[SoundChannelType::BGM_NOTE]->addDSP(c * 2 + 1, DSPBgm[c][SoundChannelType::BGM_NOTE]);
        channelGroup[SoundChannelType::BGM_SYS]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::BGM_SYS]);
        channelGroup[SoundChannelType::BGM_SYS]->addDSP(c * 2 + 1, DSPBgm[c][SoundChannelType::BGM_SYS]);
        channelGroup[SoundChannelType::KEY_SYS]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::KEY_SYS]);
        channelGroup[SoundChannelType::KEY_SYS]->addDSP(c * 2 + 1, DSPKey[c][SoundChannelType::KEY_SYS]);
        channelGroup[SoundChannelType::KEY_LEFT]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::KEY_LEFT]);
        channelGroup[SoundChannelType::KEY_LEFT]->addDSP(c * 2 + 1, DSPKey[c][SoundChannelType::KEY_LEFT]);
        channelGroup[SoundChannelType::KEY_RIGHT]->addDSP(c * 2 + 0, DSPMaster[c][SoundChannelType::KEY_RIGHT]);
        channelGroup[SoundChannelType::KEY_RIGHT]->addDSP(c * 2 + 1, DSPKey[c][SoundChannelType::KEY_RIGHT]);
    }

    // create PITCHSHIFT dsp
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &PitchShiftFilter[e]);
        PitchShiftFilter[e]->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 512.f);
        channelGroup[e]->addDSP(6, PitchShiftFilter[e]);
    }

    // create MULTIBAND_EQ dsp
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &EQFilter[0][e]);
        fmodSystem->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &EQFilter[1][e]);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_B_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_C_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_D_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_E_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, 62.5f);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_B_FREQUENCY, 160.f);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_C_FREQUENCY, 400.f);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_D_FREQUENCY, 1000.f);
        EQFilter[0][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_E_FREQUENCY, 2500.f);
        EQFilter[1][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_A_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[1][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_B_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[1][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_C_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[1][e]->setParameterInt(FMOD_DSP_MULTIBAND_EQ_D_FILTER, FMOD_DSP_MULTIBAND_EQ_FILTER_PEAKING);
        EQFilter[1][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, 1000.f);
        EQFilter[1][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_B_FREQUENCY, 2500.f);
        EQFilter[1][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_C_FREQUENCY, 6250.f);
        EQFilter[1][e]->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_D_FREQUENCY, 16000.f);
        channelGroup[e]->addDSP(7, EQFilter[0][e]);
        channelGroup[e]->addDSP(8, EQFilter[1][e]);
    }

}

SoundDriverFMOD::~SoundDriverFMOD()
{
    bLoading = false;
    if (tLoadSampleThread.joinable())
        tLoadSampleThread.join();

    // release before system release
    freeNoteSamples();
    freeSysSamples();
    channelGroup.clear();

    if (initRet == FMOD_OK && fmodSystem != nullptr)
        fmodSystem->release();

    LOG_DEBUG << "FMOD System released.";
}

std::vector<std::string> getDeviceListASIO()
{
    std::vector<std::string> devList;

#ifdef _WIN32

    HKEY hkASIO;
    if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\ASIO", 0, KEY_READ, &hkASIO))
    {
        return devList;
    }

    WCHAR subKeyName[MAX_PATH] = { 0 };
    LRESULT ret;
    DWORD idx = 0;
    for (;;)
    {
        ret = RegEnumKeyW(hkASIO, idx++, subKeyName, MAX_PATH);
        if (ret == ERROR_MORE_DATA) continue;
        if (ret == ERROR_NO_MORE_ITEMS) break;
        if (ret != ERROR_SUCCESS) return devList;

        WCHAR desc[MAX_PATH] = { 0 };
        DWORD descLength = MAX_PATH;
        DWORD dwType = 0;
        while (ERROR_MORE_DATA == (ret = RegGetValueW(hkASIO, subKeyName, L"Description", RRF_RT_REG_SZ, &dwType, desc, &descLength)));

        DWORD dwNum;
        dwNum = WideCharToMultiByte(CP_UTF8, NULL, desc, descLength, NULL, 0, NULL, FALSE);
        char ustr[MAX_PATH * 4] = { 0 };
        WideCharToMultiByte(CP_UTF8, NULL, desc, descLength, ustr, dwNum, NULL, FALSE);

        if (ret == ERROR_SUCCESS)
        {
            devList.push_back(ustr);
        }
    }
    RegCloseKey(hkASIO);

#endif

    return devList;
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
            /*
#ifdef _WIN32
            // ASIO
            f->setOutput(FMOD_OUTPUTTYPE_ASIO);
            f->getNumDrivers(&numDrivers); // this call may throw 0xc0000008 when asio device is in use. Also affected by /EHsc flag automatically added by Ninja
            for (int i = 0; i < numDrivers; ++i)
            {
                if (FMOD_OK == f->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels))
                {
                    res.push_back(std::make_pair(0x40000000 | i, std::string(name)));
                }
            }
#endif
            */
            size_t idx = 0;
            for (auto& devName : getDeviceListASIO())
            {
                res.push_back(std::make_pair(-1, std::string(devName)));
            }
        }

        f->release();
        return res;
    }

    return {};
}

int SoundDriverFMOD::findDriver(const std::string& driverName, bool asio)
{
    FMOD::System* f = nullptr;
    int numDrivers = 0;
    if (FMOD_OK == FMOD::System_Create(&f))
    {
        std::vector<std::pair<int, std::string>> res;

        int driver = -1;

        char name[512] = { 0 };
        FMOD_GUID guid;
        int systemrate;
        FMOD_SPEAKERMODE speakermode;
        int speakermodechannels;

        if (asio)
        {
#ifdef _WIN32
            // ASIO
            assert(fmodSystem == nullptr); // app must not using ASIO device when finding ASIO drivers
            f->setOutput(FMOD_OUTPUTTYPE_ASIO);
            [&]()
            {
                __try
                {
                    f->getNumDrivers(&numDrivers); // this call may throw 0xc0000008 when asio device is in use. It's ok to just continue
                }
                __except (EXCEPTION_INVALID_HANDLE)
                {
                }
            }();
            for (int i = 0; i < numDrivers; ++i)
            {
                if (FMOD_OK == f->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels) &&
                    driverName == name)
                {
                    driver = i;
                    break;
                }
            }
#endif
        }
        else
        {
            // Auto
            f->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
            f->getNumDrivers(&numDrivers);
            for (int i = 0; i < numDrivers; ++i)
            {
                if (FMOD_OK == f->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels) &&
                    driverName == name)
                {
                    driver = i;
                    break;
                }
            }
        }

        f->release();
        return driver;
    }

    return -1;
}

int SoundDriverFMOD::setDevice(size_t index, bool asio)
{
    FMOD::System* pOldSystem = fmodSystem;
    fmodSystem = nullptr;

    // release old system
    bLoading = false;
    if (tLoadSampleThread.joinable())
        tLoadSampleThread.join();

    freeNoteSamples();
    freeSysSamples();
    channelGroup.clear();

    if (pOldSystem != nullptr)
    {
        pOldSystem->release();
    }

    FMOD::System* pSystem = nullptr;
    initRet = FMOD::System_Create(&pSystem);      // Create the main system object.
    if (initRet != FMOD_OK)
    {
        LOG_ERROR << "Create FMOD System Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet);
        return 1;
    }

    auto devList = getDeviceList(true);
    if (index < devList.size())
    {
        if (devList[index].first < 0)
        {
            int driver = findDriver(devList[index].second, true);
            if (driver >= 0)
            {
                asio = true;
                index = driver;

                if (FMOD_RESULT res = pSystem->setOutput(FMOD_OUTPUTTYPE_ASIO); res != FMOD_OK)
                {
                    LOG_WARNING << "[FMOD] Set output type to ASIO failed: " << FMOD_ErrorString((FMOD_RESULT)res);
                    return 1;
                }

                if (FMOD_RESULT res = pSystem->setDriver(index); res != FMOD_OK)
                {
                    LOG_WARNING << "[FMOD] Set driver failed: " << FMOD_ErrorString((FMOD_RESULT)res);
                    return 1;
                }
            }
        }
    }

    if (FMOD_RESULT res = pSystem->setDSPBufferSize(
            ConfigMgr::get('A', cfg::A_BUFLEN, 128),
            ConfigMgr::get('A', cfg::A_BUFCOUNT, 2)
        ); res != FMOD_OK)
    {
        LOG_WARNING << "[FMOD] Set DSP buffer size failed: " << FMOD_ErrorString((FMOD_RESULT)res);
        return 1;
    }

    if (FMOD_RESULT res = pSystem->init(512, FMOD_INIT_NORMAL, 0); res != FMOD_OK)
    {
        LOG_ERROR << "FMOD System Initialize Failed: " << FMOD_ErrorString((FMOD_RESULT)res);
        return 1;
    }

    // Recreate samples
    for (auto& s : sysSamples)
    {
        if (!s.path.empty())
        {
            pSystem->createSound(s.path.c_str(), s.flags, 0, &s.objptr);
        }
    }
    for (auto& s : noteSamples)
    {
        if (!s.path.empty())
        {
            pSystem->createSound(s.path.c_str(), s.flags, 0, &s.objptr);
        }
    }

    {
        LOG_DEBUG << "FMOD System Initialize Finished.";
        fmodSystem = pSystem;

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
    }

    createChannelGroups();

    setVolume(SampleChannel::MASTER, volume[SampleChannel::MASTER]);
    setVolume(SampleChannel::KEY, volume[SampleChannel::KEY]);
    setVolume(SampleChannel::BGM, volume[SampleChannel::BGM]);

    return 0;
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

int SoundDriverFMOD::loadNoteSample(const Path& spath, size_t index)
{
    if (spath.empty()) return -1;

    if (noteSamples[index].objptr != nullptr)
    {
        noteSamples[index].objptr->release();
        noteSamples[index].objptr = nullptr;
    }
    
    std::string path = spath.u8string();
    int flags = FMOD_LOOP_OFF | FMOD_UNIQUE | FMOD_CREATESAMPLE;

	FMOD_RESULT r = FMOD_ERR_FILE_NOTFOUND;
	if (fs::exists(spath) && fs::is_regular_file(spath))
		r = fmodSystem->createSound(path.c_str(), flags, 0, &noteSamples[index].objptr);

    // Also find ogg with the same filename
    if (r == FMOD_ERR_FILE_NOTFOUND && strEqual(spath.extension().string(), ".wav", true))
    {
        path = path.replace(path.length() - 4, 4, ".ogg");
        r = fmodSystem->createSound(path.c_str(), flags, 0, &noteSamples[index].objptr);
    }

    if (r == FMOD_OK)
    {
        noteSamples[index].path = path;
        noteSamples[index].flags = flags;
    }
    else
    {
        LOG_DEBUG << "[FMOD] Loading Sample (" + path + ") Error: " << r << ", " << FMOD_ErrorString(r);
    }

    return (r == FMOD_OK) ? 0 : 1;
}

void SoundDriverFMOD::playNoteSample(SoundChannelType ch, size_t count, size_t index[])
{
    for (size_t i = 0; i < count; i++)
    {
        FMOD_RESULT r = FMOD_OK;
        if (noteSamples[index[i]].objptr != nullptr)
            r = fmodSystem->playSound(noteSamples[index[i]].objptr, &*channelGroup[ch], false, 0);
        if (r != FMOD_OK)
            LOG_WARNING << "[FMOD] Playing Sample Error: " << r << ", " << FMOD_ErrorString(r);
    }
}

void SoundDriverFMOD::stopNoteSamples()
{
    channelGroup[SoundChannelType::BGM_NOTE]->stop();
    channelGroup[SoundChannelType::KEY_LEFT]->stop();
    channelGroup[SoundChannelType::KEY_RIGHT]->stop();
}

void SoundDriverFMOD::freeNoteSamples()
{
    for (auto& s : noteSamples)
    {
        if (s.objptr != nullptr)
        {
            s.objptr->release();
            s.objptr = nullptr;
        }
    }
}

long long SoundDriverFMOD::getNoteSampleLength(size_t index)
{
    if (noteSamples[index].objptr == nullptr) return 0;
    auto sample = noteSamples[index].objptr;

    unsigned length = 0;
    sample->getLength(&length, FMOD_TIMEUNIT_MS);
    return length;
}

int SoundDriverFMOD::loadSysSample(const Path& spath, size_t index, bool isStream, bool loop)
{
    if (spath.empty()) return -1;
    
    if (sysSamples[index].objptr != nullptr)
    {
        sysSamples[index].objptr->release();
        sysSamples[index].objptr = nullptr;
    }

    std::string path = spath.u8string();

    int flags = FMOD_DEFAULT | FMOD_UNIQUE;
    flags |= isStream ? FMOD_CREATESTREAM : FMOD_CREATESAMPLE;
    flags |= loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

    FMOD_RESULT r = FMOD_ERR_FILE_NOTFOUND;
    if (fs::exists(spath) && fs::is_regular_file(spath))
        r = fmodSystem->createSound(path.c_str(), flags, 0, &sysSamples[index].objptr);

    // Also find ogg with the same filename
    if (r == FMOD_ERR_FILE_NOTFOUND && strEqual(spath.extension().string(), ".wav", true))
    {
        path = path.replace(path.length() - 4, 4, ".ogg");
        r = fmodSystem->createSound(path.c_str(), flags, 0, &sysSamples[index].objptr);
    }

    if (r == FMOD_OK)
    {
        sysSamples[index].path = path;
        sysSamples[index].flags = flags;
    }
    else
    {
        LOG_DEBUG << "[FMOD] Loading Sample (" + path + ") Error: " << r << ", " << FMOD_ErrorString(r);
    }

    return (r == FMOD_OK) ? 0 : 1;
}

void SoundDriverFMOD::playSysSample(SoundChannelType ch, size_t index)
{
    FMOD_RESULT r = FMOD_OK;
    if (sysSamples[index].objptr != nullptr)
        r = fmodSystem->playSound(sysSamples[index].objptr, &*channelGroup[ch], false, 0);
    if (r != FMOD_OK)
        LOG_WARNING << "[FMOD] Playing Sample Error: " << r << ", " << FMOD_ErrorString(r);
}

void SoundDriverFMOD::stopSysSamples()
{
    channelGroup[SoundChannelType::BGM_SYS]->stop();
    channelGroup[SoundChannelType::KEY_SYS]->stop();
}

void SoundDriverFMOD::freeSysSamples()
{
    for (auto& s : sysSamples)
    {
        if (s.objptr != nullptr)
        {
            s.objptr->release();
            s.objptr = nullptr;
        }
    }
}

void SoundDriverFMOD::update()
{
    if (!fmodSystem) return;

    if (sysVolume != sysVolumeGradientEnd)
    {
        if (sysVolumeGradientLength == 0)
        {
            sysVolume = sysVolumeGradientEnd;
        }
        else
        {
            double progress = double((Time() - sysVolumeGradientBeginTime).norm()) / sysVolumeGradientLength;
            if (progress >= 1.0)
            {
                sysVolume = sysVolumeGradientEnd;
            }
            else
            {
                sysVolume = sysVolumeGradientBegin + (sysVolumeGradientEnd - sysVolumeGradientBegin) * progress;
            }
        }
        setVolume(SampleChannel::BGM, volume[SampleChannel::BGM]);
    }

    if (noteVolume != noteVolumeGradientEnd)
    {
        if (noteVolumeGradientLength == 0)
        {
            noteVolume = noteVolumeGradientEnd;
        }
        else
        {
            double progress = double((Time() - noteVolumeGradientBeginTime).norm()) / noteVolumeGradientLength;
            if (progress >= 1.0)
            {
                noteVolume = noteVolumeGradientEnd;
            }
            else
            {
                noteVolume = noteVolumeGradientBegin + (noteVolumeGradientEnd - noteVolumeGradientBegin) * progress;
            }
        }
        setVolume(SampleChannel::MASTER, volume[SampleChannel::MASTER]);
    }

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

void SoundDriverFMOD::setSysVolume(float v, int gradientTime)
{
    sysVolumeGradientBegin = sysVolume;
    sysVolumeGradientEnd = v;
    sysVolumeGradientBeginTime = Time();
    sysVolumeGradientLength = gradientTime;
}

void SoundDriverFMOD::setNoteVolume(float v, int gradientTime)
{
    noteVolumeGradientBegin = noteVolume;
    noteVolumeGradientEnd = v;
    noteVolumeGradientBeginTime = Time();
    noteVolumeGradientLength = gradientTime;
}

void SoundDriverFMOD::setVolume(SampleChannel ch, float v)
{
    volume[ch] = v;

    switch (ch)
    {
    case SampleChannel::MASTER:
        channelGroup[SoundChannelType::BGM_SYS]->setVolume(sysVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::BGM]);
        channelGroup[SoundChannelType::BGM_NOTE]->setVolume(noteVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::BGM]);
        channelGroup[SoundChannelType::KEY_SYS]->setVolume(sysVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::KEY]);
        channelGroup[SoundChannelType::KEY_LEFT]->setVolume(noteVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::KEY]);
        channelGroup[SoundChannelType::KEY_RIGHT]->setVolume(noteVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::KEY]);
        break;
    case SampleChannel::KEY:
        channelGroup[SoundChannelType::KEY_SYS]->setVolume(sysVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::KEY]);
        channelGroup[SoundChannelType::KEY_LEFT]->setVolume(noteVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::KEY]);
        channelGroup[SoundChannelType::KEY_RIGHT]->setVolume(noteVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::KEY]);
        break;
    case SampleChannel::BGM:
        channelGroup[SoundChannelType::BGM_SYS]->setVolume(sysVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::BGM]);
        channelGroup[SoundChannelType::BGM_NOTE]->setVolume(noteVolume * volume[SampleChannel::MASTER] * volume[SampleChannel::BGM]);
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
        for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
        {
            DSPMaster[dspIndex][e]->setBypass(true);
        }
        DSPBgm[dspIndex][SoundChannelType::BGM_NOTE]->setBypass(true);
        DSPBgm[dspIndex][SoundChannelType::BGM_SYS]->setBypass(true);
        DSPKey[dspIndex][SoundChannelType::KEY_SYS]->setBypass(true);
        DSPKey[dspIndex][SoundChannelType::KEY_LEFT]->setBypass(true);
        DSPKey[dspIndex][SoundChannelType::KEY_RIGHT]->setBypass(true);
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
            DSPBgm[dspIndex][SoundChannelType::BGM_NOTE]->setBypass(true);
            DSPBgm[dspIndex][SoundChannelType::BGM_SYS]->setBypass(true);
            DSPKey[dspIndex][SoundChannelType::KEY_SYS]->setBypass(true);
            DSPKey[dspIndex][SoundChannelType::KEY_LEFT]->setBypass(true);
            DSPKey[dspIndex][SoundChannelType::KEY_RIGHT]->setBypass(true);
            for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
            {
                updateDSP(channelGroup[e], DSPMaster[dspIndex][e]);
            }
            break;
        case SampleChannel::KEY:
            for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
            {
                DSPMaster[dspIndex][e]->setBypass(true);
            }
            DSPBgm[dspIndex][SoundChannelType::BGM_NOTE]->setBypass(true);
            DSPBgm[dspIndex][SoundChannelType::BGM_SYS]->setBypass(true);
            updateDSP(channelGroup[SoundChannelType::KEY_SYS], DSPKey[dspIndex][SoundChannelType::KEY_SYS]);
            updateDSP(channelGroup[SoundChannelType::KEY_LEFT], DSPKey[dspIndex][SoundChannelType::KEY_LEFT]);
            updateDSP(channelGroup[SoundChannelType::KEY_RIGHT], DSPKey[dspIndex][SoundChannelType::KEY_RIGHT]);
            break;
        case SampleChannel::BGM:
            for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
            {
                DSPMaster[dspIndex][e]->setBypass(true);
            }
            DSPKey[dspIndex][SoundChannelType::KEY_SYS]->setBypass(true);
            DSPKey[dspIndex][SoundChannelType::KEY_LEFT]->setBypass(true);
            DSPKey[dspIndex][SoundChannelType::KEY_RIGHT]->setBypass(true);
            updateDSP(channelGroup[SoundChannelType::BGM_SYS], DSPBgm[dspIndex][SoundChannelType::BGM_SYS]);
            updateDSP(channelGroup[SoundChannelType::BGM_NOTE], DSPBgm[dspIndex][SoundChannelType::BGM_NOTE]);
            break;
        }
    }
}

void SoundDriverFMOD::setFreqFactor(double f)
{
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        channelGroup[e]->setPitch(f);
        PitchShiftFilter[e]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 1.0f);
        PitchShiftFilter[e]->setBypass(true);
    }
}

void SoundDriverFMOD::setSpeed(double speed)
{
    double pitch = 1.0 / speed;
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        channelGroup[e]->setPitch(speed);
        PitchShiftFilter[e]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
        PitchShiftFilter[e]->setBypass(false);
    }
}

void SoundDriverFMOD::setPitch(double pitch)
{
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        channelGroup[e]->setPitch(1.0);
        PitchShiftFilter[e]->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
        PitchShiftFilter[e]->setBypass(false);
    }
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
    for (SoundChannelType e = SoundChannelType::BGM_SYS; e != SoundChannelType::TYPE_COUNT; ++(*(int*)(&e)))
    {
        EQFilter[i][e]->setParameterFloat(ch, (float)gain);
    }
}