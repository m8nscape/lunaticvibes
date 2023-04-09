#include "scene_select.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/runtime/i18n.h"
#include "game/arena/arena_data.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "git_version.h"

#ifdef _WIN32
#include <shellapi.h>
#include <VersionHelpers.h>
#endif

void SceneSelect::imguiSampleDialog()
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

void SceneSelect::imguiInit()
{
    imguiRefreshProfileList();
    old_profile_index = imgui_profile_index;

    std::string playerName = ConfigMgr::get('P', cfg::P_PLAYERNAME, "Unnamed");
    strncpy(imgui_player_name_buf, playerName.c_str(), std::max(sizeof(imgui_player_name_buf) - 1, playerName.length()));

    imguiRefreshLanguageList();
    old_language_index = imgui_language_index;

    imgui_log_level = ConfigMgr::get("E", cfg::E_LOG_LEVEL, 1);
    SetLogLevel(imgui_log_level);

    imguiRefreshFolderList();
    imguiRefreshTableList();

    imgui_video_ssLevel = ConfigMgr::get("V", cfg::V_RES_SUPERSAMPLE, 1);

    imguiRefreshVideoDisplayResolutionList();
    auto windowX = ConfigMgr::get("V", cfg::V_DISPLAY_RES_X, CANVAS_HEIGHT);
    auto windowY = ConfigMgr::get("V", cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT);
    imgui_video_display_resolution_index = -1;
    for (int i = 0; i < (int)imgui_video_display_resolution_size.size(); ++i)
    {
        if (imgui_video_display_resolution_size[i].first == windowX && imgui_video_display_resolution_size[i].second == windowY)
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

    auto [count, size] = SoundMgr::getDSPBufferSize();
    imgui_audio_bufferCount = count;
    imgui_audio_bufferSize = size;
    imguiRefreshAudioDevices();
    old_audio_device_index = imgui_audio_device_index;

    imgui_adv_scrollSpeed[0] = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
    imgui_adv_scrollSpeed[1] = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
    imgui_adv_missBGATime = ConfigMgr::get("P", cfg::P_MISSBGA_LENGTH, 500);
    imgui_adv_minInputInterval = ConfigMgr::get("P", cfg::P_MIN_INPUT_INTERVAL, 16);
    imgui_adv_newSongDuration = ConfigMgr::get("P", cfg::P_NEW_SONG_DURATION, 24);

    imgui_play_inputPollingRate = 0;
    switch (ConfigMgr::get("P", cfg::P_INPUT_POLLING_RATE, 1000))
    {
    case 1000: imgui_play_inputPollingRate = 0; break;
    case 2000: imgui_play_inputPollingRate = 1; break;
    case 4000: imgui_play_inputPollingRate = 2; break;
    case 8000: imgui_play_inputPollingRate = 3; break;
    }

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

    imgui_sel_onlyDisplayMainTitleOnBars = ConfigMgr::get("P", cfg::P_ONLY_DISPLAY_MAIN_TITLE_ON_BARS, false);
    imgui_sel_disablePlaymodeAll = ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_ALL, false);
    imgui_sel_disableDifficultyAll = ConfigMgr::get("P", cfg::P_DISABLE_DIFFICULTY_ALL, false);
    imgui_sel_disablePlaymodeSingle = ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_SINGLE, false);
    imgui_sel_disablePlaymodeDouble = ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_DOUBLE, false);
    imgui_sel_ignoreDPCharts = ConfigMgr::get("P", cfg::P_IGNORE_DP_CHARTS, false);
    imgui_sel_ignore9keys = ConfigMgr::get("P", cfg::P_IGNORE_9KEYS_CHARTS, false);
    imgui_sel_ignore5keysif7keysexist = ConfigMgr::get("P", cfg::P_IGNORE_5KEYS_IF_7KEYS_EXIST, false);

    imgui_play_adjustHispeedWithUpDown = ConfigMgr::get('P', cfg::P_ADJUST_HISPEED_WITH_ARROWKEYS, false);
    imgui_play_adjustHispeedWithSelect = ConfigMgr::get('P', cfg::P_ADJUST_HISPEED_WITH_SELECT, false);
    imgui_play_adjustLanecoverWithStart67 = ConfigMgr::get('P', cfg::P_ADJUST_LANECOVER_WITH_START_67, false);
    imgui_play_adjustLanecoverWithMousewheel = ConfigMgr::get('P', cfg::P_ADJUST_LANECOVER_WITH_MOUSEWHEEL, false);
    imgui_play_adjustLanecoverWithLeftRight = ConfigMgr::get('P', cfg::P_ADJUST_LANECOVER_WITH_ARROWKEYS, false);

    // auto popup settings for first runs
    if (imgui_folders.empty())
    {
        imguiShow = true;
        pSkin->setHandleMouseEvents(false);
    }
}


