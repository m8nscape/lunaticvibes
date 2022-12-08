#include "scene_pre_select.h"
#include "config/config_mgr.h"
#include "scene_context.h"
#include "common/coursefile/lr2crs.h"
#include "common/entry/entry_table.h"
#include "common/entry/entry_course.h"
#include "common/entry/entry_arena.h"
#include "imgui.h"
#include <future>
#include <boost/format.hpp>
#include "game/runtime/i18n.h"
#include "git_version.h"

ScenePreSelect::ScenePreSelect(): vScene(eMode::PRE_SELECT, 240)
{
	_updateCallback = std::bind(&ScenePreSelect::updateLoadSongs, this);

    rootFolderProp = SongListProperties{
        "",
        ROOT_FOLDER_HASH,
        "",
        {},
        {},
        0
    };

    graphics_set_maxfps(30);

    if (gNextScene == eScene::PRE_SELECT)
    {
        textHint = i18n::s(i18nText::INITIALIZING);
    }
}

ScenePreSelect::~ScenePreSelect()
{
    if (startedLoadSong && loadSongEnd.valid())
    {
    }
}

void ScenePreSelect::_updateAsync()
{
    if (gNextScene != eScene::PRE_SELECT && gNextScene != eScene::SELECT) return;

    if (gAppIsExiting)
    {
        gNextScene = eScene::EXIT_TRANS;
        g_pSongDB->stopLoading();
    }

	_updateCallback();
}

void ScenePreSelect::updateLoadSongs()
{
    if (!startedLoadSong)
    {
        startedLoadSong = true;

        // wait for Initializing... text to draw
        pushAndWaitMainThreadTask<void>([] {});

        // wait for another frame
        pushAndWaitMainThreadTask<void>([] {});


        // load files
        loadSongEnd = std::async(std::launch::async, [&]() {

            textHint = i18n::s(i18nText::CHECKING_FOLDERS);

            loadSongTimer = std::chrono::system_clock::now();

            // get folders from config
            auto folderList = ConfigMgr::General()->getFoldersPath();
            std::vector<Path> pathList;
            for (auto& f : folderList)
            {
                pathList.push_back(Path(f));
            }

            g_pSongDB->addFolders(pathList);

            auto top = g_pSongDB->browse(ROOT_FOLDER_HASH, false);
            for (size_t i = 0; i < top.getContentsCount(); ++i)
            {
                auto entry = top.getEntry(i);

                bool deleted = true;
                for (auto& f : folderList)
                {
                    if (fs::exists(f) && fs::exists(entry->getPath()) && fs::equivalent(f, entry->getPath()))
                    {
                        deleted = false;
                        break;
                    }
                }
                if (!deleted)
                {
                    g_pSongDB->browse(entry->md5, true);
                    rootFolderProp.dbBrowseEntries.push_back({ entry, nullptr });
                }
            }

            // NEW SONG
            auto newSongList = g_pSongDB->findChartFromTime(ROOT_FOLDER_HASH,
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - State::get(IndexNumber::NEW_ENTRY_SECONDS));
            if (!newSongList.empty())
            {
                size_t index = 0;
                std::shared_ptr<EntryFolderNewSong> entry = std::make_shared<EntryFolderNewSong>("NEW SONGS");
                for (auto& c : newSongList)
                {
                    //std::shared_ptr<EntryFolderSong> f = std::make_shared<EntryFolderSong>(HashMD5((boost::format("%032lu") % index++).str()), "", c->title, c->title2);
                    //f->pushChart(c);
                    //entry->pushEntry(f);
                    entry->pushEntry(std::make_shared<EntryFolderSong>(c));
                }
                rootFolderProp.dbBrowseEntries.insert(rootFolderProp.dbBrowseEntries.begin(), {entry, nullptr});
            }

            // ARENA
            if (!rootFolderProp.dbBrowseEntries.empty())
            {
                std::shared_ptr<EntryFolderArena> entry = std::make_shared<EntryFolderArena>();

                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::HOST_LOBBY, "HOST LOBBY"));
                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::JOIN_LOBBY, "JOIN LOBBY"));
                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::LEAVE_LOBBY, "LEAVE LOBBY"));

                // TODO load lobby list from file

                rootFolderProp.dbBrowseEntries.push_back({ entry, nullptr });
            }

            });
    }

    if (g_pSongDB->addChartTaskFinishCount != prevChartLoaded)
    {
        std::shared_lock l(g_pSongDB->addCurrentPathMutex);

        prevChartLoaded = g_pSongDB->addChartTaskFinishCount;
        textHint = (
            boost::format(i18n::c(i18nText::LOADING_CHARTS))
                % g_pSongDB->addChartTaskFinishCount
                % g_pSongDB->addChartTaskCount
            ).str();
        textHint2 = g_pSongDB->addCurrentPath;
    }
    
    if (loadSongEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        g_pSongDB->transactionStop();
        loadSongEnd.get();
        _updateCallback = std::bind(&ScenePreSelect::updateLoadTables, this);
    }
}

