#include "scene_select.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/runtime/i18n.h"
#include "imgui.h"
#include "git_version.h"

#ifdef _WIN32
#include <shellapi.h>
#endif

void SceneSelect::_imguiSampleDialog()
{
    if (imguiShow)
    {
        ImGui::ShowDemoWindow(NULL);
    }
}

// from imgui_demo.cpp
// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void SceneSelect::_imguiInit()
{
    _imguiRefreshProfileList();
    old_profile_index = imgui_profile_index;

    std::string playerName = ConfigMgr::get('P', cfg::P_PLAYERNAME, "Unnamed");
    strncpy(imgui_player_name_buf, playerName.c_str(), std::max(sizeof(imgui_player_name_buf) - 1, playerName.length()));

    _imguiRefreshLanguageList();
    old_language_index = imgui_language_index;

    _imguiRefreshFolderList();
    _imguiRefreshTableList();

    imgui_video_ssLevel = ConfigMgr::get("V", cfg::V_RES_SUPERSAMPLE, 1);

    _imguiRefreshVideoDisplayResolutionList();
    auto windowY = ConfigMgr::get("V", cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT);
    imgui_video_display_resolution_index = -1;
    for (int i = 0; i < (int)imgui_video_display_resolution_size.size(); ++i)
    {
        if (imgui_video_display_resolution_size[i].second == windowY)
        {
            imgui_video_display_resolution_index = i;
            break;
        }
    }
    if (imgui_video_display_resolution_index < 0)
    {
        imgui_video_display_resolution_index = 0;
    }
    old_video_display_resolution_index = imgui_video_display_resolution_index;

    auto winMode = ConfigMgr::get("V", cfg::V_WINMODE, cfg::V_WINMODE_WINDOWED);
    if (winMode == cfg::V_WINMODE_FULL)
        imgui_video_mode = 1;
    else if (winMode == cfg::V_WINMODE_BORDERLESS)
        imgui_video_mode = 2;
    else
        imgui_video_mode = 0;
    old_video_mode = imgui_video_mode;

    imgui_video_vsync_index = ConfigMgr::get("V", cfg::V_VSYNC, 0);
#if _WIN32
    if (imgui_video_vsync_index >= 2)
        imgui_video_vsync_index = 1;
#endif

    imgui_video_maxFPS = ConfigMgr::get("V", cfg::V_MAXFPS, 240);

    imgui_audio_bufferCount = ConfigMgr::get("A", cfg::A_BUFCOUNT, 2);
    imgui_audio_bufferSize = ConfigMgr::get("A", cfg::A_BUFLEN, 256);
    _imguiRefreshAudioDevices();
    old_audio_device_index = imgui_audio_device_index;

    imgui_adv_scrollSpeed[0] = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
    imgui_adv_scrollSpeed[1] = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
    imgui_adv_missBGATime = ConfigMgr::get("P", cfg::P_MISSBGA_LENGTH, 500);
    imgui_adv_minInputInterval = ConfigMgr::get("P", cfg::P_MIN_INPUT_INTERVAL, 16);
    imgui_adv_newSongDuration = ConfigMgr::get("P", cfg::P_NEW_SONG_DURATION, 24);

    imgui_adv_previewDedicated = ConfigMgr::get("P", cfg::P_PREVIEW_DEDICATED, true);
    imgui_adv_previewDirect = ConfigMgr::get("P", cfg::P_PREVIEW_DIRECT, true);

    imgui_adv_selectKeyBindings = old_adv_selectKeyBindings = 0;
    static const char* imgui_select_keybindings_str[] =
    {
        cfg::P_SELECT_KEYBINDINGS_7K,
        cfg::P_SELECT_KEYBINDINGS_5K,
        cfg::P_SELECT_KEYBINDINGS_9K,
    };
    for (size_t i = 0; i < sizeof(imgui_select_keybindings_str) / sizeof(imgui_select_keybindings_str[0]); ++i)
    {
        if (ConfigMgr::get('P', cfg::P_SELECT_KEYBINDINGS, cfg::P_SELECT_KEYBINDINGS_7K) == imgui_select_keybindings_str[i])
        {
            imgui_adv_selectKeyBindings = old_adv_selectKeyBindings = i;
            break;
        }
    }

    imgui_adv_enableNewRandom = ConfigMgr::get("P", cfg::P_ENABLE_NEW_RANDOM, false);
    imgui_adv_enableNewGauge = ConfigMgr::get("P", cfg::P_ENABLE_NEW_GAUGE, false);
    imgui_adv_enableNewLaneOption = ConfigMgr::get("P", cfg::P_ENABLE_NEW_LANE_OPTION, false);

    // auto popup settings for first runs
    if (imgui_folders.empty())
    {
        imguiShow = true;
        _skin->setHandleMouseEvents(false);
    }
}

