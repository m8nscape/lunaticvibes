#include <set>
#include <regex>
#include "common/utils.h"
#include "db_song.h"
#include "common/log.h"
#include "common/chartformat/chartformat_types.h"
#include "game/chart/chart_types.h"
#include "BS_thread_pool.hpp"

static std::map<SongDB*, BS::thread_pool> DBThreadPool;

const char* CREATE_FOLDER_TABLE_STR =
"CREATE TABLE IF NOT EXISTS folder( "
"pathmd5 TEXT PRIMARY KEY UNIQUE NOT NULL, "
"parent TEXT, "
"name TEXT, "
"type INTEGER NOT NULL DEFAULT 0, "
"path TEXT NOT NULL "
");";

const char* CREATE_SONG_TABLE_STR =
"CREATE TABLE IF NOT EXISTS song("
"md5 TEXT NOT NULL, "           // 0
"parent TEXT NOT NULL, "        // 1
"file TEXT NOT NULL, "          // 2
"type INTEGER NOT NULL, "       // 3
"title TEXT NOT NULL, "         // 4
"title2 TEXT NOT NULL, "        // 5
"artist TEXT NOT NULL, "        // 6
"artist2 TEXT NOT NULL, "       // 7
"genre TEXT NOT NULL, "         // 8
"version TEXT NOT NULL, "       // 9
"level REAL NOT NULL, "         // 10
"bpm REAL NOT NULL, "           // 11
"minbpm REAL NOT NULL, "        // 12
"maxbpm REAL NOT NULL, "        // 13
"length INTEGER NOT NULL, "     // 14
"totalnotes INTEGER NOT NULL, " // 15
"stagefile TEXT, "             // 16
"bannerfile TEXT, "            // 17
"gamemode INTEGER, "           // 18
"judgerank INTEGER, "          // 19
"total INTEGER, "              // 20
"playlevel INTEGER, "          // 21
"difficulty INTEGER, "         // 22
"longnote INTEGER, "           // 23
"landmine INTEGER, "           // 24
"metricmod INTEGER, "          // 25
"stop INTEGER, "               // 26
"bga INTEGER, "                // 27
"random INTEGER, "             // 28
"addtime INTEGER, "            // 29
"CONSTRAINT pk_pf PRIMARY KEY (parent,file) "
");";
struct song_all_params
{
    std::string md5;
    std::string parent;
    std::string file;
    long long type = 0;
    std::string title;
    std::string title2;
    std::string artist;
    std::string artist2;
    std::string genre;
    std::string version;
    double level = 0;
    double bpm = 0;
    double minbpm = 0;
    double maxbpm = 0;
    long long length = 0;
    long long totalnotes = 0;
    std::string stagefile;
    std::string bannerfile;
    long long gamemode = 0;
    long long judgerank = 0;
    long long total = 0;
    long long playlevel = 0;
    long long difficulty = 0;
    long long longnote = 0;
    long long landmine = 0;
    long long metricmod = 0;
    long long stop = 0;
    long long bga = 0;
    long long random = 0;
    long long addtime = 0;

