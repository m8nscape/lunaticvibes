#include "ruleset_bms.h"
#include "common/log.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/data/data.h"
#include "game/scene/scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "config/config_mgr.h"

using namespace chart;

void setJudgeInternalTimer1P(RulesetBMS::JudgeType judge, long long t)
{
	gTimers.set(eTimer::_JUDGE_1P_0, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_1P_1, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_1P_2, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_1P_3, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_1P_4, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_1P_5, TIMER_NEVER);
	switch (judge)
	{
	case RulesetBMS::JudgeType::BPOOR:   gTimers.set(eTimer::_JUDGE_1P_0, t); break;
	case RulesetBMS::JudgeType::MISS:    gTimers.set(eTimer::_JUDGE_1P_1, t); break;
	case RulesetBMS::JudgeType::BAD:     gTimers.set(eTimer::_JUDGE_1P_2, t); break;
	case RulesetBMS::JudgeType::GOOD:    gTimers.set(eTimer::_JUDGE_1P_3, t); break;
	case RulesetBMS::JudgeType::GREAT:   gTimers.set(eTimer::_JUDGE_1P_4, t); break;
	case RulesetBMS::JudgeType::PERFECT: gTimers.set(eTimer::_JUDGE_1P_5, t); break;
	default: break;
	}
}

void setJudgeInternalTimer2P(RulesetBMS::JudgeType judge, long long t)
{
	gTimers.set(eTimer::_JUDGE_2P_0, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_2P_1, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_2P_2, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_2P_3, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_2P_4, TIMER_NEVER);
	gTimers.set(eTimer::_JUDGE_2P_5, TIMER_NEVER);
	switch (judge)
	{
    case RulesetBMS::JudgeType::BPOOR:   gTimers.set(eTimer::_JUDGE_2P_0, t); break;
    case RulesetBMS::JudgeType::MISS:    gTimers.set(eTimer::_JUDGE_2P_1, t); break;
    case RulesetBMS::JudgeType::BAD:     gTimers.set(eTimer::_JUDGE_2P_2, t); break;
    case RulesetBMS::JudgeType::GOOD:    gTimers.set(eTimer::_JUDGE_2P_3, t); break;
    case RulesetBMS::JudgeType::GREAT:   gTimers.set(eTimer::_JUDGE_2P_4, t); break;
    case RulesetBMS::JudgeType::PERFECT: gTimers.set(eTimer::_JUDGE_2P_5, t); break;
	default: break;
	}
}

