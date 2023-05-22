#include "common/pch.h"
#include "ruleset_bms_auto.h"
#include "game/scene/scene.h"
#include "game/chart/chart_types.h"
#include "game/data/data_types.h"

namespace lunaticvibes
{

static const int KEY_PRESS_DURATION_MS = 83;

RulesetBMSAuto::RulesetBMSAuto(
    std::shared_ptr<ChartFormatBase> format,
    std::shared_ptr<ChartObjectBase> chart,
    PlayModifierGaugeType gauge,
    GameModeKeys keys,
    JudgeDifficulty difficulty,
    double health,
    PlaySide side) : RulesetBase(format, chart), RulesetBMS(format, chart, gauge, keys, difficulty, health, side)
{
    assert(side == PlaySide::AUTO || side == PlaySide::AUTO_DOUBLE || side == PlaySide::AUTO_2P || side == PlaySide::RIVAL);

    showJudge = (_side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE || _side == PlaySide::AUTO_2P);

    isPressingLN.fill(false);

    switch (side)
    {
    case RulesetBMS::PlaySide::AUTO:
    case RulesetBMS::PlaySide::AUTO_DOUBLE:
        _judgeScratch = !(PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;

    case RulesetBMS::PlaySide::AUTO_2P:
    case RulesetBMS::PlaySide::RIVAL:
        _judgeScratch = !(PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;
    }

    setTargetRate(1.0);
}

void RulesetBMSAuto::setTargetRate(double rate)
{
    targetRate = rate;
    unsigned count = getNoteCount();
    unsigned score = (unsigned)std::round(2 * count * rate);
    if (!_judgeScratch)
    {
        auto pChart = std::dynamic_pointer_cast<ChartObjectBMS>(_chart);
        if (pChart != nullptr)
        {
            count -= pChart->getScratchCount();
            if (score > count * 2) score = count * 2;
        }
    }

    if (rate == 1.0)
    {
        noteJudges.resize(count);
        std::fill(noteJudges.begin(), noteJudges.end(), JudgeArea::EXACT_PERFECT);
        totalJudgeCount[JudgeArea::EXACT_PERFECT] = count;
        totalJudgeCount[JudgeArea::EARLY_GREAT] = 0;
        totalJudgeCount[JudgeArea::EARLY_GOOD] = 0;
        return;
    }

    unsigned count0 = 0, count1 = 0, count2 = 0;
    if (rate >= 0.5)
    {
        count1 = 2 * count - score;
        count2 = score - count;
        count0 = count - count1 - count2;
    }
    else
    {
        count2 = 0;
        count1 = score;
        count0 = count - count1;
    }
    totalJudgeCount[JudgeArea::EXACT_PERFECT] = count2;
    totalJudgeCount[JudgeArea::EARLY_GREAT] = count1;
    totalJudgeCount[JudgeArea::EARLY_GOOD] = count0;

    double interval0 = count0 ? (double)count / count0 : 0.;
    double interval1 = count1 ? (double)count / count1 : 0.;
    double interval2 = count2 ? (double)count / count2 : 0.;
    double c0 = 0.;
    double c1 = 0.;
    double c2 = 0.;
    noteJudges.clear();
    noteJudges.reserve(count);
    while (noteJudges.size() < count)
    {
        if (count2 > 0 && noteJudges.size() >= c2 - 0.000001)
        {
            noteJudges.push_back(JudgeArea::EXACT_PERFECT);
            count2--;
            c2 += interval2;
        }
        if (count1 > 0 && noteJudges.size() >= c1 - 0.000001)
        {
            noteJudges.push_back(JudgeArea::EARLY_GREAT);
            count1--;
            c1 += interval1;
        }
        if (count0 > 0 && noteJudges.size() >= c0 - 0.000001)
        {
            noteJudges.push_back(JudgeArea::EARLY_GOOD);
            count0--;
            c0 += interval0;
        }
    }
    while (count0--)
    {
        noteJudges.push_back(JudgeArea::EARLY_GOOD);
    }
    while (count1--)
    {
        noteJudges.push_back(JudgeArea::EARLY_GREAT);
    }
    while (count2--)
    {
        noteJudges.push_back(JudgeArea::EXACT_PERFECT);
    }

    assert(noteJudges.size() == count);
}

void RulesetBMSAuto::update(const Time& t)
{
    if (!_hasStartTime)
        setStartTime(t);

    auto rt = t - _startTime.norm();
    using namespace chart;

    auto updateSection = [&](Input::Pad begin, Input::Pad end, int side)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            bool scratch = false;
            switch (k)
            {
            case Input::S1L:
            case Input::S1R:
            case Input::S2L:
            case Input::S2R:
                scratch = true;
                break;
            }

            JudgeRes j;
            NoteLaneIndex idx;

            idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Note, idx, itNote) && !itNote->expired && rt >= itNote->time)
                {
                    itNote->hit = true;
                    itNote->expired = true;

                    if (!scratch || _judgeScratch)
                    {
                        updateJudge(t, idx, noteJudges[judgeIndex++], side);

                        if (_side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE || _side == PlaySide::AUTO_2P)
                        {
                            SystemData.timers[InputGamePressMap.at(Input::Pad(k))] = t.norm();
                            SystemData.timers[InputGameReleaseMap.at(Input::Pad(k))] = TIMER_NEVER;

                            if (k == Input::S1L || k == Input::S1R)
                            {
                                SystemData.timers["key_on_s_1p"] = t.norm();
                                SystemData.timers["key_off_s_1p"] = TIMER_NEVER;
                            }
                            if (k == Input::S2L || k == Input::S2R)
                            {
                                SystemData.timers["key_on_s_2p"] = t.norm();
                                SystemData.timers["key_off_s_2p"] = TIMER_NEVER;
                            }
                        }
                    }

                    notesExpired++;
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::LN, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx);
                while (!_chart->isLastNote(NoteLaneCategory::LN, idx, itNote) && !itNote->expired)
                {
                    if (!(itNote->flags & Note::LN_TAIL))
                    {
                        const Time& hitTime = judgeTime[(size_t)_judgeDifficulty].BAD;
                        if (!itNote->expired && rt >= itNote->time)
                        {
                            itNote->hit = true;
                            itNote->expired = true;

                            if (!scratch || _judgeScratch)
                            {
                                if (_side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE || _side == PlaySide::AUTO_2P)
                                {
                                    SystemData.timers[InputGamePressMap.at(Input::Pad(k))] = t.norm();
                                    SystemData.timers[InputGameReleaseMap.at(Input::Pad(k))] = TIMER_NEVER;

                                    if (k == Input::S1L || k == Input::S1R)
                                    {
                                        SystemData.timers["key_on_s_1p"] = t.norm();
                                        SystemData.timers["key_off_s_1p"] = TIMER_NEVER;
                                    }
                                    if (k == Input::S2L || k == Input::S2R)
                                    {
                                        SystemData.timers["key_on_s_2p"] = t.norm();
                                        SystemData.timers["key_off_s_2p"] = TIMER_NEVER;
                                    }

                                    if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                                        PlayData.timers[_bombLNTimerMap->at(idx)] = t.norm();

                                    isPressingLN[k] = true;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (rt >= itNote->time)
                        {
                            itNote->hit = true;
                            itNote->expired = true;

                            if (!scratch || _judgeScratch)
                            {
                                updateJudge(t, idx, noteJudges[judgeIndex++], side);

                                if (_side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE || _side == PlaySide::AUTO_2P)
                                {
                                    SystemData.timers[InputGamePressMap.at(Input::Pad(k))] = TIMER_NEVER;
                                    SystemData.timers[InputGameReleaseMap.at(Input::Pad(k))] = t.norm();

                                    if (k == Input::S1L || k == Input::S1R)
                                    {
                                        SystemData.timers["key_on_s_1p"] = TIMER_NEVER;
                                        SystemData.timers["key_off_s_1p"] = t.norm();
                                    }
                                    if (k == Input::S2L || k == Input::S2R)
                                    {
                                        SystemData.timers["key_on_s_2p"] = TIMER_NEVER;
                                        SystemData.timers["key_off_s_2p"] = t.norm();
                                    }

                                    if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                                        PlayData.timers[_bombLNTimerMap->at(idx)] = TIMER_NEVER;

                                    isPressingLN[k] = false;
                                }
                            }
                            notesExpired++;
                        }
                    }
                    ++itNote;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Invs, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Invs, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Invs, idx, itNote) && !itNote->expired && rt >= itNote->time)
                {
                    itNote->hit = true;
                    itNote->expired = true;
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Mine, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Mine, idx, itNote) && !itNote->expired && rt >= itNote->time)
                {
                    itNote->hit = true;
                    itNote->expired = true;
                    itNote++;
                }
            }

            if (!scratch || _judgeScratch)
            {
                if (_side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE || _side == PlaySide::AUTO_2P)
                {
                    if (t.norm() - SystemData.timers[InputGamePressMap.at(Input::Pad(k))] > KEY_PRESS_DURATION_MS && !isPressingLN[k])
                    {
                        SystemData.timers[InputGamePressMap.at(Input::Pad(k))] = TIMER_NEVER;
                        SystemData.timers[InputGameReleaseMap.at(Input::Pad(k))] = t.norm();

                        if (k == Input::S1L || k == Input::S1R)
                        {
                            SystemData.timers["key_on_s_1p"] = TIMER_NEVER;
                            SystemData.timers["key_off_s_1p"] = t.norm();
                        }
                        if (k == Input::S2L || k == Input::S2R)
                        {
                            SystemData.timers["key_on_s_2p"] = TIMER_NEVER;
                            SystemData.timers["key_off_s_2p"] = t.norm();
                        }
                    }
                }
            }
        }
    };
    if (_side != PlaySide::AUTO_2P)
        updateSection(Input::S1L, Input::K19, showJudge ? PLAYER_SLOT_PLAYER : -1);
    if (_side != PlaySide::AUTO)
        updateSection(Input::S2L, Input::K29, showJudge ? PLAYER_SLOT_TARGET : -1);

    unsigned max = _chart->getNoteTotalCount() * 2;
    _basic.total_acc = 100.0 * exScore / max;
    _basic.acc = notesExpired ? (100.0 * exScore / (notesExpired * 2)) : 0;
}

void RulesetBMSAuto::fail()
{
    _isFailed = true;

    notesExpired = notesReached = _chart->getNoteTotalCount();
    _basic.combo = getMaxCombo();

    _basic.judge[JUDGE_PERFECT] = totalJudgeCount[JudgeArea::EXACT_PERFECT];
    _basic.judge[JUDGE_GREAT] = totalJudgeCount[JudgeArea::EARLY_GREAT];
    _basic.judge[JUDGE_GOOD] = totalJudgeCount[JudgeArea::EARLY_GOOD];
    exScore = _basic.judge[JUDGE_PERFECT] * 2 + _basic.judge[JUDGE_GREAT];

    _basic.total_acc = notesExpired ? (100.0 * exScore / (notesExpired * 2)) : 0;
    _basic.acc = _basic.total_acc;
}

}