void ScenePreSelect::updateLoadTables()
{
    if (!startedLoadTable)
    {
        startedLoadTable = true;

        loadTableEnd = std::async(std::launch::async, [&]() {

            textHint = i18n::s(i18nText::CHECKING_TABLES);

            // initialize table list
            auto tableList = ConfigMgr::General()->getTablesUrl();
            for (auto& tableUrl : tableList)
            {
                LOG_INFO << "[List] Add table " << tableUrl;
                textHint2 = tableUrl;

                gSelectContext.tables.emplace_back();
                DifficultyTableBMS& t = gSelectContext.tables.back();
                t.setUrl(tableUrl);

                auto convertTable = [&](DifficultyTableBMS& t)
                {
                    std::shared_ptr<EntryFolderTable> tbl = std::make_shared<EntryFolderTable>(t.getName(), "");
                    size_t index = 0;
                    for (const auto& lv : t.getLevelList())
                    {
                        std::string folderName = (boost::format("%s%s") % t.getSymbol() % lv).str();
                        std::shared_ptr<EntryFolderTable> tblLevel = std::make_shared<EntryFolderTable>(folderName, "");
                        for (const auto& r : t.getEntryList(lv))
                        {
                            auto charts = g_pSongDB->findChartByHash(r->md5);
                            bool added = false;
                            for (auto& c : charts)
                            {
                                if (fs::exists(c->absolutePath))
                                {
                                    //std::shared_ptr<EntryFolderSong> f = std::make_shared<EntryFolderSong>(HashMD5((boost::format("%032lu") % index++).str()), "", c->title, c->title2);
                                    //f->pushChart(c);
                                    //tblLevel->pushEntry(f);
                                    tblLevel->pushEntry(std::make_shared<EntryFolderSong>(c));
                                    added = true;
                                    break;
                                }
                            }
                            /*
                            if (!added)
                            {
                                std::shared_ptr<FolderSong> f = std::make_shared<FolderSong>(HashMD5(), "", r->_name, r->_name2);
                                f->pushChart(c);
                                tblLevel->pushEntry(f);
                                added = true;
                                break;
                            }
                            */
                        }
                        tbl->pushEntry(tblLevel);
                    }
                    return tbl;
                };

                textHint = (boost::format(i18n::c(i18nText::LOADING_TABLE)) % t.getUrl()).str();
                textHint2 = "";

                if (t.loadFromFile())
                {
                    rootFolderProp.dbBrowseEntries.push_back({ convertTable(t), nullptr });
                }
                else
                {
                    textHint2 = i18n::s(i18nText::DOWNLOADING_TABLE);

                    t.updateFromUrl([&](DifficultyTable::UpdateResult result)
                        {
                            if (result == DifficultyTable::UpdateResult::OK)
                            {
                                rootFolderProp.dbBrowseEntries.push_back({ convertTable(t), nullptr });
                            }
                            else
                            {
                                switch (result)
                                {
                                case DifficultyTable::UpdateResult::INTERNAL_ERROR:         LOG_WARNING << "[List] Update table " << tableUrl << " failed: INTERNAL_ERROR";      break;
                                case DifficultyTable::UpdateResult::WEB_PATH_ERROR:         LOG_WARNING << "[List] Update table " << tableUrl << " failed: WEB_PATH_ERROR";      break;
                                case DifficultyTable::UpdateResult::WEB_CONNECT_ERR:        LOG_WARNING << "[List] Update table " << tableUrl << " failed: WEB_CONNECT_ERR";     break;
                                case DifficultyTable::UpdateResult::WEB_TIMEOUT:            LOG_WARNING << "[List] Update table " << tableUrl << " failed: WEB_TIMEOUT";         break;
                                case DifficultyTable::UpdateResult::WEB_PARSE_FAILED:       LOG_WARNING << "[List] Update table " << tableUrl << " failed: WEB_PARSE_FAILED";    break;
                                case DifficultyTable::UpdateResult::HEADER_PATH_ERROR:      LOG_WARNING << "[List] Update table " << tableUrl << " failed: HEADER_PATH_ERROR";   break;
                                case DifficultyTable::UpdateResult::HEADER_CONNECT_ERR:     LOG_WARNING << "[List] Update table " << tableUrl << " failed: HEADER_CONNECT_ERR";  break;
                                case DifficultyTable::UpdateResult::HEADER_TIMEOUT:         LOG_WARNING << "[List] Update table " << tableUrl << " failed: HEADER_TIMEOUT";      break;
                                case DifficultyTable::UpdateResult::HEADER_PARSE_FAILED:    LOG_WARNING << "[List] Update table " << tableUrl << " failed: HEADER_PARSE_FAILED"; break;
                                case DifficultyTable::UpdateResult::DATA_PATH_ERROR:        LOG_WARNING << "[List] Update table " << tableUrl << " failed: DATA_PATH_ERROR";     break;
                                case DifficultyTable::UpdateResult::DATA_CONNECT_ERR:       LOG_WARNING << "[List] Update table " << tableUrl << " failed: DATA_CONNECT_ERR";    break;
                                case DifficultyTable::UpdateResult::DATA_TIMEOUT:           LOG_WARNING << "[List] Update table " << tableUrl << " failed: DATA_TIMEOUT";        break;
                                case DifficultyTable::UpdateResult::DATA_PARSE_FAILED:      LOG_WARNING << "[List] Update table " << tableUrl << " failed: DATA_PARSE_FAILED1";  break;
                                }
                            }
                        });
                }
            }
            });
    }

    if (loadTableEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        loadTableEnd.get();
        _updateCallback = std::bind(&ScenePreSelect::updateLoadCourses, this);
    }
}