RulesetBMS::RulesetBMS(std::shared_ptr<vChartFormat> format, std::shared_ptr<vChart> chart,
    eModGauge gauge, GameModeKeys keys, RulesetBMS::JudgeDifficulty difficulty, double health, RulesetBMS::PlaySide side) :
    vRuleset(format, chart), _judgeDifficulty(difficulty)
{
    switch (gauge)
    {
        case eModGauge::HARD        : _gauge = GaugeType::HARD;    break;
        case eModGauge::DEATH       : _gauge = GaugeType::DEATH;   break;
        case eModGauge::EASY        : _gauge = GaugeType::EASY;    break;
        case eModGauge::PATTACK     : _gauge = GaugeType::P_ATK;   break;
        case eModGauge::GATTACK     : _gauge = GaugeType::G_ATK;   break;
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
    case 7:  _bombTimerMap = &bombTimer7k;  _bombLNTimerMap = &bombTimer7kLN; break;
    case 9:  _bombTimerMap = &bombTimer9k;  _bombLNTimerMap = &bombTimer9kLN; break;
    case 14: _bombTimerMap = &bombTimer14k; _bombLNTimerMap = &bombTimer14kLN; break;
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
	case RulesetBMS::PlaySide::DOUBLE:
		_k1P = true;
		_k2P = true;
		break;
	case RulesetBMS::PlaySide::BATTLE_1P:
		_k1P = true;
		_k2P = false;
		break;
	case RulesetBMS::PlaySide::BATTLE_2P:
		_k1P = false;
		_k2P = true;
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

void RulesetBMS::_judgePress(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
    switch (cat)
    {
    case NoteLaneCategory::Note:
        switch (judge.area)
        {
        case judgeArea::EARLY_PERFECT:
        case judgeArea::EXACT_PERFECT:
        case judgeArea::LATE_PERFECT:
            updateHit(t, idx, RulesetBMS::JudgeType::PERFECT, slot);
            break;

        case judgeArea::EARLY_GREAT:
            _basic.fast++;
            updateHit(t, idx, RulesetBMS::JudgeType::GREAT, slot);
            break;
        case judgeArea::LATE_GREAT:
            _basic.slow++;
            updateHit(t, idx, RulesetBMS::JudgeType::GREAT, slot);
            break;

        case judgeArea::EARLY_GOOD:
            _basic.fast++;
            updateHit(t, idx, RulesetBMS::JudgeType::GOOD, slot);
            break;
        case judgeArea::LATE_GOOD:
            _basic.slow++;
            updateHit(t, idx, RulesetBMS::JudgeType::GOOD, slot);
            break;

        case judgeArea::EARLY_BAD:
            _basic.fast++;
            updateMiss(t, idx, RulesetBMS::JudgeType::BAD, slot);
            break;
        case judgeArea::LATE_BAD:
            _basic.slow++;
            updateMiss(t, idx, RulesetBMS::JudgeType::BAD, slot);
            break;

        case judgeArea::EARLY_BPOOR:
            _basic.fast++;
            updateMiss(t, idx, RulesetBMS::JudgeType::BPOOR, slot);
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
                if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                    gTimers.set(_bombLNTimerMap->at(idx), t.norm());
                break;

            case judgeArea::EARLY_BAD:
                _basic.fast++;
                _lnJudge[idx] = judge.area;
                note.hit = true;
                break;
            case judgeArea::LATE_BAD:
                _basic.slow++;
                _lnJudge[idx] = judge.area;
                note.hit = true;
                break;

            case judgeArea::EARLY_BPOOR:
                _basic.fast++;
                updateMiss(t, idx, RulesetBMS::JudgeType::BPOOR, slot);
                break;
            }
            break;
        }

        // TODO scratch LN miss
        break;
    }

    if (note.hit || judge.area == judgeArea::EARLY_BPOOR)
    {
        _lastNoteJudge = judge;
    }
}
void RulesetBMS::_judgeHold(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
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
            if (slot == PLAYER_SLOT_1P)
            {
                gTimers.set(eTimer::PLAY_JUDGE_1P, t.norm());
                setJudgeInternalTimer1P(JudgeType::BPOOR, t.norm());
            }
            else if (slot == PLAYER_SLOT_2P)
            {
                gTimers.set(eTimer::PLAY_JUDGE_2P, t.norm());
                setJudgeInternalTimer2P(JudgeType::BPOOR, t.norm());
            }

            // TODO play mine sound + volume

            _lastNoteJudge = { judgeArea::EARLY_BPOOR, t.norm() };
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

                if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                    gTimers.set(_bombLNTimerMap->at(idx), TIMER_NEVER);

                _lastNoteJudge = judge;
            }
        }
        break;

    default:
        break;
    }
}
void RulesetBMS::_judgeRelease(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
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
                break;

            case judgeArea::EARLY_GOOD:
                _basic.fast++;
                updateHit(t, idx, JudgeType::GOOD, slot);
                note.hit = true;
                _basic.notesExpired++;
                _lnJudge[idx] = RulesetBMS::judgeArea::NOTHING;
                break;

            case judgeArea::EARLY_BAD:
            default:
                _basic.fast++;
                updateMiss(t, idx, JudgeType::BAD, slot);
                note.hit = true;
                _basic.notesExpired++;
                _lnJudge[idx] = RulesetBMS::judgeArea::NOTHING;
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
}

