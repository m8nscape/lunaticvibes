#include "ruleset_bms.h"
#include "common/log.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/runtime/state.h"
#include "game/scene/scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/chart/chart_types.h"
#include "config/config_mgr.h"

using namespace chart;

void setJudgeInternalTimer1P(RulesetBMS::JudgeType judge, long long t)
{
	State::set(IndexTimer::_JUDGE_1P_0, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_1P_1, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_1P_2, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_1P_3, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_1P_4, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_1P_5, TIMER_NEVER);
	switch (judge)
	{
	case RulesetBMS::JudgeType::BPOOR:   State::set(IndexTimer::_JUDGE_1P_0, t); break;
	case RulesetBMS::JudgeType::MISS:    State::set(IndexTimer::_JUDGE_1P_1, t); break;
	case RulesetBMS::JudgeType::BAD:     State::set(IndexTimer::_JUDGE_1P_2, t); break;
	case RulesetBMS::JudgeType::GOOD:    State::set(IndexTimer::_JUDGE_1P_3, t); break;
	case RulesetBMS::JudgeType::GREAT:   State::set(IndexTimer::_JUDGE_1P_4, t); break;
	case RulesetBMS::JudgeType::PERFECT: State::set(IndexTimer::_JUDGE_1P_5, t); break;
	default: break;
	}
}

void setJudgeInternalTimer2P(RulesetBMS::JudgeType judge, long long t)
{
	State::set(IndexTimer::_JUDGE_2P_0, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_2P_1, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_2P_2, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_2P_3, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_2P_4, TIMER_NEVER);
	State::set(IndexTimer::_JUDGE_2P_5, TIMER_NEVER);
	switch (judge)
	{
    case RulesetBMS::JudgeType::BPOOR:   State::set(IndexTimer::_JUDGE_2P_0, t); break;
    case RulesetBMS::JudgeType::MISS:    State::set(IndexTimer::_JUDGE_2P_1, t); break;
    case RulesetBMS::JudgeType::BAD:     State::set(IndexTimer::_JUDGE_2P_2, t); break;
    case RulesetBMS::JudgeType::GOOD:    State::set(IndexTimer::_JUDGE_2P_3, t); break;
    case RulesetBMS::JudgeType::GREAT:   State::set(IndexTimer::_JUDGE_2P_4, t); break;
    case RulesetBMS::JudgeType::PERFECT: State::set(IndexTimer::_JUDGE_2P_5, t); break;
	default: break;
	}
}

RulesetBMS::RulesetBMS(std::shared_ptr<ChartFormatBase> format, std::shared_ptr<ChartObjectBase> chart,
    eModGauge gauge, GameModeKeys keys, RulesetBMS::JudgeDifficulty difficulty, double health, RulesetBMS::PlaySide side) :
    vRuleset(format, chart), _judgeDifficulty(difficulty)
{
    switch (gauge)
    {
        case eModGauge::HARD        : _gauge = GaugeType::HARD;    break;
        case eModGauge::DEATH       : _gauge = GaugeType::DEATH;   break;
        case eModGauge::EASY        : _gauge = GaugeType::EASY;    break;
        //case eModGauge::PATTACK     : _gauge = GaugeType::P_ATK;   break;
        //case eModGauge::GATTACK     : _gauge = GaugeType::G_ATK;   break;
        case eModGauge::ASSISTEASY  : _gauge = GaugeType::ASSIST;  break;
        case eModGauge::GRADE_NORMAL: _gauge = GaugeType::GRADE;   break;
        case eModGauge::GRADE_DEATH : _gauge = GaugeType::EXGRADE; break;
        case eModGauge::EXHARD      : _gauge = GaugeType::EXHARD;  break;
        case eModGauge::GRADE_EX    : _gauge = GaugeType::EXGRADE; break;
        case eModGauge::NORMAL:
        default: 
            _gauge = GaugeType::GROOVE;  break;
    }

    switch (keys)
    {
    case 5:
    case 10: _bombTimerMap = &bombTimer5k;  _bombLNTimerMap = &bombTimer5kLN; break;
    case 7:  
    case 14: _bombTimerMap = &bombTimer7k;  _bombLNTimerMap = &bombTimer7kLN; break;
    case 9:  _bombTimerMap = &bombTimer9k;  _bombLNTimerMap = &bombTimer9kLN; break;
    default: break;
    }

    using namespace std::string_literals;

    _basic.health = health;

    int total = 160;
    switch (format->type())
    {
    case eChartFormat::BMS:
        double total_bms;
        format->getExtendedProperty("TOTAL", (void*)&total_bms);
        total = int(total_bms);
        if (total < 0) total = 160;
        break;

    case eChartFormat::BMSON:
    default:
        break;
    }

    switch (_gauge)
    {
    case GaugeType::HARD:
        _basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT]   = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD]    = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD]     = -0.06;
        _healthGain[JudgeType::MISS]    = -0.1;
        _healthGain[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::EXHARD:
        _basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT]   = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD]    = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD]     = -0.12;
        _healthGain[JudgeType::MISS]    = -0.2;
        _healthGain[JudgeType::BPOOR]   = -0.1;
        break;

    case GaugeType::DEATH:
        _basic.health               = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT]   = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::GOOD]    = 0.0;
        _healthGain[JudgeType::BAD]     = -1.0;
        _healthGain[JudgeType::MISS]    = -1.0;
        _healthGain[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::P_ATK:
        _basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT]   = -0.02;
        _healthGain[JudgeType::GOOD]    = -1.0;
        _healthGain[JudgeType::BAD]     = -1.0;
        _healthGain[JudgeType::MISS]    = -1.0;
        _healthGain[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::G_ATK:
        _basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = -0.02;
        _healthGain[JudgeType::GREAT]   = -0.02;
        _healthGain[JudgeType::GOOD]    = 0.0;
        _healthGain[JudgeType::BAD]     = -1.0;
        _healthGain[JudgeType::MISS]    = -1.0;
        _healthGain[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::GROOVE:
        _basic.health             = 0.2;
        _minHealth                  = 0.02;
        _clearHealth                = 0.8;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / chart->getNoteTotalCount();
        _healthGain[JudgeType::GREAT]   = 0.01 * total / chart->getNoteTotalCount();
        _healthGain[JudgeType::GOOD]    = 0.01 * total / chart->getNoteTotalCount() / 2;
        _healthGain[JudgeType::BAD]     = -0.04;
        _healthGain[JudgeType::MISS]    = -0.06;
        _healthGain[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::EASY:
        _basic.health             = 0.2;
        _minHealth                  = 0.02;
        _clearHealth                = 0.8;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / chart->getNoteTotalCount() * 1.2;
        _healthGain[JudgeType::GREAT]   = 0.01 * total / chart->getNoteTotalCount() * 1.2;
        _healthGain[JudgeType::GOOD]    = 0.01 * total / chart->getNoteTotalCount() / 2 * 1.2;
        _healthGain[JudgeType::BAD]     = -0.032;
        _healthGain[JudgeType::MISS]    = -0.048;
        _healthGain[JudgeType::BPOOR]   = -0.016;
        break;

    case GaugeType::ASSIST:
        _basic.health             = 0.2;
        _minHealth                  = 0.02;
        _clearHealth                = 0.6;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / chart->getNoteTotalCount() * 1.2;
        _healthGain[JudgeType::GREAT]   = 0.01 * total / chart->getNoteTotalCount() * 1.2;
        _healthGain[JudgeType::GOOD]    = 0.01 * total / chart->getNoteTotalCount() / 2 * 1.2;
        _healthGain[JudgeType::BAD]     = -0.032;
        _healthGain[JudgeType::MISS]    = -0.048;
        _healthGain[JudgeType::BPOOR]   = -0.016;
        break;

    case GaugeType::GRADE:
        _basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT]   = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD]    = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD]     = -0.02;
        _healthGain[JudgeType::MISS]    = -0.03;
        _healthGain[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::EXGRADE:
        _basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT]   = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD]    = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD]     = -0.12;
        _healthGain[JudgeType::MISS]    = -0.2;     // FIXME not as hard as exhard
        _healthGain[JudgeType::BPOOR]   = -0.1;
        break;

    default:
        break;
    }

    _side = side;
	switch (side)
	{
	case RulesetBMS::PlaySide::SINGLE:
        _k1P = true;
        _k2P = false;
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;
	case RulesetBMS::PlaySide::DOUBLE:
		_k1P = true;
		_k2P = true;
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
		break;
	case RulesetBMS::PlaySide::BATTLE_1P:
		_k1P = true;
		_k2P = false;
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
		break;
	case RulesetBMS::PlaySide::BATTLE_2P:
		_k1P = false;
		_k2P = true;
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
		break;
    default:
        _k1P = true;
        _k2P = true;
        _judgeScratch = false;
        break;
	}

    _lnJudge.fill(judgeArea::NOTHING);

    for (size_t k = Input::S1L; k <= Input::K2SPDDN; ++k)
    {
        NoteLaneIndex idx;
        idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
        if (idx != NoteLaneIndex::_) _noteListIterators[{NoteLaneCategory::Note, idx}] = _chart->firstNote(NoteLaneCategory::Note, idx);
        idx = _chart->getLaneFromKey(NoteLaneCategory::LN, (Input::Pad)k);
        if (idx != NoteLaneIndex::_) _noteListIterators[{NoteLaneCategory::LN,   idx}] = _chart->firstNote(NoteLaneCategory::LN, idx);
        idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, (Input::Pad)k);
        if (idx != NoteLaneIndex::_) _noteListIterators[{NoteLaneCategory::Mine, idx}] = _chart->firstNote(NoteLaneCategory::Mine, idx);
        idx = _chart->getLaneFromKey(NoteLaneCategory::Invs, (Input::Pad)k);
        if (idx != NoteLaneIndex::_) _noteListIterators[{NoteLaneCategory::Invs, idx}] = _chart->firstNote(NoteLaneCategory::Invs, idx);
    }
}

