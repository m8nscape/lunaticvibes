#include <set>
#include <regex>
#include "common/utils.h"
#include "db_song.h"
#include "common/log.h"
#include "common/chartformat/chartformat_types.h"
#include "game/chart/chart_types.h"

const char* CREATE_FOLDER_TABLE_STR =
"CREATE TABLE IF NOT EXISTS folder(         \
pathmd5 TEXT     PRIMARY KEY UNIQUE NOT NULL,   \
parent  TEXT                                ,   \
name    TEXT                                ,   \
type    INTEGER                     NOT NULL DEFAULT 0, \
path    TEXT                        NOT NULL    \
);";

const char* CREATE_SONG_TABLE_STR =
"CREATE TABLE IF NOT EXISTS song(           \
md5     TEXT                    NOT NULL,   \
parent  TEXT                    NOT NULL,   \
file    TEXT                    NOT NULL,   \
type    INTEGER                 NOT NULL,   \
title   TEXT                    NOT NULL,   \
title2  TEXT                    NOT NULL,   \
artist  TEXT                    NOT NULL,   \
artist2 TEXT                    NOT NULL,   \
genre   TEXT                    NOT NULL,   \
version TEXT                    NOT NULL,   \
level   REAL                    NOT NULL,   \
bpm     REAL                    NOT NULL,   \
minbpm  REAL                    NOT NULL,   \
maxbpm  REAL                    NOT NULL,   \
length  INTEGER                 NOT NULL,   \
totalnotes  INTEGER             NOT NULL,   \
stagefile   TEXT                        ,   \
bannerfile  TEXT                        ,   \
gamemode    INTEGER                     ,   \
judgerank   INTEGER                     ,   \
total       INTEGER                     ,   \
playlevel   INTEGER                     ,   \
difficulty  INTEGER                     ,   \
longnote    INTEGER                     ,   \
landmine    INTEGER                     ,   \
metricmod   INTEGER                     ,   \
stop        INTEGER                     ,   \
bga         INTEGER                     ,   \
random      INTEGER                     ,   \
addtime     INTEGER                     ,   \
CONSTRAINT pk_pf PRIMARY KEY (parent,file)  \
);";


