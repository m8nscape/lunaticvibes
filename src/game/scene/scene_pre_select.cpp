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

ScenePreSelect::ScenePreSelect(): SceneBase(SkinType::PRE_SELECT, 240)
{
	_updateCallback = std::bind(&ScenePreSelect::updateLoadSongs, this);

    rootFolderProp = SongListProperties{
        {},
        ROOT_FOLDER_HASH,
        "",
        {},
        {},
        0
    };

    graphics_set_maxfps(30);

    LOG_INFO << "[List] ------------------------------------------------------------";

    if (gNextScene == SceneType::PRE_SELECT)
    {
        // score db
        LOG_INFO << "[List] Initializing score.db...";
        std::string scoreDBPath = (ConfigMgr::Profile()->getPath() / "score.db").u8string();
        g_pScoreDB = std::make_shared<ScoreDB>(scoreDBPath.c_str());
        g_pScoreDB->preloadScore();

        // song db
        LOG_INFO << "[List] Initializing song.db...";
        Path dbPath = Path(GAMEDATA_PATH) / "database";
        if (!fs::exists(dbPath)) fs::create_directories(dbPath);
        g_pSongDB = std::make_shared<SongDB>(dbPath / "song.db");

        std::unique_lock l(gSelectContext._mutex);
        gSelectContext.entries.clear();
        gSelectContext.backtrace.clear();

        textHint = i18n::s(i18nText::INITIALIZING);
    }

    LOG_INFO << "[List] ------------------------------------------------------------";
}

ScenePreSelect::~ScenePreSelect()
{
    if (startedLoadSong && loadSongEnd.valid())
    {
    }
}

void ScenePreSelect::_updateAsync()
{
    if (gNextScene != SceneType::PRE_SELECT && gNextScene != SceneType::SELECT) return;

    if (gAppIsExiting)
    {
        gNextScene = SceneType::EXIT_TRANS;
        g_pSongDB->stopLoading();
    }

	_updateCallback();
}

void ScenePreSelect::updateLoadSongs()
{
    if (!startedLoadSong)
    {
        startedLoadSong = true;
        LOG_INFO << "[List] Start loading songs...";

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

            LOG_INFO << "[List] Refreshing folders...";
            g_pSongDB->initializeFolders(pathList);
            LOG_INFO << "[List] Refreshing folders complete.";

            LOG_INFO << "[List] Building song list cache...";
            g_pSongDB->prepareCache();
            LOG_INFO << "[List] Building song list cache finished.";

            LOG_INFO << "[List] Generating root folders...";
            auto top = g_pSongDB->browse(ROOT_FOLDER_HASH, false);
            if (top && !top->empty())
            {
                for (size_t i = 0; i < top->getContentsCount(); ++i)
                {
                    auto entry = top->getEntry(i);

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
            }
            LOG_INFO << "[List] Added " << rootFolderProp.dbBrowseEntries.size() << " root folders";

            g_pSongDB->optimize();

            // NEW SONG
            LOG_INFO << "[List] Generating NEW SONG folder...";
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
            LOG_INFO << "[List] NEW SONG folder has " << newSongList.size() << " entries";

            // ARENA
            LOG_INFO << "[List] Generating ARENA folder...";
            if (!rootFolderProp.dbBrowseEntries.empty())
            {
                std::shared_ptr<EntryFolderArena> entry = std::make_shared<EntryFolderArena>(i18n::s(i18nText::ARENA_FOLDER_TITLE), i18n::s(i18nText::ARENA_FOLDER_SUBTITLE));

                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::HOST_LOBBY, i18n::s(i18nText::ARENA_HOST), i18n::s(i18nText::ARENA_HOST_DESCRIPTION)));
                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::JOIN_LOBBY, i18n::s(i18nText::ARENA_JOIN), i18n::s(i18nText::ARENA_JOIN_DESCRIPTION)));
                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::LEAVE_LOBBY, i18n::s(i18nText::ARENA_LEAVE), i18n::s(i18nText::ARENA_LEAVE_DESCRIPTION)));

                // TODO load lobby list from file

                rootFolderProp.dbBrowseEntries.push_back({ entry, nullptr });
            }
            LOG_INFO << "[List] ARENA has " << 0 << " known hosts (placeholder)";

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
        g_pSongDB->waitLoadingFinish();
        loadSongEnd.get();
        LOG_INFO << "[List] Loading songs complete.";
        LOG_INFO << "[List] ------------------------------------------------------------";

        _updateCallback = std::bind(&ScenePreSelect::updateLoadTables, this);
    }
}

