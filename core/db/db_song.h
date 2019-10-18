#pragma once
#include <string>
#include <vector>
#include <memory>
#include "types.h"
#include "db_conn.h"

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
    };

public:
    SongDB() = delete;
    SongDB(const char* path);
    ~SongDB() = default;
    SongDB(SongDB&) = delete;
    SongDB& operator= (SongDB&) = delete;

protected:
    int addChart(const std::string& path);
    int removeChart(const HashMD5& md5);
    
    HashMD5 searchFolderParentFromPath(const Path& path) const;
    HashMD5 searchFolderHash(const Path& path) const;
    

public:
    std::vector<pChart> findChartByName(const HashMD5& folder, const std::string&, unsigned limit = 1000) const;  // search from genre, version, artist, artist2, title, title2
    std::vector<pChart> findChartByHash(const HashMD5&) const;  // chart may duplicate

    int addFolder(const std::string& path);
    int addFolderContent(const Path& folder);
    int removeFolder(const HashMD5& hash);

    HashMD5 getFolderParent(const HashMD5& folder) const;
    Path getFolderPathFromHash(const HashMD5& folder) const;

};
