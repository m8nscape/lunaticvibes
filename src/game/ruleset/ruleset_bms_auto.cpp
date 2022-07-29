#include "ruleset_bms_auto.h"
#include "game/scene/scene_context.h"

RulesetBMSAuto::RulesetBMSAuto(
    std::shared_ptr<vChartFormat> format,
    std::shared_ptr<chart::vChart> chart,
    eModGauge gauge,
    GameModeKeys keys,
    JudgeDifficulty difficulty,
    double health,
    PlaySide side) : RulesetBMS(format, chart, gauge, keys, difficulty, health, side)
{
    assert(side == PlaySide::AUTO || side == PlaySide::AUTO_2P || side == PlaySide::RIVAL);

}

void RulesetBMSAuto::setTargetRate(double rate)
{
    targetRate = rate;
    unsigned count = _chart->getNoteCount();
    unsigned score = (unsigned)std::round(count * rate);

    if (rate == 1.0)
    {
        noteJudges.resize(count);
        std::fill(noteJudges.begin(), noteJudges.end(), JudgeType::PERFECT);
        totalJudgeCount[JudgeType::PERFECT] = count;
        totalJudgeCount[JudgeType::GREAT] = 0;
        totalJudgeCount[JudgeType::GOOD] = 0;
        return;
    }

    // x + y + z = s
    // 2x + y = 2rs
    //  =>
    // x = (2r-1)s + z
    unsigned count0 = 0, count1 = 0, count2 = 0;
    count0 = unsigned((rate >= 0.8) ? 0 : (0.8 - rate) / 0.8 * count);
    count2 = count * (2 * rate - 1.0) + count0;
    count1 = count - count0 - count2;
    totalJudgeCount[JudgeType::PERFECT] = count0;
    totalJudgeCount[JudgeType::GREAT] = count1;
    totalJudgeCount[JudgeType::GOOD] = count2;

    unsigned interval0 = count0 ? count / count0 : 0;
    unsigned interval1 = count1 ? count / count1 : 0;
    unsigned interval2 = count2 ? count / count2 : 0;
    unsigned c = 0;
    noteJudges.clear();
    noteJudges.reserve(count);
    while (noteJudges.size() < count)
    {
        ++c;
        if (count2 > 0 && c % interval2 == 0)
        {
            noteJudges.push_back(JudgeType::PERFECT);
            count2--;
        }
        if (count1 > 0 && c % interval1 == 0)
        {
            noteJudges.push_back(JudgeType::GREAT);
            count1--;
        }
        if (count0 > 0 && c % interval0 == 0)
        {
            noteJudges.push_back(JudgeType::GOOD);
            count0--;
        }
    }
    while (count0--)
    {
        noteJudges.push_back(JudgeType::GOOD);
    }
    while (count1--)
    {
        noteJudges.push_back(JudgeType::GREAT);
    }
    while (count2--)
    {
        noteJudges.push_back(JudgeType::PERFECT);
    }

    assert(noteJudges.size() == count);
}

void RulesetBMSAuto::update(const Time& t)
{
    auto rt = t - gTimers.get(eTimer::PLAY_START);
    using namespace chart;

    auto updateSection = [&](Input::Pad begin, Input::Pad end)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            auto [cat, idx] = _chart->getLaneFromKey((Input::Pad)k);
            if (cat == NoteLaneCategory::_) return;

            auto n = _chart->incomingNote(cat, idx);
            if (_judge(*n, rt).area >= judgeArea::EXACT_PERFECT)
            {
                updateHit(t, idx, noteJudges[judgeIndex++], -1);
                n->hit = true;
                _basic.totalnr++;
            }
        }
    };
    updateSection(Input::S1L, Input::K1SPDDN);
    updateSection(Input::S2L, Input::K2SPDDN);

    unsigned max = _chart->getNoteCount() * 2;
    _basic.total_acc = 100.0 * _basic.score2 / max;
    _basic.acc = _basic.totaln ? (100.0 * _basic.score2 / _basic.totaln / 2) : 0;
    _basic.score = int(std::round(inner_score));

    updateGlobals();
}

void RulesetBMSAuto::fail()
{
    _isFailed = true;

    _basic.totaln = _basic.totalnr = _chart->getNoteCount();
    _basic.combo = getMaxCombo();

    _count[JudgeType::PERFECT] = totalJudgeCount[JudgeType::PERFECT];
    _count[JudgeType::GREAT] = totalJudgeCount[JudgeType::GREAT];
    _count[JudgeType::GOOD] = totalJudgeCount[JudgeType::GOOD];

    unsigned exscore = _count[JudgeType::PERFECT] * 2 + _count[JudgeType::GREAT];
    _basic.total_acc = _basic.totaln ? (100.0 * exscore / _basic.totaln / 2) : 0;
    _basic.acc = _basic.total_acc;
}

void RulesetBMSAuto::reset()
{
    vRuleset::reset();

    _count.clear();

    judgeIndex = 0;

    updateGlobals();
}