#ifdef WIN32
#include <ShlObj.h>
#include <ShlObj_core.h>
#endif
void SceneSelect::_imguiSettings()
{
    assert(IsMainThread());
    if (gNextScene != eScene::SELECT) return;

    using namespace i18nText;

    if (imguiShow)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ConfigMgr::get('V', cfg::V_DISPLAY_RES_X, CANVAS_WIDTH), ConfigMgr::get('V', cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT)), ImGuiCond_Always);
        if (ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse))
        {
            if (ImGui::BeginTable("##main", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
            {
                static const float mainTagWidth = 120.f;

                ImGui::TableSetupColumn("##menu", 0, mainTagWidth);
                ImGui::TableSetupColumn("##page", 0, ImGui::GetWindowWidth() - mainTagWidth);

                enum MenuPage
                {
                    MENU_NONE = 0,

                    MENU_KEYCONFIG,
                    MENU_OPTIONS,
                    MENU__________,
                    MENU_ABOUT,
                    MENU_EXIT,
                    MENU_COUNT
                };

                if (ImGui::TableNextColumn())
                {
                    float height = 40.0f;
                    float interval = 5.0f;

                    ImGui::Dummy( { mainTagWidth, ImGui::GetWindowHeight() - (height + interval) * MENU_COUNT - 20.0f });

                    if (ImGui::Button(i18n::c(MAIN_KEYCONFIG), {-1.f, height}))
                    {
                        imgui_main_index = MENU_KEYCONFIG;
                    }
                    ImGui::Dummy({ 0.f, interval });

                    if (ImGui::Button(i18n::c(MAIN_SETTINGS), {-1.f, height}))
                    {
                        imgui_main_index = MENU_OPTIONS;
                    }
                    ImGui::Dummy({ 0.f, interval });

                    if (ImGui::InvisibleButton("##dummy123123123", { -1.f, height }))
                    {
                    }
                    ImGui::Dummy({ 0.f, interval });

                    if (ImGui::Button(i18n::c(MAIN_ABOUT), {-1.f, height}))
                    {
                        imgui_main_index = MENU_ABOUT;
                    }
                    ImGui::Dummy({ 0.f, interval });

                    if (ImGui::Button(i18n::c(MAIN_EXIT), {-1.f, height}))
                    {
                        imgui_main_index = MENU_EXIT;
                    }
                    ImGui::Dummy({ 0.f, 20.0f });

                    if (ImGui::TableNextColumn())
                    {
                        if (imgui_main_index == MENU_KEYCONFIG && ImGui::BeginTabBar("##keyconfig", ImGuiTabBarFlags_FittingPolicyScroll))
                        {
                            _imguiPage_KeyConfig();
                            ImGui::EndTabBar();
                        }

                        if (imgui_main_index == MENU_OPTIONS && ImGui::BeginTabBar("##option", ImGuiTabBarFlags_FittingPolicyScroll))
                        {
                            _imguiPage_Options();
                            ImGui::EndTabBar();
                        }

                        if (imgui_main_index == MENU_ABOUT && ImGui::BeginTabBar("##about", ImGuiTabBarFlags_FittingPolicyScroll))
                        {
                            _imguiPage_About();
                            ImGui::EndTabBar();
                        }

                        if (imgui_main_index == MENU_EXIT)
                        {
                            _imguiPage_Exit();
                        }
                    }
                }
                ImGui::EndTable();
            }

            if (imgui_add_profile_popup)
            {
                ImGui::OpenPopup(i18n::c(NEW_PROFILE));

                ImGui::SetNextWindowSize(ImVec2(480.f, 180.f), ImGuiCond_Always);
                if (ImGui::BeginPopupModal(i18n::c(NEW_PROFILE), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
                {
                    static std::string errorMessage;
                    int old_imgui_add_profile_popup_error = imgui_add_profile_popup_error;
                    bool ok = false;

                    ImGui::Text(i18n::c(NEW_PROFILE_NAME));
                    ImGui::SameLine();
                    ok |= ImGui::InputText("##newprofilename", imgui_add_profile_buf, sizeof(imgui_add_profile_buf), ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    HelpMarker(i18n::c(NEW_PROFILE_NAME_HINT));

                    ImGui::Checkbox(i18n::c(NEW_PROFILE_COPY_FROM_CURRENT), &imgui_add_profile_copy_from_current);

                    ok |= ImGui::Button(i18n::c(OK));
                    if (ok)
                    {
                        if (strnlen(imgui_add_profile_buf, sizeof(imgui_add_profile_buf)) == 0)
                        {
                            imgui_add_profile_popup_error = 1;
                        }
                        else
                        {
                            if (ConfigMgr::createProfile(imgui_add_profile_buf, 
                                imgui_add_profile_copy_from_current ? ConfigMgr::get('E', cfg::E_PROFILE, cfg::PROFILE_DEFAULT) : "") == 0)
                            {
                                memset(imgui_add_profile_buf, 0, sizeof(imgui_add_profile_buf));
                                ImGui::CloseCurrentPopup();
                                _imguiRefreshProfileList();

                                imgui_add_profile_popup = false;
                                imgui_add_profile_popup_error = 0;
                                
                            }
                            else
                            {
                                imgui_add_profile_popup_error = 2;
                            }
                        }
                    }

                    ImGui::SameLine();
                    if (ImGui::Button(i18n::c(CANCEL)))
                    {
                        memset(imgui_add_profile_buf, 0, sizeof(imgui_add_profile_buf));
                        ImGui::CloseCurrentPopup();
                        imgui_add_profile_popup = false;
                        imgui_add_profile_popup_error = 0;
                    }

                    if (old_imgui_add_profile_popup_error != imgui_add_profile_popup_error)
                    {
                        errorMessage.clear();
                    }
                    if (imgui_add_profile_popup_error == 1)
                    {
                        if (errorMessage.empty()) 
                            errorMessage = i18n::c(NEW_PROFILE_EMPTY);
                    }
                    else if (imgui_add_profile_popup_error == 2)
                    {
                        if (errorMessage.empty())
                            errorMessage = (boost::format(i18n::c(NEW_PROFILE_DUPLICATE)) % imgui_add_profile_buf).str();
                    }
                    ImGui::TextColored({ 1.f, 0.2f, 0.2f, 1.f }, errorMessage.c_str());

                    ImGui::EndPopup();
                }
            }
        }
        ImGui::End();
    }

    _imguiCheckSettings();
}

static const float tabItemWidth = 120.f;
static const float infoRowWidth = 240.f;

void SceneSelect::_imguiPage_KeyConfig()
{
    using namespace i18nText;
    ImGui::Text(i18n::c(TODO));
}

void SceneSelect::_imguiPage_Options()
{
    using namespace i18nText;

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_GENERAL), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        _imguiPage_Options_General();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_JUKEBOX), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        _imguiPage_Options_Jukebox();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_VIDEO), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        _imguiPage_Options_Video();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_AUDIO), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        _imguiPage_Options_Audio();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_PLAY), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        _imguiPage_Options_Play();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_ADVANCED), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        _imguiPage_Options_Advanced();
        ImGui::EndTabItem();
    }
}