void ScenePreSelect::updateLoadCourses()
{
    if (!startedLoadCourse)
    {
        startedLoadCourse = true;

        loadCourseEnd = std::async(std::launch::async, [&]() {

            textHint = i18n::s(i18nText::LOADING_COURSES);

            std::map<EntryCourse::CourseType, std::vector<std::shared_ptr<EntryCourse>>> courses;

            // initialize table list
            Path coursePath = Path(GAMEDATA_PATH) / "courses";
            if (!fs::exists(coursePath))
                fs::create_directories(coursePath);
            for (auto& courseFile : fs::recursive_directory_iterator(coursePath))
            {
                if (!(fs::is_regular_file(courseFile) && strEqual(courseFile.path().extension().u8string(), ".lr2crs", true)))
                    continue;

                Path coursePath = courseFile.path();
                LOG_INFO << "[List] Add course file: " << coursePath.u8string();
                textHint2 = coursePath.u8string();

                CourseLr2crs lr2crs(coursePath);
                for (auto& c : lr2crs.courses)
                {
                    std::shared_ptr<EntryCourse> entry = std::make_shared<EntryCourse>(c, lr2crs.addTime);
                    if (entry->courseType != EntryCourse::UNDEFINED)
                        courses[entry->courseType].push_back(entry);
                }
            }

            for (auto& [type, courses] : courses)
            {
                if (courses.empty()) continue;

                std::string folderTitle = i18n::s(i18nText::COURSE_TITLE);
                std::string folderTitle2 = i18n::s(i18nText::COURSE_SUBTITLE);
                switch (type)
                {
                case EntryCourse::CourseType::GRADE:
                    folderTitle = i18n::s(i18nText::CLASS_TITLE);
                    folderTitle2 = i18n::s(i18nText::CLASS_SUBTITLE);
                    break;
                }

                auto folder = std::make_shared<EntryFolderCourse>(folderTitle, folderTitle2);
                for (auto& c : courses)
                {
                    folder->pushEntry(c);
                }
                rootFolderProp.dbBrowseEntries.push_back({ folder, nullptr });
            }

            });
    }

    if (loadCourseEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        loadCourseEnd.get();
        _updateCallback = std::bind(&ScenePreSelect::loadFinished, this);
    }
}

void ScenePreSelect::loadFinished()
{
    if (!loadingFinished)
    {
        while (!gSelectContext.backtrace.empty())
            gSelectContext.backtrace.pop();
        gSelectContext.backtrace.push(rootFolderProp);

        if (rootFolderProp.dbBrowseEntries.empty())
        {
            State::set(IndexText::PLAY_TITLE, i18n::s(i18nText::BMS_NOT_FOUND));
            State::set(IndexText::PLAY_ARTIST, i18n::s(i18nText::BMS_NOT_FOUND_HINT));
        }
        if (gNextScene == eScene::PRE_SELECT)
        {
            textHint = (boost::format("%s %s %s (%s %s)")
                % PROJECT_NAME % PROJECT_VERSION
#if _DEBUG
                % "Debug"
#else
                % ""
#endif
                % GIT_BRANCH % GIT_COMMIT
                ).str();
            textHint2 = i18n::s(i18nText::PLEASE_WAIT);
        }

        // wait for updated text to draw
        pushAndWaitMainThreadTask<void>([] {});

        // wait for another frame
        pushAndWaitMainThreadTask<void>([] {});

        int maxFPS = ConfigMgr::get("V", cfg::V_MAXFPS, 480);
        if (maxFPS < 30 && maxFPS != 0)
            maxFPS = 30;
        graphics_set_maxfps(maxFPS);

        gNextScene = eScene::SELECT;
        loadingFinished = true;
    }
}


void ScenePreSelect::_updateImgui()
{
    if (gInCustomize) return;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ConfigMgr::get('V', cfg::V_DISPLAY_RES_X, CANVAS_WIDTH), ConfigMgr::get('V', cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT)), ImGuiCond_Always);
    if (ImGui::Begin("LoadSong", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text(textHint.c_str());
        ImGui::Text(textHint2.c_str());

        ImGui::End();
    }
}


bool ScenePreSelect::isLoadingFinished() const
{
    return loadingFinished;
}