RulesetBMS::JudgeRes RulesetBMS::_judge(const Note& note, Time time)
{
    // spot judge area
    judgeArea a = judgeArea::NOTHING;
	Time error = time - note.time;
    if (error > -judgeTime[(size_t)_judgeDifficulty].BPOOR)
    {
        if (error < -judgeTime[(size_t)_judgeDifficulty].BAD)
            a = judgeArea::EARLY_BPOOR;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].GOOD)
            a = judgeArea::EARLY_BAD;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].GREAT)
            a = judgeArea::EARLY_GOOD;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].PERFECT)
            a = judgeArea::EARLY_GREAT;
        else if (error < 0)
            a = judgeArea::EARLY_PERFECT;
        else if (error == 0)
            a = judgeArea::EXACT_PERFECT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].PERFECT)
            a = judgeArea::LATE_PERFECT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].GREAT)
            a = judgeArea::LATE_GREAT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].GOOD)
            a = judgeArea::LATE_GOOD;
        else if (error < judgeTime[(size_t)_judgeDifficulty].BAD)
            a = judgeArea::LATE_BAD;
    }

    // log
    /*
    switch (a)
    {
    case judgeArea::EARLY_BPOOR:   LOG_DEBUG << "EARLY  BPOOR   " << error; break;
    case judgeArea::EARLY_BAD:     LOG_DEBUG << "EARLY  BAD     " << error; break;
    case judgeArea::EARLY_GOOD:    LOG_DEBUG << "EARLY  GOOD    " << error; break;
    case judgeArea::EARLY_GREAT:   LOG_DEBUG << "EARLY  GREAT   " << error; break;
    case judgeArea::EARLY_PERFECT: LOG_DEBUG << "EARLY  PERFECT " << error; break;
    case judgeArea::LATE_PERFECT:  LOG_DEBUG << "LATE   PERFECT " << error; break;
    case judgeArea::LATE_GREAT:    LOG_DEBUG << "LATE   GREAT   " << error; break;
    case judgeArea::LATE_GOOD:     LOG_DEBUG << "LATE   GOOD    " << error; break;
    case judgeArea::LATE_BAD:      LOG_DEBUG << "LATE   BAD     " << error; break;
    }
    */

    return { a, error };
}

static const std::map<RulesetBMS::judgeArea, ReplayChart::Commands::Type> judgeAreaReplayCommandType[] =
{
    {
        {RulesetBMS::judgeArea::EXACT_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_EXACT_0},
        {RulesetBMS::judgeArea::EARLY_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_0},
        {RulesetBMS::judgeArea::EARLY_GREAT, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_1},
        {RulesetBMS::judgeArea::EARLY_GOOD, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_2},
        {RulesetBMS::judgeArea::EARLY_BAD, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_3},
        {RulesetBMS::judgeArea::EARLY_BPOOR, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_5},
        {RulesetBMS::judgeArea::LATE_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_0},
        {RulesetBMS::judgeArea::LATE_GREAT, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_1},
        {RulesetBMS::judgeArea::LATE_GOOD, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_2},
        {RulesetBMS::judgeArea::LATE_BAD, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_3},
        {RulesetBMS::judgeArea::MISS, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4},
    },
    {
        {RulesetBMS::judgeArea::EXACT_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_EXACT_0},
        {RulesetBMS::judgeArea::EARLY_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_0},
        {RulesetBMS::judgeArea::EARLY_GREAT, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_1},
        {RulesetBMS::judgeArea::EARLY_GOOD, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_2},
        {RulesetBMS::judgeArea::EARLY_BAD, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_3},
        {RulesetBMS::judgeArea::EARLY_BPOOR, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_5},
        {RulesetBMS::judgeArea::LATE_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_0},
        {RulesetBMS::judgeArea::LATE_GREAT, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_1},
        {RulesetBMS::judgeArea::LATE_GOOD, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_2},
        {RulesetBMS::judgeArea::LATE_BAD, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_3},
        {RulesetBMS::judgeArea::MISS, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4},
    }
};