void SceneSelect::_imguiPage_Options_General()
{
    using namespace i18nText;

    if (ImGui::BeginChild("##pagesub11"))
    {
        ImGui::Text(i18n::c(PROFILE));
        ImGui::SameLine(infoRowWidth);
        ImGui::Combo("##profile", &imgui_profile_index, imgui_profiles_display.data(), (int)imgui_profiles_display.size());

        if (ImGui::Button(i18n::c(ADD_MORE)))
        {
            imgui_add_profile_popup = true;
        }

        ImGui::Dummy({});
        ImGui::Separator();

        /////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Text(i18n::c(PLAYER_NAME));
        ImGui::SameLine(infoRowWidth);
        bool modifiedPlayName = false;
        modifiedPlayName |= ImGui::InputText("##playername", imgui_player_name_buf, sizeof(imgui_player_name_buf), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();
        modifiedPlayName |= ImGui::Button((i18n::s(APPLY) + "##applyname").c_str());
        if (modifiedPlayName)
        {
            _imguiApplyPlayerName();
        }

        ImGui::Text(i18n::c(LANGUAGE));
        ImGui::SameLine(infoRowWidth);
        ImGui::Combo("##language", &imgui_language_index, imgui_languages_display.data(), (int)imgui_languages_display.size());

        ImGui::EndChild();
    }
}

void SceneSelect::_imguiPage_Options_Jukebox()
{
    using namespace i18nText;

    if (ImGui::BeginChild("##pagesub12"))
    {
        ImGui::Text(i18n::c(JUKEBOX_REFRESH_HINT));

        ImGui::Dummy({});
        ImGui::Separator();

        /////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Text(i18n::c(JUKEBOX_FOLDER));
        {
            bool enterPath = ImGui::InputText("##NewPath", imgui_folder_path_buf, sizeof(imgui_folder_path_buf), ImGuiInputTextFlags_EnterReturnsTrue);
            if (enterPath || (ImGui::SameLine(), ImGui::Button(" + ")))
            {
                _imguiAddFolder(imgui_folder_path_buf);
                memset(imgui_folder_path_buf, 0, sizeof(imgui_folder_path_buf));
            }

            ImGui::ListBox("##Paths", &imgui_folder_index, imgui_folders_display.data(), imgui_folders_display.size());

            if (ImGui::Button(i18n::c(ADD_MORE)))
            {
                _imguiAddFolder();
            }
            ImGui::SameLine();
            if (ImGui::Button((i18n::s(JUKEBOX_DELETE_SELECTED) + "##delfolder").c_str()))
            {
                _imguiDelFolder();
            }
            ImGui::SameLine();
            if (ImGui::Button(i18n::c(JUKEBOX_BROWSE_SELECTED)))
            {
                _imguiBrowseFolder();
            }
        }

        ImGui::Dummy({});
        ImGui::Separator();

        /////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Text(i18n::c(JUKEBOX_TABLES));
        {

            bool enterUrl = ImGui::InputText("##NewURL", imgui_table_url_buf, sizeof(imgui_table_url_buf), ImGuiInputTextFlags_EnterReturnsTrue);
            if (enterUrl || (ImGui::SameLine(), ImGui::Button(" + ")))
            {
                _imguiAddTable();
                memset(imgui_table_url_buf, 0, sizeof(imgui_table_url_buf));
            }

            ImGui::ListBox("##URLs", &imgui_table_index, imgui_tables_display.data(), imgui_tables_display.size());

            if (ImGui::Button((i18n::s(JUKEBOX_DELETE_SELECTED) + "##deltable").c_str()))
            {
                _imguiDelTable();
            }
        }
        ImGui::EndChild();
    }
}

void SceneSelect::_imguiPage_Options_Video()
{
    using namespace i18nText;

    if (ImGui::BeginChild("##pagesub13"))
    {
        ImGui::Text(i18n::c(VIDEO_RESOLUTION));
        ImGui::SameLine(infoRowWidth);
        ImGui::Combo("##resolution", &imgui_video_display_resolution_index, imgui_video_display_resolution_display.data(), (int)imgui_video_display_resolution_display.size());

        ImGui::Text(i18n::c(VIDEO_SS_LEVEL));
        ImGui::SameLine(infoRowWidth);
        static const char* imgui_video_ss_display[] =
        {
            "1x",
            "2x",
            "3x",
        };
        int ssLevel = imgui_video_ssLevel - 1;
        if (ImGui::Combo("##sslevel", &ssLevel, imgui_video_ss_display, sizeof(imgui_video_ss_display) / sizeof(char*)))
        {
            imgui_video_ssLevel = ssLevel + 1;
        }
        ImGui::SameLine();
        HelpMarker(i18n::c(VIDEO_SS_LEVEL_HINT));

        ImGui::Text(i18n::c(VIDEO_SCREEN_MODE));
        ImGui::SameLine(infoRowWidth);
        const char* imgui_video_mode_display[] =
        {
            i18n::c(VIDEO_WINDOWED),
            i18n::c(VIDEO_FULLSCREEN),
            i18n::c(VIDEO_BORDERLESS)
        };
        ImGui::Combo("##window", &imgui_video_mode, imgui_video_mode_display, sizeof(imgui_video_mode_display) / sizeof(char*));

        ImGui::Text(i18n::c(VIDEO_VSYNC));
        ImGui::SameLine(infoRowWidth);
        const char* imgui_vsync_mode_display[] =
        {
            i18n::c(OFF),
            i18n::c(ON),
#if _WIN32
#else
            i18n::c(VIDEO_ADAPTIVE)
#endif
        };
        ImGui::Combo("##vsync", &imgui_video_vsync_index, imgui_vsync_mode_display, sizeof(imgui_vsync_mode_display) / sizeof(char*));

        ImGui::Text(i18n::c(VIDEO_MAXFPS));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt("##maxfps", &imgui_video_maxFPS, 0);

        if (ImGui::Button(i18n::c(APPLY), { 80.f, 0.f }))
        {
            _imguiApplyResolution();

            if (imgui_video_maxFPS < 30 && imgui_video_maxFPS != 0)
            {
                imgui_video_maxFPS = 30;
            }
            if (imgui_video_maxFPS != ConfigMgr::get("V", cfg::V_MAXFPS, infoRowWidth))
            {
                ConfigMgr::set("V", cfg::V_MAXFPS, imgui_video_maxFPS);
                graphics_set_maxfps(imgui_video_maxFPS);
            }
        }
        ImGui::EndChild();
    }
}

void SceneSelect::_imguiPage_Options_Audio()
{
    using namespace i18nText;

    if (ImGui::BeginChild("##pagesub14"))
    {
        ImGui::Text(i18n::c(AUDIO_DEVICE));
        ImGui::SameLine(infoRowWidth);
        ImGui::Combo("##audiodevice", &imgui_audio_device_index, imgui_audio_devices_display.data(), (int)imgui_audio_devices_display.size());

        if (ImGui::Button(i18n::c(AUDIO_REFRESH_DEVICE_LIST)))
        {
            _imguiRefreshAudioDevices();
        }

        ImGui::Dummy({});

        ImGui::Text(i18n::c(AUDIO_BUFFER_COUNT));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt("##bufcount", &imgui_audio_bufferCount, 0);

        ImGui::Text(i18n::c(AUDIO_BUFFER_LENGTH));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt("##bufsize", &imgui_audio_bufferSize, 0);

        if (ImGui::Button(i18n::c(APPLY), { 80.f, 0.f }))
        {
            _imguiApplyAudioSettings();
        }

        ImGui::EndChild();
    }
}

void SceneSelect::_imguiPage_Options_Play()
{
    using namespace i18nText;

    if (ImGui::BeginChild("##pagesub15"))
    {
        ImGui::Text(i18n::c(MISS_BGA_TIME));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt("##misstime", &imgui_adv_missBGATime, 0);

        ImGui::Text(i18n::c(MIN_INPUT_INTERVAL));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt("##mininputinterval", &imgui_adv_minInputInterval, 1, 10);
        ImGui::SameLine();
        HelpMarker(i18n::c(MIN_INPUT_INTERVAL_HINT));
        //ImGui::Checkbox("Accept mouse movements as Analog input", &imgui_adv_mouseAnalog);

        ImGui::Text(i18n::c(DEFAULT_TARGET));
        ImGui::SameLine(infoRowWidth);
        imgui_play_defaultTarget = State::get(IndexNumber::DEFAULT_TARGET_RATE);
        if (ImGui::SliderInt("##defaulttarget", &imgui_play_defaultTarget, 0, 100, "%d %%", ImGuiSliderFlags_None))
        {
            State::set(IndexNumber::DEFAULT_TARGET_RATE, std::clamp(imgui_play_defaultTarget, 0, 100));
        }

        ImGui::Text(i18n::c(JUDGE_TIMING));
        ImGui::SameLine(infoRowWidth);
        imgui_play_judgeTiming = State::get(IndexNumber::TIMING_ADJUST_VISUAL);
        if (ImGui::SliderInt("##judgetiming", &imgui_play_judgeTiming, -99, 99, "%d ms", ImGuiSliderFlags_None))
        {
            State::set(IndexNumber::TIMING_ADJUST_VISUAL, imgui_play_judgeTiming);
        }
        ImGui::SameLine();
        HelpMarker(i18n::c(JUDGE_TIMING_HINT));

        imgui_play_lockGreenNumber = State::get(IndexSwitch::P1_LOCK_SPEED);
        if (ImGui::Checkbox(i18n::c(LOCK_GREENNUMBER), &imgui_play_lockGreenNumber))
        {
            State::set(IndexSwitch::P1_LOCK_SPEED, imgui_play_lockGreenNumber);
        }

        ImGui::BeginDisabled(imgui_play_lockGreenNumber);
        ImGui::Text(i18n::c(HISPEED));
        ImGui::SameLine(infoRowWidth);
        imgui_play_hispeed = gPlayContext.Hispeed;
        if (ImGui::SliderFloat("##hispeed", &imgui_play_hispeed, 0.01f, 10.0f, "%.02f", ImGuiSliderFlags_None))
        {
            int hsInt = int(std::round(imgui_play_hispeed * 100.0));
            State::set(IndexNumber::HS_1P, hsInt);
            gPlayContext.Hispeed = imgui_play_hispeed = hsInt / 100.0;
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!imgui_play_lockGreenNumber);
        ImGui::Text(i18n::c(GREENNUMBER));
        ImGui::SameLine(infoRowWidth);
        imgui_play_greenNumber = ConfigMgr::get('P', cfg::P_GREENNUMBER, 0);
        if (ImGui::SliderInt("##greennumber", &imgui_play_greenNumber, 1, 1200, "%d", ImGuiSliderFlags_None))
        {
            ConfigMgr::set('P', cfg::P_GREENNUMBER, imgui_play_greenNumber >= 0 ? imgui_play_greenNumber : 0);
        }
        ImGui::SameLine();
        HelpMarker(i18n::c(GREENNUMBER_HINT));
        ImGui::EndDisabled();

        ImGui::EndChild();
    }
}

void SceneSelect::_imguiPage_Options_Advanced()
{
    using namespace i18nText;

    if (ImGui::BeginChild("##pagesub16"))
    {
        ImGui::Text(i18n::c(SCROLL_SPEED));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt2("##adv1", imgui_adv_scrollSpeed);
        ImGui::SameLine();
        HelpMarker(i18n::c(SCROLL_SPEED_HINT));

        ImGui::Text(i18n::c(NEW_SONG_DURATION));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt("##adv2", &imgui_adv_newSongDuration, 1, 10);
        ImGui::SameLine();
        HelpMarker(i18n::c(NEW_SONG_DURATION_HINT));

        ImGui::Checkbox(i18n::c(PREVIEW_DEDICATED), &imgui_adv_previewDedicated);
        ImGui::SameLine();
        HelpMarker(i18n::c(PREVIEW_DEDICATED_HINT));

        ImGui::Checkbox(i18n::c(PREVIEW_DIRECT), &imgui_adv_previewDedicated);
        ImGui::SameLine();
        HelpMarker(i18n::c(PREVIEW_DIRECT_HINT));

        ImGui::Text(i18n::c(SELECT_KEYBINDINGS));
        ImGui::SameLine(infoRowWidth);
        static const char* imgui_adv_select_keybindings_display[] =
        {
            "7 Keys",
            "5 Keys",
            "9 Buttons"
        };
        ImGui::Combo("##adv6", &imgui_adv_selectKeyBindings, imgui_adv_select_keybindings_display, sizeof(imgui_adv_select_keybindings_display) / sizeof(char*));
        ImGui::SameLine();
        HelpMarker(i18n::c(SELECT_KEYBINDINGS_HINT));

        ImGui::Checkbox(i18n::c(ENABLE_NEW_RANDOM_OPTIONS), &imgui_adv_enableNewRandom);
        ImGui::SameLine();
        HelpMarker(i18n::c(ENABLE_NEW_RANDOM_OPTIONS_HINT));

        ImGui::Checkbox(i18n::c(ENABLE_NEW_GAUGE_OPTIONS), &imgui_adv_enableNewGauge);
        ImGui::SameLine();
        HelpMarker(i18n::c(ENABLE_NEW_GAUGE_OPTIONS_HINT));

        ImGui::Checkbox(i18n::c(ENABLE_NEW_LANE_OPTIONS), &imgui_adv_enableNewLaneOption);
        ImGui::SameLine();
        HelpMarker(i18n::c(ENABLE_NEW_LANE_OPTIONS_HINT));

        ImGui::EndChild();
    }
}

void SceneSelect::_imguiPage_About()
{
    using namespace i18nText;

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem("Lunatic Vibes", NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        if (ImGui::BeginChild("##pagesub21"))
        {
            ImGui::TextUnformatted("\"Lunatic Vibes\" 2017-2022");
            ImGui::TextUnformatted((boost::format("Version: %s %s (%s %s)")
                % PROJECT_VERSION
#if _DEBUG
                % "Debug" 
#else
                % "Release"
#endif
                % GIT_BRANCH % GIT_COMMIT
                ).str().c_str());
                
            ImGui::TextUnformatted("https://github.com/yaasdf/lunaticvibes");

            ImGui::TextUnformatted(R"(


Casts:（敬称略）

Leader:
    rustbell

Main Programmer:
    rustbell
Artwork:
    - (Not yet! XD)
Music:
    -
Sound Effects:
    -
Contributors:
    jbscj
    lxdlam

Testers:
    XX
    ast
    SonicTV
    And more!

Donators:
    XX
    ZTNP
    诡异
    Komokom
    udon
    danRP
    秋
    Destinedone
    菡
    爱发电用户_3QVd
    有只柠檬猫
    Thank you for your generous support!

Special Thanks:
    lavalse, cyclia, SHiNKA, Kamiyu, and mur (Lunatic Rave 2)
    kenjidct (LR2IR)
    .RED (LR2スキン仕様書)

)");
            ImGui::EndChild();
        }
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("3rd-party Softwares", NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        if (ImGui::BeginChild("##pagesub22"))
        {
            static std::string text;
            if (text.empty())
            {
                std::ifstream ifsFile(Path(GAMEDATA_PATH) / "resources" / "LICENSE_3RD_PARTY");
                std::stringstream ss;
                ss << ifsFile.rdbuf();
                ss.sync();
                ifsFile.close();
                text = ss.str();
            }
            ImGui::TextUnformatted(text.c_str());

            ImGui::EndChild();
        }
        ImGui::EndTabItem();
    }
}

void SceneSelect::_imguiPage_Exit()
{
    gNextScene = eScene::EXIT_TRANS;
}



void SceneSelect::_imguiRefreshProfileList()
{
    imgui_profile_index = -1;
    imgui_profiles.clear();
    imgui_profiles_display.clear();

    static const Path profilePath = Path(GAMEDATA_PATH) / "profile";
    if (!std::filesystem::exists(profilePath))
    {
        std::filesystem::create_directories(profilePath);
    }
    for (auto& p : std::filesystem::directory_iterator(profilePath))
    {
        if (p.is_directory())
        {
            auto name = p.path().filename().u8string();
            imgui_profiles.push_back(name);
            imgui_profiles_display.push_back(imgui_profiles.back().c_str());
        }
    }
    if (!imgui_profiles.empty())
    {
        int idx = -1;
        std::string profile = ConfigMgr::get('E', cfg::E_PROFILE, cfg::PROFILE_DEFAULT);
        for (const auto& p : imgui_profiles)
        {
            idx++;
            if (profile == p)
            {
                imgui_profile_index = idx;
                break;
            }
        }
        if ((size_t)idx == imgui_profiles.size())
        {
            imgui_profile_index = -1;
        }
    }
    else
    {
        imgui_profile_index = -1;
    }
}

void SceneSelect::_imguiRefreshLanguageList()
{
    imgui_languages.clear();
    imgui_languages_display.clear();

    for (auto& l : i18n::getLanguageList())
    {
        if (l == ConfigMgr::get('P', cfg::P_LANGUAGE, "English"))
        {
            imgui_language_index = imgui_languages.size();
            old_language_index = imgui_language_index;
        }

        imgui_languages.push_back(l);
    }

    for (const auto& f : imgui_languages)
    {
        imgui_languages_display.push_back(f.c_str());
    }
}


void SceneSelect::_imguiRefreshFolderList()
{
    imgui_folder_index = -1;
    imgui_folders.clear();
    imgui_folders_display.clear();

    auto folders = ConfigMgr::General()->getFoldersStr();
    imgui_folders.assign(folders.begin(), folders.end());
    for (const auto& f : imgui_folders)
        imgui_folders_display.push_back(f.c_str());
}

void SceneSelect::_imguiRefreshTableList()
{
    imgui_table_index = -1;
    imgui_tables.clear();
    imgui_tables_display.clear();

    auto tables = ConfigMgr::General()->getTablesUrl();
    imgui_tables.assign(tables.begin(), tables.end());
    for (const auto& f : imgui_tables)
        imgui_tables_display.push_back(f.c_str());
}

void SceneSelect::_imguiRefreshVideoDisplayResolutionList()
{
    imgui_video_display_resolution_size.clear();
    imgui_video_display_resolution.clear();
    imgui_video_display_resolution_display.clear();

    // TODO get resolution list from system
    imgui_video_display_resolution_size.push_back({ 640, 480 });
    imgui_video_display_resolution_size.push_back({ 800, 600 });
    imgui_video_display_resolution_size.push_back({ 1280, 720 });
    imgui_video_display_resolution_size.push_back({ 1280, 960 });
    imgui_video_display_resolution_size.push_back({ 1366, 768 });
    imgui_video_display_resolution_size.push_back({ 1600, 900 });
    imgui_video_display_resolution_size.push_back({ 1920, 1080 });
    imgui_video_display_resolution_size.push_back({ 2560, 1440 });
    imgui_video_display_resolution_size.push_back({ 3840, 2160 });

    for (size_t i = 0; i < imgui_video_display_resolution_size.size(); ++i)
    {
        std::stringstream ss;
        ss << imgui_video_display_resolution_size[i].first; 
        ss << "x";
        ss << imgui_video_display_resolution_size[i].second;
        imgui_video_display_resolution.push_back(ss.str());
    }

    for (const auto& r : imgui_video_display_resolution)
    {
        imgui_video_display_resolution_display.push_back(r.c_str());
    }
}

void SceneSelect::_imguiCheckSettings()
{
    bool reboot = false;

    if (imgui_profile_index != old_profile_index)
    {
        old_profile_index = imgui_profile_index;
        ConfigMgr::selectProfile(imgui_profiles_display[imgui_profile_index]);
        reboot = true;
    }
    if (imgui_language_index != old_language_index)
    {
        old_language_index = imgui_language_index;
        ConfigMgr::set("P", cfg::P_LANGUAGE, imgui_languages_display[imgui_language_index]);
        i18n::setLanguage(imgui_language_index);
        reboot = true;
    }

    if (imgui_video_display_resolution_index != old_video_display_resolution_index)
    {
        old_video_display_resolution_index = imgui_video_display_resolution_index;
        if (imgui_video_display_resolution_index > (int)imgui_video_display_resolution_size.size())
        {
            imgui_video_display_resolution_index = 0;
        }
    }

    if (imgui_adv_scrollSpeed[0] != ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300))
    {
        ConfigMgr::set("P", cfg::P_LIST_SCROLL_TIME_INITIAL, imgui_adv_scrollSpeed[0]);
    }
    if (imgui_adv_scrollSpeed[1] != ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150))
    {
        ConfigMgr::set("P", cfg::P_LIST_SCROLL_TIME_HOLD, imgui_adv_scrollSpeed[1]);
    }
    if (imgui_adv_missBGATime != ConfigMgr::get("P", cfg::P_MISSBGA_LENGTH, 500))
    {
        ConfigMgr::set("P", cfg::P_MISSBGA_LENGTH, imgui_adv_missBGATime);
    }
    if (imgui_adv_minInputInterval != ConfigMgr::get("P", cfg::P_MIN_INPUT_INTERVAL, 16))
    {
        ConfigMgr::set("P", cfg::P_MIN_INPUT_INTERVAL, imgui_adv_minInputInterval);
        InputMgr::setDebounceTime(imgui_adv_minInputInterval);
    }
    if (imgui_adv_newSongDuration != ConfigMgr::get("P", cfg::P_NEW_SONG_DURATION, 24))
    {
        ConfigMgr::set("P", cfg::P_NEW_SONG_DURATION, imgui_adv_newSongDuration);
        State::set(IndexNumber::NEW_ENTRY_SECONDS, ConfigMgr::get('P', cfg::P_NEW_SONG_DURATION, 0) * 60 * 60);
    }

    if (imgui_adv_previewDedicated != ConfigMgr::get("P", cfg::P_PREVIEW_DEDICATED, true))
    {
        ConfigMgr::set("P", cfg::P_PREVIEW_DEDICATED, imgui_adv_previewDedicated);
    }
    if (imgui_adv_previewDirect != ConfigMgr::get("P", cfg::P_PREVIEW_DIRECT, true))
    {
        ConfigMgr::set("P", cfg::P_PREVIEW_DIRECT, imgui_adv_previewDirect);
    }
    if (imgui_adv_selectKeyBindings != old_adv_selectKeyBindings)
    {
        static const std::pair<GameModeKeys, std::string> imgui_select_keybindings_str[] =
        {
            { 7, cfg::P_SELECT_KEYBINDINGS_7K },
            { 5, cfg::P_SELECT_KEYBINDINGS_5K },
            { 9, cfg::P_SELECT_KEYBINDINGS_9K },
        };
        const auto& [keys, cfgstr] = imgui_select_keybindings_str[imgui_adv_selectKeyBindings];
        old_adv_selectKeyBindings = imgui_adv_selectKeyBindings;
        ConfigMgr::set("P", cfg::P_SELECT_KEYBINDINGS, cfgstr);
        bindings9K = (keys == 9);
        InputMgr::updateBindings(keys);
    }
    if (imgui_adv_enableNewRandom != ConfigMgr::get("P", cfg::P_ENABLE_NEW_RANDOM, false))
    {
        ConfigMgr::set("P", cfg::P_ENABLE_NEW_RANDOM, imgui_adv_enableNewRandom);
    }
    if (imgui_adv_enableNewGauge != ConfigMgr::get("P", cfg::P_ENABLE_NEW_GAUGE, false))
    {
        ConfigMgr::set("P", cfg::P_ENABLE_NEW_GAUGE, imgui_adv_enableNewGauge);
    }
    if (imgui_adv_enableNewLaneOption != ConfigMgr::get("P", cfg::P_ENABLE_NEW_LANE_OPTION, false))
    {
        ConfigMgr::set("P", cfg::P_ENABLE_NEW_LANE_OPTION, imgui_adv_enableNewLaneOption);
    }

    if (reboot)
    {
        imguiShow = false;
        _skin->setHandleMouseEvents(true);
        gSelectContext.isGoingToReboot = true;
    }
}