void SceneSelect::updateImgui()
{
    SceneBase::updateImgui();
    if (gNextScene != SceneType::SELECT) return;

    imguiSettings();
    imguiArenaJoinLobbyPrompt();
}


#ifdef WIN32
#include <ShlObj.h>
#include <ShlObj_core.h>
#endif
void SceneSelect::imguiSettings()
{
    using namespace i18nText;

    if (imguiShow)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

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

                    MENU_ARENADIAGNOSE,
                    MENU_________,
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

#if _DEBUG
                    if (ImGui::Button("ARENA", {-1.f, height}))
                    {
                        imgui_main_index = MENU_ARENADIAGNOSE;
                    }
                    ImGui::Dummy({ 0.f, interval });
#else
                    if (ImGui::InvisibleButton("##dummy123123121", { -1.f, height }))
                    {
                    }
                    ImGui::Dummy({ 0.f, interval });
#endif
                    if (ImGui::InvisibleButton("##dummy123123122", { -1.f, height }))
                    {
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
                        if (imgui_main_index == MENU_ARENADIAGNOSE && ImGui::BeginTabBar("##arena", ImGuiTabBarFlags_FittingPolicyScroll))
                        {
                            imguiPageArenaDiagnose();
                            ImGui::EndTabBar();
                        }

                        if (imgui_main_index == MENU_OPTIONS && ImGui::BeginTabBar("##option", ImGuiTabBarFlags_FittingPolicyScroll))
                        {
                            imguiPageOptions();
                            ImGui::EndTabBar();
                        }

                        if (imgui_main_index == MENU_ABOUT && ImGui::BeginTabBar("##about", ImGuiTabBarFlags_FittingPolicyScroll))
                        {
                            imguiPageAbout();
                            ImGui::EndTabBar();
                        }

                        if (imgui_main_index == MENU_EXIT)
                        {
                            imguiPageExit();
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
                                imguiRefreshProfileList();

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

    imguiCheckSettings();
}

static const float tabItemWidth = 120.f;
static const float infoRowWidth = 240.f;

void SceneSelect::imguiPageArenaDiagnose()
{
    auto& d = gArenaData;

    static bool online = false;
    online = d.isOnline();

    ImGui::Checkbox("Online", &online);

    if (!online) return;

    static bool expired = false;
    expired = d.isExpired();
    ImGui::Checkbox("Expired", &expired);

    if (expired) return;

    static bool client = false;
    static bool server = false;
    client = d.isClient();
    server = !client;
    ImGui::Checkbox("Client", &client);
    ImGui::SameLine();
    ImGui::Checkbox("Server", &server);

    ImGui::Spacing();
    ImGui::Separator();

    static bool playing = false;
    playing = d.isPlaying();
    ImGui::Checkbox("Playing", &expired);

    ImGui::Text("Start Time: %d", d.getPlayStartTimeMs());

    static bool finished = false;
    finished = d.isPlayingFinished();
    ImGui::Checkbox("Finished", &finished);

    ImGui::Text("Players: %lu", d.getPlayerCount());
    for (size_t i = 0; i < d.getPlayerCount(); ++i)
    {
        const auto r = d.getPlayerRuleset(i);
        if (r)
        {
            ImGui::Text("%d: %s [%s%s%s%s ]", d.getPlayerID(i), d.getPlayerName(i).c_str(),
                r->isNoScore() ? " NoScore" : "",
                r->isFinished() ? " Finished" : "",
                r->isCleared() ? " Cleared" : "",
                r->isFailed() ? " Failed" : "");
        }
        else
        {
            ImGui::Text("%d: %s empty", d.getPlayerID(i), d.getPlayerName(i).c_str());
        }
    }

    if (client && g_pArenaClient)
    {
        auto& c = *g_pArenaClient;
        
        ImGui::Text("PlayerID: %d", c.getPlayerID());
        ImGui::Text("isCreatedRuleset: %d", c.isCreatedRuleset());
        ImGui::Text("isLoadingFinished: %d", c.isLoadingFinished());
        ImGui::Text("isPlayingFinished: %d", c.isPlayingFinished());
        ImGui::Text("isResultFinished: %d", c.isResultFinished());

#if _DEBUG
        ImGui::Spacing();
        ImGui::Separator();

        static std::string reqChartHash;
        reqChartHash = c.requestChartHash.hexdigest();
        ImGui::Text("recvMessageIndex: %d", c.recvMessageIndex);
        ImGui::Text("sendMessageIndex: %d", c.sendMessageIndex);
        ImGui::Text("requestChartHash: %s", reqChartHash.c_str());
        ImGui::Text("Heartbeat: %ds ago", (Time() - c.heartbeatTime).norm() / 1000);
#endif
    }
    else if (server && g_pArenaHost)
    {
        auto& s = *g_pArenaHost;

        ImGui::Text("isCreatedRuleset: %d", s.isCreatedRuleset());
        ImGui::Text("isLoadingFinished: %d", s.isLoadingFinished());
        ImGui::Text("isPlayingFinished: %d", s.isPlayingFinished());
        ImGui::Text("isResultFinished: %d", s.isResultFinished());

#if _DEBUG
        ImGui::Spacing();
        ImGui::Separator();

        static std::string reqChartHash;
        reqChartHash = s.requestChartHash.hexdigest();
        ImGui::Text("currentChart: %s", reqChartHash.c_str());

        static std::string reqChartPending;
        reqChartPending = s.requestChartPending.hexdigest();
        ImGui::Text("requestChartPending: %s", reqChartPending.c_str());
        ImGui::Text("requestChartPendingClientKey: %s", s.requestChartPendingClientKey.c_str());
        ImGui::Text("requestChartPendingExistCount: %d", s.requestChartPendingExistCount);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("hostRequestChartHash: %s", s.hostRequestChartHash.hexdigest().c_str());

        std::shared_lock l(s.clientsMutex);
        static std::map<std::string, std::string> hashs;
        for (auto& [key, c]: s.clients)
        {
            hashs[key] = c.requestChartHash.hexdigest();
            ImGui::Text("%s %d: %s ping:%dms send:%d recv:%d hb:%ds [%s%s%s ] req:%s", key.c_str(), c.id,
                c.name.c_str(), c.ping, c.sendMessageIndex, c.recvMessageIndex, (Time() - c.heartbeatRecvTime).norm() / 1000,
                c.isLoadingFinished ? " isLoadingFinished" : "",
                c.isPlayingFinished ? " isPlayingFinished" : "",
                c.isResultFinished ? " isResultFinished" : "",
                hashs[key].c_str()
            );
        }
#endif
    }
}

void SceneSelect::imguiPageOptions()
{
    using namespace i18nText;

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_GENERAL), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        imguiPageOptionsGeneral();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_JUKEBOX), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        imguiPageOptionsJukebox();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_VIDEO), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        imguiPageOptionsVideo();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_AUDIO), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        imguiPageOptionsAudio();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_PLAY), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        imguiPageOptionsPlay();
        ImGui::EndTabItem();
    }

    ImGui::SetNextItemWidth(tabItemWidth);
    if (ImGui::BeginTabItem(i18n::c(SETTINGS_SELECT), NULL, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
    {
        imguiPageOptionsSelect();
        ImGui::EndTabItem();
    }
}

void SceneSelect::imguiPageOptionsGeneral()
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

        ImGui::Spacing();
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
            imguiApplyPlayerName();
        }

        ImGui::Text(i18n::c(LANGUAGE));
        ImGui::SameLine(infoRowWidth);
        ImGui::Combo("##language", &imgui_language_index, imgui_languages_display.data(), (int)imgui_languages_display.size());

        /////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Text(i18n::c(LOG_LEVEL));
        ImGui::SameLine(infoRowWidth);
        static const char* imgui_log_level_display[] =
        {
            "Debug",
            "Info",
            "Warning",
            "Error"
        };
        if (ImGui::Combo("##loglevel", &imgui_log_level, imgui_log_level_display, sizeof(imgui_log_level_display) / sizeof(char*)))
        {
            SetLogLevel(imgui_log_level);
            ConfigMgr::set('E', cfg::E_LOG_LEVEL, imgui_log_level);
        }

        ImGui::EndChild();
    }
}

