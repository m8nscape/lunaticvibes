#pragma once
#include <string>
#include <vector>
#include <memory>
#include "common/types.h"
#include "db_conn.h"

class vScore;
class ScoreBMS;

/* TABLE classic_chart:
    md5(TEXT), totalnotes(INTEGER), score(INTEGER), rate(REAL), reserved[1-4](INTEGER), reserved[5-6](REAL)
    exscore(INTEGER), lamp(INTEGER), pgreat(INTEGER), great(INTEGER), good(INTEGER), bad(INTEGER), bpoor(INTEGER), miss(INTEGER), option1(INTEGER), option2(INTEGER), option2(INTEGER),
    md5: hash string, calculated by "relative path to exe" OR "absolute path"
    ...
*/
class ScoreDB : public SQLite
{

public:
    ScoreDB() = delete;
    ScoreDB(const char* path);
    ~ScoreDB() = default;
    ScoreDB(ScoreDB&) = delete;
    ScoreDB& operator= (ScoreDB&) = delete;

public:
    std::shared_ptr<ScoreBMS> getChartScoreBMS(const HashMD5& hash) const;
    void updateChartScoreBMS(const HashMD5& hash, const ScoreBMS& score);

};