void ScenePreSelect::updateLoadTables()
{
    if (!startedLoadTable)
    {
        startedLoadTable = true;
        LOG_INFO << "[List] Start loading tables...";

        loadTableEnd = std::async(std::launch::async, [&]() {

            textHint = i18n::s(i18nText::CHECKING_TABLES);

            // initialize table list
            auto tableList = ConfigMgr::General()->getTablesUrl();
            size_t tableIndex = 0;
            for (auto& tableUrl : tableList)
            {
                LOG_INFO << "[List] Add table " << tableUrl;
                textHint2 = tableUrl;

                gSelectContext.tables.emplace_back();
                DifficultyTableBMS& t = gSelectContext.tables.back();
                t.setUrl(tableUrl);

                auto convertTable = [&](DifficultyTableBMS& t)
                {
                    std::shared_ptr<EntryFolderTable> tbl = std::make_shared<EntryFolderTable>(t.getName(), tableIndex);
                    size_t levelIndex = 0;
                    for (const auto& lv : t.getLevelList())
                    {
                        std::string folderName = (boost::format("%s%s") % t.getSymbol() % lv).str();
                        std::shared_ptr<EntryFolderTable> tblLevel = std::make_shared<EntryFolderTable>(folderName, levelIndex);
                        for (const auto& r : t.getEntryList(lv))
                        {
                            auto charts = g_pSongDB->findChartByHash(r->md5, false);
                            bool added = false;
                            for (auto& c : charts)
                            {
                                if (fs::exists(c->absolutePath))
                                {
                                    tblLevel->pushEntry(std::make_shared<EntryFolderSong>(c));
                                    added = true;
                                    break;
                                }
                            }
                        }
                        tbl->pushEntry(tblLevel);
                        levelIndex += 1;
                    }
                    return tbl;
                };

                textHint = (boost::format(i18n::c(i18nText::LOADING_TABLE)) % t.getUrl()).str();
                textHint2 = "";

                if (t.loadFromFile())
                {
                    // TODO should re-download the table if outdated
                    LOG_INFO << "[List] Local table file found: " << t.getFolderPath().u8string();
                    rootFolderProp.dbBrowseEntries.push_back({ convertTable(t), nullptr });
                }
                else
                {
                    LOG_INFO << "[List] Local file not found. Downloading... " << t.getFolderPath().u8string();
                    textHint2 = i18n::s(i18nText::DOWNLOADING_TABLE);

                    t.updateFromUrl([&](DifficultyTable::UpdateResult result)
                        {
                            if (result == DifficultyTable::UpdateResult::OK)
                            {
                                LOG_INFO << "[List] Table file download complete: " << t.getFolderPath().u8string();
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
                    tableIndex += 1;
                }
            }

            });
    }

    if (loadTableEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        loadTableEnd.get();
        LOG_INFO << "[List] Loading tables complete.";
        LOG_INFO << "[List] ------------------------------------------------------------";

        _updateCallback = std::bind(&ScenePreSelect::updateLoadCourses, this);
    }
}

void ScenePreSelect::updateLoadCourses()
{
    if (!startedLoadCourse)
    {
        startedLoadCourse = true;
        LOG_INFO << "[List] Start loading courses...";

        loadCourseEnd = std::async(std::launch::async, [&]() {

            textHint = i18n::s(i18nText::LOADING_COURSES);

            std::map<EntryCourse::CourseType, std::vector<std::shared_ptr<EntryCourse>>> courses;

            // initialize table list
            Path coursePath = Path(GAMEDATA_PATH) / "courses";
            if (!fs::exists(coursePath))
                fs::create_directories(coursePath);

            LOG_INFO << "[List] Loading courses from courses/*.lr2crs...";
            for (auto& courseFile : fs::recursive_directory_iterator(coursePath))
            {
                if (!(fs::is_regular_file(courseFile) && strEqual(courseFile.path().extension().u8string(), ".lr2crs", true)))
                    continue;

                Path coursePath = courseFile.path();
                LOG_INFO << "[List] Loading course file: " << coursePath.u8string();
                textHint2 = coursePath.u8string();

                CourseLr2crs lr2crs(coursePath);
                for (auto& c : lr2crs.courses)
                {
                    std::shared_ptr<EntryCourse> entry = std::make_shared<EntryCourse>(c, lr2crs.addTime);
                    if (entry->courseType != EntryCourse::UNDEFINED)
                        courses[entry->courseType].push_back(entry);
                }
            }
            LOG_INFO << "[List] *.lr2crs loading complete.";

            // TODO load courses from tables

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
                    LOG_INFO << "[List] Add course: " << c->_name;
                    folder->pushEntry(c);
                }
                rootFolderProp.dbBrowseEntries.push_back({ folder, nullptr });
            }

            });
    }

    if (loadCourseEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        loadCourseEnd.get();
        LOG_INFO << "[List] Loading courses complete.";
        LOG_INFO << "[List] ------------------------------------------------------------";

        _updateCallback = std::bind(&ScenePreSelect::loadFinished, this);
    }
}

void ScenePreSelect::loadFinished()
{
    if (!loadingFinished)
    {
        while (!gSelectContext.backtrace.empty())
            gSelectContext.backtrace.pop_front();
        gSelectContext.backtrace.push_front(rootFolderProp);

        if (rootFolderProp.dbBrowseEntries.empty())
        {
            State::set(IndexText::PLAY_TITLE, i18n::s(i18nText::BMS_NOT_FOUND));
            State::set(IndexText::PLAY_ARTIST, i18n::s(i18nText::BMS_NOT_FOUND_HINT));
        }
        if (gNextScene == SceneType::PRE_SELECT)
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

        gNextScene = SceneType::SELECT;
        loadingFinished = true;
    }
}


void ScenePreSelect::updateImgui()
{
    if (gInCustomize) return;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ConfigMgr::get('V', cfg::V_DISPLAY_RES_X, CANVAS_WIDTH), ConfigMgr::get('V', cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT)), ImGuiCond_Always);
    if (ImGui::Begin("LoadSong", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::TextUnformatted(textHint.c_str());
        ImGui::TextUnformatted(textHint2.c_str());

        ImGui::End();
    }
}


bool ScenePreSelect::isLoadingFinished() const
{
    return loadingFinished;
}
