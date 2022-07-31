#include "ruleset_bms_auto.h"
#include "game/scene/scene.h"
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

    if (_side == PlaySide::AUTO) 
        judgeSide = PLAYER_SLOT_1P;
    else if (_side == PlaySide::AUTO_2P) 
        judgeSide = PLAYER_SLOT_2P;

    isPressingLN.fill(false);

    setTargetRate(1.0);
}

void RulesetBMSAuto::setTargetRate(double rate)
{
    targetRate = rate;
    unsigned count = _chart->getNoteCount();
    unsigned score = (unsigned)std::round(2 * count * rate);

    if (rate == 1.0)
    {
        noteJudges.resize(count);
        std::fill(noteJudges.begin(), noteJudges.end(), JudgeType::PERFECT);
        totalJudgeCount[JudgeType::PERFECT] = count;
        totalJudgeCount[JudgeType::GREAT] = 0;
        totalJudgeCount[JudgeType::GOOD] = 0;
        return;
    }

    unsigned count0 = 0, count1 = 0, count2 = 0;
    count1 = 2 * count - score;
    count2 = score - count;
    count0 = count - count1 - count2;
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
            judgeRes j;
            NoteLaneIndex idx;

            idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Note, idx, itNote) && !itNote->hit && rt >= itNote->time)
                {
                    updateHit(t, idx, noteJudges[judgeIndex++], judgeSide);
                    itNote->hit = true;
                    _basic.totaln++;

                    if (_side == PlaySide::AUTO || _side == PlaySide::AUTO_2P)
                    {
                        gTimers.set(InputGamePressMapSingle[k].tm, t.norm());
                        gTimers.set(InputGameReleaseMapSingle[k].tm, TIMER_NEVER);
                        gSwitches.set(InputGamePressMapSingle[k].sw, true);

                        if (k == Input::S1L || k == Input::S1R)
                        {
                            gTimers.set(eTimer::S1_DOWN, t.norm());
                            gTimers.set(eTimer::S1_UP, TIMER_NEVER);
                            gSwitches.set(eSwitch::S1_DOWN, true);
                        }
                        if (k == Input::S2L || k == Input::S2R)
                        {
                            gTimers.set(eTimer::S2_DOWN, t.norm());
                            gTimers.set(eTimer::S2_UP, TIMER_NEVER);
                            gSwitches.set(eSwitch::S2_DOWN, true);
                        }
                    }
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::LN, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx);
                while (!_chart->isLastNote(NoteLaneCategory::LN, idx, itNote) && !itNote->hit)
                {
                    if (!(itNote->flags & Note::LN_TAIL))
                    {
                        const Time& hitTime = judgeTime[(size_t)_diff].BAD;
                        if (!itNote->hit && rt >= itNote->time)
                        {
                            itNote->hit = true;

                            if (_side == PlaySide::AUTO || _side == PlaySide::AUTO_2P)
                            {
                                gTimers.set(InputGamePressMapSingle[k].tm, t.norm());
                                gTimers.set(InputGameReleaseMapSingle[k].tm, TIMER_NEVER);
                                gSwitches.set(InputGamePressMapSingle[k].sw, true);

                                if (k == Input::S1L || k == Input::S1R)
                                {
                                    gTimers.set(eTimer::S1_DOWN, t.norm());
                                    gTimers.set(eTimer::S1_UP, TIMER_NEVER);
                                    gSwitches.set(eSwitch::S1_DOWN, true);
                                }
                                if (k == Input::S2L || k == Input::S2R)
                                {
                                    gTimers.set(eTimer::S2_DOWN, t.norm());
                                    gTimers.set(eTimer::S2_UP, TIMER_NEVER);
                                    gSwitches.set(eSwitch::S2_DOWN, true);
                                }

                                if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                                    gTimers.set(_bombLNTimerMap->at(idx), t.norm());

                                isPressingLN[k] = true;
                            }
                        }
                    }
                    else
                    {
                        if (rt >= itNote->time)
                        {
                            updateHit(t, idx, noteJudges[judgeIndex++], judgeSide);
                            itNote->hit = true;
                            _basic.totaln++;

                            if (_side == PlaySide::AUTO || _side == PlaySide::AUTO_2P)
                            {
                                gTimers.set(InputGamePressMapSingle[k].tm, TIMER_NEVER);
                                gTimers.set(InputGameReleaseMapSingle[k].tm, t.norm());
                                gSwitches.set(InputGameReleaseMapSingle[k].sw, false);

                                if (k == Input::S1L || k == Input::S1R)
                                {
                                    gTimers.set(eTimer::S1_DOWN, TIMER_NEVER);
                                    gTimers.set(eTimer::S1_UP, t.norm());
                                    gSwitches.set(eSwitch::S1_DOWN, false);
                                }
                                if (k == Input::S2L || k == Input::S2R)
                                {
                                    gTimers.set(eTimer::S2_DOWN, TIMER_NEVER);
                                    gTimers.set(eTimer::S2_UP, t.norm());
                                    gSwitches.set(eSwitch::S2_DOWN, false);
                                }

                                if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                                    gTimers.set(_bombLNTimerMap->at(idx), TIMER_NEVER);

                                isPressingLN[k] = false;
                            }
                        }
                    }
                    ++itNote;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Invs, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Invs, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Invs, idx, itNote) && !itNote->hit && rt >= itNote->time)
                {
                    itNote->hit = true;
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Mine, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Mine, idx, itNote) && !itNote->hit && rt >= itNote->time)
                {
                    itNote->hit = true;
                    itNote++;
                }
            }

            if (_side == PlaySide::AUTO || _side == PlaySide::AUTO_2P)
            {
                if (t.norm() - gTimers.get(InputGamePressMapSingle[k].tm) > 83 && !isPressingLN[k])
                {
                    gTimers.set(InputGamePressMapSingle[k].tm, TIMER_NEVER);
                    gTimers.set(InputGameReleaseMapSingle[k].tm, t.norm());
                    gSwitches.set(InputGameReleaseMapSingle[k].sw, false);

                    if (k == Input::S1L || k == Input::S1R)
                    {
                        gTimers.set(eTimer::S1_DOWN, TIMER_NEVER);
                        gTimers.set(eTimer::S1_UP, t.norm());
                        gSwitches.set(eSwitch::S1_DOWN, false);
                    }
                    if (k == Input::S2L || k == Input::S2R)
                    {
                        gTimers.set(eTimer::S2_DOWN, TIMER_NEVER);
                        gTimers.set(eTimer::S2_UP, t.norm());
                        gSwitches.set(eSwitch::S2_DOWN, false);
                    }
                }
            }
        }
    };
    updateSection(Input::S1L, Input::K1SPDDN);
    updateSection(Input::S2L, Input::K2SPDDN);

    unsigned max = _chart->getNoteCount() * 2;
    _basic.total_acc = 100.0 * _basic.score2 / max;
    _basic.acc = _basic.totaln ? (100.0 * _basic.score2 / _basic.totaln) : 0;
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