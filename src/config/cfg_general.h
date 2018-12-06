#pragma once
#include "config.h"

inline const StringPath CONFIG_FILE_GENERAL = "config.yml";

namespace cfg {

    //////////////////////////////////////////////////////////////////////////////// 
    // Audio

	inline const char* A_MODE = "OutputMode";
    inline const char* A_MODE_NULL = "Null";
    inline const char* A_MODE_AUTO = "Auto";
    inline const char* A_MODE_WINMM = "WinMM";
    inline const char* A_MODE_DS = "DirectSound";
    inline const char* A_MODE_WASAPI_SHARED = "WASAPI";
    inline const char* A_MODE_ASIO = "ASIO";
    inline const char* A_MODE_ATMOS = "Atmos";
    inline const char* A_MODE_PULSE = "PulseAudio";
    inline const char* A_MODE_ALSA = "ALSA";
    inline const char* A_MODE_COREAUDIO = "CoreAudio";
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

	inline const char* A_BUFLEN = "BufferLength";

	inline const char* A_BUFCOUNT = "BufferCount";

    //////////////////////////////////////////////////////////////////////////////// 
    // Video

	inline const char* V_RES_X = "ResolutionX";
	inline const char* V_RES_Y = "ResolutionY";
	inline const char* V_FULL_RES_X = "FullResolutionX";
	inline const char* V_FULL_RES_Y = "FullResolutionY";

	inline const char* V_WINMODE = "WindowMode";
    inline const char* V_WINMODE_FULL = "FullScreen";
    inline const char* V_WINMODE_BORDERLESS = "Borderless";
    inline const char* V_WINMODE_WINDOWED = "Windowed";
    enum class eWinMode
    {
        FULLSCREEN,
        BORDERLESS,
        WINDOWED
    };

	inline const char* V_MAXFPS = "MaxFPS";

	inline const char* V_VSYNC = "VSync";

}

class ConfigGeneral: public vConfig
{
public:
	ConfigGeneral();
	virtual ~ConfigGeneral();

	virtual void setDefaults() noexcept override;
};