void RulesetBMS::_judgePress(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
    if (cat == NoteLaneCategory::LN && 
        (note.flags & Note::LN_TAIL) &&
        (idx == NoteLaneIndex::Sc1 || idx == NoteLaneIndex::Sc2) && 
        _lnJudge[idx] != judgeArea::NOTHING)
    {
        // Handle scratch direction change as miss
        _judgeRelease(cat, idx, note, judge, t, slot);
        if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
            State::set(_bombLNTimerMap->at(idx), TIMER_NEVER);
    }

    bool pushReplayCommand = false;
    switch (cat)
    {
    case NoteLaneCategory::Note:
        switch (judge.area)
        {
        case judgeArea::EARLY_PERFECT:
        case judgeArea::EXACT_PERFECT:
        case judgeArea::LATE_PERFECT:
            updateHit(t, idx, RulesetBMS::JudgeType::PERFECT, slot);
            pushReplayCommand = true;
            break;

        case judgeArea::EARLY_GREAT:
            _basic.fast++;
            updateHit(t, idx, RulesetBMS::JudgeType::GREAT, slot);
            pushReplayCommand = true;
            break;
        case judgeArea::LATE_GREAT:
            _basic.slow++;
            updateHit(t, idx, RulesetBMS::JudgeType::GREAT, slot);
            pushReplayCommand = true;
            break;

        case judgeArea::EARLY_GOOD:
            _basic.fast++;
            updateHit(t, idx, RulesetBMS::JudgeType::GOOD, slot);
            pushReplayCommand = true;
            break;
        case judgeArea::LATE_GOOD:
            _basic.slow++;
            updateHit(t, idx, RulesetBMS::JudgeType::GOOD, slot);
            pushReplayCommand = true;
            break;

        case judgeArea::EARLY_BAD:
            _basic.fast++;
            updateMiss(t, idx, RulesetBMS::JudgeType::BAD, slot);
            pushReplayCommand = true;
            break;
        case judgeArea::LATE_BAD:
            _basic.slow++;
            updateMiss(t, idx, RulesetBMS::JudgeType::BAD, slot);
            pushReplayCommand = true;
            break;

        case judgeArea::EARLY_BPOOR:
            _basic.fast++;
            updateMiss(t, idx, RulesetBMS::JudgeType::BPOOR, slot);
            pushReplayCommand = true;
            break;
        }
        if (judge.area > judgeArea::EARLY_BPOOR)
        {
            note.hit = true;
            _basic.notesExpired++;
        }
        break;

    case NoteLaneCategory::Invs:
        break;

    case NoteLaneCategory::LN:
        if (!(note.flags & Note::LN_TAIL))
        {
            switch (judge.area)
            {
            case judgeArea::EARLY_PERFECT:
            case judgeArea::EXACT_PERFECT:
            case judgeArea::LATE_PERFECT:
            case judgeArea::EARLY_GREAT:
            case judgeArea::LATE_GREAT:
            case judgeArea::EARLY_GOOD:
            case judgeArea::LATE_GOOD:
                _lnJudge[idx] = judge.area;
                note.hit = true;
                if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                    State::set(_bombLNTimerMap->at(idx), t.norm());
                break;

            case judgeArea::EARLY_BAD:
                _basic.fast++;
                _lnJudge[idx] = judge.area;
                updateMiss(t, idx, RulesetBMS::JudgeType::BAD, slot);
                note.hit = true;
                pushReplayCommand = true;
                break;
            case judgeArea::LATE_BAD:
                _basic.slow++;
                _lnJudge[idx] = judge.area;
                updateMiss(t, idx, RulesetBMS::JudgeType::BAD, slot);
                note.hit = true;
                pushReplayCommand = true;
                break;

            case judgeArea::EARLY_BPOOR:
                _basic.fast++;
                updateMiss(t, idx, RulesetBMS::JudgeType::BPOOR, slot);
                pushReplayCommand = true;
                break;
            }
            break;
        }
        break;
    }

    if (note.hit || judge.area == judgeArea::EARLY_BPOOR)
    {
        _lastNoteJudge = judge;
    }

    // push replay command
    if (pushReplayCommand && doJudge && gChartContext.started && gPlayContext.replayNew)
    {
        if (judgeAreaReplayCommandType[slot].find(judge.area) != judgeAreaReplayCommandType[slot].end())
        {
            long long ms = t.norm() - _startTime.norm();
            ReplayChart::Commands cmd;
            cmd.ms = ms;
            cmd.type = judgeAreaReplayCommandType[slot].at(judge.area);
            gPlayContext.replayNew->commands.push_back(cmd);
        }
    }
}
void RulesetBMS::_judgeHold(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
    bool pushReplayCommand = false;
    switch (cat)
    {
    case NoteLaneCategory::Mine:
    {
        if (judge.area == judgeArea::EXACT_PERFECT ||
            judge.area == judgeArea::EARLY_PERFECT && judge.time < -2 ||
            judge.area == judgeArea::LATE_PERFECT && judge.time < 2)
        {
            note.hit = true;
            _updateHp(-0.01 * note.dvalue / 2);

            // bpoor + 1
            _judgeCount[JudgeType::BPOOR]++;
            _basic.miss++;
            if (showJudge)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    State::set(IndexTimer::PLAY_JUDGE_1P, t.norm());
                    setJudgeInternalTimer1P(JudgeType::BPOOR, t.norm());
                    SoundMgr::playSysSample(SoundChannelType::KEY_LEFT, eSoundSample::SOUND_LANDMINE);
                }
                else if (slot == PLAYER_SLOT_TARGET)
                {
                    State::set(IndexTimer::PLAY_JUDGE_2P, t.norm());
                    setJudgeInternalTimer2P(JudgeType::BPOOR, t.norm());
                    SoundMgr::playSysSample(SoundChannelType::KEY_RIGHT, eSoundSample::SOUND_LANDMINE);
                }
            }

            _lastNoteJudge = { judgeArea::EARLY_BPOOR, t.norm() };

            // push replay command
            if (doJudge && gChartContext.started && gPlayContext.replayNew)
            {
                long long ms = t.norm() - _startTime.norm();
                ReplayChart::Commands cmd;
                cmd.ms = ms;
                cmd.type = slot == PLAYER_SLOT_PLAYER ? ReplayChart::Commands::Type::JUDGE_LEFT_LANDMINE : ReplayChart::Commands::Type::JUDGE_RIGHT_LANDMINE;
                gPlayContext.replayNew->commands.push_back(cmd);
            }
        }
        break;
    }
    case NoteLaneCategory::LN:
        if ((note.flags & Note::LN_TAIL) &&
            _lnJudge[idx] != RulesetBMS::judgeArea::NOTHING &&
            _lnJudge[idx] != RulesetBMS::judgeArea::EARLY_BAD &&
            _lnJudge[idx] != RulesetBMS::judgeArea::LATE_BAD)
        {
            if (judge.area == judgeArea::EXACT_PERFECT ||
                judge.area == judgeArea::EARLY_PERFECT && judge.time < -2 ||
                judge.area == judgeArea::LATE_PERFECT && judge.time < 2)
            {
                switch (_lnJudge[idx])
                {
                case judgeArea::EARLY_GOOD:
                case judgeArea::EARLY_GREAT:
                    _basic.fast++;
                    break;
                case judgeArea::LATE_GOOD:
                case judgeArea::LATE_GREAT:
                    _basic.slow++;
                    break;
                }
                updateHit(t, idx, JudgeType::PERFECT, slot);
                note.hit = true;
                _basic.notesExpired++;
                _lnJudge[idx] = RulesetBMS::judgeArea::NOTHING;
                pushReplayCommand = true;

                if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                    State::set(_bombLNTimerMap->at(idx), TIMER_NEVER);

                _lastNoteJudge = judge;
            }
        }
        break;

    default:
        break;
    }

    // push replay command
    if (pushReplayCommand && doJudge && gChartContext.started && gPlayContext.replayNew)
    {
        if (judgeAreaReplayCommandType[slot].find(judge.area) != judgeAreaReplayCommandType[slot].end())
        {
            long long ms = t.norm() - _startTime.norm();
            ReplayChart::Commands cmd;
            cmd.ms = ms;
            cmd.type = judgeAreaReplayCommandType[slot].at(judge.area);
            gPlayContext.replayNew->commands.push_back(cmd);
        }
    }
}
void RulesetBMS::_judgeRelease(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
    bool pushReplayCommand = false;
    switch (cat)
    {
    case NoteLaneCategory::LN:
        if ((note.flags & Note::LN_TAIL) &&
            _lnJudge[idx] != RulesetBMS::judgeArea::NOTHING &&
            _lnJudge[idx] != RulesetBMS::judgeArea::EARLY_BAD &&
            _lnJudge[idx] != RulesetBMS::judgeArea::LATE_BAD)
        {
            switch (judge.area)
            {
            case judgeArea::EARLY_PERFECT:
            case judgeArea::EXACT_PERFECT:
                switch (_lnJudge[idx])
                {
                case judgeArea::EARLY_GOOD:
                case judgeArea::EARLY_GREAT:
                    _basic.fast++;
                    break;
                case judgeArea::LATE_GOOD:
                case judgeArea::LATE_GREAT:
                    _basic.slow++;
                    break;
                }
                if (_lnJudge[idx] == judgeArea::EARLY_GOOD || _lnJudge[idx] == judgeArea::LATE_GOOD)
                    updateHit(t, idx, JudgeType::GOOD, slot);
                else if (_lnJudge[idx] == judgeArea::EARLY_GREAT || _lnJudge[idx] == judgeArea::LATE_GREAT)
                    updateHit(t, idx, JudgeType::GREAT, slot);
                else
                    updateHit(t, idx, JudgeType::PERFECT, slot);
                note.hit = true;
                _basic.notesExpired++;
                _lnJudge[idx] = RulesetBMS::judgeArea::NOTHING;
                pushReplayCommand = true;
                break;

            case judgeArea::EARLY_GREAT:
                _basic.fast++;
                if (_lnJudge[idx] == judgeArea::EARLY_GOOD || _lnJudge[idx] == judgeArea::LATE_GOOD)
                    updateHit(t, idx, JudgeType::GOOD, slot);
                else
                    updateHit(t, idx, JudgeType::GREAT, slot);
                note.hit = true;
                _basic.notesExpired++;
                _lnJudge[idx] = RulesetBMS::judgeArea::NOTHING;
                pushReplayCommand = true;
                break;

            case judgeArea::EARLY_GOOD:
                _basic.fast++;
                updateHit(t, idx, JudgeType::GOOD, slot);
                note.hit = true;
                _basic.notesExpired++;
                _lnJudge[idx] = RulesetBMS::judgeArea::NOTHING;
                pushReplayCommand = true;
                break;

            case judgeArea::EARLY_BAD:
            default:
                _basic.fast++;
                updateMiss(t, idx, JudgeType::BAD, slot);
                note.hit = true;
                _basic.notesExpired++;
                _lnJudge[idx] = RulesetBMS::judgeArea::NOTHING;
                pushReplayCommand = true;
                break;
            }

            if (note.hit)
            {
                _lastNoteJudge = judge;
            }

            break;
        }
        break;

    default:
        break;
    }

    // push replay command
    if (pushReplayCommand && doJudge && gChartContext.started && gPlayContext.replayNew)
    {
        if (judgeAreaReplayCommandType[slot].find(judge.area) != judgeAreaReplayCommandType[slot].end())
        {
            long long ms = t.norm() - _startTime.norm();
            ReplayChart::Commands cmd;
            cmd.ms = ms;
            cmd.type = judgeAreaReplayCommandType[slot].at(judge.area);
            gPlayContext.replayNew->commands.push_back(cmd);
        }
    }
}

