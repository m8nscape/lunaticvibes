#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "common/types.h"
#include "db_conn.h"

class vScore;
class ScoreBMS;

/* TABLE classic_chart:
    md5(TEXT), totalnotes(INTEGER), score(INTEGER), rate(REAL), reserved[1-4](INTEGER), reserved[5-6](REAL)
    exscore(INTEGER), lamp(INTEGER), pgreat(INTEGER), great(INTEGER), good(INTEGER), bad(INTEGER), kpoor(INTEGER), miss(INTEGER)
    md5: hash string, calculated by "relative path to exe" OR "absolute path"
    ...
*/
class ScoreDB : public SQLite
{
protected:
    mutable std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<ScoreBMS>>> cache;

public:
    ScoreDB() = delete;
    ScoreDB(const char* path);
    ~ScoreDB() = default;
    ScoreDB(ScoreDB&) = delete;
    ScoreDB& operator= (ScoreDB&) = delete;

protected:
    std::shared_ptr<ScoreBMS> getScoreBMS(const char* tableName, const HashMD5& hash) const;
    void updateScoreBMS(const char* tableName, const HashMD5& hash, const ScoreBMS& score);

public:
    std::shared_ptr<ScoreBMS> getChartScoreBMS(const HashMD5& hash) const;
    void updateChartScoreBMS(const HashMD5& hash, const ScoreBMS& score);

    std::shared_ptr<ScoreBMS> getCourseScoreBMS(const HashMD5& hash) const;
    void updateCourseScoreBMS(const HashMD5& hash, const ScoreBMS& score);

    void preloadScore();
};