    song_all_params(const std::vector<std::any>& queryResult)
    {
        try 
        {
            md5         = ANY_STR(queryResult.at(0));
            parent      = ANY_STR(queryResult.at(1));
            file        = ANY_STR(queryResult.at(2));
            type        = ANY_INT(queryResult.at(3));
            title       = ANY_STR(queryResult.at(4));
            title2      = ANY_STR(queryResult.at(5));
            artist      = ANY_STR(queryResult.at(6));
            artist2     = ANY_STR(queryResult.at(7));
            genre       = ANY_STR(queryResult.at(8));
            version     = ANY_STR(queryResult.at(9));
            level       = ANY_REAL(queryResult.at(10));
            bpm         = ANY_REAL(queryResult.at(11));
            minbpm      = ANY_REAL(queryResult.at(12));
            maxbpm      = ANY_REAL(queryResult.at(13));
            length      = ANY_INT(queryResult.at(14));
            totalnotes  = ANY_INT(queryResult.at(15));
            stagefile   = ANY_STR(queryResult.at(16));
            bannerfile  = ANY_STR(queryResult.at(17));
            gamemode    = ANY_INT(queryResult.at(18));
            judgerank   = ANY_INT(queryResult.at(19));
            total       = ANY_INT(queryResult.at(20));
            playlevel   = ANY_INT(queryResult.at(21));
            difficulty  = ANY_INT(queryResult.at(22));
            longnote    = ANY_INT(queryResult.at(23));
            landmine    = ANY_INT(queryResult.at(24));
            metricmod   = ANY_INT(queryResult.at(25));
            stop        = ANY_INT(queryResult.at(26));
            bga         = ANY_INT(queryResult.at(27));
            random      = ANY_INT(queryResult.at(28));
            addtime     = ANY_INT(queryResult.at(29));
        }
        catch (std::out_of_range&)
        {
        }
    }   
};
bool convert_bms(std::shared_ptr<BMS_prop> chart, const std::vector<std::any>& in)
{
    if (in.size() < 30) return false;

#ifdef _WIN32
    const static auto locale_utf8 = std::locale(".65001");
#else
    const static auto locale_utf8 = std::locale("en_US.UTF-8");
#endif

    song_all_params params(in);
    chart->fileHash       = params.md5        ;
    chart->folderHash     = params.parent     ;
    chart->filePath       = Path(params.file, locale_utf8);
    //                        params.type       ;
    chart->title          = params.title      ;
    chart->title2         = params.title2     ;
    chart->artist         = params.artist     ;
    chart->artist2        = params.artist2    ;
    chart->genre          = params.genre      ;
    chart->version        = params.version    ;
    chart->levelEstimated = params.level      ;
    chart->startBPM       = params.bpm        ;
    chart->minBPM         = params.minbpm     ;
    chart->maxBPM         = params.maxbpm     ;
    chart->totalLength    = params.length     ;
    chart->totalNotes     = params.totalnotes ;
    chart->stagefile      = params.stagefile  ;
    chart->banner         = params.bannerfile ;
    chart->gamemode       = params.gamemode   ;
    chart->rank           = params.judgerank  ;
    chart->total          = params.total      ;
    chart->playLevel      = params.playlevel  ;
    chart->difficulty     = params.difficulty ;
    chart->haveLN         = params.longnote   ;
    chart->haveMine       = params.landmine   ;
    chart->haveMetricMod  = params.metricmod  ;
    chart->haveStop       = params.stop       ;
    chart->haveBPMChange  = params.maxbpm != params.minbpm;
    chart->haveBGA        = params.bga        ;
    chart->haveRandom     = params.random     ;
    chart->addTime        = params.addtime    ;

    if (chart->totalNotes > 0)
    {
        chart->haveNote = true;
        chart->notes = chart->totalNotes;
    }

    return true;
}