void RulesetBMS::_updateHp(const double diff)
{
    double tmp = _basic.health;
    switch (_gauge)
    {
    case RulesetBMS::GaugeType::HARD:
    case RulesetBMS::GaugeType::GRADE:
        if (tmp < 0.3000001 && diff < 0.0)
            tmp += diff * 0.6;
        else
            tmp += diff;
        break;
    default:
        tmp += diff;
        break;
    }
    _basic.health = std::max(_minHealth, std::min(1.0, tmp));

    if (failWhenNoHealth() && _basic.health <= _minHealth)
    {
        fail();
    }
}
void RulesetBMS::_updateHp(JudgeType judge)
{
    _updateHp(_healthGain.at(judge));
}

void RulesetBMS::updateHit(const Time& t, NoteLaneIndex ch, RulesetBMS::JudgeType judge, int slot, bool force)
{
    if (isFailed()) return;
    
    if (doJudge || force)
    {
        ++_judgeCount[judge];
        ++_basic.hit;
        ++_basic.combo;
        switch (judge)
        {
        case JudgeType::PERFECT:
            moneyScore += 1.0 * 150000 / _chart->getNoteTotalCount() +
                1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteTotalCount() - 55);
            _basic.score2 += 2;
            break;
        case JudgeType::GREAT:
            moneyScore += 1.0 * 100000 / _chart->getNoteTotalCount() +
                1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteTotalCount() - 55);
            _basic.score2 += 1;
            break;
        case JudgeType::GOOD:
            moneyScore += 1.0 * 20000 / _chart->getNoteTotalCount() +
                1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteTotalCount() - 55);
            break;
        default:
            break;
        }

        _updateHp(judge);
        if (_basic.combo > _basic.maxCombo)
            _basic.maxCombo = _basic.combo;
    }

    if (showJudge)
    {
        bool setBombTimer = false;
        switch (judge)
        {
        case JudgeType::PERFECT:
        case JudgeType::GREAT:
            setBombTimer = true;
            break;
        default:
            break;
        }

        if (_bombTimerMap == nullptr || _bombTimerMap->find(ch) == _bombTimerMap->end())
            setBombTimer = false;

        if (slot == PLAYER_SLOT_PLAYER)
        {
            State::set(IndexTimer::PLAY_JUDGE_1P, t.norm());
            setJudgeInternalTimer1P(judge, t.norm());
            State::set(IndexNumber::_DISP_NOWCOMBO_1P, _basic.combo);
            if (setBombTimer)
                State::set(_bombTimerMap->at(ch), t.norm());

            Option::e_judge_type judgeType = Option::JUDGE_NONE;
            switch (judge)
            {
            case JudgeType::PERFECT: judgeType = Option::JUDGE_0; break;
            case JudgeType::GREAT:   judgeType = Option::JUDGE_1; break;
            case JudgeType::GOOD:    judgeType = Option::JUDGE_2; break;
            case JudgeType::BAD:     judgeType = Option::JUDGE_3; break;
            case JudgeType::MISS:    judgeType = Option::JUDGE_4; break;
            case JudgeType::BPOOR:   judgeType = Option::JUDGE_5; break;
            }
            State::set(IndexOption::PLAY_LAST_JUDGE_1P, judgeType);
        }
        else if (slot == PLAYER_SLOT_TARGET)
        {
            State::set(IndexTimer::PLAY_JUDGE_2P, t.norm());
            setJudgeInternalTimer2P(judge, t.norm());
            State::set(IndexNumber::_DISP_NOWCOMBO_2P, _basic.combo);
            if (setBombTimer) State::set(_bombTimerMap->at(ch), t.norm());

            Option::e_judge_type judgeType = Option::JUDGE_NONE;
            switch (judge)
            {
            case JudgeType::PERFECT: judgeType = Option::JUDGE_0; break;
            case JudgeType::GREAT:   judgeType = Option::JUDGE_1; break;
            case JudgeType::GOOD:    judgeType = Option::JUDGE_2; break;
            case JudgeType::BAD:     judgeType = Option::JUDGE_3; break;
            case JudgeType::MISS:    judgeType = Option::JUDGE_4; break;
            case JudgeType::BPOOR:   judgeType = Option::JUDGE_5; break;
            }
            State::set(IndexOption::PLAY_LAST_JUDGE_2P, judgeType);
        }
    }
}

