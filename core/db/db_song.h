#pragma once
#include <string>
#include <vector>
#include <memory>
#include "types.h"
#include "db_conn.h"
#include "entries/folder.h"

class vChart;
typedef std::shared_ptr<vChart> pChart;

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
    type: see enum eChartType in chart.h
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
    SongDB(Path& path) : SongDB(path.string().c_str()) {}
    ~SongDB() = default;
    SongDB(SongDB&) = delete;
    SongDB& operator= (SongDB&) = delete;

protected:
    int addChart(const HashMD5& folder, const Path& path);
    int removeChart(const HashMD5& md5);
    
public:
    std::vector<pChart> findChartByName(const HashMD5& folder, const std::string&, unsigned limit = 1000) const;  // search from genre, version, artist, artist2, title, title2
    std::vector<pChart> findChartByHash(const HashMD5&) const;  // chart may duplicate

    int addFolder(Path path);
    int addFolderContent(const HashMD5& hash, const Path& folder);
    int refreshFolderContent(const HashMD5& hash, const Path& path, FolderType type);
    int removeFolder(const HashMD5& hash, bool removeFromDb = false);

    HashMD5 getFolderParent(const HashMD5& folder) const;
    HashMD5 getFolderParent(const Path& path) const;
    Path getFolderPath(const HashMD5& folder) const;
    HashMD5 getFolderHash(Path path) const;

    FolderRegular browse(HashMD5 root, bool recursive = true);
    FolderSong browseSong(HashMD5 root);
    FolderSong search(HashMD5 root, std::string key);

};
