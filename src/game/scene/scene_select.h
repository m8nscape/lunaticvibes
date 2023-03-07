#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <memory>
#include "scene.h"
#include "scene_context.h"
#include "scene_pre_select.h"

enum class eSelectState
{
    PREPARE,
    SELECT,
    SEARCH,
    PANEL1,     // play option
    FADEOUT,
};

class ChartFormatBase;
class SceneCustomize;
class SceneSelect : public vScene
{
private:
    eSelectState _state;
    InputMask _inputAvailable;
    
    // navigate input
    bool isHoldingUp = false;
    bool isHoldingDown = false;
    bool isScrollingByAxis = false;

    Time navigateTimestamp;

    // hold SELECT to enter version list
    bool isInVersionList = false;
    Time selectDownTimestamp;

    // preview
    std::shared_mutex previewMutex;
    enum
    {
        PREVIEW_NONE,
        PREVIEW_CHART,
        PREVIEW_LOAD,
        PREVIEW_LOADED,
        PREVIEW_PLAY,
        PREVIEW_FINISH,
    } previewState = PREVIEW_NONE;
    bool previewStandalone = false; // true if chart has a preview sound track
    long long previewStandaloneLength = 0;
    std::shared_ptr<ChartFormatBase> previewChart = nullptr;
    std::shared_ptr<ChartObjectBase> previewChartObj = nullptr;
    std::shared_ptr<vRuleset> previewRuleset = nullptr;
    Time previewStartTime;
    Time previewEndTime;
    std::array<size_t, 128> _bgmSampleIdxBuf{};
    std::array<size_t, 128> _keySampleIdxBuf{};

    // virtual Customize scene, customize option toggle in select scene support
    static std::shared_ptr<SceneCustomize> _virtualSceneCustomize;

    // smooth scrolling
    Time scrollButtonTimestamp;
    double scrollAccumulator = 0.0;
    double scrollAccumulatorAddUnit = 0.0;

    // F8
    std::shared_ptr<ScenePreSelect> _virtualSceneLoadSongs;
    bool refreshingSongList = false;

    // 5+7 / 6+7
    bool isHoldingK15 = false;
    bool isHoldingK16 = false;
    bool isHoldingK17 = false;
    bool isHoldingK25 = false;
    bool isHoldingK26 = false;
    bool isHoldingK27 = false;

    // 9K
    bool bindings9K = false;    // yellow buttons (2, 8) navigate, red button (5) enter, blue buttons (4, 6) back

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

    virtual void update() override;
    virtual void _updateImgui() override;

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
    void inputGameAxisSelect(double s1, double s2, const Time&);

    void inputGamePressPanel(InputMask&, const Time&);
    void inputGameHoldPanel(InputMask&, const Time&);
    void inputGameReleasePanel(InputMask&, const Time&);

private:
    void _navigateUpBy1(const Time& t);
    void _navigateDownBy1(const Time& t);
    void _navigateEnter(const Time& t);
    void _navigateBack(const Time& t, bool sound = true);
    void _decide();
    void _navigateVersionEnter(const Time& t);
    void _navigateVersionBack(const Time& t);
    bool _closeAllPanels(const Time& t);

protected:
    virtual bool checkAndStartTextEdit() override;
    void inputGamePressTextEdit(InputMask&, const Time&);
    virtual void stopTextEdit(bool modify) override;
    void resetJukeboxText();
    void searchSong(const std::string& text);

protected:
    void updatePreview();
    void postStopPreview();

/// //////////////////////////////////////////////////////
protected:
    void arenaCommand();
    void arenaHostLobby();
    void arenaLeaveLobby();
    void arenaJoinLobbyPrompt();
    void arenaJoinLobby();

/// //////////////////////////////////////////////////////

private:
    void _imguiInit();

    // imgui Dialogs
    void _imguiSampleDialog();
    void _imguiSettings();

    // pages
    void _imguiPage_ArenaDiagnose();
    void _imguiPage_Options();
    void _imguiPage_Options_General();
    void _imguiPage_Options_Jukebox();
    void _imguiPage_Options_Video();
    void _imguiPage_Options_Audio();
    void _imguiPage_Options_Play();
    void _imguiPage_Options_Select();
    void _imguiPage_About();
    void _imguiPage_Exit();

