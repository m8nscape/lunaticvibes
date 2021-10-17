#pragma once
#include "config.h"

constexpr char CONFIG_FILE_GENERAL[] = "config.yml";

namespace cfg {

    //////////////////////////////////////////////////////////////////////////////// 
    // Audio

	constexpr char A_MODE[] = "OutputMode";
    constexpr char A_MODE_NULL[] = "Null";
    constexpr char A_MODE_AUTO[] = "Auto";
    constexpr char A_MODE_WINMM[] = "WinMM";
    constexpr char A_MODE_DS[] = "DirectSound";
    constexpr char A_MODE_WASAPI_SHARED[] = "WASAPI";
    constexpr char A_MODE_ASIO[] = "ASIO";
    constexpr char A_MODE_ATMOS[] = "Atmos";
    constexpr char A_MODE_PULSE[] = "PulseAudio";
    constexpr char A_MODE_ALSA[] = "ALSA";
    constexpr char A_MODE_COREAUDIO[] = "CoreAudio";
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

	constexpr char A_BUFLEN[] = "BufferLength";

	constexpr char A_BUFCOUNT[] = "BufferCount";

    //////////////////////////////////////////////////////////////////////////////// 
    // Video

	constexpr char V_RES_X[] = "ResolutionX";
	constexpr char V_RES_Y[] = "ResolutionY";

	constexpr char V_WINMODE[] = "WindowMode";
    constexpr char V_WINMODE_FULL[] = "FullScreen";
    constexpr char V_WINMODE_BORDERLESS[] = "Borderless";
    constexpr char V_WINMODE_WINDOWED[] = "Windowed";
    enum class eWinMode
    {
        FULLSCREEN,
        BORDERLESS,
        WINDOWED
    };

	constexpr char V_MAXFPS[] = "MaxFPS";

	constexpr char V_VSYNC[] = "VSync";

    //////////////////////////////////////////////////////////////////////////////// 
    // etc
    constexpr char E_PROFILE[] = "Profile";
}

class ConfigGeneral: public vConfig
{
public:
    ConfigGeneral(const std::string& profile) : vConfig(profile, CONFIG_FILE_GENERAL) {}
    virtual ~ConfigGeneral() = default;

	virtual void setDefaults() noexcept override;
};