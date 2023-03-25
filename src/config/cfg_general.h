#pragma once
#include "config.h"

constexpr char CONFIG_FILE_GENERAL[] = "config.yml";

namespace cfg {

    //////////////////////////////////////////////////////////////////////////////// 
    // Audio

	constexpr char A_MODE[] = "AudioOutputMode";
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

    constexpr char A_DEVNAME[] = "AudioDeviceName";

	constexpr char A_BUFLEN[] = "BufferLength";

	constexpr char A_BUFCOUNT[] = "BufferCount";

    //////////////////////////////////////////////////////////////////////////////// 
    // Video

    constexpr char V_RES_SUPERSAMPLE[] = "SuperSampleMultiplier";
    constexpr char V_DISPLAY_RES_X[] = "DisplayResolutionX";
    constexpr char V_DISPLAY_RES_Y[] = "DisplayResolutionY";

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
    constexpr char E_LR2PATH[] = "LR2Path";
    constexpr char E_FOLDERS[] = "Folders";
    constexpr char E_TABLES[] = "Tables";

    constexpr char E_LOG_LEVEL[] = "LogLevel";
    constexpr char E_LOG_LEVEL_DEBUG[] = "Debug";
    constexpr char E_LOG_LEVEL_INFO[] = "Info";
    constexpr char E_LOG_LEVEL_WARNING[] = "Warning";
    constexpr char E_LOG_LEVEL_ERROR[] = "Error";

    constexpr char PROFILE_DEFAULT[] = "default";

}

class ConfigGeneral: public vConfig
{
public:
    ConfigGeneral(const char* file) : vConfig(file) {}
    virtual ~ConfigGeneral() = default;

	virtual void setDefaults() noexcept override;

    void setFolders(const std::vector<std::string>& path);
    void setTables(const std::vector<std::string>& urls);
    std::vector<StringPath> getFoldersPath();
    std::vector<std::string> getFoldersStr();
    std::vector<std::string> getTablesUrl();
};