void SceneSelect::imguiPageOptionsJukebox()
{
    using namespace i18nText;

    if (ImGui::BeginChild("##pagesub12"))
    {
        ImGui::Text(i18n::c(JUKEBOX_REFRESH_HINT));

        ImGui::Spacing();
        ImGui::Separator();

        /////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Text(i18n::c(JUKEBOX_FOLDER));
        {
            bool enterPath = ImGui::InputTextWithHint("##folderpath", i18n::c(JUKEBOX_FOLDER_PATH_HINT), imgui_folder_path_buf, sizeof(imgui_folder_path_buf), ImGuiInputTextFlags_EnterReturnsTrue);
            if (enterPath || (ImGui::SameLine(), ImGui::Button(" + ##folder")))
            {
                imguiAddFolder(imgui_folder_path_buf);
                memset(imgui_folder_path_buf, 0, sizeof(imgui_folder_path_buf));
            }

            ImGui::ListBox("##Paths", &imgui_folder_index, imgui_folders_display.data(), imgui_folders_display.size());

            if (ImGui::Button(i18n::c(ADD_MORE)))
            {
                imguiAddFolder();
            }
            ImGui::SameLine();
            if (ImGui::Button((i18n::s(JUKEBOX_DELETE_SELECTED) + "##delfolder").c_str()))
            {
                imguiDelFolder();
            }
            ImGui::SameLine();
            if (ImGui::Button(i18n::c(JUKEBOX_BROWSE_SELECTED)))
            {
                imguiBrowseFolder();
            }
        }

        ImGui::Spacing();
        ImGui::Separator();

        /////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Text(i18n::c(JUKEBOX_TABLES));
        {
            bool enterUrl = ImGui::InputTextWithHint("##tableurl", i18n::c(JUKEBOX_TABLE_URL_HINT), imgui_table_url_buf, sizeof(imgui_table_url_buf), ImGuiInputTextFlags_EnterReturnsTrue);
            if (enterUrl || (ImGui::SameLine(), ImGui::Button(" + ##table")))
            {
                imguiAddTable();
                memset(imgui_table_url_buf, 0, sizeof(imgui_table_url_buf));
            }

            ImGui::ListBox("##URLs", &imgui_table_index, imgui_tables_display.data(), imgui_tables_display.size());

            if (ImGui::Button((i18n::s(JUKEBOX_DELETE_SELECTED) + "##deltable").c_str()))
            {
                imguiDelTable();
            }
        }
        ImGui::EndChild();
    }
}