bool SceneSelect::_imguiApplyPlayerName()
{
    ConfigMgr::Profile()->set(cfg::P_PLAYERNAME, imgui_player_name_buf);
    State::set(IndexText::PLAYER_NAME, imgui_player_name_buf);
    return true;
}

bool SceneSelect::_imguiAddFolder(const char* path)
{
    bool added = false;

    if (!path)
    {
#ifdef WIN32
        // TODO replace with IFileDialog
        char szDisplayName[MAX_PATH] = { 0 };
        BROWSEINFOA lpbi = { 0 };
        getWindowHandle(&lpbi.hwndOwner);
        lpbi.pszDisplayName = szDisplayName;
        lpbi.lpszTitle = "Select Folder";
        lpbi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_EDITBOX | BIF_USENEWUI;
        PIDLIST_ABSOLUTE lpiil = SHBrowseForFolderA(&lpbi);
        if (lpiil)
        {
            char szPath[MAX_PATH];
            if (SHGetPathFromIDList(lpiil, szPath))
            {
                imgui_folders.push_back(szPath);
                imgui_folders_display.push_back(imgui_folders.back().c_str());
                imgui_folder_index = -1;
                added = true;

                ConfigMgr::General()->setFolders(std::vector<std::string>(imgui_folders.begin(), imgui_folders.end()));

                // TODO auto refresh?
            }
            CoTaskMemFree(lpiil);
        }
#endif
    }
    else
    {
        imgui_folders.push_back(path);
        imgui_folders_display.push_back(imgui_folders.back().c_str());
        imgui_folder_index = -1;
        added = true;

        ConfigMgr::General()->setFolders(std::vector<std::string>(imgui_folders.begin(), imgui_folders.end()));
    }
    return added;
}

