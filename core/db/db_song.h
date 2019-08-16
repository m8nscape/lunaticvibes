#pragma once
#include <string>
#include <vector>
#include <memory>
#include "db_conn.h"
#include "chart/chart.h"

typedef std::shared_ptr<vChart> pChart;

class SongDB
{
private:
    SQLite conn;

protected:
    int addChart(const std::string& path);
    int removeChart(const std::string& path);

public:
    std::vector<pChart> findChartByName(const std::string&) const;  // search from genre, version, artist, artist2, title, title2
    std::vector<pChart> findChartByHash(const std::string&) const;  // chart may duplicate

    int addFolder(const std::string& path);
    int removeFolder(const std::string& path);

};