void SceneSelect::imguiPageOptionsVideo()
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
        if (ImGui::Combo("##window", &imgui_video_mode, imgui_video_mode_display, sizeof(imgui_video_mode_display) / sizeof(char*)))
        {
            imguiRefreshVideoDisplayResolutionList();
        }

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
            imguiApplyResolution();

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

void SceneSelect::imguiPageOptionsAudio()
{
    using namespace i18nText;

    if (ImGui::BeginChild("##pagesub14"))
    {
        ImGui::Text(i18n::c(AUDIO_DEVICE));
        ImGui::SameLine(infoRowWidth);
        ImGui::Combo("##audiodevice", &imgui_audio_device_index, imgui_audio_devices_display.data(), (int)imgui_audio_devices_display.size());

        if (ImGui::Button(i18n::c(AUDIO_REFRESH_DEVICE_LIST)))
        {
            imguiRefreshAudioDevices();
        }

        ImGui::Spacing();

        ImGui::Text(i18n::c(AUDIO_BUFFER_COUNT));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt("##bufcount", &imgui_audio_bufferCount, 0);

        ImGui::Text(i18n::c(AUDIO_BUFFER_LENGTH));
        ImGui::SameLine(infoRowWidth);
        ImGui::InputInt("##bufsize", &imgui_audio_bufferSize, 0);

        if (ImGui::Button(i18n::c(APPLY), { 80.f, 0.f }))
        {
            imguiApplyAudioSettings();
        }

        ImGui::EndChild();
    }
}

void SceneSelect::imguiPageOptionsPlay()
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

        ImGui::Text(i18n::c(INPUT_POLLING_RATE));
        ImGui::SameLine(infoRowWidth);
        static const char* imgui_play_inputPollingRate_display[] =
        {
            "1000 Hz",
            "2000 Hz",
            "4000 Hz",
            "8000 Hz",
            //"Unlimited (Not recommended)"
        };
#if WIN32
        int imgui_play_inputPollingRate_count = IsWindows10OrGreater() ? 2 : 1;
#else
        int imgui_play_inputPollingRate_count = 4;
