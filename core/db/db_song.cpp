#include <set>
#include <regex>
#include "utils.h"
#include "db_song.h"
#include "plog/Log.h"
#include "common/chart/chart_types.h"
#include "game/scroll/scroll_types.h"

const HashMD5 ROOT_HASH = md5("", 0);

const char* CREATE_FOLDER_TABLE_STR =
"CREATE TABLE IF NOT EXISTS folder(         \
md5     TEXT     PRIMARY KEY UNIQUE NOT NULL,   \
parent  TEXT                            ,   \
path    TEXT                    NOT NULL,   \
name    TEXT                            ,   \
type    INTEGER                 NOT NULL DEFAULT 0, \
removed INTEGER                 NOT NULL DEFAULT 0  \
)";

const char* CREATE_SONG_TABLE_STR =
"CREATE TABLE IF NOT EXISTS song(           \
id INTEGER PRIMARY KEY AUTOINCREMENT DEFAULT 1, \
md5     TEXT                    NOT NULL,   \
folder  TEXT                    NOT NULL,   \
type    INTEGER                 NOT NULL,   \
file    TEXT                    NOT NULL,   \
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
player      INTEGER                     ,   \
rank        INTEGER                     ,   \
total       INTEGER                     ,   \
playlvl     INTEGER                     ,   \
difficulty  INTEGER                     ,   \
longnote    INTEGER                     ,   \
landmine    INTEGER                     ,   \
barchange   INTEGER                     ,   \
stop        INTEGER                     ,   \
bga         INTEGER                     ,   \
random      INTEGER                     ,   \
FOREIGN KEY (folder) REFERENCES folder(md5) \
)";

