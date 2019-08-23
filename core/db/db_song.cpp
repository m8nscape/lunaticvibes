#include "utils.h"
#include "db_song.h"
#include "plog/Log.h"
#include "common/chart/chart_types.h"
#include "game/scroll/scroll_types.h"

const HashMD5 ROOT_HASH = md5("ROOT", 4);

const char* CREATE_FOLDER_TABLE_STR =
"CREATE TABLE IF NOT EXISTS folder(         \
pathmd5 TEXT     PRIMARY KEY UNIQUE NOT NULL,   \
parent  TEXT                            ,   \
name    TEXT                    NOT NULL,   \
type    INTEGER                 NOT NULL DEFAULT 0, \
removed INTEGER                 NOT NULL DEFAULT 0  \
)";

const char* CREATE_SONG_TABLE_STR =
"CREATE TABLE IF NOT EXISTS song(           \
songmd5 TEXT     PRIMARY KEY    NOT NULL,   \
folder  TEXT     FOREIGN KEY REFERENCES folder(pathmd5), \
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
random      INTEGER                         \
)";

SongDB::SongDB() : SQLite("song.db", "SONG")
{
    if (_inst.exec(CREATE_FOLDER_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create table folder ERROR! " << _inst.errmsg();
        abort();
    }
    if (_inst.query("SELECT parent FROM folder WHERE pathmd5=?", 1, { ROOT_HASH }).empty())
    {
        if (_inst.exec("INSERT INTO folder VALUES(?,?,?,?,?", { ROOT_HASH, nullptr, "", 0, 0 }))
        {
            LOG_ERROR << "[SongDB] Insert root folder to table ERROR! " << _inst.errmsg();
            abort();
        }
    }
    

    if (_inst.exec(CREATE_SONG_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create table song ERROR! " << _inst.errmsg();
        abort();
    }

    if (_inst.exec("CREATE INDEX IF NOT EXISTS index_parent ON folder(parent)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create parent index for folder ERROR! " << _inst.errmsg();
    }
    if (_inst.exec("CREATE INDEX IF NOT EXISTS index_folder ON song(folder)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create folder index for song ERROR! " << _inst.errmsg();
    }
    if (_inst.exec("CREATE INDEX IF NOT EXISTS index_search ON song(title)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << _inst.errmsg();
    }
    if (_inst.exec("CREATE INDEX IF NOT EXISTS index_search ON song(title2)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << _inst.errmsg();
    }
    if (_inst.exec("CREATE INDEX IF NOT EXISTS index_search ON song(artist)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << _inst.errmsg();
    }
    if (_inst.exec("CREATE INDEX IF NOT EXISTS index_search ON song(artist2)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << _inst.errmsg();
    }
    if (_inst.exec("CREATE INDEX IF NOT EXISTS index_search ON song(genre)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << _inst.errmsg();
    }
    if (_inst.exec("CREATE INDEX IF NOT EXISTS index_search ON song(version)") != SQLITE_OK)
    {
        LOG_ERROR << "[SongDB] Create search index for song ERROR! " << _inst.errmsg();
    }

}

int SongDB::addChart(const std::string& path)
{
    auto c = vChart::getFromFile(path);
    if (c == nullptr)
    {
        LOG_WARNING << "[SongDB] File error: " << path;
        return 1;
    }

    auto s = vScroll::getFromChart(c);
    if (s == nullptr)
    {
        LOG_WARNING << "[SongDB] File parsing error: " << path;
        return 1;
    }

    switch (c->type())
    {
    case eChartType::BMS:
    {
        auto bmsc = std::reinterpret_pointer_cast<BMS>(c);
        if (SQLITE_OK != _inst.exec("INSERT INTO song VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",
            {
                c->_fileHash,
                c->_filePath.parent_path().string(),
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
            LOG_WARNING << "[SongDB] Insert into db error: " << path << ": " << _inst.errmsg();
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
    if (SQLITE_OK != _inst.exec("DELETE FROM song WHERE songmd5=?", { md5 }))
    {
        LOG_WARNING << "[SongDB] Delete from db error: " << md5 << ": " << _inst.errmsg();
        return 1;
    }
    return 0;
}

auto convert_basic(pChart chart, const std::vector<std::any>& in)
{
    if (in.size() < 18) return false;
    chart->_fileHash = std::any_cast<std::string>(in[0]);
    chart->_folderHash = std::any_cast<std::string>(in[1]);
    chart->_filePath = std::any_cast<std::string>(in[3]);
    chart->_title = std::any_cast<std::string>(in[4]);
    chart->_title2 = std::any_cast<std::string>(in[5]);
    chart->_artist = std::any_cast<std::string>(in[6]);
    chart->_artist2 = std::any_cast<std::string>(in[7]);
    chart->_genre = std::any_cast<std::string>(in[8]);
    chart->_version = std::any_cast<std::string>(in[9]);
    chart->_level = std::any_cast<double>(in[10]);
    chart->_itlBPM = std::any_cast<double>(in[11]);
    chart->_minBPM = std::any_cast<double>(in[12]);
    chart->_maxBPM = std::any_cast<double>(in[13]);
    chart->_totalLength_sec = std::any_cast<sqlite3_int64>(in[14]);
    chart->_totalnotes = std::any_cast<sqlite3_int64>(in[15]);
    chart->_BG = std::any_cast<std::string>(in[16]);
    chart->_banner = std::any_cast<std::string>(in[17]);
    return true;
}

// search from genre, version, artist, artist2, title, title2
std::vector<pChart> SongDB::findChartByName(const HashMD5& folder, const std::string& tag)
{
    LOG_INFO << "[SongDB] Search for songs matching: " << tag;

    auto result = _inst.query("SELECT * FROM song WHERE \
folder=? AND \
INSTR(title, ?) != 0 OR \
INSTR(title2, ?) OR \
INSTR(artist, ?) != 0 OR \
INSTR(artist2, ?) OR \
INSTR(genre, ?) != 0 OR \
INSTR(version, ?)",
29, { folder, tag, tag, tag, tag, tag, tag });

    std::vector<pChart> ret;
    for (const auto& r : result)
    {
        switch (eChartType(std::any_cast<sqlite3_int64>(r[2])))
        {
        case eChartType::BMS:
        {
            auto bms = std::make_shared<BMS>();
            bms->player = std::any_cast<sqlite3_int64>(r[18]);
            bms->rank = std::any_cast<sqlite3_int64>(r[19]);
            bms->total = std::any_cast<sqlite3_int64>(r[20]);
            bms->playLevel = std::any_cast<sqlite3_int64>(r[21]);
            bms->difficulty = std::any_cast<sqlite3_int64>(r[22]);
            bms->haveLN = std::any_cast<sqlite3_int64>(r[23]);
            bms->haveMine = std::any_cast<sqlite3_int64>(r[24]);
            bms->haveBarChange = std::any_cast<sqlite3_int64>(r[25]);
            bms->haveStop = std::any_cast<sqlite3_int64>(r[26]);
            bms->haveBGA = std::any_cast<sqlite3_int64>(r[27]);
            bms->haveRandom = std::any_cast<sqlite3_int64>(r[28]);
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
std::vector<pChart> SongDB::findChartByHash(const HashMD5& target)
{
    LOG_INFO << "[SongDB] Search for song " << target;

    auto result = _inst.query("SELECT * FROM song WHERE MD5=?", 29, { target });

    std::vector<pChart> ret;
    for (const auto& r : result)
    {
        switch (eChartType(std::any_cast<sqlite3_int64>(r[2])))
        {
        case eChartType::BMS:
        {
            auto bms = std::make_shared<BMS>();
            bms->player = std::any_cast<sqlite3_int64>(r[18]);
            bms->rank = std::any_cast<sqlite3_int64>(r[19]);
            bms->total = std::any_cast<sqlite3_int64>(r[20]);
            bms->playLevel = std::any_cast<sqlite3_int64>(r[21]);
            bms->difficulty = std::any_cast<sqlite3_int64>(r[22]);
            bms->haveLN = std::any_cast<sqlite3_int64>(r[23]);
            bms->haveMine = std::any_cast<sqlite3_int64>(r[24]);
            bms->haveBarChange = std::any_cast<sqlite3_int64>(r[25]);
            bms->haveStop = std::any_cast<sqlite3_int64>(r[26]);
            bms->haveBGA = std::any_cast<sqlite3_int64>(r[27]);
            bms->haveRandom = std::any_cast<sqlite3_int64>(r[28]);
            ret.push_back(bms);
            break;
        }

        default: break;
        }
    }

    LOG_INFO << "[SongDB] found " << ret.size() << " songs";
    return ret;

}

int SongDB::addFolder(const std::string& path)
{
    // TODO addFolder
    return 0;
}

int SongDB::removeFolder(const std::string& path)
{
    // TODO removeFolder
    return 0;
}

Path SongDB::getFolderPath(const HashMD5& folder)
{
    auto result = _inst.query("SELECT * FROM song WHERE pathmd5=?", 5, { folder });
    if (!result.empty())
    {
        auto leaf = result[0];
        Path path = std::any_cast<std::string>(leaf[2]);
        while (leaf[1].type() == typeid(std::string))
        {
            HashMD5 parent = std::any_cast<std::string>(leaf[1]);
            if (!parent.empty())
            {
                auto more = _inst.query("SELECT * FROM song WHERE pathmd5=?", 5, { parent });
                auto folder = std::any_cast<std::string>(more[2]);
                if (!folder.empty()) path = folder / path;
                leaf = more[0];
            }
        }
        return executablePath / path;
    }
    return Path();
}
