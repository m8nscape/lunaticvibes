#include "scene_pre_select.h"
#include "config/config_mgr.h"
#include "scene_context.h"
#include <future>
#include <boost/format.hpp>

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

        // load files
        loadSongEnd = std::async(std::launch::async, [&]() {

            // get folders from config
            auto folderList = ConfigMgr::General()->getFoldersPath();
            for (auto& f : folderList)
            {
                LOG_INFO << "[List] Add folder " << f;
                g_pSongDB->addFolder(f);
            }

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
            });
    }

    if (gNextScene == eScene::PRE_SELECT && g_pSongDB->addChartTaskFinishCount != prevChartLoaded)
    {
        std::shared_lock l(g_pSongDB->addCurrentPathMutex);

        prevChartLoaded = g_pSongDB->addChartTaskFinishCount;
        textHint = (
            boost::format("Loading [%d/%d]:")
                % g_pSongDB->addChartTaskFinishCount
                % g_pSongDB->addChartTaskCount
            ).str();
        textHint2 = g_pSongDB->addCurrentPath;
        State::set(IndexText::_OVERLAY_TOPLEFT, textHint);
        State::set(IndexText::_OVERLAY_TOPLEFT2, textHint2);
    }
    if (loadSongEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
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

            // initialize table list
            auto tableList = ConfigMgr::General()->getTablesUrl();
            for (auto& tableUrl : tableList)
            {
                LOG_INFO << "[List] Add table " << tableUrl;
                gSelectContext.tables.emplace_back();
                DifficultyTableBMS& t = gSelectContext.tables.back();
                t.setUrl(tableUrl);

                auto convertTable = [&](DifficultyTableBMS& t)
                {
                    std::shared_ptr<EntryFolderTable> tbl = std::make_shared<EntryFolderTable>(t.getName(), "");
                    size_t index = 0;
                    for (const auto& lv : t.getLevelList())
                    {
                        std::shared_ptr<EntryFolderTable> tblLevel = std::make_shared<EntryFolderTable>((boost::format("%s%s") % t.getSymbol() % lv).str(), "");
                        for (const auto& r : t.getEntryList(lv))
                        {
                            auto charts = g_pSongDB->findChartByHash(r->md5);
                            bool added = false;
                            for (auto& c : charts)
                            {
                                if (fs::exists(c->absolutePath))
                                {
                                    std::shared_ptr<EntryFolderSong> f = std::make_shared<EntryFolderSong>(HashMD5((boost::format("%032lu") % index++).str()), "", c->title, c->title2);
                                    f->pushChart(c);
                                    tblLevel->pushEntry(f);
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

                if (gNextScene == eScene::PRE_SELECT)
                {
                    textHint = (boost::format("Loading table: %s") % t.getUrl()).str();
                    State::set(IndexText::_OVERLAY_TOPLEFT, textHint);
                    State::set(IndexText::_OVERLAY_TOPLEFT2, "");
                }

                if (t.loadFromFile())
                {
                    rootFolderProp.dbBrowseEntries.push_back({ convertTable(t), nullptr });
                }
                else
                {
                    if (gNextScene == eScene::PRE_SELECT)
                    {
                        textHint2 = "Downloading...";
                        State::set(IndexText::_OVERLAY_TOPLEFT2, textHint2);
                    }

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

            while (!gSelectContext.backtrace.empty())
                gSelectContext.backtrace.pop();
            gSelectContext.backtrace.push(rootFolderProp);

            if (rootFolderProp.dbBrowseEntries.empty())
            {
                State::set(IndexText::PLAY_TITLE, "BMS NOT FOUND");
                State::set(IndexText::PLAY_ARTIST, "Press F9 to add folders");
            }

            if (gNextScene == eScene::PRE_SELECT)
            {
                State::set(IndexText::_OVERLAY_TOPLEFT, (boost::format("%s %s") % PROJECT_NAME % PROJECT_VERSION).str());
                State::set(IndexText::_OVERLAY_TOPLEFT2, "Please wait...");
            }
            });
    }

    if (loadTableEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        loadTableEnd.get();
        _updateCallback = [] {};

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

bool ScenePreSelect::isLoadingFinished() const
{
    return loadingFinished;
}