#endif
        if (ImGui::Combo("##inputpollingrate", &imgui_play_inputPollingRate, imgui_play_inputPollingRate_display, imgui_play_inputPollingRate_count))
        {
            switch (imgui_play_inputPollingRate)
            {
            case 0: ConfigMgr::set("P", cfg::P_INPUT_POLLING_RATE, 1000); _input.setRate(1000); break;
            case 1: ConfigMgr::set("P", cfg::P_INPUT_POLLING_RATE, 2000); _input.setRate(2000); break;
            case 2: ConfigMgr::set("P", cfg::P_INPUT_POLLING_RATE, 4000); _input.setRate(4000); break;
            case 3: ConfigMgr::set("P", cfg::P_INPUT_POLLING_RATE, 8000); _input.setRate(8000); break;
            case 4: ConfigMgr::set("P", cfg::P_INPUT_POLLING_RATE, 0); _input.setRate(0); break;
            }
        }
#if WIN32
        ImGui::SameLine();
        HelpMarker(i18n::c(INPUT_POLLING_RATE_WARNING_WINDOWS));
#endif

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

        ImGui::Spacing();
        ImGui::Separator();

        imgui_play_lockGreenNumber = State::get(IndexSwitch::P1_LOCK_SPEED);
        if (ImGui::Checkbox(i18n::c(LOCK_GREENNUMBER), &imgui_play_lockGreenNumber))
        {
            State::set(IndexOption::PLAY_HSFIX_TYPE, imgui_play_lockGreenNumber ? Option::SPEED_FIX_INITIAL : Option::SPEED_NORMAL);
            State::set(IndexText::SCROLL_TYPE, Option::s_speed_type[State::get(IndexOption::PLAY_HSFIX_TYPE)]);
            State::set(IndexSwitch::P1_LOCK_SPEED, imgui_play_lockGreenNumber);
        }

        ImGui::BeginDisabled(imgui_play_lockGreenNumber);
        ImGui::Text(i18n::c(HISPEED));
        ImGui::SameLine(infoRowWidth);
        imgui_play_hispeed = gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed;
        if (ImGui::SliderFloat("##hispeed", &imgui_play_hispeed, 0.01f, 10.0f, "%.02f", ImGuiSliderFlags_None))
        {
            int hsInt = int(std::round(imgui_play_hispeed * 100.0));
            State::set(IndexNumber::HS_1P, hsInt);
            gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed = imgui_play_hispeed = hsInt / 100.0;
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!imgui_play_lockGreenNumber);
        ImGui::Text(i18n::c(GREENNUMBER));
        ImGui::SameLine(infoRowWidth);
        imgui_play_greenNumber = State::get(IndexNumber::GREEN_NUMBER_1P);
        if (ImGui::SliderInt("##greennumber", &imgui_play_greenNumber, 1, 1200, "%d", ImGuiSliderFlags_None))
        {
            State::set(IndexNumber::GREEN_NUMBER_1P, imgui_play_greenNumber >= 0 ? imgui_play_greenNumber : 0);
        }
        ImGui::SameLine();
        HelpMarker(i18n::c(GREENNUMBER_HINT));
        ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Separator();

		if (ImGui::Checkbox(i18n::c(PLAY_ADJUST_HISPEED_WITH_ARROWKEYS), &imgui_play_adjustHispeedWithUpDown))
		{
			ConfigMgr::set('P', cfg::P_ADJUST_HISPEED_WITH_ARROWKEYS, imgui_play_adjustHispeedWithUpDown);
		}

        if (ImGui::Checkbox(i18n::c(PLAY_ADJUST_HISPEED_WITH_SELECT), &imgui_play_adjustHispeedWithSelect))
        {
            ConfigMgr::set('P', cfg::P_ADJUST_HISPEED_WITH_SELECT, imgui_play_adjustHispeedWithSelect);
        }

		if (ImGui::Checkbox(i18n::c(PLAY_ADJUST_LANECOVER_WITH_START_67), &imgui_play_adjustLanecoverWithStart67))
		{
			ConfigMgr::set('P', cfg::P_ADJUST_LANECOVER_WITH_START_67, imgui_play_adjustLanecoverWithStart67);
		}

		if (ImGui::Checkbox(i18n::c(PLAY_ADJUST_LANECOVER_WITH_ARROWKEYS), &imgui_play_adjustLanecoverWithLeftRight))
		{
			ConfigMgr::set('P', cfg::P_ADJUST_LANECOVER_WITH_ARROWKEYS, imgui_play_adjustLanecoverWithLeftRight);
		}

        if (ImGui::Checkbox(i18n::c(PLAY_ADJUST_LANECOVER_WITH_MOUSEWHEEL), &imgui_play_adjustLanecoverWithMousewheel))
        {
            ConfigMgr::set('P', cfg::P_ADJUST_LANECOVER_WITH_MOUSEWHEEL, imgui_play_adjustLanecoverWithMousewheel);
        }


        ImGui::EndChild();
    }
}

