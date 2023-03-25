#include <set>
#include <regex>
#include "common/utils.h"
#include "db_song.h"
#include "common/log.h"
#include "common/chartformat/chartformat_types.h"
#include "game/chart/chart_types.h"
#include "re2/re2.h"

#define BOOST_ASIO_NO_EXCEPTIONS
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

const char* CREATE_FOLDER_TABLE_STR =
"CREATE TABLE IF NOT EXISTS folder( "
"pathmd5 TEXT PRIMARY KEY UNIQUE NOT NULL, "
"parent TEXT, "
"name TEXT, "
"type INTEGER NOT NULL DEFAULT 0, "
"path TEXT NOT NULL,"
"modtime INTEGER"
");";
static constexpr size_t FOLDER_PARAM_COUNT = 6;

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
static constexpr size_t SONG_PARAM_COUNT = 30;
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
bool convert_bms(std::shared_ptr<ChartFormatBMSMeta> chart, const std::vector<std::any>& in)
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
    chart->fileName       = Path(params.file, locale_utf8);
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
        chart->notes_total = chart->totalNotes;
    }

    return true;
}


SongDB::SongDB(const char* path) : SQLite(path, "SONG")
{
    if (exec("PRAGMA cache_size = -512000") != SQLITE_OK)
    {
        LOG_WARNING << "[SongDB] Set cache_size ERROR! " << errmsg();
    }

    poolThreadCount = std::thread::hardware_concurrency() - 1;

    if (exec(CREATE_FOLDER_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create table folder ERROR! " << errmsg();
        abort();
    }
    if (query("SELECT parent FROM folder WHERE pathmd5=?", 1, { ROOT_FOLDER_HASH.hexdigest()}).empty())
    {
        if (exec("INSERT INTO folder(pathmd5,parent,path,name,type,modtime) VALUES(?,?,?,?,?,?)", 
            { ROOT_FOLDER_HASH.hexdigest(), nullptr, "", "ROOT", 0, 0}))
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

SongDB::~SongDB()
{
    if (threadPool)
    {
        delete (boost::asio::thread_pool*)threadPool;
        threadPool = nullptr;
    }
}

bool SongDB::addChart(const HashMD5& folder, const Path& path)
{
    auto closure = [&]() -> bool
    {
        decltype(path.filename().u8string()) filename;
        try
        {
            filename = path.filename().u8string();
        }
        catch (const std::exception& e)
        {
            LOG_WARNING << "[SongDB] " << e.what() << ": " << path.filename().wstring();
            return false;
        }

        if (auto result = query("SELECT md5 FROM song WHERE parent=? AND file=?", 1,
            { folder.hexdigest(), filename }); !result.empty() && !result[0].empty())
        {
            // check if file exists in db
            HashMD5 dbmd5 = ANY_STR(result[0][0]);
            HashMD5 filemd5 = md5file(path);
            if (dbmd5 == filemd5)
            {
                return false;
            }
            // remove existing entry
            removeChart(path, folder);
        }

        std::shared_ptr<ChartFormatBase> c = ChartFormatBase::createFromFile(path, 2356);
        if (c == nullptr)
        {
            LOG_WARNING << "[SongDB] File error: " << path.u8string();
            return false;
        }

        auto s = ChartObjectBase::createFromChartFormat(0, c);
        if (s == nullptr)
        {
            LOG_WARNING << "[SongDB] File parsing error: " << path.u8string();
            return false;
        }

        {
            std::unique_lock l(addCurrentPathMutex, std::try_to_lock);
            if (l.owns_lock())
            {
                addCurrentPath = path.u8string();
            }
        }

        switch (c->type())
        {
        case eChartFormat::BMS:
        {
            auto bmsc = std::dynamic_pointer_cast<ChartFormatBMS>(c);
            assert(bmsc != nullptr);
            if (SQLITE_OK == exec("INSERT INTO song("
                "md5,parent,type,file,title,title2,artist,artist2,genre,version,"
                "level,bpm,minbpm,maxbpm,length,totalnotes,stagefile,bannerfile,gamemode,judgerank,"
                "total,playlevel,difficulty,longnote,landmine,metricmod,stop,bga,random,addtime) "
                "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
                {
                    c->fileHash.hexdigest(),
                    folder.hexdigest(),
                    int(c->type()),
                    c->fileName.filename().u8string(),
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
                    static_cast<int>(s->getNoteTotalCount()),
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
                    getFileTimeNow()
                }))
            {
                return true;
            }
            else
            {
                LOG_WARNING << "[SongDB] Insert chart into db error: " << path.u8string() << ": " << errmsg();
                return false;
            }
            break;
        }
        }

        return false;
    };

    bool ret = closure();
    if (ret) addChartSuccess++;

    addChartTaskFinishCount++;
    return ret;
}

bool SongDB::removeChart(const Path& path, const HashMD5& parent)
{
    if (SQLITE_OK != exec("DELETE FROM song WHERE file=? AND parent=?", { path.filename().u8string(), parent.hexdigest()}))
    {
        LOG_WARNING << "[SongDB] Delete chart from db error: " << path.u8string() << ": " << errmsg();
        return false;
    }
    return true;
}

bool SongDB::removeChart(const HashMD5& md5, const HashMD5& parent)
{
    if (SQLITE_OK != exec("DELETE FROM song WHERE md5=? AND parent=?", { md5.hexdigest(), parent.hexdigest()}))
    {
        LOG_WARNING << "[SongDB] Delete chart from db error: " << md5.hexdigest() << ": " << errmsg();
        return false;
    }
    return true;
}

// search from genre, version, artist, artist2, title, title2
std::vector<std::shared_ptr<ChartFormatBase>> SongDB::findChartByName(const HashMD5& folder, const std::string& tagRaw, unsigned limit) const
{
    LOG_INFO << "[SongDB] Search for songs matching: " << tagRaw;

    std::string tag = tagRaw;
    static const std::pair<RE2, re2::StringPiece> search_replace_pattern[]
    {
        {"%", "\\\\%"},
        {"_", "\\\\_"},
    };
    for (const auto& [in, out] : search_replace_pattern)
    {
        RE2::GlobalReplace(&tag, in, out);
    }

    std::stringstream ss;
    ss << "SELECT * FROM song WHERE ";
    if (folder != ROOT_FOLDER_HASH) 
        ss << "parent=" << folder.hexdigest() << " AND ";
    ss << "(title   LIKE '%' || ? || '%' ESCAPE '\\' OR "
        << "title2  LIKE '%' || ? || '%' ESCAPE '\\' OR "
        << "artist  LIKE '%' || ? || '%' ESCAPE '\\' OR "
        << "artist2 LIKE '%' || ? || '%' ESCAPE '\\' OR "
        << "genre   LIKE '%' || ? || '%' ESCAPE '\\' OR "
        << "version LIKE '%' || ? || '%' ESCAPE '\\' )";
    if (limit > 0) 
        ss << " LIMIT " << limit;

    std::string strSql = ss.str();
    auto result = query(strSql.c_str(), SONG_PARAM_COUNT, {tag, tag, tag, tag, tag, tag});

    std::vector<std::shared_ptr<ChartFormatBase>> ret;
    for (const auto& r : result)
    {
        switch (eChartFormat(ANY_INT(r[3])))
        {
        case eChartFormat::BMS:
        {
            auto p = std::make_shared<ChartFormatBMSMeta>();
            if (convert_bms(p, r))
            {
                if (p->fileName.is_absolute())
                {
                    p->absolutePath = p->fileName;
                    ret.push_back(p);
                }
                else
                {
                    const auto [hasFolderPath, folderPath] = getFolderPath(p->folderHash);
                    if (hasFolderPath)
                    {
                        p->absolutePath = folderPath / p->fileName;
                        ret.push_back(p);
                    }
                }
            }
            break;
        }

        default: break;
        }
    }

    LOG_INFO << "[SongDB] found " << ret.size() << " songs";
    return ret;
}

// chart may duplicate, return all found
std::vector<std::shared_ptr<ChartFormatBase>> SongDB::findChartByHash(const HashMD5& target, bool checksum) const
{
    // LOG_DEBUG << "[SongDB] Search for song " << target.hexdigest();

    std::vector<std::shared_ptr<ChartFormatBase>> ret;

    if (songQueryHashMap.find(target) == songQueryHashMap.end())
    {
        return ret;
    }
    for (const auto& index : songQueryHashMap.at(target))
    {
        const auto& r = songQueryPool[index];
        switch (eChartFormat(ANY_INT(r[3])))
        {
        case eChartFormat::BMS:
        {
            auto p = std::make_shared<ChartFormatBMSMeta>();
            if (convert_bms(p, r))
            {
                if (p->fileName.is_absolute())
                {
                    p->absolutePath = p->fileName;
                    ret.push_back(p);
                }
                else
                {
                    const auto [hasFolderPath, folderPath] = getFolderPath(p->folderHash);
                    if (hasFolderPath)
                    {
                        p->absolutePath = folderPath / p->fileName;
                        ret.push_back(p);
                    }
                }
            }
            break;
        }

        default: break;
        }
    }

    if (checksum)
    {
        // remove file mismatch
        std::list<size_t> removing;
        for (size_t i = 0; i < ret.size(); ++i)
        {
            auto hash = md5file(ret[i]->absolutePath);
            if (hash != target)
            {
                LOG_WARNING << "[SongDB] Chart " << ret[i]->absolutePath.u8string() << " has been modified, ignoring";
                removing.push_front(i);
            }
        }
        for (size_t i : removing)
        {
            ret.erase(ret.begin() + i);
        }
    }

    // LOG_DEBUG << "[SongDB] found " << ret.size() << " songs";
    return ret;
}

// chart may duplicate, return all found
std::vector<std::shared_ptr<ChartFormatBase>> SongDB::findChartFromTime(const HashMD5& folder, unsigned long long addTime) const
{
    LOG_INFO << "[SongDB] Search from epoch time " << addTime;

    std::stringstream ss;
    ss << "SELECT * FROM song WHERE ";
    if (folder != ROOT_FOLDER_HASH)
        ss << "parent=" << folder.hexdigest() << " AND ";
    ss << "addtime>=?";

    std::string strSql = ss.str();
    auto result = query(strSql.c_str(), SONG_PARAM_COUNT, { (long long)addTime });

    std::vector<std::shared_ptr<ChartFormatBase>> ret;
    for (const auto& r : result)
    {
        switch (eChartFormat(ANY_INT(r[3])))
        {
        case eChartFormat::BMS:
        {
            auto p = std::make_shared<ChartFormatBMSMeta>();
            if (convert_bms(p, r))
            {
                if (p->fileName.is_absolute())
                {
                    p->absolutePath = p->fileName;
                    ret.push_back(p);
                }
                else
                {
                    const auto [hasFolderPath, folderPath] = getFolderPath(p->folderHash);
                    if (hasFolderPath)
                    {
                        p->absolutePath = folderPath / p->fileName;
                        ret.push_back(p);
                    }
                }
            }
            break;
        }

        default: break;
        }
    }

    LOG_INFO << "[SongDB] found " << ret.size() << " songs";
    return ret;

}


void SongDB::prepareCache()
{
    LOG_DEBUG << "[SongDB] prepareCache ";

    // compress db i/o
    freeCache();

    size_t count = 0;
    for (auto& row : query("SELECT * FROM song", SONG_PARAM_COUNT))
    {
        songQueryHashMap[HashMD5(ANY_STR(row[0]))].push_back(count);
        songQueryParentMap[HashMD5(ANY_STR(row[1]))].push_back(count);
        songQueryPool.push_back(std::move(row));
        count++;
    }

    count = 0;
    for (auto& row : query("SELECT * FROM folder", FOLDER_PARAM_COUNT))
    {
        folderQueryHashMap[HashMD5(ANY_STR(row[0]))].push_back(count);
        if (row[1].has_value()) 
            folderQueryParentMap[HashMD5(ANY_STR(row[1]))].push_back(count);
        folderQueryPool.push_back(std::move(row));
        count++;
    }
}

void SongDB::freeCache()
{
    songQueryPool.clear();
    songQueryPool.shrink_to_fit();
    songQueryHashMap.clear();
    songQueryParentMap.clear();
    folderQueryPool.clear();
    folderQueryPool.shrink_to_fit();
    folderQueryHashMap.clear();
    folderQueryParentMap.clear();
}

int SongDB::initializeFolders(const std::vector<Path>& paths)
{
    resetAddSummary();

    std::future<void> sessionFuture;
    std::chrono::system_clock::time_point sessionTimestamp = std::chrono::system_clock::now();

    bool inAddFolderSession = true;
    transactionStart();
    sessionFuture = std::async(std::launch::async, [&]()
        {
            while (inAddFolderSession)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                if (inAddFolderSession && std::chrono::system_clock::now() - sessionTimestamp >= std::chrono::seconds(10))
                {
                    sessionTimestamp = std::chrono::system_clock::now();
                    transactionStop();
                    transactionStart();
                }
            }
        });

    int count = 0;
    for (const auto& p : paths)
    {
        int subCount = addSubFolder(p, ROOT_FOLDER_HASH);
        count += subCount;
        LOG_INFO << "[SongDB] " << p.u8string() << ": added " << subCount << " entries";
    }

    inAddFolderSession = false;
    transactionStop();
    sessionFuture.wait_for(std::chrono::seconds(10));

    waitLoadingFinish();

    return count;
}

int SongDB::addSubFolder(Path path, const HashMD5& parentHash)
{
    LOG_VERBOSE << "[SongDB] Add folder: " << path.u8string();

    path = (path / ".").lexically_normal();

    if (!fs::is_directory(path))
    {
        LOG_WARNING << "[SongDB] Add folder fail: path is not folder (" << path.u8string() << ")";
        return -1;
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
    int count = 0;
    HashMD5 folderHash = md5(path.u8string());
    long long nowTime = getFileTimeNow();
    long long folderModifyTime = getFileLastWriteTime(path);

    if (auto q = query("SELECT pathmd5,path,type,modtime FROM folder WHERE path=?", 4, { path.u8string() }); !q.empty())
    {
        LOG_VERBOSE << "[SongDB] Sub folder already exists (" << path.u8string() << ")";

        std::string folderMD5 = ANY_STR(q[0][0]);
        std::string folderPath = ANY_STR(q[0][1]);
        FolderType folderType = (FolderType)ANY_INT(q[0][2]);
        long long folderModifyTimeDB = ANY_INT(q[0][3]);

        if (folderType == FolderType::SONG_BMS)
        {
            // only update song folder if recently modified
            if (folderModifyTime > folderModifyTimeDB)
            {
                count = refreshExistingFolder(folderMD5, path, folderType);
            }
            else
            {
                LOG_VERBOSE << "[SongDB] Skip refreshing song folder: " << folderPath;
            }
        }
        else
        {
            // step in sub folders
            count = refreshExistingFolder(folderMD5, path, folderType);
        }
    }
    else
    {
        count = addNewFolder(folderHash, path, parentHash);
    }

    return count;
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

void SongDB::waitLoadingFinish()
{
    if (threadPool)
    {
        LOG_DEBUG << "[SongDB] Waiting for all loading threads...";

        // wait for all tasks
        boost::asio::thread_pool& pool = *(boost::asio::thread_pool*)threadPool;
        pool.join();

        LOG_DEBUG << "[SongDB] All loading threads finished, continue";

        // The old pool is not valid anymore, removing
        delete (boost::asio::thread_pool*)threadPool;
        threadPool = nullptr;
    }
}

int SongDB::addNewFolder(const HashMD5& hash, const Path& path, const HashMD5& parentHash)
{
    LOG_DEBUG << "[SongDB] Add new folder " << path.u8string();

    FolderType type = FolderType::FOLDER;
    for (auto& f : std::filesystem::directory_iterator(path))
    {
        if (analyzeChartType(f) == eChartFormat::BMS)
        {
            type = FolderType::SONG_BMS;
            break;  // break for
        }
        // else ...
    }

    int ret;
    auto folderName = fs::weakly_canonical(path).filename();
    long long folderModifyTime = getFileLastWriteTime(path);
    if (!parentHash.empty())
    {
        ret = exec("INSERT INTO folder VALUES(?,?,?,?,?,?)", {
            hash.hexdigest(),
            parentHash.hexdigest(),
            folderName.u8string(),
            (int)type,
            path.u8string(),
            folderModifyTime });
    }
    else
    {
        ret = exec("INSERT INTO folder VALUES(?,?,?,?,?,?)", {
            hash.hexdigest(),
            nullptr,
            folderName.u8string(),
            (int)type,
            path.u8string(),
            folderModifyTime });
    }
    if (SQLITE_OK != ret)
    {
        LOG_WARNING << "[SongDB] Insert folder into db fail: [" << ret << "] " << errmsg() << " (" << path.u8string() << ")";
        return -1;
    }

    int count = 0;

    bool isSongFolder = false;
    for (const auto& f : fs::directory_iterator(path))
    {
        if (stopRequested) break;

        if (analyzeChartType(f) != eChartFormat::UNKNOWN)
        {
            isSongFolder = true;
            break;
        }
    }

    std::vector<Path> subFolderList;
    for (const auto& f : fs::directory_iterator(path))
    {
        if (stopRequested) break;

        if (!isSongFolder && fs::is_directory(f))
        {
            subFolderList.push_back(f);
        }
        else if (isSongFolder && analyzeChartType(f) != eChartFormat::UNKNOWN)
        {
            addChartTaskCount++;

            if (!threadPool)
            {
                threadPool = (void*)new boost::asio::thread_pool(poolThreadCount);
            }
            boost::asio::thread_pool& pool = *(boost::asio::thread_pool*)threadPool;
            boost::asio::post(pool, std::bind(&SongDB::addChart, this, hash, f));
            ++count;
        }
    }

    for (const auto& sub : subFolderList)
    {
        if (stopRequested) break;

        int addedCount = addSubFolder(sub, hash);
        if (addedCount > 0)
            count += addedCount;
    }

    return count;
}

int SongDB::refreshExistingFolder(const HashMD5& hash, const Path& path, FolderType type)
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
        LOG_DEBUG << "[SongDB] Re-analyzing" << " (" << path.u8string() << ")";

        // analyze the folder again
        removeFolder(hash, true);
        return addSubFolder(path, hash);
    }
    else if (type == FolderType::SONG_BMS)
    {
        LOG_DEBUG << "[SongDB] Checking for new entries" << " (" << path.u8string() << ")";

        int count = 0;

        // browse chart files
        std::vector<Path> bmsFiles;
        for (auto& f : fs::directory_iterator(path))
        {
            if (analyzeChartType(f) != eChartFormat::UNKNOWN)
                bmsFiles.push_back(fs::absolute(f));
        }
        std::sort(bmsFiles.begin(), bmsFiles.end());

        // get charts from db
        std::vector<Path> existedFiles;
        bool hasDeletedEntry = false;
        bool hasModifiedEntry = false;
        auto existedList = browseSong(hash);
        if (existedList && !existedList->empty())
        {
            for (size_t i = 0; i < existedList->getContentsCount(); ++i)
            {
                existedFiles.push_back(existedList->getChart(i)->absolutePath);
            }
            std::sort(existedFiles.begin(), existedFiles.end());

            // delete file-not-found song entries
            if (!existedFiles.empty())
            {
                std::vector<HashMD5> deletedFiles;
                std::vector<Path> modifiedFiles;

                for (size_t i = 0; i < existedList->getContentsCount(); ++i)
                {
                    std::shared_ptr<ChartFormatBase> chart = existedList->getChart(i);
                    if (!fs::exists(chart->absolutePath))
                    {
                        deletedFiles.push_back(chart->fileHash);
                    }
                    else
                    {
                        long long fstime = getFileLastWriteTime(path);
                        if (auto q = query("SELECT addtime FROM song WHERE md5=? AND parent=?", 1, { chart->fileHash.hexdigest(), hash.hexdigest() }); !q.empty())
                        {
                            long long dbTime = ANY_INT(q[0][0]);

                            if (fstime > dbTime && md5file(chart->absolutePath) != chart->fileHash)
                            {
                                modifiedFiles.push_back(chart->absolutePath);
                            }
                        }
                    }
                }

                hasDeletedEntry = !deletedFiles.empty();
                for (auto& chartMD5 : deletedFiles)
                {
                    if (removeChart(chartMD5, hash))
                    {
                        addChartDeleted++;
                    }
                }

                hasModifiedEntry = !modifiedFiles.empty();
                for (auto& chartPath : modifiedFiles)
                {
                    if (removeChart(chartPath, hash))
                    {
                        addChartModified++;
                    }
                }

                // remove modified files from existedFiles, and add again below
                std::vector<Path> existedFilesReal;
                std::set_difference(existedFiles.begin(), existedFiles.end(), modifiedFiles.begin(), modifiedFiles.end(), std::back_inserter(existedFilesReal));
                std::swap(existedFiles, existedFilesReal);
            }
        }

        // only add new entries
        std::vector<Path> newFiles;
        std::set_difference(bmsFiles.begin(), bmsFiles.end(), existedFiles.begin(), existedFiles.end(), std::back_inserter(newFiles));
        for (auto& p : newFiles)
        {
            if (stopRequested)
            {
                break;
            }
            addChartTaskCount++;

            if (!threadPool)
            {
                threadPool = (void*)new boost::asio::thread_pool(poolThreadCount);
            }
            boost::asio::thread_pool& pool = *(boost::asio::thread_pool*)threadPool;
            boost::asio::post(pool, std::bind(&SongDB::addChart, this, hash, p));
            count++;
        }

        if (hasDeletedEntry || hasModifiedEntry || count > 0)
        {
            // update modification time
            long long nowTime = getFileTimeNow();
            long long folderModifyTime = getFileLastWriteTime(path);
            if (int ret = exec("UPDATE folder SET modtime=? WHERE pathmd5=?", { nowTime, hash.hexdigest() }); ret != SQLITE_OK)
            {
                LOG_WARNING << "[SongDB] Update modification time fail: [" << ret << "] " << errmsg() << " (" << path.u8string() << ")";
            }
    }

        LOG_DEBUG << "[SongDB] Folder originally has " << existedFiles.size() << " entries, added " << count << " (" << path.u8string() << ")";
        return count;
    }
    else
    {
        LOG_DEBUG << "[SongDB] Checking for new subfolders of " << path.u8string();

        // get folders from db
        std::vector<Path> existedFiles;
        bool hasDeletedEntry = false;
        auto existedList = browse(hash, false);
        if (existedList && !existedList->empty())
        {
            for (size_t i = 0; i < existedList->getContentsCount(); ++i)
            {
                existedFiles.push_back(existedList->getEntry(i)->getPath());
            }
            std::sort(existedFiles.begin(), existedFiles.end());

            // delete file-not-found folders
            if (!existedFiles.empty())
            {
                std::vector<HashMD5> deletedFiles;
                for (size_t i = 0; i < existedList->getContentsCount(); ++i)
                {
                    if (!fs::exists(existedList->getEntry(i)->getPath()))
                    {
                        LOG_DEBUG << "[SongDB] Deleting file-not-found folder: " << existedList->getEntry(i)->getPath();
                        deletedFiles.push_back(existedList->getEntry(i)->md5);
                    }
                }
                hasDeletedEntry = !deletedFiles.empty();
                for (auto& folderMD5 : deletedFiles)
                {
                    removeFolder(folderMD5);
                }
            }
        }

        // just add new entries
        int count = 0;

        std::vector<Path> subFolders;
        for (auto& f : fs::directory_iterator(path))
        {
            if (stopRequested)
            {
                break;
            }
            if (fs::is_directory(f))
            {
                int addedCount = addSubFolder(f, hash);
                if (addedCount > 0)
                    count += addedCount;
            }
        }

        if (hasDeletedEntry || count > 0)
        {
            // update modification time
            long long nowTime = getFileTimeNow();
            long long folderModifyTime = getFileLastWriteTime(path);
            if (int ret = exec("UPDATE folder SET modtime=? WHERE pathmd5=?", { nowTime, hash.hexdigest() }); ret != SQLITE_OK)
            {
                LOG_WARNING << "[SongDB] Update modification time fail: [" << ret << "] " << errmsg() << " (" << path.u8string() << ")";
            }
        }

        LOG_DEBUG << "[SongDB] Checking for new subfolders finished. Added " << count << " entries from " << path.u8string();
        return count;
    }
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


std::pair<bool, Path> SongDB::getFolderPath(const HashMD5& folder) const
{
    if (!folderQueryHashMap.empty())
    {
        if (folderQueryHashMap.find(folder) != folderQueryHashMap.end())
        {
            auto cols = folderQueryPool[folderQueryHashMap.at(folder)[0]];
            return { true, PathFromUTF8(ANY_STR(cols[4])) };
        }
    }
    else
    {
        auto result = query("SELECT type,path FROM folder WHERE pathmd5=?", 2, { folder.hexdigest() });
        if (!result.empty())
        {
            auto leaf = result[0];
            //if (ANY_INT(leaf[0]) != FOLDER)
            //{
            //    LOG_WARNING << "[SongDB] Get folder path type error: excepted " << FOLDER << ", get " << ANY_INT(leaf[0]) <<
            //        " (" << folder << ")";
            //    return Path();
            //}
            return { true, PathFromUTF8(ANY_STR(leaf[1])) };
        }
    }
    LOG_INFO << "[SongDB] Get folder path fail: target " << folder.hexdigest() << " not found";
    return { false, Path() };
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



std::shared_ptr<EntryFolderRegular> SongDB::browse(HashMD5 root, bool recursive)
{
    const auto [hasPath, path] = getFolderPath(root);
    if (!hasPath)
    {
        return nullptr;
    }

    LOG_DEBUG << "[SongDB] browse folder " << path.u8string() << (recursive ? " RECURSIVE" : "");

    std::shared_ptr<EntryFolderRegular> list = std::make_shared<EntryFolderRegular>(root, path);

    if (folderQueryParentMap.find(root) != folderQueryParentMap.end())
    {
        for (const auto& index : folderQueryParentMap.at(root))
        {
            const auto& c = folderQueryPool[index];
            auto md5 = ANY_STR(c[0]);
            auto parent = ANY_STR(c[1]);
            auto name = ANY_STR(c[2]);
            auto type = (FolderType)ANY_INT(c[3]);
            auto path = ANY_STR(c[4]);
            auto modtime = ANY_INT(c[5]);

            switch (type)
            {
            case FOLDER:
            {
                if (recursive)
                {
                    auto sub = browse(md5, false);
                    if (sub && !sub->empty())
                    {
                        sub->_name = name;
                        sub->_addTime = modtime;
                        list->pushEntry(sub);
                    }
                }
                else
                {
                    auto sub = std::make_shared<EntryFolderRegular>(md5, PathFromUTF8(path), name, "");
                    sub->_addTime = modtime;
                    list->pushEntry(sub);
                }
                break;
            }
            case SONG_BMS:
                auto bmsList = browseSong(md5);
                // name is set inside browseSong
                if (bmsList && !bmsList->empty())
                {
                    bmsList->_addTime = modtime;
                    list->pushEntry(bmsList);
                }
                break;
            }
        }
    }

    LOG_DEBUG << "[SongDB] browse folder: " << list->getContentsCount() << " entries";

    return list;
}

std::shared_ptr<EntryFolderSong> SongDB::browseSong(HashMD5 root)
{
    LOG_VERBOSE << "[SongDB] browse from " << root.hexdigest();

    const auto [hasPath, path] = getFolderPath(root);
    if (!hasPath)
        return nullptr;

    std::shared_ptr<EntryFolderSong> list = std::make_shared<EntryFolderSong>(root, path);
    bool isNameSet = false;

    if (songQueryParentMap.find(root) != songQueryParentMap.end())
    {
        for (const auto& index : songQueryParentMap.at(root))
        {
            const auto& c = songQueryPool[index];
            auto type = (eChartFormat)ANY_INT(c[3]);
            switch (type)
            {
            case eChartFormat::BMS:
            {
                auto p = std::make_shared<ChartFormatBMSMeta>();
                if (convert_bms(p, c))
                {
                    if (p->fileName.is_absolute())
                        p->absolutePath = p->fileName;
                    else
                        p->absolutePath = path / p->fileName;

                    list->pushChart(p);
                }
                if (!isNameSet)
                {
                    isNameSet = true;
                    list->_name = p->title;
                    list->_name2 = p->title2;
                }
                break;
            }
            default:
                break;
            }
        }
    }

    LOG_VERBOSE << "[SongDB] browsed song: " << list->getContentsCount() << " entries";

    return list;
}


std::shared_ptr<EntryFolderRegular> SongDB::search(HashMD5 root, std::string key)
{
    LOG_DEBUG << "[SongDB] search " << root.hexdigest() << " " << key;

    const auto [hasPath, path] = getFolderPath(root);
    if (!hasPath)
        return nullptr;

    std::shared_ptr<EntryFolderRegular> list = std::make_shared<EntryFolderRegular>(md5(key), "");
    for (auto& c : findChartByName(root, key))
    {
        auto f = std::make_shared<EntryFolderSong>(HashMD5(), "", c->title, c->title2);
        f->pushChart(c);
        list->pushEntry(f);
    }

    LOG_DEBUG << "[SongDB] " << list->getContentsCount() << " entries found";

    return list;
}

void SongDB::resetAddSummary()
{
    addChartTaskCount = 0;
    addChartTaskFinishCount = 0;
    addChartSuccess = 0;
    addChartModified = 0;
    addChartDeleted = 0;
    addCurrentPath.clear();
}

void SongDB::stopLoading()
{
    if (threadPool)
    {
        // stop all tasks
        boost::asio::thread_pool& pool = *(boost::asio::thread_pool*)threadPool;
        pool.stop();

        // The old pool is not valid anymore, removing
        delete (boost::asio::thread_pool*)threadPool;
        threadPool = nullptr;
    }
    stopRequested = true;
}