void RulesetBMS::updateMiss(const Time& t, NoteLaneIndex ch, RulesetBMS::JudgeType judge, int slot, bool force)
{
    if (isFailed()) return;

    if (doJudge || force)
    {
        ++_judgeCount[judge];
        ++_basic.miss;
        if (judge != JudgeType::BPOOR)
        {
            if (_basic.combo == 0) ++_judgeCount[JudgeType::COMBOBREAK];
            _basic.combo = 0;
        }

        _updateHp(judge);
    }

    if (showJudge)
    {
        if (slot == PLAYER_SLOT_PLAYER)
        {
            State::set(IndexTimer::PLAY_JUDGE_1P, t.norm());
            setJudgeInternalTimer1P(judge, t.norm());
            State::set(IndexNumber::_DISP_NOWCOMBO_1P, _basic.combo);

            State::set(IndexOption::PLAY_LAST_JUDGE_1P, (judge != JudgeType::BPOOR ? Option::JUDGE_4 : Option::JUDGE_5));
        }
        else
        {
            State::set(IndexTimer::PLAY_JUDGE_2P, t.norm());
            setJudgeInternalTimer2P(judge, t.norm());
            State::set(IndexNumber::_DISP_NOWCOMBO_2P, _basic.combo);

            State::set(IndexOption::PLAY_LAST_JUDGE_2P, (judge != JudgeType::BPOOR ? Option::JUDGE_4 : Option::JUDGE_5));
        }
    }
}

void RulesetBMS::judgeNotePress(Input::Pad k, const Time& t, const Time& rt, int slot)
{
    NoteLaneIndex idx1 = _chart->getLaneFromKey(NoteLaneCategory::Note, k);
    HitableNote* pNote1 = nullptr;
    if (idx1 != _ && !_chart->isLastNote(NoteLaneCategory::Note, idx1))
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx1);
        while (!_chart->isLastNote(NoteLaneCategory::Note, idx1, itNote) && itNote->hit)
            ++itNote;
        if (!_chart->isLastNote(NoteLaneCategory::Note, idx1, itNote))
            pNote1 = &*itNote;
    }
    NoteLaneIndex idx2 = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    HitableNote* pNote2 = nullptr;
    if (idx2 != _ && !_chart->isLastNote(NoteLaneCategory::LN, idx2))
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx2);
        while (!_chart->isLastNote(NoteLaneCategory::LN, idx2, itNote) && itNote->hit)
            ++itNote;
        if (!_chart->isLastNote(NoteLaneCategory::LN, idx2, itNote))
            pNote2 = &*itNote;
    }

    JudgeRes j;
    if (pNote1 && (pNote2 == nullptr || pNote1->time < pNote2->time) && !pNote1->hit)
    {
        j = _judge(*pNote1, rt);
        _judgePress(NoteLaneCategory::Note, idx1, *pNote1, j, t, slot);
    }
    else if (pNote2 && !pNote2->hit)
    {
        j = _judge(*pNote2, rt);
        _judgePress(NoteLaneCategory::LN, idx2, *pNote2, j, t, slot);
    }

    // break-out BAD chain 
    if (j.area == judgeArea::LATE_BAD)
    {
         judgeNotePress(k, t, rt, slot);
    }
}
void RulesetBMS::judgeNoteHold(Input::Pad k, const Time& t, const Time& rt, int slot)
{
    NoteLaneIndex idx; 

    idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, k);
    if (idx != _ && !_chart->isLastNote(NoteLaneCategory::Mine, idx))
    {
        auto& note = *_chart->incomingNote(NoteLaneCategory::Mine, idx);
        auto j = _judge(note, rt);
        _judgeHold(NoteLaneCategory::Mine, idx, note, j, t, slot);
    }

    idx = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    if (idx != _ && !_chart->isLastNote(NoteLaneCategory::LN, idx))
    {
        auto& note = *_chart->incomingNote(NoteLaneCategory::LN, idx);
        auto j = _judge(note, rt);
        _judgeHold(NoteLaneCategory::LN, idx, note, j, t, slot);
    }
}
void RulesetBMS::judgeNoteRelease(Input::Pad k, const Time& t, const Time& rt, int slot)
{
    NoteLaneIndex idx = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    if (idx != _)
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx);
        while (!_chart->isLastNote(NoteLaneCategory::LN, idx, itNote))
        {
            if (!itNote->hit)
            {
                auto j = _judge(*itNote, rt);
                _judgeRelease(NoteLaneCategory::LN, idx, *itNote, j, t, slot);
                break;
            }
            ++itNote;
        }
    }

    if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
        State::set(_bombLNTimerMap->at(idx), TIMER_NEVER);
}