void RulesetBMS::_updateHp(const double diff)
{
    double tmp = _basic.health;
    switch (_gauge)
    {
    case RulesetBMS::GaugeType::HARD:
    case RulesetBMS::GaugeType::GRADE:
        if (tmp < 0.30 && diff < 0.0)
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

void RulesetBMS::updateHit(const Time& t, NoteLaneIndex ch, RulesetBMS::JudgeType judge, int slot)
{
    ++_judgeCount[judge];
    ++_basic.hit;
    ++_basic.combo;
    bool setTimer = false;
    switch (judge)
    {
    case JudgeType::PERFECT:
        moneyScore += 1.0 * 150000 / _chart->getNoteTotalCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteTotalCount() - 55);
        _basic.score2 += 2;
        setTimer = true;
        break;
    case JudgeType::GREAT:
        moneyScore += 1.0 * 100000 / _chart->getNoteTotalCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteTotalCount() - 55);
        _basic.score2 += 1;
        setTimer = true;
        break;
    case JudgeType::GOOD:
        moneyScore += 1.0 * 20000 / _chart->getNoteTotalCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteTotalCount() - 55);
        break;
    default:
        break;
    }
    if (_bombTimerMap == nullptr || _bombTimerMap->find(ch) == _bombTimerMap->end())
        setTimer = false;

    _updateHp(judge);
    if (_basic.combo > _basic.maxCombo)
        _basic.maxCombo = _basic.combo;

    if (slot == PLAYER_SLOT_1P)
    {
        gTimers.set(eTimer::PLAY_JUDGE_1P, t.norm());
        setJudgeInternalTimer1P(judge, t.norm());
        gNumbers.set(eNumber::_DISP_NOWCOMBO_1P, _basic.combo);
        if (setTimer) gTimers.set(_bombTimerMap->at(ch), t.norm());

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
        gOptions.set(eOption::PLAY_LAST_JUDGE_1P, judgeType);
    }
    else if (slot == PLAYER_SLOT_2P)
    {
        gTimers.set(eTimer::PLAY_JUDGE_2P, t.norm());
        setJudgeInternalTimer2P(judge, t.norm());
        gNumbers.set(eNumber::_DISP_NOWCOMBO_2P, _basic.combo);
        if (setTimer) gTimers.set(_bombTimerMap->at(ch), t.norm());

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
        gOptions.set(eOption::PLAY_LAST_JUDGE_2P, judgeType);
    }
}

void RulesetBMS::updateMiss(const Time& t, NoteLaneIndex ch, RulesetBMS::JudgeType judge, int slot)
{
    ++_judgeCount[judge];
    ++_basic.miss;
    _updateHp(judge);
    if (judge != JudgeType::BPOOR)
    {
        if (_basic.combo == 0) ++_judgeCount[JudgeType::COMBOBREAK];
        _basic.combo = 0;
    }

    if (slot == PLAYER_SLOT_1P)
    {
        gTimers.set(eTimer::PLAY_JUDGE_1P, t.norm());
        setJudgeInternalTimer1P(judge, t.norm());
        gNumbers.set(eNumber::_DISP_NOWCOMBO_1P, _basic.combo);

        gOptions.set(eOption::PLAY_LAST_JUDGE_1P, (judge != JudgeType::BPOOR ? Option::JUDGE_4 : Option::JUDGE_5));
    }
    else
    {
        gTimers.set(eTimer::PLAY_JUDGE_2P, t.norm());
        setJudgeInternalTimer2P(judge, t.norm());
        gNumbers.set(eNumber::_DISP_NOWCOMBO_2P, _basic.combo);

        gOptions.set(eOption::PLAY_LAST_JUDGE_2P, (judge != JudgeType::BPOOR ? Option::JUDGE_4 : Option::JUDGE_5));
    }
}

void RulesetBMS::judgeNotePress(Input::Pad k, const Time& t, const Time& rt, int slot)
{
    NoteLaneIndex idx1 = _chart->getLaneFromKey(NoteLaneCategory::Note, k);
    HitableNote* pNote1 = nullptr;
    if (!_chart->isLastNote(NoteLaneCategory::Note, idx1))
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx1);
        while (itNote->hit && !_chart->isLastNote(NoteLaneCategory::Note, idx1, itNote))
            ++itNote;
        if (!_chart->isLastNote(NoteLaneCategory::Note, idx1, itNote))
            pNote1 = &*itNote;
    }
    NoteLaneIndex idx2 = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    HitableNote* pNote2 = nullptr;
    if (!_chart->isLastNote(NoteLaneCategory::LN, idx2))
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx2);
        while (itNote->hit && !_chart->isLastNote(NoteLaneCategory::LN, idx2, itNote))
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
    if (!_chart->isLastNote(NoteLaneCategory::Mine, idx))
    {
        auto& note = *_chart->incomingNote(NoteLaneCategory::Mine, idx);
        auto j = _judge(note, rt);
        _judgeHold(NoteLaneCategory::Mine, idx, note, j, t, slot);
    }

    idx = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    if (!_chart->isLastNote(NoteLaneCategory::LN, idx))
    {
        auto& note = *_chart->incomingNote(NoteLaneCategory::LN, idx);
        auto j = _judge(note, rt);
        _judgeHold(NoteLaneCategory::LN, idx, note, j, t, slot);
    }
}
void RulesetBMS::judgeNoteRelease(Input::Pad k, const Time& t, const Time& rt, int slot)
{
    NoteLaneIndex idx = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
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

    if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
        gTimers.set(_bombLNTimerMap->at(idx), TIMER_NEVER);
}

