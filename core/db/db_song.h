#pragma once
#include <string>
#include <vector>
#include <memory>
#include "types.h"
#include "db_conn.h"

class vChart;
typedef std::shared_ptr<vChart> pChart;

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
    static int addChart(const std::string& path);
    static int removeChart(const HashMD5& md5);

public:
    static std::vector<pChart> findChartByName(const HashMD5& folder, const std::string&);  // search from genre, version, artist, artist2, title, title2
    static std::vector<pChart> findChartByHash(const HashMD5&);  // chart may duplicate

    static int addFolder(const std::string& path);
    static int removeFolder(const std::string& path);

    static Path getFolderPath(const HashMD5& folder);
};