void RulesetBMS::updatePress(InputMask& pg, const Time& t)
{
	Time rt = t - _startTime.norm();
    if (rt.norm() < 0) return;
    if (gPlayContext.isAuto) return;
    auto updatePressRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!pg[k]) continue;
            judgeNotePress((Input::Pad)k, t, rt, slot);
        }
    };
    if (_k1P) updatePressRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P) updatePressRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P) updatePressRange(Input::S1L, Input::S1R, PLAYER_SLOT_PLAYER);
        if (_k2P) updatePressRange(Input::S2L, Input::S2R, PLAYER_SLOT_TARGET);
    }
}
void RulesetBMS::updateHold(InputMask& hg, const Time& t)
{
	Time rt = t - _startTime.norm();
    if (rt < 0) return;
    if (gPlayContext.isAuto) return;

    auto updateHoldRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!hg[k]) continue;
            judgeNoteHold((Input::Pad)k, t, rt, slot);
        }
    };
    if (_k1P) updateHoldRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P) updateHoldRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P) updateHoldRange(Input::S1L, Input::S1R, PLAYER_SLOT_PLAYER);
        if (_k2P) updateHoldRange(Input::S2L, Input::S2R, PLAYER_SLOT_TARGET);
    }
}
void RulesetBMS::updateRelease(InputMask& rg, const Time& t)
{
	Time rt = t - _startTime.norm();
    if (rt < 0) return;
    if (gPlayContext.isAuto) return;

    auto updateReleaseRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!rg[k]) continue;
            judgeNoteRelease((Input::Pad)k, t, rt, slot);
        }
    };
    if (_k1P) updateReleaseRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P) updateReleaseRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P) updateReleaseRange(Input::S1L, Input::S1R, PLAYER_SLOT_PLAYER);
        if (_k2P) updateReleaseRange(Input::S2L, Input::S2R, PLAYER_SLOT_TARGET);
    }
}
void RulesetBMS::updateAxis(double s1, double s2, const Time& t)
{
    Time rt = t - _startTime.norm();
    if (rt.norm() < 0) return;

    using namespace Input;

    if (!gPlayContext.isAuto && (!gPlayContext.isReplay || !gChartContext.started))
    {
        _scratchAccumulator[PLAYER_SLOT_PLAYER] += s1;
        _scratchAccumulator[PLAYER_SLOT_TARGET] += s2;
    }
}

void RulesetBMS::update(const Time& t)
{
    if (!_hasStartTime)
        setStartTime(t);

	auto rt = t - _startTime.norm();

    for (auto& [c, n]: _noteListIterators)
    {
        auto [cat, idx] = c;
        while (!_chart->isLastNote(cat, idx, n) && rt >= n->time)
        {
            switch (cat)
            {
            case NoteLaneCategory::Note:
                _basic.notesReached++;
                break;

            case NoteLaneCategory::LN:
                if (n->flags & Note::LN_TAIL)
                    _basic.notesReached++;
                break;
            }

            n++;
        }
    }

    auto updateRange = [&](Input::Pad begin, Input::Pad end, int slot)
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

            NoteLaneIndex idx;

            idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Note, idx, itNote) && !itNote->hit)
                {
                    Time hitTime = (!scratch || _judgeScratch) ? judgeTime[(size_t)_judgeDifficulty].BAD : 0;
                    if (rt - itNote->time >= hitTime)
                    {
                        itNote->hit = true;

                        if (!scratch || _judgeScratch)
                        {
                            _basic.slow++;
                            updateMiss(t, idx, RulesetBMS::JudgeType::MISS, slot);
                            _lastNoteJudge.area = judgeArea::MISS;
                            _lastNoteJudge.time = hitTime;

                            // push replay command
                            if (doJudge && gChartContext.started && gPlayContext.replayNew)
                            {
                                long long ms = t.norm() - _startTime.norm();
                                ReplayChart::Commands cmd;
                                cmd.ms = ms;
                                cmd.type = slot == PLAYER_SLOT_PLAYER ? ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4 : ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4;
                                gPlayContext.replayNew->commands.push_back(cmd);
                            }
                        }

                        _basic.notesExpired++;
                        //LOG_DEBUG << "LATE   POOR    "; break;
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
                        if (rt >= itNote->time)
                        {
                            Time hitTime = itNote->time + judgeTime[(size_t)_judgeDifficulty].BAD;
                            auto itTail = itNote;
                            itTail++;
                            if (!_chart->isLastNote(NoteLaneCategory::LN, idx, itTail) && (itTail->flags & Note::LN_TAIL) && hitTime > itTail->time)
                            {
                                hitTime = itTail->time;
                            }
                            if (rt >= hitTime)
                            {
                                itNote->hit = true;

                                if (!scratch || _judgeScratch)
                                {
                                    _basic.slow++;
                                    _lnJudge[idx] = judgeArea::LATE_BAD;
                                    updateMiss(t, idx, RulesetBMS::JudgeType::MISS, slot);

                                    // push replay command
                                    if (doJudge && gChartContext.started && gPlayContext.replayNew)
                                    {
                                        long long ms = t.norm() - _startTime.norm();
                                        ReplayChart::Commands cmd;
                                        cmd.ms = ms;
                                        cmd.type = slot == PLAYER_SLOT_PLAYER ? ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4 : ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4;
                                        gPlayContext.replayNew->commands.push_back(cmd);
                                    }
                                }

                                //LOG_DEBUG << "LATE   POOR    "; break;
                            }
                        }
                    }
                    else
                    {
                        if (rt >= itNote->time)
                        {
                            itNote->hit = true;

                            if (!scratch || _judgeScratch)
                            {
                                //_basic.slow++;
                                if (_lnJudge[idx] == judgeArea::EARLY_BAD || _lnJudge[idx] == judgeArea::LATE_BAD)
                                {
                                    _basic.notesExpired++;
                                    _lnJudge[idx] = judgeArea::NOTHING;
                                }
                            }
                        }
                    }
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Invs, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                const Time& hitTime = -judgeTime[(size_t)_judgeDifficulty].BAD;
                auto itNote = _chart->incomingNote(NoteLaneCategory::Invs, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Invs, idx, itNote) && !itNote->hit && rt - itNote->time >= hitTime)
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
        }
    };
    if (_k1P) updateRange(Input::S1L, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P) updateRange(Input::S2L, Input::K29, PLAYER_SLOT_TARGET);

    if (_judgeScratch)
    {
        auto updateScratch = [&](const Time& t, Input::Pad up, Input::Pad dn, double& val, int slot)
        {
            double scratchThreshold = 0.001;
            double scratchRewind = 0.0001;
            if (val > scratchThreshold)
            {
                // scratch down
                val -= scratchThreshold;

                switch (_scratchDir[slot])
                {
                case AxisDir::AXIS_DOWN:
                    judgeNoteHold(dn, t, rt, slot);
                    break;
                case AxisDir::AXIS_UP:
                    judgeNoteRelease(up, t, rt, slot);
                    judgeNotePress(dn, t, rt, slot);
                    break;
                case AxisDir::AXIS_NONE:
                    judgeNoteRelease(up, t, rt, slot);
                    judgeNotePress(dn, t, rt, slot);
                    break;
                }

                _scratchLastUpdate[slot] = t;
                _scratchDir[slot] = AxisDir::AXIS_DOWN;
            }
            else if (val < -scratchThreshold)
            {
                // scratch up
                val += scratchThreshold;

                switch (_scratchDir[slot])
                {
                case AxisDir::AXIS_UP:
                    judgeNoteHold(up, t, rt, slot);
                    break;
                case AxisDir::AXIS_DOWN:
                    judgeNoteRelease(dn, t, rt, slot);
                    judgeNotePress(up, t, rt, slot);
                    break;
                case AxisDir::AXIS_NONE:
                    judgeNoteRelease(dn, t, rt, slot);
                    judgeNotePress(up, t, rt, slot);
                    break;
                }

                _scratchLastUpdate[slot] = t;
                _scratchDir[slot] = AxisDir::AXIS_UP;
            }

            if (val > scratchRewind)
                val -= scratchRewind;
            else if (val < -scratchRewind)
                val += scratchRewind;
            else
                val = 0.;

            if ((t - _scratchLastUpdate[slot]).norm() > 133)
            {
                // release
                switch (_scratchDir[slot])
                {
                case AxisDir::AXIS_UP:
                    judgeNoteRelease(up, t, rt, slot);
                    break;
                case AxisDir::AXIS_DOWN:
                    judgeNoteRelease(dn, t, rt, slot);
                    break;
                }

                _scratchDir[slot] = AxisDir::AXIS_NONE;
                _scratchLastUpdate[slot] = TIMER_NEVER;
            }
        };
        updateScratch(t, Input::S1L, Input::S1R, _scratchAccumulator[PLAYER_SLOT_PLAYER], PLAYER_SLOT_PLAYER);
        updateScratch(t, Input::S2L, Input::S2R, _scratchAccumulator[PLAYER_SLOT_TARGET], PLAYER_SLOT_TARGET);
    }

	unsigned max = _chart->getNoteTotalCount() * 2;
	_basic.total_acc = 100.0 * _basic.score2 / max;
    _basic.acc = _basic.notesExpired ? (100.0 * _basic.score2 / _basic.notesExpired / 2) : 0;
    _basic.score = int(std::round(moneyScore));

    updateGlobals();
}