void SceneSelect::imguiPageOptionsSelect()
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

        ImGui::Checkbox(i18n::c(PREVIEW_DEDICATED), &imgui_adv_previewDedicated);
        ImGui::SameLine();
        HelpMarker(i18n::c(PREVIEW_DEDICATED_HINT));

        ImGui::Checkbox(i18n::c(PREVIEW_DIRECT), &imgui_adv_previewDirect);
        ImGui::SameLine();
        HelpMarker(i18n::c(PREVIEW_DIRECT_HINT));

        ImGui::Checkbox(i18n::c(ENABLE_NEW_RANDOM_OPTIONS), &imgui_adv_enableNewRandom);
        ImGui::SameLine();
        HelpMarker(i18n::c(ENABLE_NEW_RANDOM_OPTIONS_HINT));

        ImGui::Checkbox(i18n::c(ENABLE_NEW_GAUGE_OPTIONS), &imgui_adv_enableNewGauge);
        ImGui::SameLine();
        HelpMarker(i18n::c(ENABLE_NEW_GAUGE_OPTIONS_HINT));

        ImGui::Checkbox(i18n::c(ENABLE_NEW_LANE_OPTIONS), &imgui_adv_enableNewLaneOption);
        ImGui::SameLine();
        HelpMarker(i18n::c(ENABLE_NEW_LANE_OPTIONS_HINT));

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Checkbox(i18n::c(ONLY_DISPLAY_MAIN_TITLE_ON_BARS), &imgui_sel_onlyDisplayMainTitleOnBars))
        {
            ConfigMgr::set("P", cfg::P_ONLY_DISPLAY_MAIN_TITLE_ON_BARS, imgui_sel_onlyDisplayMainTitleOnBars);
        }
        if (ImGui::Checkbox(i18n::c(DISABLE_PLAYMODE_ALL), &imgui_sel_disablePlaymodeAll))
        {
            ConfigMgr::set("P", cfg::P_DISABLE_PLAYMODE_ALL, imgui_sel_disablePlaymodeAll);
        }
        if (ImGui::Checkbox(i18n::c(DISABLE_DIFFICULTY_ALL), &imgui_sel_disableDifficultyAll))
        {
            ConfigMgr::set("P", cfg::P_DISABLE_DIFFICULTY_ALL, imgui_sel_disableDifficultyAll);
        }
        if (ImGui::Checkbox(i18n::c(DISABLE_PLAYMODE_SINGLE), &imgui_sel_disablePlaymodeSingle))
        {
            ConfigMgr::set("P", cfg::P_DISABLE_PLAYMODE_SINGLE, imgui_sel_disablePlaymodeSingle);
        }
        if (ImGui::Checkbox(i18n::c(DISABLE_PLAYMODE_DOUBLE), &imgui_sel_disablePlaymodeDouble))
        {
            ConfigMgr::set("P", cfg::P_DISABLE_PLAYMODE_DOUBLE, imgui_sel_disablePlaymodeDouble);
        }
        if (ImGui::Checkbox(i18n::c(IGNORE_DP_CHARTS), &imgui_sel_ignoreDPCharts))
        {
            ConfigMgr::set("P", cfg::P_IGNORE_DP_CHARTS, imgui_sel_ignoreDPCharts);
        }
        if (ImGui::Checkbox(i18n::c(IGNORE_9K_CHARTS), &imgui_sel_ignore9keys))
        {
            ConfigMgr::set("P", cfg::P_IGNORE_9KEYS_CHARTS, imgui_sel_ignore9keys);
        }
        if (ImGui::Checkbox(i18n::c(IGNORE_5K_IF_7K_EXISTS), &imgui_sel_ignore5keysif7keysexist))
        {
            ConfigMgr::set("P", cfg::P_IGNORE_5KEYS_IF_7KEYS_EXIST, imgui_sel_ignore5keysif7keysexist);
        }

        ImGui::EndChild();
    }
}