SongDB::SongDB(const char* path) : SQLite(path, "SONG")
{
    if (exec(CREATE_FOLDER_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create table folder ERROR! " << errmsg();
        abort();
    }
    if (query("SELECT parent FROM folder WHERE md5=?", 1, { ROOT_HASH }).empty())
    {
        if (exec("INSERT INTO folder(md5,parent,path,name,type) VALUES(?,?,?,?,?)", { ROOT_HASH, nullptr, "", "ROOT", 0 }))
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
    if (exec("CREATE INDEX IF NOT EXISTS index_folder ON song(folder)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create folder index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_search0 ON song(md5)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_search1 ON song(title)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_search2 ON song(title2)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_search3 ON song(artist)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_search4 ON song(artist2)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_search5 ON song(genre)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << errmsg();
    }
    if (exec("CREATE INDEX IF NOT EXISTS index_search6 ON song(version)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << errmsg();
    }

}

int SongDB::addChart(const HashMD5& folder, const Path& path)
{
    auto filename = path.filename();
    if (!query("SELECT md5 FROM song WHERE folder=? AND file=?", 1, { folder, filename.string() }).empty())
    {
        // file exist in db
        if (SQLITE_OK != exec("DELETE FROM song WHERE folder=? AND file=?", { folder, filename.string() }))
        {
            LOG_WARNING << "[SongDB] Remove existing chart from db failed: " << path.string();
            return 1;
        }
    }

    auto c = vChart::getFromFile(path);
    if (c == nullptr)
    {
        LOG_WARNING << "[SongDB] File error: " << path.string();
        return 1;
    }

    auto s = vScroll::getFromChart(c);
    if (s == nullptr)
    {
        LOG_WARNING << "[SongDB] File parsing error: " << path.string();
        return 1;
    }

    switch (c->type())
    {
    case eChartType::BMS:
    {
        auto bmsc = std::reinterpret_pointer_cast<BMS>(c);
        if (SQLITE_OK != exec("INSERT INTO song("
            "md5,folder,type,file,title,title2,artist,artist2,genre,version,level,bpm,minbpm,maxbpm,length,totalnotes,stagefile,"
            "bannerfile,player,rank,total,playlvl,difficulty,longnote,landmine,barchange,stop,bga,random) "
            "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",
            {
                c->_fileHash,
                folder,
                int(c->type()),
                c->_filePath.filename().string(),
                c->_title,
                c->_title2,
                c->_artist,
                c->_artist2,
                c->_genre,
                c->_version,
                c->_level,
                c->_itlBPM,
                c->_minBPM,
                c->_maxBPM,
                s->getTotalLength().norm() / 1000,
                static_cast<int>(s->getNoteCount()),
                c->_BG,
                c->_banner,
                bmsc->player,
                bmsc->rank,
                bmsc->total,
                bmsc->playLevel,
                bmsc->difficulty,
                bmsc->haveLN,
                bmsc->haveMine,
                bmsc->haveBarChange,
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
    if (in.size() < 19) return false;
    // 0: id
    chart->_fileHash = ANY_STR(in[1]);
    chart->_folderHash = ANY_STR(in[2]);
    // 3: type
    chart->_filePath = ANY_STR(in[4]);
    chart->_title = ANY_STR(in[5]);
    chart->_title2 = ANY_STR(in[6]);
    chart->_artist = ANY_STR(in[7]);
    chart->_artist2 = ANY_STR(in[8]);
    chart->_genre = ANY_STR(in[9]);
    chart->_version = ANY_STR(in[10]);
    chart->_level = ANY_REAL(in[11]);
    chart->_itlBPM = ANY_REAL(in[12]);
    chart->_minBPM = ANY_REAL(in[13]);
    chart->_maxBPM = ANY_REAL(in[14]);
    chart->_totalLength_sec = ANY_INT(in[15]);
    chart->_totalnotes = ANY_INT(in[16]);
    chart->_BG = ANY_STR(in[17]);
    chart->_banner = ANY_STR(in[18]);
    return true;
}

bool convert_bms(std::shared_ptr<BMS> chart, const std::vector<std::any>& in)
{
    if (in.size() < 30) return false;
    if (!convert_basic(chart, in)) return false;
    chart->player = ANY_INT(in[19]);
    chart->rank = ANY_INT(in[20]);
    chart->total = ANY_INT(in[21]);
    chart->playLevel = ANY_INT(in[22]);
    chart->difficulty = ANY_INT(in[23]);
    chart->haveLN = ANY_INT(in[24]);
    chart->haveMine = ANY_INT(in[25]);
    chart->haveBarChange = ANY_INT(in[26]);
    chart->haveStop = ANY_INT(in[27]);
    chart->haveBGA = ANY_INT(in[28]);
    chart->haveRandom = ANY_INT(in[29]);
    if (chart->_totalnotes > 0)
    {
        chart->haveNote = true;
        chart->notes = chart->_totalnotes;
    }

    return true;
}



// search from genre, version, artist, artist2, title, title2
std::vector<pChart> SongDB::findChartByName(const HashMD5& folder, const std::string& tag, unsigned limit) const
{
    LOG_INFO << "[SongDB] Search for songs matching: " << tag;

    decltype(query("", {})) result;
    if (limit > 0)
    {
        result = query("SELECT * FROM song WHERE \
folder=? AND \
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
        result = query("SELECT * FROM song WHERE \
folder=? AND \
INSTR(title, ?) OR \
INSTR(title2, ?) OR \
INSTR(artist, ?) OR \
INSTR(artist2, ?) OR \
INSTR(genre, ?) OR \
INSTR(version, ?)", 
29, { folder, tag, tag, tag, tag, tag, tag});
    }

    std::vector<pChart> ret;
    for (const auto& r : result)
    {
        switch (eChartType(ANY_INT(r[2])))
        {
        case eChartType::BMS:
        {
            auto bms = std::make_shared<BMS>();
            bms->player = ANY_INT(r[18]);
            bms->rank = ANY_INT(r[19]);
            bms->total = ANY_INT(r[20]);
            bms->playLevel = ANY_INT(r[21]);
            bms->difficulty = ANY_INT(r[22]);
            bms->haveLN = ANY_INT(r[23]);
            bms->haveMine = ANY_INT(r[24]);
            bms->haveBarChange = ANY_INT(r[25]);
            bms->haveStop = ANY_INT(r[26]);
            bms->haveBGA = ANY_INT(r[27]);
            bms->haveRandom = ANY_INT(r[28]);
            ret.push_back(bms);
            break;
        }

        default: break;
        }
    }

    LOG_INFO << "[SongDB] found " << ret.size() << " songs";
    return ret;
}

// chart may duplicate, return all found
std::vector<pChart> SongDB::findChartByHash(const HashMD5& target) const
{
    LOG_INFO << "[SongDB] Search for song " << target;

    auto result = query("SELECT * FROM song WHERE md5=?", 29, { target });

    std::vector<pChart> ret;
    for (const auto& r : result)
    {
        switch (eChartType(ANY_INT(r[2])))
        {
        case eChartType::BMS:
        {
            auto bms = std::make_shared<BMS>();
            bms->player = ANY_INT(r[18]);
            bms->rank = ANY_INT(r[19]);
            bms->total = ANY_INT(r[20]);
            bms->playLevel = ANY_INT(r[21]);
            bms->difficulty = ANY_INT(r[22]);
            bms->haveLN = ANY_INT(r[23]);
            bms->haveMine = ANY_INT(r[24]);
            bms->haveBarChange = ANY_INT(r[25]);
            bms->haveStop = ANY_INT(r[26]);
            bms->haveBGA = ANY_INT(r[27]);
            bms->haveRandom = ANY_INT(r[28]);
            ret.push_back(bms);
            break;
        }

        default: break;
        }
    }

    LOG_INFO << "[SongDB] found " << ret.size() << " songs";
    return ret;

}

int SongDB::addFolder(Path path)
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

    HashMD5 folderHash = md5(path.string());
    if (auto q = query("select md5,type from folder where path=?", 2, { path.string() }); !q.empty())
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
                if (matchChartType(f) == eChartType::BMS)
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
            ret = exec("INSERT INTO folder VALUES(?,?,?,?,?,?)", {
            folderHash,
            parentHash,
            fs::absolute(path).string(),
            filename.string(),
            (int)type,
            0 });
        else
            ret = exec("INSERT INTO folder VALUES(?,?,?,?,?,?)", {
            folderHash,
            nullptr,
            fs::absolute(path).string(),
            filename.string(),
            (int)type,
            0 });
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
        if (fs::is_regular_file(f) && matchChartType(f) != eChartType::UNKNOWN)
        {
            isSongFolder = true;
            break;
        }
    }

    for (const auto& f : files)
    {
        if (!isSongFolder && fs::is_directory(f))
        {
            if (0 == addFolder(f))
                ++count;
        }
        else if (isSongFolder && fs::is_regular_file(f) && matchChartType(f) != eChartType::UNKNOWN)
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
        if (fs::is_regular_file(f) && matchChartType(f) != eChartType::UNKNOWN)
        {
            isSongFolder = true;
            break;
        }
    }

    if ((type == FolderType::SONG_BMS) != isSongFolder)
    {
        removeFolder(hash, true);
        return addFolder(path);
    }

    for (const auto& f : files)
    {
        if (!isSongFolder && fs::is_directory(f))
        {
            if (0 == addFolder(f))
                ++count;
        }
        else if (isSongFolder && fs::is_regular_file(f) && matchChartType(f) != eChartType::UNKNOWN)
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
    auto result = query("SELECT path,type,removed FROM folder WHERE md5=?", 3, { parentHash });
    if (!result.empty())
    {
        for (const auto& leaf : result)
            if (ANY_INT(leaf[2]) == 0 && ANY_INT(leaf[1]) == FOLDER)
            {
                return parentHash;
            }
    }

    return "";

    // return from db directly if found
    if (auto q = query("SELECT parent FROM folder WHERE path=?", 3, { path.string() }); !q.empty())
    {
        HashMD5 parent = ANY_STR(q[0]);
        if (!parent.empty()) return parent;
    }

    // search
    if (isParentPath(executablePath, path))
    {
        auto relative = fs::absolute(path).lexically_relative(executablePath);
        Path currIteratePath;
        HashMD5 currIterateHash = ROOT_HASH;
        for (auto p : relative)
        {
            auto result = query("SELECT path,type,removed FROM folder WHERE md5=?", 3, { currIterateHash });
            if (!result.empty())
            {
                for (const auto& leaf : result)
                    if (ANY_INT(leaf[2]) != 0 && ANY_INT(leaf[1]) == FOLDER)
                    {
                        currIteratePath /= p;
                        currIterateHash = md5(currIteratePath.string());
                        break;
                    }
            }
            else
            {
                LOG_INFO << "[SongDB] Folder parent not found for " << path.string();
                currIterateHash = "";
                break;
            }
        }

        return currIterateHash;
    }

    else if (path.is_absolute())
    {
        auto parent = (path / "..").lexically_normal();
        HashMD5 parentHash = md5(parent.string());
        auto result = query("SELECT path,type,removed FROM folder WHERE md5=?", 3, { parentHash });
        if (!result.empty())
        {
            for (const auto& leaf : result)
                if (ANY_INT(leaf[2]) != 0 && ANY_INT(leaf[1]) == FOLDER)
                {
                    return parentHash;
                }
        }
    }

    return "";
}

int SongDB::removeFolder(const HashMD5& hash, bool removeFromDb)
{
    if (!removeFromDb)
        return exec("UPDATE folder SET removed=1 WHERE md5=?", { hash });

    if (SQLITE_OK == exec("DELETE FROM song WHERE folder=?", { hash }))
        return exec("DELETE FROM folder WHERE md5=?", { hash });

    return -1;
}

HashMD5 SongDB::getFolderParent(const HashMD5& folder) const
{
    auto result = query("SELECT type,parent FROM folder WHERE md5=?", 2, { folder });
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


Path SongDB::getFolderPath(const HashMD5& folder) const
{
    auto result = query("SELECT type,path FROM folder WHERE md5=?", 2, { folder });
    if (!result.empty())
    {
        auto leaf = result[0];
        //if (ANY_INT(leaf[0]) != FOLDER)
        //{
        //    LOG_WARNING << "[SongDB] Get folder path type error: excepted " << FOLDER << ", get " << ANY_INT(leaf[0]) <<
        //        " (" << folder << ")";
        //    return Path();
        //}
        return ANY_STR(leaf[1]);
    }
    LOG_INFO << "[SongDB] Get folder path fail: target " << folder << " not found";
    return Path();
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
    auto path = getFolderPath(root);
    if (path.empty())
        return FolderRegular("");

    FolderRegular list(path);

    auto result = query("SELECT md5,path,name,type,removed FROM folder WHERE parent=?", 5, { root });
    if (!result.empty())
    {
        for (auto& c : result)
        {
            auto md5 = ANY_STR(c[0]);
            auto path = ANY_STR(c[1]);
            auto name = ANY_STR(c[2]);
            auto type = ANY_INT(c[3]);
            auto removed = ANY_INT(c[4]);

            switch (type)
            {
            case FOLDER:
                list.pushEntry(std::make_shared<FolderRegular>(browse(md5, false)));
                break;
            case SONG_BMS:
                auto bmsList = std::make_shared<FolderSong>(browseSong(md5));
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
    auto path = getFolderPath(root);
    if (path.empty())
        return FolderSong("");

    FolderSong list(path);
    auto result = query("SELECT * from song WHERE folder=?", 30, { root });
    if (!result.empty())
    {
        for (auto& c : result)
        {
            auto type = (eChartType)ANY_INT(c[3]);
            switch (type)
            {
            case eChartType::BMS:
            {
                auto p = std::make_shared<BMS>();
                if (convert_bms(p, c)) list.pushChart(p);
                break;
            }
            default:
                break;
            }
        }
    }

    return list;
}


FolderSong SongDB::search(HashMD5 root, std::string key)
{
    auto path = getFolderPath(root);
    if (path.empty())
        return FolderSong("");

    FolderSong list(root);
    for (auto& c : findChartByName(root, key))
    {
        list.pushChart(c);
    }

    return list;
}
