#pragma once
#include <string>
#include <vector>
#include <memory>
#include "db_conn.h"
#include "common/types.h"
#include "common/utils.h"
#include "common/entry/entry_folder.h"
#include "common/entry/entry_song.h"

class ChartFormatBase;
typedef std::shared_ptr<ChartFormatBase> pChartFormat;

inline const HashMD5 ROOT_FOLDER_HASH = md5("", 0);

/* TABLE folder:
    md5(TEXT), parent(TEXT), path(TEXT), name(TEXT), type(INTEGER), removed(INTEGER)
    md5: hash string, calculated by "relative path to exe" OR "absolute path"
    parent: parent dir md5
    path: "relative path to exe" OR "absolute path"
    name: typically folder name, use path if NULL
    type: see enum FolderType
    removed: whether this folder has been removed
*/
/* TABLE song:
    md5(TEXT), folder(TEXT), type(INTEGER), file(TEXT), ...
    md5: file hash
    folder: folder hash
    type: see enum eChartFormat in chartformat.h
    file: file name (not including path)
*/
class SongDB: public SQLite
{
public:
    enum FolderType
    {
        FOLDER,
        SONG_BMS,

        CUSTOM_FOLDER,
        COURSE,
        GRADE,
    };

public:
    SongDB() = delete;
    SongDB(const char* path);
    SongDB(Path& path) : SongDB(path.u8string().c_str()) {}
    ~SongDB() = default;
    SongDB(SongDB&) = delete;
    SongDB& operator= (SongDB&) = delete;

protected:
    int addChart(const HashMD5& folder, const Path& path);
    int removeChart(const HashMD5& md5);
    
public:
    std::vector<pChartFormat> findChartByName(const HashMD5& folder, const std::string&, unsigned limit = 1000) const;  // search from genre, version, artist, artist2, title, title2
    std::vector<pChartFormat> findChartByHash(const HashMD5&) const;  // chart may duplicate

protected:
    int addFolderCharts(const HashMD5& hash, const Path& folder);

public:
    int addFolder(Path path, HashMD5 parent = ROOT_FOLDER_HASH);
    int refreshFolder(const HashMD5& hash, const Path& path, FolderType type);
    int removeFolder(const HashMD5& hash, bool removeSong = false);

    HashMD5 getFolderParent(const HashMD5& folder) const;
    HashMD5 getFolderParent(const Path& path) const;
    int getFolderPath(const HashMD5& folder, Path& output) const;
    HashMD5 getFolderHash(Path path) const;

    EntryFolderRegular browse(HashMD5 root, bool recursive = true);
    EntryFolderSong browseSong(HashMD5 root);
    EntryFolderRegular search(HashMD5 root, std::string key);

};