void SceneSelect::imguiPageAbout()
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
    yaasdf a.k.a. rustbell

Main Programmer:
    yaasdf a.k.a. rustbell
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
    冰兔
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

void SceneSelect::imguiPageExit()
{
    gNextScene = SceneType::EXIT_TRANS;
}



void SceneSelect::imguiRefreshProfileList()
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

void SceneSelect::imguiRefreshLanguageList()
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


void SceneSelect::imguiRefreshFolderList()
{
    imgui_folder_index = -1;
    imgui_folders.clear();
    imgui_folders_display.clear();

    auto folders = ConfigMgr::General()->getFoldersStr();
    imgui_folders.assign(folders.begin(), folders.end());
    for (const auto& f : imgui_folders)
        imgui_folders_display.push_back(f.c_str());
}

void SceneSelect::imguiRefreshTableList()
{
    imgui_table_index = -1;
    imgui_tables.clear();
    imgui_tables_display.clear();

    auto tables = ConfigMgr::General()->getTablesUrl();
    imgui_tables.assign(tables.begin(), tables.end());
    for (const auto& f : imgui_tables)
        imgui_tables_display.push_back(f.c_str());
}

void SceneSelect::imguiRefreshVideoDisplayResolutionList()
{
    auto oldRes = std::make_pair<unsigned, unsigned>(0, 0);
    if (!imgui_video_display_resolution_size.empty())
    {
        oldRes = imgui_video_display_resolution_size[imgui_video_display_resolution_index];
    }

    imgui_video_display_resolution_size.clear();
    imgui_video_display_resolution.clear();
    imgui_video_display_resolution_display.clear();

    if (imgui_video_mode == 1)
    {
        // get resolution list from system
        auto list = graphics_get_resolution_list();
        std::set<std::pair<int, int>> resolutions;
        for (auto& r : list)
        {
            int x = std::get<0>(r);
            int y = std::get<1>(r);
            if (x >= 640 && y >= 480)
            {
                resolutions.insert({ x, y });
            }
        }
        for (auto& r : resolutions)
        {
            imgui_video_display_resolution_size.push_back(r);
        }
    }
    else
    {
        // filter out resolutions bigger than desktop
        auto res = graphics_get_desktop_resolution();
        int w = res.first;
        int h = res.second;
        auto addResolution = [w, h, this](int dw, int dh)
        {
            if (w >= dw && h >= dh)
            {
                imgui_video_display_resolution_size.push_back({ dw, dh });
            }
        };
        addResolution(640, 480);
        addResolution(800, 600);
        addResolution(1280, 720);
        addResolution(1280, 960);
        addResolution(1366, 768);
        addResolution(1600, 900);
        addResolution(1920, 1080);
        addResolution(2560, 1440);
        addResolution(3840, 2160);
    }

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

    if (oldRes.first != 0)
    {
        for (int i = 0; i < (int)imgui_video_display_resolution_size.size(); i++)
        {
            auto& res = imgui_video_display_resolution_size[i];
            if (oldRes == res)
            {
                imgui_video_display_resolution_index = i;
                return;
            }
        }
    }
    imgui_video_display_resolution_index = 0;
}

void SceneSelect::imguiCheckSettings()
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
        pSkin->setHandleMouseEvents(true);
        gSelectContext.isGoingToReboot = true;
    }
}

bool SceneSelect::imguiApplyPlayerName()
{
    ConfigMgr::Profile()->set(cfg::P_PLAYERNAME, imgui_player_name_buf);
    State::set(IndexText::PLAYER_NAME, imgui_player_name_buf);
    return true;
}

bool SceneSelect::imguiAddFolder(const char* path)
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
        if (strnlen(path, 1) == 0) 
            return false;

        for (auto& f : imgui_folders)
        {
            if (f == path)
                return false;
        }
        
        imgui_folders.push_back(path);
        imgui_folders_display.push_back(imgui_folders.back().c_str());
        imgui_folder_index = -1;
        added = true;

        ConfigMgr::General()->setFolders(std::vector<std::string>(imgui_folders.begin(), imgui_folders.end()));
    }
    return added;
}

bool SceneSelect::imguiDelFolder()
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

bool SceneSelect::imguiBrowseFolder()
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

