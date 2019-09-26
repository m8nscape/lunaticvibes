#pragma once
#include "config.h"

inline const StringPath CONFIG_FILE_GENERAL = "config.yml";

namespace cfg {

    //////////////////////////////////////////////////////////////////////////////// 
    // Audio

	inline const StringContent A_MODE = "OutputMode";
    inline const StringContent A_MODE_NULL = "Null";
    inline const StringContent A_MODE_AUTO = "Auto";
    inline const StringContent A_MODE_WINMM = "WinMM";
    inline const StringContent A_MODE_DS = "DirectSound";
    inline const StringContent A_MODE_WASAPI_SHARED = "WASAPI";
    inline const StringContent A_MODE_ASIO = "ASIO";
    inline const StringContent A_MODE_ATMOS = "Atmos";
    inline const StringContent A_MODE_PULSE = "PulseAudio";
    inline const StringContent A_MODE_ALSA = "ALSA";
    inline const StringContent A_MODE_COREAUDIO = "CoreAudio";
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

	inline const StringContent A_BUFLEN = "BufferLength";

	inline const StringContent A_BUFCOUNT = "BufferCount";

    //////////////////////////////////////////////////////////////////////////////// 
    // Video

	inline const StringContent V_RES_X = "ResolutionX";
	inline const StringContent V_RES_Y = "ResolutionY";
	inline const StringContent V_FULL_RES_X = "FullResolutionX";
	inline const StringContent V_FULL_RES_Y = "FullResolutionY";

	inline const StringContent V_WINMODE = "WindowMode";
    inline const StringContent V_WINMODE_FULL = "FullScreen";
    inline const StringContent V_WINMODE_BORDERLESS = "Borderless";
    inline const StringContent V_WINMODE_WINDOWED = "Windowed";
    enum class eWinMode
    {
        FULLSCREEN,
        BORDERLESS,
        WINDOWED
    };

	inline const StringContent V_MAXFPS = "MaxFPS";

	inline const StringContent V_VSYNC = "VSync";

}

class ConfigGeneral: public vConfig
{
public:
	ConfigGeneral();
	virtual ~ConfigGeneral();

	virtual void setDefaults() noexcept override;
};