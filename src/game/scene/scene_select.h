#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <memory>
#include "scene.h"
#include "scene_context.h"

enum class eSelectState
{
    PREPARE,
    SELECT,
    SEARCH,
    PANEL1,     // play option
    FADEOUT,
};

class vChartFormat;
class SceneSelect : public vScene
{
private:
    eSelectState _state;
    InputMask _inputAvailable;
    
    // navigate input
    bool isHoldingUp = false;
    bool isHoldingDown = false;
    bool isScrollingByAxis = false;
    bool isInVersionList = false;
    Time scrollTimestamp = -1;
    Time selectDownTimestamp = -1;

    // imgui
    bool imguiShow = false;

public:
    SceneSelect();
    virtual ~SceneSelect();

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    std::function<void()> _updateCallback;
    void updatePrepare();
    void updateSelect();
    void updateSearch();
    void updatePanel(unsigned idx);
    void updateFadeout();

protected:
    // Inner-state updates

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const Time&);
    void inputGameHold(InputMask&, const Time&);
    void inputGameRelease(InputMask&, const Time&);

private:
    void inputGamePressSelect(InputMask&, const Time&);
    void inputGameHoldSelect(InputMask&, const Time&);
    void inputGameReleaseSelect(InputMask&, const Time&);
    void inputGameAxisSelect(InputAxisPlus&, const Time&);

    void inputGamePressPanel(InputMask&, const Time&);
    void inputGameHoldPanel(InputMask&, const Time&);
    void inputGameReleasePanel(InputMask&, const Time&);

private:
    void loadSongList();
    void _navigateUpBy1(const Time& t);
    void _navigateDownBy1(const Time& t);
    void _navigateEnter(const Time& t);
    void _navigateBack(const Time& t);
    void _decide();
    void _navigateSongEnter(const Time& t);
    void _navigateSongBack(const Time& t);
    bool _closeAllPanels(const Time& t);

private:
    void _imguiInit();

    // imgui Dialogs
    void _imguiSampleDialog();
    void _imguiSettings();

    // misc
    void _imguiRefreshProfileList();
    void _imguiRefreshFolderList();
    void _imguiRefreshVideoResolutionList();
    void _imguiRefreshVideoDisplayResolutionList();
    void _imguiCheckSettings();

    // buttons
    bool _imguiAddFolder();
    bool _imguiDelFolder();
    bool _imguiBrowseFolder();
    bool _imguiApplyResolution();
    bool _imguiRefreshAudioDevices();

    // imgui variables
    std::list<std::string> imgui_profiles;
    std::vector<const char*> imgui_profiles_display;
    int old_profile_index, imgui_profile_index;

    std::list<std::string> imgui_folders;
    std::vector<const char*> imgui_folders_display;
    int imgui_folder_index;

    std::list<std::string> imgui_video_resolution;
    std::vector<const char*> imgui_video_resolution_display;
    int old_video_resolution_index, imgui_video_resolution_index;   // 0:480p 1:720p 2:1080p

    std::vector<std::pair<unsigned, unsigned>> imgui_video_display_resolution_size;
    std::vector<std::string> imgui_video_display_resolution;
    std::vector<const char*> imgui_video_display_resolution_display;
    int old_video_display_resolution_index, imgui_video_display_resolution_index;

    int old_video_mode, imgui_video_mode;   // 0:windowed 1:fullscreen 2:borderless
    bool imgui_video_vsync;
    int imgui_video_maxFPS;

    std::list<std::pair<int, std::string>> imgui_audio_devices;
    std::vector<const char*> imgui_audio_devices_display;
    int old_audio_device_index, imgui_audio_device_index;
    bool imgui_audio_checkASIODevices;
    int imgui_audio_bufferCount;
    int imgui_audio_bufferSize;

    int imgui_adv_scrollSpeed[2];
    int imgui_adv_missBGATime;
    int imgui_adv_minInputInterval;
    int imgui_adv_newSongDuration;
    bool imgui_adv_mouseAnalog;
};