void RulesetBMS::updatePress(InputMask& pg, const Time& t)
{
	Time rt = t - gTimers.get(eTimer::PLAY_START);
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
    if (_k1P) updatePressRange(Input::S1L, Input::K1SPDDN, PLAYER_SLOT_1P);
    if (_k2P) updatePressRange(Input::S2L, Input::K2SPDDN, PLAYER_SLOT_2P);
}
void RulesetBMS::updateHold(InputMask& hg, const Time& t)
{
	Time rt = t - gTimers.get(eTimer::PLAY_START);
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
    if (_k1P) updateHoldRange(Input::S1L, Input::K1SPDDN, PLAYER_SLOT_1P);
    if (_k2P) updateHoldRange(Input::S2L, Input::K2SPDDN, PLAYER_SLOT_2P);
}
void RulesetBMS::updateRelease(InputMask& rg, const Time& t)
{
	Time rt = t - gTimers.get(eTimer::PLAY_START);
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
    if (_k1P) updateReleaseRange(Input::S1L, Input::K1SPDDN, PLAYER_SLOT_1P);
    if (_k2P) updateReleaseRange(Input::S2L, Input::K2SPDDN, PLAYER_SLOT_2P);
}
void RulesetBMS::updateAxis(double s1, double s2, const Time& t)
{
    Time rt = t - gTimers.get(eTimer::PLAY_START);
    if (rt.norm() < 0) return;
    if (gPlayContext.isAuto) return;

    using namespace Input;

    _scratchAccumulator[PLAYER_SLOT_1P] += s1;
    _scratchAccumulator[PLAYER_SLOT_2P] += s2;
}

