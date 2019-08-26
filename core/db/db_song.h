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
private:
    static SongDB _inst;
public:
    enum FolderType
    {
        FOLDER,
        SONG_BMS,
    };

private:
    SongDB();
    ~SongDB() = default;
    SongDB(SongDB&) = delete;
    SongDB& operator= (SongDB&) = delete;

protected:
    int addChart(const std::string& path);
    int removeChart(const HashMD5& md5);
    
    HashMD5 searchFolderParent(const Path& path);
    HashMD5 searchFolderHash(const Path& path);
    

public:
    static std::vector<pChart> findChartByName(const HashMD5& folder, const std::string&);  // search from genre, version, artist, artist2, title, title2
    static std::vector<pChart> findChartByHash(const HashMD5&);  // chart may duplicate

    static int addFolder(const std::string& path);
    static int removeFolder(const HashMD5& hash);

    static HashMD5 getFolderParent(const HashMD5& folder);
    static Path getFolderPath(const HashMD5& folder);

};
