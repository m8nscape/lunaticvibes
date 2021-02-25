#include "db_score.h"
#include "score.h"
#include <plog/Log.h>

const char* CREATE_CHART_CLASSIC_TABLE_STR =
"CREATE TABLE IF NOT EXISTS chart_classic(  \
md5     TEXT     PRIMARY KEY UNIQUE NOT NULL,   \
notes   INTEGER                 NOT NULL,   \
score   INTEGER                 NOT NULL,   \
rate    REAL                    NOT NULL,   \
r1      INTEGER                 NOT NULL DEFAULT 0,   \
r2      INTEGER                 NOT NULL DEFAULT 0,   \
r3      INTEGER                 NOT NULL DEFAULT 0,   \
r4      INTEGER                 NOT NULL DEFAULT 0,   \
r5      REAL                    NOT NULL DEFAULT 0,   \
r6      REAL                    NOT NULL DEFAULT 0,   \
exscore INTEGER                 NOT NULL,   \
lamp    INTEGER                 NOT NULL,   \
pgreat  INTEGER                 NOT NULL,   \
great   INTEGER                 NOT NULL,   \
good    INTEGER                 NOT NULL,   \
bad     INTEGER                 NOT NULL,   \
bpoor   INTEGER                 NOT NULL,   \
miss    INTEGER                 NOT NULL,   \
bp      INTEGER                 NOT NULL    \
)";


ScoreDB::ScoreDB(const char* path): SQLite(path, "SCORE")
{
    if (exec(CREATE_CHART_CLASSIC_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[ScoreDB] Create table chart_classic ERROR! " << errmsg();
        abort();
    }
}

std::shared_ptr<ScoreClassic> ScoreDB::getChartScoreClassic(const HashMD5& hash) const
{
    auto result = query("SELECT * FROM chart_classic WHERE md5=?", 19, { hash });

    if (!result.empty())
    {
        const auto& r = result[0];
        auto ret = std::make_shared<ScoreClassic>();
        ret->notes = ANY_INT(r[1]);
        ret->score = ANY_INT(r[2]);
        ret->rate = ANY_REAL(r[3]);
        ret->exscore = ANY_INT(r[10]);
        ret->lamp = (ScoreClassic::Lamp)ANY_INT(r[11]);
        ret->pgreat = ANY_INT(r[12]);
        ret->great = ANY_INT(r[13]);
        ret->good = ANY_INT(r[14]);
        ret->bad = ANY_INT(r[15]);
        ret->bpoor = ANY_INT(r[16]);
        ret->miss = ANY_INT(r[17]);
        ret->bp = ANY_INT(r[18]);
        return ret;
    }
    return nullptr;
}

void ScoreDB::updateChartScoreClassic(const HashMD5& hash, const ScoreClassic& score)
{
    auto pRecord = getChartScoreClassic(hash);
    if (pRecord)
    {
        auto record = *pRecord;

        if (score.notes != record.notes)
        {
            record.notes = score.notes;
        }

        if (score.score > record.score)
        {
            record.score = score.score;
        }

        if (score.exscore > record.exscore)
        {
            record.rate = score.rate;
            record.exscore = score.exscore;
            record.pgreat = score.pgreat;
            record.great = score.great;
            record.good = score.good;
            record.bad = score.bad;
            record.bpoor = score.bpoor;
            record.miss = score.miss;
        }

        if ((int)score.lamp > (int)record.lamp)
        {
            record.lamp = score.lamp;
        }

        if (score.bp < record.bp)
        {
            record.bp = score.bp;
        }

        exec("UPDATE chart_classic SET notes=?,score=?,rate=?,exscore=?,lamp=?,pgreat=?,great=?,good=?,bad=?,bpoor=?,miss=?,bp=? WHERE md5=?",
            { record.notes, record.score, record.rate, record.exscore, (int)record.lamp, record.pgreat, record.great, record.good, record.bad, record.bpoor, record.miss, record.bp, hash });
    }
    else
    {
        exec("INSERT INTO chart_classic(md5,notes,score,rate,exscore,lamp,pgreat,great,good,bad,bpoor,miss,bp) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)",
            { hash, score.notes, score.score, score.rate, score.exscore, (int)score.lamp, score.pgreat, score.great, score.good, score.bad, score.bpoor, score.miss, score.bp });
    }
}