unsigned RulesetBMS::getMaxCombo() const
{
    if (_judgeScratch)
    {
        return _chart->getNoteTotalCount();
    }
    else
    {
        unsigned count = _chart->getNoteTotalCount();
        auto pChart = std::dynamic_pointer_cast<ChartObjectBMS>(_chart);
        if (pChart != nullptr)
        {
            count -= pChart->getScratchCount();
        }
        return count;
    }
}


void RulesetBMS::fail()
{
    _isFailed = true;

    _basic.health = _minHealth; 
    _basic.combo = 0;

    int notesRemain = _chart->getNoteTotalCount() - _basic.notesExpired;
    _basic.miss += notesRemain;
    _judgeCount[JudgeType::MISS] += notesRemain;
    _basic.notesExpired = _basic.notesReached = _chart->getNoteTotalCount();

    _basic.acc = _basic.total_acc;
}

void RulesetBMS::reset()
{
    vRuleset::reset();

    _judgeCount.clear();

    updateGlobals();
}

void RulesetBMS::updateGlobals()
{
    if (_side == PlaySide::SINGLE || _side == PlaySide::DOUBLE || _side == PlaySide::BATTLE_1P || _side == PlaySide::AUTO) // includes DP
    {
        State::set(IndexBargraph::PLAY_EXSCORE, _basic.total_acc / 100.0);
        State::set(IndexBargraph::PLAY_EXSCORE_PREDICT, _basic.acc / 100.0);

        State::set(IndexNumber::PLAY_1P_SCORE, _basic.score);
        State::set(IndexNumber::PLAY_1P_NOWCOMBO, _basic.combo);
        State::set(IndexNumber::PLAY_1P_MAXCOMBO, _basic.maxCombo);
        State::set(IndexNumber::PLAY_1P_RATE, int(std::floor(_basic.acc)));
        State::set(IndexNumber::PLAY_1P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        State::set(IndexNumber::PLAY_1P_TOTALNOTES, _chart->getNoteTotalCount());
        State::set(IndexNumber::PLAY_1P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        State::set(IndexNumber::PLAY_1P_TOTAL_RATE_DECIMAL2, int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        State::set(IndexNumber::PLAY_1P_PERFECT, _judgeCount[JudgeType::PERFECT]);
        State::set(IndexNumber::PLAY_1P_GREAT, _judgeCount[JudgeType::GREAT]);
        State::set(IndexNumber::PLAY_1P_GOOD, _judgeCount[JudgeType::GOOD]);
        State::set(IndexNumber::PLAY_1P_BAD, _judgeCount[JudgeType::BAD]);
        State::set(IndexNumber::PLAY_1P_POOR, _judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]);
        State::set(IndexNumber::PLAY_1P_GROOVEGAUGE, int(_basic.health * 100));

        State::set(IndexNumber::PLAY_1P_JUDGE_TIME_ERROR_MS, _lastNoteJudge.time.norm());
        State::set(IndexNumber::PLAY_1P_MISS, _judgeCount[JudgeType::MISS]);
        State::set(IndexNumber::PLAY_1P_FAST_COUNT, _basic.fast);
        State::set(IndexNumber::PLAY_1P_SLOW_COUNT, _basic.slow);
        State::set(IndexNumber::PLAY_1P_COMBOBREAK, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::MISS]);
        State::set(IndexNumber::PLAY_1P_BPOOR, _judgeCount[JudgeType::BPOOR]);
        State::set(IndexNumber::PLAY_1P_BP, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]);
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_JUDGE_TIME_ERROR_MS, _lastNoteJudge.time.norm());
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_FAST_COUNT, _basic.fast);
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_SLOW_COUNT, _basic.slow);
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_COMBOBREAK, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::MISS]);

        int fastslow = 0;   // 1:fast 2:slow
        switch (_lastNoteJudge.area)
        {
        case judgeArea::EARLY_GREAT:
        case judgeArea::EARLY_GOOD:
        case judgeArea::EARLY_BAD:
        case judgeArea::EARLY_BPOOR:
            fastslow = 1;
            break;

        case judgeArea::LATE_GREAT:
        case judgeArea::LATE_GOOD:
        case judgeArea::LATE_BAD:
        case judgeArea::MISS:
            fastslow = 2;
            break;
        }
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_1P_FAST_SLOW, fastslow);
        State::set(IndexOption::PLAY_LAST_JUDGE_FASTSLOW_1P, fastslow);

        State::set(IndexBargraph::RESULT_PG, (double)_judgeCount[JudgeType::PERFECT] / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_GR, (double)_judgeCount[JudgeType::GREAT] / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_GD, (double)_judgeCount[JudgeType::GOOD] / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_BD, (double)_judgeCount[JudgeType::BAD] / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_PR, (double)(_judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]) / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_MAXCOMBO, (double)_basic.maxCombo / getMaxCombo());
        State::set(IndexBargraph::RESULT_SCORE, (double)_basic.score / 200000);
        State::set(IndexBargraph::RESULT_EXSCORE, (double)_basic.score2 / getMaxScore());
        State::set(IndexBargraph::PLAY_1P_FAST_COUNT, (double)_basic.fast / _chart->getNoteTotalCount());
        State::set(IndexBargraph::PLAY_1P_SLOW_COUNT, (double)_basic.slow / _chart->getNoteTotalCount());

        State::set(IndexOption::PLAY_RANK_ESTIMATED_1P, Option::getRankType(_basic.acc));
        State::set(IndexOption::PLAY_RANK_BORDER_1P, Option::getRankType(_basic.total_acc));
        State::set(IndexOption::PLAY_HEALTH_1P, Option::getHealthType(_basic.health));

        int maxScore = getMaxScore();
        //if      (dp.total_acc >= 94.44) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * 1.000 - dp.score2));    // MAX-
        if      (_basic.total_acc >= 100.0 * 8.0 / 9) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, _basic.score2 - maxScore);    // MAX-
        else if (_basic.total_acc >= 100.0 * 7.0 / 9) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 8.0 / 9));    // AAA-
        else if (_basic.total_acc >= 100.0 * 6.0 / 9) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 7.0 / 9));    // AA-
        else if (_basic.total_acc >= 100.0 * 5.0 / 9) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 6.0 / 9));    // A-
        else if (_basic.total_acc >= 100.0 * 4.0 / 9) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 5.0 / 9));    // B-
        else if (_basic.total_acc >= 100.0 * 3.0 / 9) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 4.0 / 9));    // C-
        else if (_basic.total_acc >= 100.0 * 2.0 / 9) State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 3.0 / 9));    // D-
        else                                          State::set(IndexNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 2.0 / 9));    // E-

    }
    else if (_side == PlaySide::BATTLE_2P || _side == PlaySide::AUTO_2P || _side == PlaySide::RIVAL) // excludes DP
    {
        State::set(IndexBargraph::PLAY_RIVAL_EXSCORE, _basic.total_acc / 100.0);

        State::set(IndexNumber::PLAY_2P_SCORE, _basic.score);
        State::set(IndexNumber::PLAY_2P_NOWCOMBO, _basic.combo);
        State::set(IndexNumber::PLAY_2P_MAXCOMBO, _basic.maxCombo);
        State::set(IndexNumber::PLAY_2P_RATE, int(std::floor(_basic.acc)));
        State::set(IndexNumber::PLAY_2P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        State::set(IndexNumber::PLAY_2P_TOTALNOTES, _chart->getNoteTotalCount());
        State::set(IndexNumber::PLAY_2P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        State::set(IndexNumber::PLAY_2P_TOTAL_RATE_DECIMAL2, int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        State::set(IndexNumber::PLAY_2P_PERFECT, _judgeCount[JudgeType::PERFECT]);
        State::set(IndexNumber::PLAY_2P_GREAT, _judgeCount[JudgeType::GREAT]);
        State::set(IndexNumber::PLAY_2P_GOOD, _judgeCount[JudgeType::GOOD]);
        State::set(IndexNumber::PLAY_2P_BAD, _judgeCount[JudgeType::BAD]);
        State::set(IndexNumber::PLAY_2P_POOR, _judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]);
        State::set(IndexNumber::PLAY_2P_GROOVEGAUGE, int(_basic.health * 100));

        State::set(IndexNumber::PLAY_2P_JUDGE_TIME_ERROR_MS, _lastNoteJudge.time.norm());
        State::set(IndexNumber::PLAY_2P_MISS, _judgeCount[JudgeType::MISS]);
        State::set(IndexNumber::PLAY_2P_FAST_COUNT, _basic.fast);
        State::set(IndexNumber::PLAY_2P_SLOW_COUNT, _basic.slow);
        State::set(IndexNumber::PLAY_2P_COMBOBREAK, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::MISS]);
        State::set(IndexNumber::PLAY_2P_BPOOR, _judgeCount[JudgeType::BPOOR]);
        State::set(IndexNumber::PLAY_2P_BP, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]);

        int fastslow = 0;   // 1:fast 2:slow
        switch (_lastNoteJudge.area)
        {
        case judgeArea::EARLY_GREAT:
        case judgeArea::EARLY_GOOD:
        case judgeArea::EARLY_BAD:
        case judgeArea::EARLY_BPOOR:
            fastslow = 1;
            break;

        case judgeArea::LATE_GREAT:
        case judgeArea::LATE_GOOD:
        case judgeArea::LATE_BAD:
        case judgeArea::MISS:
            fastslow = 2;
            break;
        }
        State::set(IndexNumber::LR2IR_REPLACE_PLAY_2P_FAST_SLOW, fastslow);
        State::set(IndexOption::PLAY_LAST_JUDGE_FASTSLOW_2P, fastslow);

        State::set(IndexBargraph::RESULT_RIVAL_PG, (double)_judgeCount[JudgeType::PERFECT] / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_RIVAL_GR, (double)_judgeCount[JudgeType::GREAT] / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_RIVAL_GD, (double)_judgeCount[JudgeType::GOOD] / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_RIVAL_BD, (double)_judgeCount[JudgeType::BAD] / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_RIVAL_PR, (double)(_judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]) / _chart->getNoteTotalCount());
        State::set(IndexBargraph::RESULT_RIVAL_MAXCOMBO, (double)_basic.maxCombo / getMaxCombo());
        State::set(IndexBargraph::RESULT_RIVAL_SCORE, (double)_basic.score / 200000);
        State::set(IndexBargraph::RESULT_RIVAL_EXSCORE, (double)_basic.score2 / getMaxScore());
        State::set(IndexBargraph::PLAY_2P_FAST_COUNT, (double)_basic.fast / _chart->getNoteTotalCount());
        State::set(IndexBargraph::PLAY_2P_SLOW_COUNT, (double)_basic.slow / _chart->getNoteTotalCount());

        State::set(IndexOption::PLAY_RANK_ESTIMATED_2P, Option::getRankType(_basic.acc));
        State::set(IndexOption::PLAY_RANK_BORDER_2P, Option::getRankType(_basic.total_acc));
        State::set(IndexOption::PLAY_HEALTH_2P, Option::getHealthType(_basic.health));
    }
    else if (_side == PlaySide::MYBEST)
    {
        State::set(IndexBargraph::PLAY_MYBEST_NOW, _basic.total_acc / 100.0);
    }
}