void RulesetBMS::update(const Time& t)
{
	auto rt = t - gTimers.get(eTimer::PLAY_START);

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
            NoteLaneIndex idx;

            idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Note, idx, itNote) && !itNote->hit)
                {
                    const Time& hitTime = judgeTime[(size_t)_judgeDifficulty].BAD;
                    if (rt - itNote->time >= hitTime)
                    {
                        itNote->hit = true;
                        _basic.slow++;
                        updateMiss(t, idx, RulesetBMS::JudgeType::MISS, slot);
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
                                _basic.slow++;
                                _lnJudge[idx] = judgeArea::LATE_BAD;
                                itNote->hit = true;
                                updateMiss(t, idx, RulesetBMS::JudgeType::MISS, slot);
                                //LOG_DEBUG << "LATE   POOR    "; break;
                            }
                        }
                    }
                    else
                    {
                        if (rt >= itNote->time)
                        {
                            //_basic.slow++;
                            itNote->hit = true;
                            if (_lnJudge[idx] == judgeArea::EARLY_BAD || _lnJudge[idx] == judgeArea::LATE_BAD)
                            {
                                _basic.notesExpired++;
                                _lnJudge[idx] = judgeArea::NOTHING;
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
    if (_k1P) updateRange(Input::S1L, Input::K1SPDDN, PLAYER_SLOT_1P);
    if (_k2P) updateRange(Input::S2L, Input::K2SPDDN, PLAYER_SLOT_2P);


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
    updateScratch(t, Input::S1L, Input::S1R, _scratchAccumulator[PLAYER_SLOT_1P], PLAYER_SLOT_1P);
    updateScratch(t, Input::S2L, Input::S2R, _scratchAccumulator[PLAYER_SLOT_2P], PLAYER_SLOT_2P);


	unsigned max = _chart->getNoteTotalCount() * 2;
	_basic.total_acc = 100.0 * _basic.score2 / max;
    _basic.acc = _basic.notesExpired ? (100.0 * _basic.score2 / _basic.notesExpired / 2) : 0;
    _basic.score = int(std::round(moneyScore));

    updateGlobals();
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
        gBargraphs.queue(eBargraph::PLAY_EXSCORE, _basic.total_acc / 100.0);
        gBargraphs.queue(eBargraph::PLAY_EXSCORE_PREDICT, _basic.acc / 100.0);

        gNumbers.queue(eNumber::PLAY_1P_EXSCORE, _basic.score2);
        gNumbers.queue(eNumber::PLAY_1P_SCORE, _basic.score);
        gNumbers.queue(eNumber::PLAY_1P_NOWCOMBO, _basic.combo);
        gNumbers.queue(eNumber::PLAY_1P_MAXCOMBO, _basic.maxCombo);
        gNumbers.queue(eNumber::PLAY_1P_RATE, int(std::floor(_basic.acc)));
        gNumbers.queue(eNumber::PLAY_1P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        gNumbers.queue(eNumber::PLAY_1P_TOTALNOTES, _chart->getNoteTotalCount());
        gNumbers.queue(eNumber::PLAY_1P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        gNumbers.queue(eNumber::PLAY_1P_TOTAL_RATE_DECIMAL2, int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        gNumbers.queue(eNumber::PLAY_1P_PERFECT, _judgeCount[JudgeType::PERFECT]);
        gNumbers.queue(eNumber::PLAY_1P_GREAT, _judgeCount[JudgeType::GREAT]);
        gNumbers.queue(eNumber::PLAY_1P_GOOD, _judgeCount[JudgeType::GOOD]);
        gNumbers.queue(eNumber::PLAY_1P_BAD, _judgeCount[JudgeType::BAD]);
        gNumbers.queue(eNumber::PLAY_1P_POOR, _judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]);
        gNumbers.queue(eNumber::PLAY_1P_GROOVEGAUGE, int(_basic.health * 100));

        gNumbers.queue(eNumber::PLAY_1P_JUDGE_TIME_ERROR_MS, _lastNoteJudge.time.norm());
        gNumbers.queue(eNumber::PLAY_1P_MISS, _judgeCount[JudgeType::MISS]);
        gNumbers.queue(eNumber::PLAY_1P_FAST_COUNT, _basic.fast);
        gNumbers.queue(eNumber::PLAY_1P_SLOW_COUNT, _basic.slow);
        gNumbers.queue(eNumber::PLAY_1P_COMBOBREAK, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::MISS]);
        gNumbers.queue(eNumber::PLAY_1P_BPOOR, _judgeCount[JudgeType::BPOOR]);
        gNumbers.queue(eNumber::PLAY_1P_BP, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]);
        gNumbers.queue(eNumber::LR2IR_REPLACE_PLAY_1P_JUDGE_TIME_ERROR_MS, _lastNoteJudge.time.norm());
        gNumbers.queue(eNumber::LR2IR_REPLACE_PLAY_1P_FAST_COUNT, _basic.fast);
        gNumbers.queue(eNumber::LR2IR_REPLACE_PLAY_1P_SLOW_COUNT, _basic.slow);
        gNumbers.queue(eNumber::LR2IR_REPLACE_PLAY_1P_COMBOBREAK, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::MISS]);

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
        gNumbers.queue(eNumber::LR2IR_REPLACE_PLAY_1P_FAST_SLOW, fastslow);
        gOptions.queue(eOption::PLAY_LAST_JUDGE_FASTSLOW_1P, fastslow);

        gBargraphs.queue(eBargraph::RESULT_PG, (double)_judgeCount[JudgeType::PERFECT] / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_GR, (double)_judgeCount[JudgeType::GREAT] / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_GD, (double)_judgeCount[JudgeType::GOOD] / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_BD, (double)_judgeCount[JudgeType::BAD] / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_PR, (double)(_judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]) / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_MAXCOMBO, (double)_basic.maxCombo / getMaxCombo());
        gBargraphs.queue(eBargraph::RESULT_SCORE, (double)_basic.score / 200000);
        gBargraphs.queue(eBargraph::RESULT_EXSCORE, (double)_basic.score2 / getMaxScore());
        gBargraphs.queue(eBargraph::PLAY_1P_FAST_COUNT, (double)_basic.fast / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::PLAY_1P_SLOW_COUNT, (double)_basic.slow / _chart->getNoteTotalCount());

        {
            using namespace Option;
            gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::getRankType(_basic.acc));
            gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::getRankType(_basic.total_acc));
            gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::getAccType(_basic.acc));
        }

        int maxScore = getMaxScore();
        //if      (dp.total_acc >= 94.44) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * 1.000 - dp.score2));    // MAX-
        if      (_basic.total_acc >= 100.0 * 8.0 / 9) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, _basic.score2 - maxScore);    // MAX-
        else if (_basic.total_acc >= 100.0 * 7.0 / 9) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 8.0 / 9));    // AAA-
        else if (_basic.total_acc >= 100.0 * 6.0 / 9) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 7.0 / 9));    // AA-
        else if (_basic.total_acc >= 100.0 * 5.0 / 9) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 6.0 / 9));    // A-
        else if (_basic.total_acc >= 100.0 * 4.0 / 9) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 5.0 / 9));    // B-
        else if (_basic.total_acc >= 100.0 * 3.0 / 9) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 4.0 / 9));    // C-
        else if (_basic.total_acc >= 100.0 * 2.0 / 9) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 3.0 / 9));    // D-
        else                                          gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(_basic.score2 - maxScore * 2.0 / 9));    // E-

    }
    else if (_side == PlaySide::BATTLE_2P || _side == PlaySide::AUTO_2P || _side == PlaySide::RIVAL) // excludes DP
    {
        gBargraphs.queue(eBargraph::PLAY_RIVAL_EXSCORE, _basic.total_acc / 100.0);

        gNumbers.queue(eNumber::PLAY_2P_EXSCORE, _basic.score2);
        gNumbers.queue(eNumber::PLAY_2P_SCORE, _basic.score);
        gNumbers.queue(eNumber::PLAY_2P_NOWCOMBO, _basic.combo);
        gNumbers.queue(eNumber::PLAY_2P_MAXCOMBO, _basic.maxCombo);
        gNumbers.queue(eNumber::PLAY_2P_RATE, int(std::floor(_basic.acc)));
        gNumbers.queue(eNumber::PLAY_2P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        gNumbers.queue(eNumber::PLAY_2P_TOTALNOTES, _chart->getNoteTotalCount());
        gNumbers.queue(eNumber::PLAY_2P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        gNumbers.queue(eNumber::PLAY_2P_TOTAL_RATE_DECIMAL2, int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        gNumbers.queue(eNumber::PLAY_2P_PERFECT, _judgeCount[JudgeType::PERFECT]);
        gNumbers.queue(eNumber::PLAY_2P_GREAT, _judgeCount[JudgeType::GREAT]);
        gNumbers.queue(eNumber::PLAY_2P_GOOD, _judgeCount[JudgeType::GOOD]);
        gNumbers.queue(eNumber::PLAY_2P_BAD, _judgeCount[JudgeType::BAD]);
        gNumbers.queue(eNumber::PLAY_2P_POOR, _judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]);
        gNumbers.queue(eNumber::PLAY_2P_GROOVEGAUGE, int(_basic.health * 100));

        gNumbers.queue(eNumber::PLAY_2P_JUDGE_TIME_ERROR_MS, _lastNoteJudge.time.norm());
        gNumbers.queue(eNumber::PLAY_2P_MISS, _judgeCount[JudgeType::MISS]);
        gNumbers.queue(eNumber::PLAY_2P_FAST_COUNT, _basic.fast);
        gNumbers.queue(eNumber::PLAY_2P_SLOW_COUNT, _basic.slow);
        gNumbers.queue(eNumber::PLAY_2P_COMBOBREAK, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::MISS]);
        gNumbers.queue(eNumber::PLAY_2P_BPOOR, _judgeCount[JudgeType::BPOOR]);
        gNumbers.queue(eNumber::PLAY_2P_BP, _judgeCount[JudgeType::BAD] + _judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]);

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
        gNumbers.queue(eNumber::LR2IR_REPLACE_PLAY_2P_FAST_SLOW, fastslow);
        gOptions.queue(eOption::PLAY_LAST_JUDGE_FASTSLOW_2P, fastslow);

        gBargraphs.queue(eBargraph::RESULT_RIVAL_PG, (double)_judgeCount[JudgeType::PERFECT] / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_RIVAL_GR, (double)_judgeCount[JudgeType::GREAT] / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_RIVAL_GD, (double)_judgeCount[JudgeType::GOOD] / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_RIVAL_BD, (double)_judgeCount[JudgeType::BAD] / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_RIVAL_PR, (double)(_judgeCount[JudgeType::BPOOR] + _judgeCount[JudgeType::MISS]) / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::RESULT_RIVAL_MAXCOMBO, (double)_basic.maxCombo / getMaxCombo());
        gBargraphs.queue(eBargraph::RESULT_RIVAL_SCORE, (double)_basic.score / 200000);
        gBargraphs.queue(eBargraph::RESULT_RIVAL_EXSCORE, (double)_basic.score2 / getMaxScore());
        gBargraphs.queue(eBargraph::PLAY_2P_FAST_COUNT, (double)_basic.fast / _chart->getNoteTotalCount());
        gBargraphs.queue(eBargraph::PLAY_2P_SLOW_COUNT, (double)_basic.slow / _chart->getNoteTotalCount());

        {
            using namespace Option;
            gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::getRankType(_basic.acc));
            gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::getRankType(_basic.total_acc));
            gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::getAccType(_basic.acc));

        }
    }
    gNumbers.flush();
    gOptions.flush();
    gBargraphs.flush();
}