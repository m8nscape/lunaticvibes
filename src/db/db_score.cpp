#include "db_score.h"
#include "common/types.h"
#include "common/log.h"

const char* CREATE_SCORE_BMS_TABLE_STR =
"CREATE TABLE IF NOT EXISTS score_bms( "
"md5 TEXT PRIMARY KEY UNIQUE NOT NULL, " // 0
"notes INTEGER NOT NULL, "               // 1
"score INTEGER NOT NULL, "               // 2
"rate REAL NOT NULL, "                   // 3
"fast INTEGER NOT NULL, "                // 4
"slow INTEGER NOT NULL, "                // 5
"maxcombo INTEGER NOT NULL DEFAULT 0, "  // 6
"addtime INTEGER NOT NULL DEFAULT 0, "   // 7
"pc INTEGER NOT NULL DEFAULT 0, "        // 8
"exscore INTEGER NOT NULL, "             // 9
"lamp INTEGER NOT NULL, "                // 10
"pgreat INTEGER NOT NULL, "              // 11
"great INTEGER NOT NULL, "               // 12
"good INTEGER NOT NULL, "                // 13
"bad INTEGER NOT NULL, "                 // 14
"bpoor INTEGER NOT NULL, "               // 15
"miss INTEGER NOT NULL, "                // 16
"bp INTEGER NOT NULL, "                  // 17
"cb INTEGER NOT NULL "                   // 18
")";
struct score_bms_all_params
{
    std::string md5;
    long long notes   = 0;
    long long score   = 0;
    double    rate    = 0;
    long long fast = 0;
    long long slow = 0;
    long long maxcombo = 0;
    long long addtime = 0;
    long long pc      = 0;
    long long exscore = 0;
    long long lamp    = 0;
    long long pgreat  = 0;
    long long great   = 0;
    long long good    = 0;
    long long bad     = 0;
    long long bpoor   = 0;
    long long miss    = 0;
    long long bp      = 0;
    long long cb      = 0;

    score_bms_all_params(const std::vector<std::any>& queryResult)
    {
        try
        {
            md5     = ANY_STR(queryResult.at(0));
            notes   = ANY_INT(queryResult.at(1));
            score   = ANY_INT(queryResult.at(2));
            rate    = ANY_REAL(queryResult.at(3));
            fast    = ANY_INT(queryResult.at(4));
            slow    = ANY_INT(queryResult.at(5));
            maxcombo = ANY_INT(queryResult.at(6));
            addtime = ANY_INT(queryResult.at(7));
            pc      = ANY_INT(queryResult.at(8));
            exscore = ANY_INT(queryResult.at(9));
            lamp    = ANY_INT(queryResult.at(10));
            pgreat  = ANY_INT(queryResult.at(11));
            great   = ANY_INT(queryResult.at(12));
            good    = ANY_INT(queryResult.at(13));
            bad     = ANY_INT(queryResult.at(14));
            bpoor   = ANY_INT(queryResult.at(15));
            miss    = ANY_INT(queryResult.at(16));
            bp      = ANY_INT(queryResult.at(17));
            cb      = ANY_INT(queryResult.at(18));
        }
        catch (std::out_of_range&)
        {
        }
    }
};
bool convert_score_bms(std::shared_ptr<ScoreBMS> out, const std::vector<std::any>& in)
{
    if (in.size() < 19) return false;

    score_bms_all_params params(in);

    out->notes      = params.notes  ;
    out->score      = params.score  ;
    out->rate       = params.rate   ;
    out->fast       = params.fast;
    out->slow       = params.slow;
    out->maxcombo   = params.maxcombo;
    out->addtime    = params.addtime;
    out->playcount  = params.pc;
    out->exscore    = params.exscore;
    out->lamp       = (ScoreBMS::Lamp)params.lamp   ;
    out->pgreat     = params.pgreat ;
    out->great      = params.great  ;
    out->good       = params.good   ;
    out->bad        = params.bad    ;
    out->bpoor      = params.bpoor  ;
    out->miss       = params.miss   ;
    out->bp         = params.bp     ;
    out->combobreak = params.cb;
    return true;
}


ScoreDB::ScoreDB(const char* path): SQLite(path, "SCORE")
{
    if (exec(CREATE_SCORE_BMS_TABLE_STR) != SQLITE_OK)
    {
        LOG_ERROR << "[ScoreDB] Create table score_bms ERROR! " << errmsg();
        abort();
    }
}

std::shared_ptr<ScoreBMS> ScoreDB::getChartScoreBMS(const HashMD5& hash) const
{
    auto result = query("SELECT * FROM score_bms WHERE md5=?", 19, { hash.hexdigest() });

    if (!result.empty())
    {
        const auto& r = result[0];
        auto ret = std::make_shared<ScoreBMS>();
        convert_score_bms(ret, r);
        return ret;
    }
    return nullptr;
}

void ScoreDB::updateChartScoreBMS(const HashMD5& hash, const ScoreBMS& score)
{
    auto pRecord = getChartScoreBMS(hash);
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
            record.fast = score.fast;
            record.slow = score.slow;
            record.exscore = score.exscore;
            record.pgreat = score.pgreat;
            record.great = score.great;
            record.good = score.good;
            record.bad = score.bad;
            record.bpoor = score.bpoor;
            record.miss = score.miss;
            record.combobreak = score.combobreak;
        }

        if (score.maxcombo > record.maxcombo)
        {
            record.maxcombo = score.maxcombo;
        }

        if (score.bp < record.bp)
        {
            record.bp = score.bp;
        }

        if (score.playcount > record.playcount)
        {
            record.playcount = score.playcount;
        }

        if ((int)score.lamp > (int)record.lamp)
        {
            record.lamp = score.lamp;
        }

        if (score.bp < record.bp)
        {
            record.bp = score.bp;
        }

        exec("UPDATE score_bms SET notes=?,score=?,rate=?,fast=?,slow=?,maxcombo=?,addtime=?,pc=?,exscore=?,lamp=?,"
            "pgreat=?,great=?,good=?,bad=?,bpoor=?,miss=?,bp=?,cb=? WHERE md5=?",
            { record.notes, record.score, record.rate, record.fast, record.slow, 
            record.maxcombo, (long long)std::time(nullptr), record.playcount, record.exscore, record.exscore, (int)record.lamp, 
            record.pgreat, record.great, record.good, record.bad, record.bpoor, record.miss, record.bp, record.combobreak,
            hash });
    }
    else
    {
        exec("INSERT INTO score_bms(md5,notes,score,rate,fast,slow,maxcombo,addtime,pc,exscore,lamp,"
            "pgreat,great,good,bad,bpoor,miss,bp,cb) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)",
            { hash, 
            score.notes, score.score, score.rate, score.fast, score.slow,
            score.maxcombo, (long long)std::time(nullptr), score.playcount, score.exscore, (int)score.lamp, 
            score.pgreat, score.great, score.good, score.bad, score.bpoor, score.miss, score.bp, score.combobreak });
    }
}