    // misc
    void _imguiRefreshProfileList();
    void _imguiRefreshLanguageList();
    void _imguiRefreshFolderList();
    void _imguiRefreshTableList();
    void _imguiRefreshVideoDisplayResolutionList();
    void _imguiCheckSettings();

    // buttons
    bool _imguiApplyPlayerName();
    bool _imguiAddFolder(const char* path = NULL);
    bool _imguiDelFolder();
    bool _imguiBrowseFolder();
    bool _imguiAddTable();
    bool _imguiDelTable();
    bool _imguiApplyResolution();
    bool _imguiRefreshAudioDevices();
    bool _imguiApplyAudioSettings();

    // etc
    bool _imguiArenaJoinLobbyPrompt();

    // imgui variables
    int imgui_main_index = 0;

    std::list<std::string> imgui_profiles;
    std::vector<const char*> imgui_profiles_display;
    int old_profile_index, imgui_profile_index;

    bool imgui_add_profile_popup = false;
    int imgui_add_profile_popup_error = 0;
    char imgui_add_profile_buf[256] = { 0 };
    bool imgui_add_profile_copy_from_current = true;

    char imgui_player_name_buf[256] = { 0 };

    std::list<std::string> imgui_languages;
    std::vector<const char*> imgui_languages_display;
    int old_language_index, imgui_language_index;

    int imgui_log_level;

    std::list<std::string> imgui_folders;
    std::vector<const char*> imgui_folders_display;
    int imgui_folder_index;

    char imgui_folder_path_buf[256] = { 0 };
    char imgui_table_url_buf[256] = { 0 };
    std::list<std::string> imgui_tables;
    std::vector<const char*> imgui_tables_display;
    int imgui_table_index;

    std::vector<std::pair<unsigned, unsigned>> imgui_video_display_resolution_size;
    std::vector<std::string> imgui_video_display_resolution;
    std::vector<const char*> imgui_video_display_resolution_display;
    int old_video_display_resolution_index, imgui_video_display_resolution_index;

    int old_video_mode, imgui_video_mode;   // 0:windowed 1:fullscreen 2:borderless
    int imgui_video_ssLevel;

    int imgui_video_vsync_index;

    int imgui_video_maxFPS;

    std::list<std::pair<int, std::string>> imgui_audio_devices;
    std::vector<std::string> imgui_audio_devices_name;
    std::vector<const char*> imgui_audio_devices_display;
    int old_audio_device_index, imgui_audio_device_index;
    bool imgui_audio_listASIODevices;
    int imgui_audio_bufferCount;
    int imgui_audio_bufferSize;

    int imgui_adv_missBGATime;
    int imgui_adv_minInputInterval;
    int imgui_play_inputPollingRate;
    int imgui_play_defaultTarget;
    int imgui_play_judgeTiming;
    bool imgui_play_lockGreenNumber;
    float imgui_play_hispeed;
    int imgui_play_greenNumber;

    int imgui_adv_scrollSpeed[2];
    int imgui_adv_newSongDuration;
    bool imgui_adv_previewDedicated;
    bool imgui_adv_previewDirect;
    int old_adv_selectKeyBindings;
    int imgui_adv_selectKeyBindings;
    bool imgui_adv_enableNewRandom;
    bool imgui_adv_enableNewGauge;
    bool imgui_adv_enableNewLaneOption;
    bool imgui_sel_onlyDisplayMainTitleOnBars;
    bool imgui_sel_disablePlaymodeAll;
    bool imgui_sel_disableDifficultyAll;
    bool imgui_sel_disablePlaymodeSingle;
    bool imgui_sel_disablePlaymodeDouble;
    bool imgui_sel_ignoreDPCharts;
    bool imgui_sel_ignore9keys;
    bool imgui_sel_ignore5keysif7keysexist;

    bool imgui_show_arenaJoinLobbyPrompt = false;
    bool imgui_arena_joinLobby = false;
    char imgui_arena_address_buf[256] = { 0 };

    bool imgui_play_adjustHispeedWithUpDown = false;
    bool imgui_play_adjustHispeedWithSelect = false;
    bool imgui_play_adjustLanecoverWithStart67 = false;
    bool imgui_play_adjustLanecoverWithMousewheel = false;
    bool imgui_play_adjustLanecoverWithLeftRight = false;
};