bool SceneSelect::_imguiDelFolder()
{
    if (imgui_folder_index < 0 || imgui_folder_index >= imgui_folders_display.size()) return false;

    int oldSize = imgui_folders.size();
    imgui_folders.erase(std::next(imgui_folders.begin(), imgui_folder_index));
    imgui_folders_display.erase(std::next(imgui_folders_display.begin(), imgui_folder_index));
    if (imgui_folder_index == oldSize - 1)
        imgui_folder_index--;

    ConfigMgr::General()->setFolders(std::vector<std::string>(imgui_folders.begin(), imgui_folders.end()));

    return true;
}

bool SceneSelect::_imguiBrowseFolder()
{
    if (imgui_folder_index < 0 || imgui_folder_index >= imgui_folders_display.size()) return false;

    std::string pathstr = Path(imgui_folders_display[imgui_folder_index]).u8string();

#ifdef WIN32
    ShellExecute(NULL, "open", pathstr.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#elif defined __linux__
    // linux has many WMs that may have to handle differently
#endif

    return true;
}

bool SceneSelect::_imguiAddTable()
{
    bool added = false;

    imgui_tables.push_back(imgui_table_url_buf);
    imgui_tables_display.push_back(imgui_tables.back().c_str());
    added = true;

    if (added)
    {
        imgui_table_index = imgui_tables.size() - 1;

        ConfigMgr::General()->setTables(std::vector<std::string>(imgui_tables.begin(), imgui_tables.end()));
    }

    return added;
}

bool SceneSelect::_imguiDelTable()
{
    if (imgui_table_index < 0 || imgui_table_index >= imgui_tables_display.size()) return false;

    int oldSize = imgui_tables.size();
    imgui_tables.erase(std::next(imgui_tables.begin(), imgui_table_index));
    imgui_tables_display.erase(std::next(imgui_tables_display.begin(), imgui_table_index));
    if (imgui_table_index == oldSize - 1)
        imgui_table_index--;

    ConfigMgr::General()->setTables(std::vector<std::string>(imgui_tables.begin(), imgui_tables.end()));

    return false;
}

bool SceneSelect::_imguiApplyResolution()
{
    const auto& [windowW, windowH] = imgui_video_display_resolution_size[imgui_video_display_resolution_index];

    graphics_change_window_mode(imgui_video_mode);
    graphics_resize_window(windowW, windowH);
    graphics_set_supersample_level(imgui_video_ssLevel);
    graphics_change_vsync(imgui_video_vsync_index);

    ConfigMgr::set("V", cfg::V_DISPLAY_RES_X, windowW);
    ConfigMgr::set("V", cfg::V_DISPLAY_RES_Y, windowH);
    if (imgui_video_mode != old_video_mode)
    {
        old_video_mode = imgui_video_mode;
        const char* windowMode = NULL;
        switch (imgui_video_mode)
        {
        case 0: windowMode = cfg::V_WINMODE_WINDOWED;   break;
        case 1: windowMode = cfg::V_WINMODE_FULL;       break;
        case 2: windowMode = cfg::V_WINMODE_BORDERLESS; break;
        }
        ConfigMgr::set("V", cfg::V_WINMODE, windowMode);
    }
    ConfigMgr::set("V", cfg::V_RES_SUPERSAMPLE, imgui_video_ssLevel);
    ConfigMgr::set("V", cfg::V_VSYNC, imgui_video_vsync_index);

    // windowed
    {
        State::set(IndexOption::SYS_WINDOWED, imgui_video_mode == 1 ? 1 : 0);

        static const std::map<int, std::string> smap =
        {
            {0, "WINDOWED"},
            {1, "FULLSCREEN"},
            {2, "BORDERLESS"},
        };

        auto&& s = imgui_video_mode;
        if (smap.find(s) != smap.end())
            State::set(IndexText::WINDOWMODE, smap.at(s));
        else
            State::set(IndexText::WINDOWMODE, "WINDOWED");
    }
    // vsync
    {
        State::set(IndexOption::SYS_VSYNC, imgui_video_vsync_index == 0 ? 0 : 1);

        static const std::map<int, std::string> smap =
        {
            {0, "OFF"},
            {1, "ON"},
#if _WIN32
            {2, "ON"}
#else
            {2, "ADAPTIVE"}
#endif
        };

        auto&& s = imgui_video_vsync_index;
        if (smap.find(s) != smap.end())
            State::set(IndexText::VSYNC, smap.at(s));
        else
            State::set(IndexText::VSYNC, "OFF");
    }

    return true;
}

bool SceneSelect::_imguiRefreshAudioDevices()
{
    imgui_audio_device_index = -1;
    imgui_audio_devices.clear();
    imgui_audio_devices_display.clear();
    auto adev = ConfigMgr::get('A', cfg::A_DEVNAME, "");

    auto devList = SoundMgr::getDeviceList(true);
    for (auto& d : devList)
    {
        if (adev == d.second)
        {
            imgui_audio_device_index = (int)imgui_audio_devices.size();
        }

        if (d.first & 0x40000000)
        {
            // ASIO device
            imgui_audio_devices_name.push_back(std::string("[ASIO] ") + d.second);
        }
        else
        {
            imgui_audio_devices_name.push_back(d.second);
        }
        imgui_audio_devices.push_back(d);
        imgui_audio_devices_display.push_back(imgui_audio_devices_name.back().c_str());

    }
    if (imgui_audio_device_index == -1)
    {
        imgui_audio_device_index = 0;
    }

    return false;
}

bool SceneSelect::_imguiApplyAudioSettings()
{
    if (SoundMgr::setDevice(imgui_audio_device_index, true) == 0)
    {
        auto& mode = std::next(imgui_audio_devices.begin(), imgui_audio_device_index);
        ConfigMgr::set('A', cfg::A_DEVNAME, mode->second);
        ConfigMgr::set('A', cfg::A_MODE, mode->first < 0 ? cfg::A_MODE_ASIO : cfg::A_MODE_AUTO);
        ConfigMgr::set("A", cfg::A_BUFCOUNT, imgui_audio_bufferCount);
        ConfigMgr::set("A", cfg::A_BUFLEN, imgui_audio_bufferSize);

        SoundMgr::stopSysSamples();
        SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);
        return true;
    }
    return false;
}