bool SceneSelect::imguiAddTable()
{
    bool added = false;

    if (strnlen(imgui_table_url_buf, 1) == 0)
        return false;

    for (auto& f : imgui_tables)
    {
        if (f == imgui_table_url_buf)
            return false;
    }

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

bool SceneSelect::imguiDelTable()
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

bool SceneSelect::imguiApplyResolution()
{
    const auto& [windowW, windowH] = imgui_video_display_resolution_size[imgui_video_display_resolution_index];
    auto [desktopW, desktopH] = graphics_get_desktop_resolution();

    if (imgui_video_mode == 2 && windowW == desktopW && windowH == desktopH)
    {
        graphics_change_window_mode(3);
        graphics_resize_window(windowW, windowH);
    }
    else
    {
        graphics_change_window_mode(imgui_video_mode);
        graphics_resize_window(windowW, windowH);
    }
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
            {0, "WINDOW"},
            {1, "FULL"},
            {2, "BORDERLESS"},
        };

        auto&& s = imgui_video_mode;
        if (smap.find(s) != smap.end())
            State::set(IndexText::WINDOWMODE, smap.at(s));
        else
            State::set(IndexText::WINDOWMODE, "WINDOW");
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

bool SceneSelect::imguiRefreshAudioDevices()
{
    imgui_audio_device_index = -1;
    imgui_audio_devices.clear();
    imgui_audio_devices_display.clear();
    auto adev = ConfigMgr::get('A', cfg::A_DEVNAME, "");

    auto devList = SoundMgr::getDeviceList();
    for (auto& d : devList)
    {
        if (adev == d.second)
        {
            imgui_audio_device_index = (int)imgui_audio_devices.size();
        }

        if (d.first == DriverIDUnknownASIO)
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

bool SceneSelect::imguiApplyAudioSettings()
{
    bool ret;
    ConfigMgr::set("A", cfg::A_BUFCOUNT, imgui_audio_bufferCount);
    ConfigMgr::set("A", cfg::A_BUFLEN, imgui_audio_bufferSize);
    if (SoundMgr::setDevice(imgui_audio_device_index) == 0)
    {
        const auto mode = std::next(imgui_audio_devices.begin(), imgui_audio_device_index);
        ConfigMgr::set('A', cfg::A_DEVNAME, mode->second);
        ConfigMgr::set('A', cfg::A_MODE, mode->first < 0 ? cfg::A_MODE_ASIO : cfg::A_MODE_AUTO);

        SoundMgr::stopSysSamples();
        SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);
        
        ret = true;
    }
    else
    {
        ret = false;
    }

    auto [count, size] = SoundMgr::getDSPBufferSize();
    ConfigMgr::set("A", cfg::A_BUFCOUNT, count);
    ConfigMgr::set("A", cfg::A_BUFLEN, size);
    imgui_audio_bufferCount = count;
    imgui_audio_bufferSize = size;

    return ret;
}


#include "game/arena/arena_data.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

bool SceneSelect::imguiArenaJoinLobbyPrompt()
{
    if (imgui_show_arenaJoinLobbyPrompt)
    {
        using namespace i18nText;
        bool ok = false;

        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ConfigMgr::get('V', cfg::V_DISPLAY_RES_X, CANVAS_WIDTH), ConfigMgr::get('V', cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT)), ImGuiCond_Always);
        if (ImGui::Begin("Join Lobby Main", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::OpenPopup("JOIN LOBBY");

            ImGui::SetNextWindowSize(ImVec2(480.f, 180.f), ImGuiCond_Always);
            if (ImGui::BeginPopupModal("JOIN LOBBY", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
            {
                ImGui::Text("IP Address");
                ImGui::SameLine();
                if (ImGui::IsWindowAppearing())
                    ImGui::SetKeyboardFocusHere();
                ok |= ImGui::InputText("##joinlobbyaddr", imgui_arena_address_buf, sizeof(imgui_arena_address_buf), ImGuiInputTextFlags_EnterReturnsTrue);

                ok |= ImGui::Button(i18n::c(OK));

                if (ok)
                {
                    ImGui::CloseCurrentPopup();
                    imgui_show_arenaJoinLobbyPrompt = false;
                }

                ImGui::SameLine();
                if (ImGui::Button(i18n::c(CANCEL)))
                {
                    ImGui::CloseCurrentPopup();
                    imgui_show_arenaJoinLobbyPrompt = false;
                    pSkin->setHandleMouseEvents(true);
                }

                ImGui::EndPopup();
            }
            ImGui::End();
        }

        if (ok)
        {
            imgui_arena_joinLobby = true;
        }

        return true;
    }
    return false;
}