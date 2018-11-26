#pragma once
#include "config.h"

static const StringPath CONFIG_FILE_GENERAL = "config.yml";

namespace cfg {

    //////////////////////////////////////////////////////////////////////////////// 
    // Audio

	const char* A_MODE = "OutputMode";
    const char* A_MODE_NULL = "Null";
    const char* A_MODE_AUTO = "Auto";
    const char* A_MODE_WINMM = "WinMM";
    const char* A_MODE_DS = "DirectSound";
    const char* A_MODE_WASAPI_SHARED = "WASAPI";
    const char* A_MODE_ASIO = "ASIO";
    const char* A_MODE_ATMOS = "Atmos";
    const char* A_MODE_PULSE = "PulseAudio";
    const char* A_MODE_ALSA = "ALSA";
    const char* A_MODE_COREAUDIO = "CoreAudio";
    enum class eAudioMode
    {
        NOSOUND,
        AUTO,

        // Windows
        WINMM,
        DSOUND,
        WASAPI,
        ASIO,
        ATMOS,

        // Linux
        PULSEAUDIO,
        ALSA,

        // Mac
        COREAUDIO
    };

	const char* A_BUFLEN = "BufferLength";

	const char* A_BUFCOUNT = "BufferCount";

    //////////////////////////////////////////////////////////////////////////////// 
    // Video

	const char* V_RES_X = "ResolutionX";
	const char* V_RES_Y = "ResolutionY";
	const char* V_FULL_RES_X = "FullResolutionX";
	const char* V_FULL_RES_Y = "FullResolutionY";

	const char* V_WINMODE = "WindowMode";
    const char* V_WINMODE_FULL = "FullScreen";
    const char* V_WINMODE_BORDERLESS = "Borderless";
    const char* V_WINMODE_WINDOWED = "Windowed";
    enum class eWinMode
    {
        FULLSCREEN,
        BORDERLESS,
        WINDOWED
    };

	const char* V_MAXFPS = "MaxFPS";

	const char* V_VSYNC = "VSync";

}

class ConfigGeneral: public vConfig
{
public:
	ConfigGeneral();
	virtual ~ConfigGeneral();

	virtual void setDefaults() noexcept override;
};