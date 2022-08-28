#include "scene_select.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "imgui.h"

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

void SceneSelect::_imguiInit()
{
    _imguiRefreshProfileList();
    old_profile_index = imgui_profile_index;

    _imguiRefreshFolderList();
    _imguiRefreshTableList();

    _imguiRefreshVideoResolutionList();
    auto resolutionY = ConfigMgr::get("V", cfg::V_RES_Y, CANVAS_HEIGHT);
    auto ss = ConfigMgr::get("V", cfg::V_RES_SUPERSAMPLE, 1);
    imgui_video_resolution_index = 0;
    switch (resolutionY)
    {
    case 480:
    default:
        switch (ss)
        {
        case 1: imgui_video_resolution_index = 0; break;
        case 2: imgui_video_resolution_index = 3; break;
        case 3: imgui_video_resolution_index = 4; break;
        }
        break;
    case 720:
        switch (ss)
        {
        case 1: imgui_video_resolution_index = 1; break;
        case 2: imgui_video_resolution_index = 5; break;
        case 3: imgui_video_resolution_index = 6; break;
        }
        break;
    case 1080:
        switch (ss)
        {
        case 1: imgui_video_resolution_index = 2; break;
        case 2: imgui_video_resolution_index = 7; break;
        }
        break;
    }
    old_video_resolution_index = imgui_video_resolution_index;

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

    imgui_video_vsync = ConfigMgr::get("V", cfg::V_VSYNC, false);
    imgui_video_maxFPS = ConfigMgr::get("V", cfg::V_MAXFPS, 240);

    imgui_audio_checkASIODevices = ConfigMgr::get("A", cfg::A_MODE, cfg::A_MODE_ASIO) == cfg::A_MODE_ASIO;
    imgui_audio_listASIODevices = imgui_audio_checkASIODevices;
    imgui_audio_bufferCount = ConfigMgr::get("A", cfg::A_BUFCOUNT, 2);
    imgui_audio_bufferSize = ConfigMgr::get("A", cfg::A_BUFLEN, 256);
    _imguiRefreshAudioDevices();
    old_audio_device_index = imgui_audio_device_index;

    imgui_adv_scrollSpeed[0] = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
    imgui_adv_scrollSpeed[1] = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
    imgui_adv_missBGATime = ConfigMgr::get("P", cfg::P_MISSBGA_LENGTH, 500);
    imgui_adv_minInputInterval = ConfigMgr::get("P", cfg::P_MIN_INPUT_INTERVAL, 16);
    imgui_adv_newSongDuration = ConfigMgr::get("P", cfg::P_NEW_SONG_DURATION, 24);
    imgui_adv_mouseAnalog = ConfigMgr::get("P", cfg::P_MOUSE_ANALOG, false);

    // auto popup settings for first runs
    if (imgui_folders.empty())
    {
        imguiShow = true;
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

    if (imguiShow)
    {
        if (ImGui::Begin("Settings (F9)", NULL, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(500, 450), ImGuiCond_Once);
            ImGui::PushItemWidth(ImGui::GetFontSize() * -20);

            if (ImGui::CollapsingHeader("General"))
            {
                ImGui::Combo("Profile", &imgui_profile_index, imgui_profiles_display.data(), (int)imgui_profiles_display.size());

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Folder"))
            {
                ImGui::Text("*Changing folder settings requires a restart now.");

                ImGui::ListBox("Folders", &imgui_folder_index, imgui_folders_display.data(), imgui_folders_display.size());

                if (ImGui::Button("Add..."))
                {
                    _imguiAddFolder();
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete Selected"))
                {
                    _imguiDelFolder();
                }
                ImGui::SameLine();
                if (ImGui::Button("Browse Selected"))
                {
                    _imguiBrowseFolder();
                }

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Table"))
            {
                ImGui::Text("*Changing table settings requires a restart now.");

                ImGui::ListBox("Tables", &imgui_table_index, imgui_tables_display.data(), imgui_tables_display.size());

                if (ImGui::Button("Add..."))
                {
                    imgui_table_popup = true;
                    ImGui::OpenPopup("Input table URL");
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete Selected"))
                {
                    _imguiDelTable();
                }
                ImGui::SameLine();
                if (ImGui::Button("Update Selected"))
                {
                    _imguiUpdateTable();
                }

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Video"))
            {
                ImGui::Combo("Internal Resolution", &imgui_video_resolution_index, imgui_video_resolution_display.data(), (int)imgui_video_resolution_display.size());
                ImGui::Combo("Display Resolution", &imgui_video_display_resolution_index, imgui_video_display_resolution_display.data(), (int)imgui_video_display_resolution_display.size());

                ImGui::RadioButton("Windowed", &imgui_video_mode, 0);
                ImGui::SameLine();
                ImGui::RadioButton("FullScreen", &imgui_video_mode, 1);
                ImGui::SameLine();
                ImGui::RadioButton("Borderless", &imgui_video_mode, 2);
                ImGui::SameLine();
                ImGui::Checkbox("VSync", &imgui_video_vsync);
                ImGui::SameLine();
                ImGui::Spacing();

                ImGui::InputInt("Max FPS", &imgui_video_maxFPS, 0);

                if (ImGui::Button("Apply"))
                {
                    _imguiApplyResolution();

                    if (imgui_video_maxFPS < 30 && imgui_video_maxFPS != 0)
                    {
                        imgui_video_maxFPS = 30;
                    }
                    if (imgui_video_maxFPS != ConfigMgr::get("V", cfg::V_MAXFPS, 240))
                    {
                        ConfigMgr::set("V", cfg::V_MAXFPS, imgui_video_maxFPS);
                        graphics_set_maxfps(imgui_video_maxFPS);
                    }
                }

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Audio"))
            {
                ImGui::Combo("Device", &imgui_audio_device_index, imgui_audio_devices_display.data(), (int)imgui_audio_devices_display.size());

                ImGui::Checkbox("Check ASIO Drivers", &imgui_audio_checkASIODevices);
                ImGui::SameLine();
                if (ImGui::Button("Refresh"))
                {
                    imgui_audio_listASIODevices = imgui_audio_checkASIODevices;
                    _imguiRefreshAudioDevices();
                }

                ImGui::Spacing();

                ImGui::InputInt("Buffer Count", &imgui_audio_bufferCount, 0);
                ImGui::InputInt("Buffer Size", &imgui_audio_bufferSize, 0);

                if (ImGui::Button("Apply"))
                {
                    _imguiApplyAudioSettings();
                }

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Play"))
            {
                ImGui::InputInt("Miss BGA time (ms)", &imgui_adv_missBGATime, 0);
                ImGui::InputInt("Min input interval (ms)", &imgui_adv_minInputInterval, 1, 10);
                //ImGui::Checkbox("Accept mouse movements as Analog input", &imgui_adv_mouseAnalog);

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Advanced"))
            {
                ImGui::InputInt2("Song select scroll speed (ms)", imgui_adv_scrollSpeed);
                ImGui::InputInt("New song duration (hour)", &imgui_adv_newSongDuration, 1, 10);

                ImGui::Separator();
                ImGui::Spacing();
            }

        }
        if (imgui_table_popup)
        {
            if (ImGui::BeginPopupModal("Input table URL", nullptr, ImGuiWindowFlags_NoCollapse))
            {
                if (ImGui::InputText("URL", imgui_table_url_buf, sizeof(imgui_table_url_buf), ImGuiInputTextFlags_EnterReturnsTrue)
                    || ImGui::Button("OK"))
                {
                    _imguiAddTable();
                    memset(imgui_table_url_buf, 0, sizeof(imgui_table_url_buf));
                    ImGui::CloseCurrentPopup();
                    imgui_table_popup = false;
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    memset(imgui_table_url_buf, 0, sizeof(imgui_table_url_buf));
                    ImGui::CloseCurrentPopup();
                    imgui_table_popup = false;
                }
            }
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    _imguiCheckSettings();
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

void SceneSelect::_imguiRefreshVideoResolutionList()
{
    imgui_video_resolution.clear();
    imgui_video_resolution_display.clear();

    imgui_video_resolution.push_back("480p SD (640x480)");
    imgui_video_resolution.push_back("720p HD (1280x720)");
    imgui_video_resolution.push_back("1080p FHD (1920x1080)");
    imgui_video_resolution.push_back("480p SD 2xSS (1280x960)");
    imgui_video_resolution.push_back("480p SD 3xSS (1920x1440)");
    imgui_video_resolution.push_back("720p HD 2xSS (2560x1440)");
    imgui_video_resolution.push_back("720p HD 3xSS (3840x2160)");
    imgui_video_resolution.push_back("1080p FHD 2xSS (3840x2160)");
    //imgui_video_resolution.push_back("2K WQHD (2560x1440)");
    //imgui_video_resolution.push_back("4K UHD (3840x2160)");
    for (const auto& r : imgui_video_resolution)
    {
        imgui_video_resolution_display.push_back(r.c_str());
    }
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
    if (imgui_profile_index != old_profile_index)
    {
        // TODO reload profile
    }

    if (imgui_video_resolution_index != old_video_resolution_index)
    {
        old_video_resolution_index = imgui_video_resolution_index;
        int x, y, ss;
        switch (imgui_video_resolution_index)
        {
        case 0: x = 640;  y = 480;  ss = 1; break;
        case 1: x = 1280; y = 720;  ss = 1; break;
        case 2: x = 1920; y = 1080; ss = 1; break;
        case 3: x = 640;  y = 480;  ss = 2; break;
        case 4: x = 640;  y = 480;  ss = 3; break;
        case 5: x = 1280; y = 720;  ss = 2; break;
        case 6: x = 1280; y = 720;  ss = 3; break;
        case 7: x = 1920; y = 1080; ss = 2; break;
        }
        ConfigMgr::set("V", cfg::V_RES_X, x);
        ConfigMgr::set("V", cfg::V_RES_Y, y);
        ConfigMgr::set("V", cfg::V_RES_SUPERSAMPLE, ss);
    }
    if (imgui_video_display_resolution_index != old_video_display_resolution_index)
    {
        old_video_display_resolution_index = imgui_video_display_resolution_index;
        if (imgui_video_display_resolution_index > (int)imgui_video_display_resolution_size.size())
        {
            imgui_video_display_resolution_index = 0;
        }
        auto& res = imgui_video_display_resolution_size[imgui_video_display_resolution_index];
        int x, y;
        x = res.first;
        y = res.second;
        ConfigMgr::set("V", cfg::V_DISPLAY_RES_X, x);
        ConfigMgr::set("V", cfg::V_DISPLAY_RES_Y, y);
    }
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
    if (imgui_video_vsync != ConfigMgr::get("V", cfg::V_VSYNC, false))
    {
        ConfigMgr::set("V", cfg::V_VSYNC, imgui_video_vsync);
    }

    if (imgui_audio_device_index != old_audio_device_index && imgui_audio_devices_display[imgui_audio_device_index] != ConfigMgr::get('A', cfg::A_DEVNAME, ""))
    {
        // TODO recreate audio device
    }
    if (imgui_audio_bufferCount != ConfigMgr::get("A", cfg::A_BUFCOUNT, 2))
    {
        ConfigMgr::set("A", cfg::A_BUFCOUNT, imgui_audio_bufferCount);
    }
    if (imgui_audio_bufferSize != ConfigMgr::get("A", cfg::A_BUFLEN, 256))
    {
        ConfigMgr::set("A", cfg::A_BUFLEN, imgui_audio_bufferSize);
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
    }
    if (imgui_adv_newSongDuration != ConfigMgr::get("P", cfg::P_NEW_SONG_DURATION, 24))
    {
        ConfigMgr::set("P", cfg::P_NEW_SONG_DURATION, imgui_adv_newSongDuration);
    }
    if (imgui_adv_mouseAnalog != ConfigMgr::get("P", cfg::P_MOUSE_ANALOG, false))
    {
        ConfigMgr::set("P", cfg::P_MOUSE_ANALOG, imgui_adv_mouseAnalog);
    }

}

bool SceneSelect::_imguiAddFolder()
{
    bool added = false;

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

            // TODO reload?
        }
        CoTaskMemFree(lpiil);
    }
#endif
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

    return false;
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

        _imguiUpdateTable();
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

    return false;
}

bool SceneSelect::_imguiUpdateTable()
{
    if (imgui_table_index < 0 || imgui_table_index >= imgui_tables_display.size()) return false;

    // TODO call update table

    return true;
}

bool SceneSelect::_imguiApplyResolution()
{
    int renderW, renderH;
    int windowW, windowH;
    windowW = ConfigMgr::get("V", cfg::V_DISPLAY_RES_X, CANVAS_WIDTH);
    windowH = ConfigMgr::get("V", cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT);
    renderW = ConfigMgr::get("V", cfg::V_RES_X, CANVAS_WIDTH);
    renderH = ConfigMgr::get("V", cfg::V_RES_Y, CANVAS_HEIGHT);
    int ss = ConfigMgr::get("V", cfg::V_RES_SUPERSAMPLE, 1);

    graphics_change_window_mode(imgui_video_mode);
    graphics_resize_window(windowW, windowH);
    graphics_set_supersample_level(ss);
    graphics_resize_canvas(renderW, renderH);
    graphics_change_vsync(imgui_video_vsync);

    return true;
}

bool SceneSelect::_imguiRefreshAudioDevices()
{
    imgui_audio_device_index = -1;
    imgui_audio_devices.clear();
    imgui_audio_devices_display.clear();
    auto adev = ConfigMgr::get('A', cfg::A_DEVNAME, "");

    auto devList = SoundMgr::getDeviceList(imgui_audio_checkASIODevices);
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
    if (SoundMgr::setDevice(imgui_audio_device_index, imgui_audio_listASIODevices) == 0)
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