SongDB::SongDB(const char* path) : SQLite(path, "SONG")
{
    if (exec(CREATE_FOLDER_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create table folder ERROR! " << errmsg();
        abort();
    }
    if (query("SELECT parent FROM folder WHERE pathmd5=?", 1, { ROOT_FOLDER_HASH }).empty())
    {
        if (exec("INSERT INTO folder(pathmd5,parent,path,name,type) VALUES(?,?,?,?,?)", { ROOT_FOLDER_HASH, nullptr, "", "ROOT", 0 }))
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
    decltype(path.filename().string()) filename;
    try
    {
        filename = path.filename().string();
    }
    catch (const std::exception& e)
    {
        LOG_WARNING << "[SongDB] "<< e.what() << ": " << path.filename().wstring();
        return 1;
    }

    if (auto result = query("SELECT md5 FROM song WHERE parent=? AND file=?", 1, { folder, filename }); !result.empty() && !result[0].empty())
    {
        // file exists in db
        auto dbmd5 = ANY_STR(result[0][0]);
        auto filemd5 = md5file(path);
        if (dbmd5 == filemd5)
        {
            return 0;
        }
        else
        {
            LOG_INFO << "[SongDB] File " << path.string() << " exists, but hash not match. Removing old entry from db";
            if (SQLITE_OK != exec("DELETE FROM song WHERE parent=? AND file=?", { folder, filename }))
            {
                LOG_WARNING << "[SongDB] Remove existing chart from db failed: " << path.string();
                return 1;
            }
        }
    }

    auto c = vChartFormat::getFromFile(path);
    if (c == nullptr)
    {
        LOG_WARNING << "[SongDB] File error: " << path.string();
        return 1;
    }

    auto s = chart::vChart::createFromChartFormat(c);
    if (s == nullptr)
    {
        LOG_WARNING << "[SongDB] File parsing error: " << path.string();
        return 1;
    }

    switch (c->type())
    {
    case eChartFormat::BMS:
    {
        auto bmsc = std::reinterpret_pointer_cast<BMS>(c);
        if (SQLITE_OK != exec("INSERT INTO song("
            "md5,parent,type,file,title,title2,artist,artist2,genre,version,level,bpm,minbpm,maxbpm,length,totalnotes,stagefile,bannerfile,"
            "gamemode,judgerank,total,playlevel,difficulty,longnote,landmine,metricmod,stop,bga,random) "
            "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
            {
                c->fileHash,
                folder,
                int(c->type()),
                c->filePath.filename().string(),
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
                bmsc->haveRandom
            }))
        {
            LOG_WARNING << "[SongDB] Insert into db error: " << path.string() << ": " << errmsg();
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
    if (SQLITE_OK != exec("DELETE FROM song WHERE md5=?", { md5 }))
    {
        LOG_WARNING << "[SongDB] Delete from db error: " << md5 << ": " << errmsg();
        return 1;
    }
    return 0;
}

bool convert_basic(pChart chart, const std::vector<std::any>& in)
{
    if (in.size() < 18) return false;
    chart->fileHash = ANY_STR(in[0]);
    chart->folderHash = ANY_STR(in[1]);
    chart->filePath = ANY_STR(in[2]);
    // 3: type
    chart->title = ANY_STR(in[4]);
    chart->title2 = ANY_STR(in[5]);
    chart->artist = ANY_STR(in[6]);
    chart->artist2 = ANY_STR(in[7]);
    chart->genre = ANY_STR(in[8]);
    chart->version = ANY_STR(in[9]);
    chart->levelEstimated = ANY_REAL(in[10]);
    chart->startBPM = ANY_REAL(in[11]);
    chart->minBPM = ANY_REAL(in[12]);
    chart->maxBPM = ANY_REAL(in[13]);
    chart->totalLength = (int)ANY_INT(in[14]);
    chart->totalNotes = (int)ANY_INT(in[15]);
    chart->stagefile = ANY_STR(in[16]);
    chart->banner = ANY_STR(in[17]);
    return true;
}

bool convert_bms(std::shared_ptr<BMS_prop> chart, const std::vector<std::any>& in)
{
    if (in.size() < 29) return false;
    if (!convert_basic(chart, in)) return false;
    chart->gamemode = (int)ANY_INT(in[18]);
    chart->rank = (int)ANY_INT(in[19]);
    chart->total = (int)ANY_INT(in[20]);
    chart->playLevel = (int)ANY_INT(in[21]);
    chart->difficulty = (int)ANY_INT(in[22]);
    chart->haveLN = (bool)ANY_INT(in[23]);
    chart->haveMine = (bool)ANY_INT(in[24]);
    chart->haveMetricMod = (bool)ANY_INT(in[25]);
    chart->haveStop = (bool)ANY_INT(in[26]);
    chart->haveBGA = (bool)ANY_INT(in[27]);
    chart->haveRandom = (bool)ANY_INT(in[28]);
    if (chart->totalNotes > 0)
    {
        chart->haveNote = true;
        chart->notes = chart->totalNotes;
    }

    return true;
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
            bms->gamemode = (int)ANY_INT(r[18]);
            bms->rank = (int)ANY_INT(r[19]);
            bms->total = (int)ANY_INT(r[20]);
            bms->playLevel = (int)ANY_INT(r[21]);
            bms->difficulty = (int)ANY_INT(r[22]);
            bms->haveLN = (bool)ANY_INT(r[23]);
            bms->haveMine = (bool)ANY_INT(r[24]);
            bms->haveMetricMod = (bool)ANY_INT(r[25]);
            bms->haveStop = (bool)ANY_INT(r[26]);
            bms->haveBGA = (bool)ANY_INT(r[27]);
            bms->haveRandom = (bool)ANY_INT(r[28]);
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
parent=? AND \
INSTR(title, ?) OR \
INSTR(title2, ?) OR \
INSTR(artist, ?) OR \
INSTR(artist2, ?) OR \
INSTR(genre, ?) OR \
INSTR(version, ?) \
LIMIT ?",
29, { folder, tag, tag, tag, tag, tag, tag, limit });
    }
    else
    {
        result = query(
"SELECT * FROM song WHERE \
parent=? AND \
INSTR(title, ?) OR \
INSTR(title2, ?) OR \
INSTR(artist, ?) OR \
INSTR(artist2, ?) OR \
INSTR(genre, ?) OR \
INSTR(version, ?)", 
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
    LOG_INFO << "[SongDB] Search for song " << target;

    auto result = query("SELECT * FROM song WHERE md5=?", 29, { target });

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
        LOG_WARNING << "[SongDB] Add folder fail: folder not exist (" << path.string() << ")";
        return 1;
    }
    if (!fs::is_directory(path))
    {
        LOG_WARNING << "[SongDB] Add folder fail: path is not folder (" << path.string() << ")";
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

    HashMD5 folderHash = md5(path.string());
    if (auto q = query("select pathmd5,type from folder where path=?", 2, { path.string() }); !q.empty())
    {
        LOG_INFO << "[SongDB] Folder already exists (" << path.string() << ")";
        refreshFolderContent(ANY_STR(q[0][0]), path, (FolderType)ANY_INT(q[0][1]));
        return 0;
    }
    else
    {
        auto files = findFiles(path / "*");
        FolderType type = FolderType::FOLDER;
        for (auto& f : files)
        {
            if (fs::is_regular_file(f))
            {
                if (matchChartType(f) == eChartFormat::BMS)
                {
                    type = FolderType::SONG_BMS;
                    break;  // break for
                }
                // else ...
            }
        }

        int ret;
        auto filename = fs::weakly_canonical(path).filename();
        if (!parentHash.empty())
            ret = exec("INSERT INTO folder VALUES(?,?,?,?,?)", {
            folderHash,
            parentHash,
            filename.string(),
            (int)type,
            path.string() });
        else
            ret = exec("INSERT INTO folder VALUES(?,?,?,?,?)", {
            folderHash,
            nullptr,
            filename.string(),
            (int)type,
            path.string() });
        if (SQLITE_OK != ret)
        {
            LOG_WARNING << "[SongDB] Add folder fail: [" << ret << "] " << errmsg() << " (" << path.string() << ")";
            return 1;
        }
    }

    int count = addFolderContent(folderHash, path);
    LOG_INFO << "[SongDB] " << path.string() << ": added " << count << " entries";
    return 0;
}

int SongDB::addFolderContent(const HashMD5& hash, const Path& folder)
{
    int count = 0;
    auto files = findFiles(folder / "*");

    bool isSongFolder = false;
    for (auto& f : files)
    {
        if (fs::is_regular_file(f) && matchChartType(f) != eChartFormat::UNKNOWN)
        {
            isSongFolder = true;
            break;
        }
    }

    for (const auto& f : files)
    {
        if (!isSongFolder && fs::is_directory(f))
        {
            if (0 == addFolder(f, hash))
                ++count;
        }
        else if (isSongFolder && fs::is_regular_file(f) && matchChartType(f) != eChartFormat::UNKNOWN)
        {
            if (0 == addChart(hash, f))
                ++count;
        }
    }
    return count;
}

int SongDB::refreshFolderContent(const HashMD5& hash, const Path& path, FolderType type)
{
    int count = 0;
    auto files = findFiles(path / "*");

    bool isSongFolder = false;
    for (auto& f : files)
    {
        if (fs::is_regular_file(f) && matchChartType(f) != eChartFormat::UNKNOWN)
        {
            isSongFolder = true;
            break;
        }
    }

    if ((type == FolderType::SONG_BMS) != isSongFolder)
    {
        removeFolder(hash, true);
        return addFolder(path, hash);
    }

    for (const auto& f : files)
    {
        if (!isSongFolder && fs::is_directory(f))
        {
            if (0 == addFolder(f, hash))
                ++count;
        }
        else if (isSongFolder && fs::is_regular_file(f) && matchChartType(f) != eChartFormat::UNKNOWN)
        {
            if (0 == addChart(hash, f))
                ++count;
        }
    }
    return count;
}

HashMD5 SongDB::getFolderParent(const Path& path) const
{
    if (!fs::is_directory(path)) return "";

    auto parent = (path / "..").lexically_normal();
    HashMD5 parentHash = md5(parent.string());
    auto result = query("SELECT name,type FROM folder WHERE parent=?", 3, { parentHash });
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
        if (SQLITE_OK != exec("DELETE FROM song WHERE parent=?", { hash }))
            LOG_WARNING << "[SongDB] remove song from db error: " << errmsg();
    }

    return exec("DELETE FROM folder WHERE pathmd5=?", { hash });
}

HashMD5 SongDB::getFolderParent(const HashMD5& folder) const
{
    auto result = query("SELECT type,parent FROM folder WHERE path=?", 2, { folder });
    if (!result.empty())
    {
        auto leaf = result[0];
        if (ANY_INT(leaf[0]) != FOLDER)
        {
            LOG_WARNING << "[SongDB] Get folder parent type error: excepted " << FOLDER << ", get " << ANY_INT(leaf[0]) <<
                " (" << folder << ")";
            return "";
        }
        return ANY_STR(leaf[1]);
    }
    LOG_INFO << "[SongDB] Get folder parent fail: target " << folder << " not found";
    return "";
}


int SongDB::getFolderPath(const HashMD5& folder, Path& output) const
{
    auto result = query("SELECT type,path FROM folder WHERE pathmd5=?", 2, { folder });
    if (!result.empty())
    {
        auto leaf = result[0];
        //if (ANY_INT(leaf[0]) != FOLDER)
        //{
        //    LOG_WARNING << "[SongDB] Get folder path type error: excepted " << FOLDER << ", get " << ANY_INT(leaf[0]) <<
        //        " (" << folder << ")";
        //    return Path();
        //}
        output = ANY_STR(leaf[1]);
        return 0;
    }
    LOG_INFO << "[SongDB] Get folder path fail: target " << folder << " not found";
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
    return md5(path.string());
}



FolderRegular SongDB::browse(HashMD5 root, bool recursive)
{
    Path path;
    if (getFolderPath(root, path) < 0)
        return FolderRegular("", path);

    FolderRegular list(root, path);

    auto result = query("SELECT pathmd5,parent,name,type FROM folder WHERE parent=?", 5, { root });
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
        return FolderSong("", path);

    FolderSong list(root, path);
    bool isNameSet = false;

    auto result = query("SELECT * from song WHERE parent=?", 30, { root });
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
        return FolderRegular("", "");

    FolderRegular list("", "");
    for (auto& c : findChartByName(root, key))
    {
        //list.pushChart(c);
    }

    return list;
}