SongDB::SongDB(const char* path) : SQLite(path, "SONG")
{
    DBThreadPool.emplace(this, std::thread::hardware_concurrency() - 1);

    if (exec(CREATE_FOLDER_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create table folder ERROR! " << errmsg();
        abort();
    }
    if (query("SELECT parent FROM folder WHERE pathmd5=?", 1, { ROOT_FOLDER_HASH.hexdigest()}).empty())
    {
        if (exec("INSERT INTO folder(pathmd5,parent,path,name,type) VALUES(?,?,?,?,?)", 
            { ROOT_FOLDER_HASH.hexdigest(), nullptr, "", "ROOT", 0}))
        {
            LOG_ERROR << "[SongDB] Insert root folder to table ERROR! " << errmsg();
            abort();
        }
    }
    

    if (exec(CREATE_SONG_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create table song ERROR! " << errmsg();
        abort();
    }

    if (exec("CREATE INDEX IF NOT EXISTS index_parent ON folder(parent)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create parent index for folder ERROR! " << errmsg();
    }

    if (exec("CREATE INDEX IF NOT EXISTS index_md5 ON song(md5)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create md5 index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_parent ON song(parent)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create parent index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_gamemode ON song(gamemode)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create gamemode index for song ERROR! " << errmsg();
    }

}

int SongDB::addChart(const HashMD5& folder, const Path& path)
{
    decltype(path.filename().u8string()) filename;
    try
    {
        filename = path.filename().u8string();
    }
    catch (const std::exception& e)
    {
        LOG_WARNING << "[SongDB] "<< e.what() << ": " << path.filename().wstring();
        return 1;
    }

    if (auto result = query("SELECT md5 FROM song WHERE parent=? AND file=?", 1, 
        { folder.hexdigest(), filename }); !result.empty() && !result[0].empty())
    {
        // file exists in db
        HashMD5 dbmd5 = ANY_STR(result[0][0]);
        HashMD5 filemd5 = md5file(path);
        if (dbmd5 == filemd5)
        {
            return 0;
        }
        else
        {
            LOG_INFO << "[SongDB] File " << path.u8string() << " exists, but hash not match. Removing old entry from db";
            if (SQLITE_OK != exec("DELETE FROM song WHERE parent=? AND file=?", { folder.hexdigest(), filename }))
            {
                LOG_WARNING << "[SongDB] Remove existing chart from db failed: " << path.u8string();
                return 1;
            }
        }
    }

    auto c = vChartFormat::createFromFile(path);
    if (c == nullptr)
    {
        LOG_WARNING << "[SongDB] File error: " << path.u8string();
        return 1;
    }

    auto s = chart::vChart::createFromChartFormat(0, c);
    if (s == nullptr)
    {
        LOG_WARNING << "[SongDB] File parsing error: " << path.u8string();
        return 1;
    }

    switch (c->type())
    {
    case eChartFormat::BMS:
    {
        auto bmsc = std::reinterpret_pointer_cast<BMS>(c);
        if (SQLITE_OK != exec("INSERT INTO song("
            "md5,parent,type,file,title,title2,artist,artist2,genre,version,"
            "level,bpm,minbpm,maxbpm,length,totalnotes,stagefile,bannerfile,gamemode,judgerank,"
            "total,playlevel,difficulty,longnote,landmine,metricmod,stop,bga,random,addtime) "
            "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
            {
                c->fileHash.hexdigest(),
                folder.hexdigest(),
                int(c->type()),
                c->filePath.filename().u8string(),
                c->title,
                c->title2,
                c->artist,
                c->artist2,
                c->genre,
                c->version,

                c->levelEstimated,
                c->startBPM,
                c->minBPM,
                c->maxBPM,
                s->getTotalLength().norm() / 1000,
                static_cast<int>(s->getNoteCount()),
                c->stagefile,
                c->banner,
                bmsc->gamemode,
                bmsc->rank,

                bmsc->total,
                bmsc->playLevel,
                bmsc->difficulty,
                bmsc->haveLN,
                bmsc->haveMine,
                bmsc->haveMetricMod,
                bmsc->haveStop,
                bmsc->haveBGA,
                bmsc->haveRandom,
                static_cast<long long>(std::time(nullptr))
            }))
        {
            LOG_WARNING << "[SongDB] Insert into db error: " << path.u8string() << ": " << errmsg();
            return 1;
        }
    }
        break;

    default:
        break;
    }
    return 0;
}

int SongDB::removeChart(const HashMD5& md5)
{
    if (SQLITE_OK != exec("DELETE FROM song WHERE md5=?", { md5.hexdigest() }))
    {
        LOG_WARNING << "[SongDB] Delete from db error: " << md5.hexdigest() << ": " << errmsg();
        return 1;
    }
    return 0;
}

int pushResult(std::vector<pChart>& list, const std::vector<std::vector<std::any>>& result)
{
    int count = 0;
    for (const auto& r : result)
    {
        switch (eChartFormat(ANY_INT(r[3])))
        {
        case eChartFormat::BMS:
        {
            auto bms = std::make_shared<BMS>();
            convert_bms(bms, r);
            list.push_back(bms);
            ++count;
            break;
        }

        default: break;
        }
    }
    return count;
}

// search from genre, version, artist, artist2, title, title2
std::vector<pChart> SongDB::findChartByName(const HashMD5& folder, const std::string& tag, unsigned limit) const
{
    LOG_INFO << "[SongDB] Search for songs matching: " << tag;

    decltype(query("", {})) result;
    if (limit > 0)
    {
        result = query(
"SELECT * FROM song WHERE \
parent=? AND (\
INSTR(title, ?) OR \
INSTR(title2, ?) OR \
INSTR(artist, ?) OR \
INSTR(artist2, ?) OR \
INSTR(genre, ?) OR \
INSTR(version, ?))\
LIMIT ?",
29, { folder, tag, tag, tag, tag, tag, tag, limit });
    }
    else
    {
        result = query(
"SELECT * FROM song WHERE \
parent=? AND (\
INSTR(title, ?) OR \
INSTR(title2, ?) OR \
INSTR(artist, ?) OR \
INSTR(artist2, ?) OR \
INSTR(genre, ?) OR \
INSTR(version, ?))", 
29, { folder, tag, tag, tag, tag, tag, tag});
    }

    std::vector<pChart> ret;
    pushResult(ret, result);

    LOG_INFO << "[SongDB] found " << ret.size() << " songs";
    return ret;
}

// chart may duplicate, return all found
std::vector<pChart> SongDB::findChartByHash(const HashMD5& target) const
{
    LOG_INFO << "[SongDB] Search for song " << target.hexdigest();

    auto result = query("SELECT * FROM song WHERE md5=?", 29, { target.hexdigest() });

    std::vector<pChart> ret;
    pushResult(ret, result);

    LOG_INFO << "[SongDB] found " << ret.size() << " songs";
    return ret;

}

int SongDB::addFolder(Path path, HashMD5 parentHash)
{
    path = (path / ".").lexically_normal();

    if (!fs::exists(path))
    {
        LOG_WARNING << "[SongDB] Add folder fail: folder not exist (" << path.u8string() << ")";
        return 1;
    }
    if (!fs::is_directory(path))
    {
        LOG_WARNING << "[SongDB] Add folder fail: path is not folder (" << path.u8string() << ")";
        return 1;
    }

    if (isParentPath(executablePath, path))
    {
        if (parentHash.empty())
        {
            // parent is empty, add with absolute path
            path = fs::absolute(path);
        }
        else
        {
            // parent is not empty, add with relative path
        }
    }
    else
    {
        path = fs::absolute(path);
    }

    // check if the folder is already added
    HashMD5 folderHash = md5(path.u8string());
    if (auto q = query("select pathmd5,type from folder where path=?", 2, { path.u8string() }); !q.empty())
    {
        LOG_DEBUG << "[SongDB] Folder already exists (" << path.u8string() << ")";

        // TODO check if refresh all folder on run. This is really slow
        if (parentHash == ROOT_FOLDER_HASH)
        {
            refreshFolder(ANY_STR(q[0][0]), path, (FolderType)ANY_INT(q[0][1]));
        }

        return 0;
    }
    else
    {
        auto files = std::filesystem::directory_iterator(path);
        FolderType type = FolderType::FOLDER;
        for (auto& f : files)
        {
            if (analyzeChartType(f) == eChartFormat::BMS)
            {
                type = FolderType::SONG_BMS;
                break;  // break for
            }
            // else ...
        }

        int ret;
        auto filename = fs::weakly_canonical(path).filename();
        if (!parentHash.empty())
            ret = exec("INSERT INTO folder VALUES(?,?,?,?,?)", {
            folderHash.hexdigest(),
            parentHash.hexdigest(),
            filename.u8string(),
            (int)type,
            path.u8string() });
        else
            ret = exec("INSERT INTO folder VALUES(?,?,?,?,?)", {
            folderHash.hexdigest(),
            nullptr,
            filename.u8string(),
            (int)type,
            path.u8string() });
        if (SQLITE_OK != ret)
        {
            LOG_WARNING << "[SongDB] Add folder fail: [" << ret << "] " << errmsg() << " (" << path.u8string() << ")";
            return 1;
        }
    }

    int count = addFolderCharts(folderHash, path);
    LOG_INFO << "[SongDB] " << path.u8string() << ": queued " << count << " entries";
    return 0;
}

int SongDB::addFolderCharts(const HashMD5& hash, const Path& folder)
{
    int count = 0;

    bool isSongFolder = false;
    for (auto& f : fs::directory_iterator(folder))
    {
        if (analyzeChartType(f) != eChartFormat::UNKNOWN)
        {
            isSongFolder = true;
            break;
        }
    }

    std::vector<Path> subFolderList;

    addChartBuffer.clear();
    for (const auto& f : fs::directory_iterator(folder))
    {
        if (!isSongFolder && fs::is_directory(f))
        {
            subFolderList.push_back(f);
        }
        else if (isSongFolder && analyzeChartType(f) != eChartFormat::UNKNOWN)
        {
            addChartBuffer.push_back(std::make_pair(hash, f));
            ++count;
        }
    }
    handleAddChartBuffer();

    for (const auto& sub : subFolderList)
    {
        if (0 == addFolder(sub, hash))
            ++count;
    }

    return count;
}

int SongDB::refreshFolder(const HashMD5& hash, const Path& path, FolderType type)
{
    LOG_DEBUG << "[SongDB] Refreshing contents of " << path.u8string();

    bool isSongFolder = false;
    for (auto& f: fs::directory_iterator(path))
    {
        if (analyzeChartType(f) != eChartFormat::UNKNOWN)
        {
            isSongFolder = true;
            break;
        }
    }
    if (!isSongFolder && (type == FolderType::SONG_BMS))
    {
        LOG_DEBUG << "[SongDB] Re-analyzing";

        // analyze the folder again
        removeFolder(hash, true);
        return addFolder(path, hash);
    }
    else if (type == FolderType::SONG_BMS)
    {
        LOG_DEBUG << "[SongDB] Checking for new entries";

        // only add new entries
        int count = 0;

        std::vector<Path> bmsFiles;
        for (auto& f : fs::directory_iterator(path))
        {
            if (analyzeChartType(f) != eChartFormat::UNKNOWN)
                bmsFiles.push_back(fs::absolute(f));
        }
        std::sort(bmsFiles.begin(), bmsFiles.end());

        std::vector<Path> existedFiles;
        auto existedList = std::make_shared<FolderSong>(browseSong(hash));
        for (size_t i = 0; i < existedList->getContentsCount(); ++i)
        {
            existedFiles.push_back(existedList->getChart(i)->absolutePath);
        }
        std::sort(existedFiles.begin(), existedFiles.end());

        std::vector<Path> newFiles;
        std::set_difference(bmsFiles.begin(), bmsFiles.end(), existedFiles.begin(), existedFiles.end(), std::back_inserter(newFiles));

        addChartBuffer.clear();
        for (auto& p : newFiles)
        {
            addChartBuffer.push_back(std::make_pair(hash, p));
            count++;
        }
        handleAddChartBuffer();

        // TODO set delete flag on not-found entries

        LOG_DEBUG << "[SongDB] Folder originally has " << existedFiles.size() << " entries, added " << count;
        return count;
    }
    else
    {
        LOG_DEBUG << "[SongDB] Checking for new subfolders";

        // just add new entries
        int count = 0;

        std::vector<Path> subFolders;
        for (auto& f : fs::directory_iterator(path))
        {
            if (fs::is_directory(f))
                if (0 == addFolder(f, hash))
                    ++count;
        }

        LOG_DEBUG << "[SongDB] Checked " << count;
        return count;
    }
}

int SongDB::handleAddChartBuffer()
{
    int count = 0;

    BS::thread_pool& pool = DBThreadPool[this];

    unsigned finishedTaskCount = 0;
    for (auto& chart : addChartBuffer)
    {
        HashMD5 hash = chart.first;
        Path path = chart.second;
        using namespace std::placeholders;

        pool.submit([&, hash, path]()
            {
                try
                {
                    if (addChart(hash, path) == 0)
                        count++;
                }
                catch (...)
                {
                }
                finishedTaskCount++;
            });
    }
    while (true)
    {
        using namespace std::chrono_literals;

        if (finishedTaskCount != addChartBuffer.size())
        {
            std::this_thread::sleep_for(100ms);
        }
        else
        {
            break;
        }
    }

    addChartBuffer.clear();

    return count;
}

HashMD5 SongDB::getFolderParent(const Path& path) const
{
    if (!fs::is_directory(path)) return "";

    auto parent = (path / "..").lexically_normal();
    HashMD5 parentHash = md5(parent.u8string());
    auto result = query("SELECT name,type FROM folder WHERE parent=?", 3, { parentHash.hexdigest() });
    if (!result.empty())
    {
        for (const auto& leaf : result)
            if (ANY_INT(leaf[1]) == FOLDER)
            {
                return parentHash;
            }
    }

    return "";
}

int SongDB::removeFolder(const HashMD5& hash, bool removeSong)
{
    if (removeSong)
    {
        if (SQLITE_OK != exec("DELETE FROM song WHERE parent=?", { hash.hexdigest() }))
            LOG_WARNING << "[SongDB] remove song from db error: " << errmsg();
    }

    return exec("DELETE FROM folder WHERE pathmd5=?", { hash.hexdigest() });
}

HashMD5 SongDB::getFolderParent(const HashMD5& folder) const
{
    auto result = query("SELECT type,parent FROM folder WHERE path=?", 2, { folder.hexdigest() });
    if (!result.empty())
    {
        auto leaf = result[0];
        if (ANY_INT(leaf[0]) != FOLDER)
        {
            LOG_WARNING << "[SongDB] Get folder parent type error: excepted " << FOLDER << ", get " << ANY_INT(leaf[0]) <<
                " (" << folder.hexdigest() << ")";
            return "";
        }
        return ANY_STR(leaf[1]);
    }
    LOG_INFO << "[SongDB] Get folder parent fail: target " << folder.hexdigest() << " not found";
    return "";
}


int SongDB::getFolderPath(const HashMD5& folder, Path& output) const
{
#ifdef _WIN32
    const static auto locale_utf8 = std::locale(".65001");
#else
    const static auto locale_utf8 = std::locale("en_US.UTF-8");
#endif

    auto result = query("SELECT type,path FROM folder WHERE pathmd5=?", 2, { folder.hexdigest()});
    if (!result.empty())
    {
        auto leaf = result[0];
        //if (ANY_INT(leaf[0]) != FOLDER)
        //{
        //    LOG_WARNING << "[SongDB] Get folder path type error: excepted " << FOLDER << ", get " << ANY_INT(leaf[0]) <<
        //        " (" << folder << ")";
        //    return Path();
        //}
        output = Path(ANY_STR(leaf[1]), locale_utf8);
        return 0;
    }
    LOG_INFO << "[SongDB] Get folder path fail: target " << folder.hexdigest() << " not found";
    return -1;
}

HashMD5 SongDB::getFolderHash(Path path) const
{
    path = (path / ".").lexically_normal();
    auto parentHash = getFolderParent(path);
    if (isParentPath(executablePath, path))
    {
        if (parentHash.empty())
        {
            // parent is empty, add with absolute path
            path = fs::absolute(path);
        }
        else
        {
            // parent is not empty, add with relative path
        }
    }
    else
    {
        path = fs::absolute(path);
    }
    return md5(path.u8string());
}



FolderRegular SongDB::browse(HashMD5 root, bool recursive)
{
    Path path;
    if (getFolderPath(root, path) < 0)
        return FolderRegular(HashMD5(), path);

    FolderRegular list(root, path);

    auto result = query("SELECT pathmd5,parent,name,type FROM folder WHERE parent=?", 5, { root.hexdigest() });
    if (!result.empty())
    {
        for (auto& c : result)
        {
            auto md5 = ANY_STR(c[0]);
            auto parent = ANY_STR(c[1]);
            auto name = ANY_STR(c[2]);
            auto type = ANY_INT(c[3]);

            switch (type)
            {
            case FOLDER:
            {
                auto sub = std::make_shared<FolderRegular>(browse(md5, false));
                sub->_name = name;
                list.pushEntry(sub);
                break;
            }
            case SONG_BMS:
                auto bmsList = std::make_shared<FolderSong>(browseSong(md5));
                // name is set inside browseSong
                if (!bmsList->empty())
                    list.pushEntry(bmsList);
                break;
            }
        }
    }

    return list;
}

FolderSong SongDB::browseSong(HashMD5 root)
{
    Path path;
    if (getFolderPath(root, path) < 0)
        return FolderSong(HashMD5(), path);

    FolderSong list(root, path);
    bool isNameSet = false;

    auto result = query("SELECT * from song WHERE parent=?", 30, { root.hexdigest() });
    if (!result.empty())
    {
        for (auto& c : result)
        {
            auto type = (eChartFormat)ANY_INT(c[3]);
            switch (type)
            {
            case eChartFormat::BMS:
            {
                auto p = std::make_shared<BMS_prop>();
                if (convert_bms(p, c))
                {
                    if (p->filePath.is_absolute())
                        p->absolutePath = p->filePath;
                    else
                        p->absolutePath = path / p->filePath;

                    list.pushChart(p);
                }
                if (!isNameSet)
                {
                    isNameSet = true;
                    list._name = p->title;
                    list._name2 = p->title2;
                }
                break;
            }
            default:
                break;
            }
        }
    }

    return list;
}


FolderRegular SongDB::search(HashMD5 root, std::string key)
{
    Path path;
    if (getFolderPath(root, path) < 0)
        return FolderRegular(HashMD5(), "");

    FolderRegular list(md5(key), "");
    for (auto& c : findChartByName(root, key))
    {
        //list.pushChart(c);
    }

    return list;
}
