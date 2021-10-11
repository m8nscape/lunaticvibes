#include "ruleset_bms.h"
#include "common/log.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/data/data.h"
#include "game/scene/scene_context.h"

void setJudgeTimer1PInner(RulesetBMS::JudgeType judge, long long t)
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

void setJudgeTimer2PInner(RulesetBMS::JudgeType judge, long long t)
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
    eModGauge gauge, unsigned keys, RulesetBMS::JudgeDifficulty difficulty, double health, RulesetBMS::PlaySide side) :
    vRuleset(format, chart, 6), _diff(difficulty)
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
        //_basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _health[JudgeType::PERFECT] = 1.0 / 1001.0;
        _health[JudgeType::GREAT]   = 1.0 / 1001.0;
        _health[JudgeType::GOOD]    = 1.0 / 1001.0 / 2;
        _health[JudgeType::BAD]     = -0.06;
        _health[JudgeType::MISS]    = -0.1;
        _health[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::EXHARD:
        //_basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _health[JudgeType::PERFECT] = 1.0 / 1001.0;
        _health[JudgeType::GREAT]   = 1.0 / 1001.0;
        _health[JudgeType::GOOD]    = 1.0 / 1001.0 / 2;
        _health[JudgeType::BAD]     = -0.12;
        _health[JudgeType::MISS]    = -0.2;
        _health[JudgeType::BPOOR]   = -0.1;
        break;

    case GaugeType::DEATH:
        _basic.health               = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _health[JudgeType::PERFECT] = 1.0 / 1001.0;
        _health[JudgeType::GREAT]   = 1.0 / 1001.0 / 2;
        _health[JudgeType::GOOD]    = 0.0;
        _health[JudgeType::BAD]     = -1.0;
        _health[JudgeType::MISS]    = -1.0;
        _health[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::P_ATK:
        //_basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _health[JudgeType::PERFECT] = 1.0 / 1001.0;
        _health[JudgeType::GREAT]   = -0.02;
        _health[JudgeType::GOOD]    = -1.0;
        _health[JudgeType::BAD]     = -1.0;
        _health[JudgeType::MISS]    = -1.0;
        _health[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::G_ATK:
        //_basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _health[JudgeType::PERFECT] = -0.02;
        _health[JudgeType::GREAT]   = -0.02;
        _health[JudgeType::GOOD]    = 0.0;
        _health[JudgeType::BAD]     = -1.0;
        _health[JudgeType::MISS]    = -1.0;
        _health[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::GROOVE:
        //_basic.health             = 0.2;
        _minHealth                  = 0.02;
        _clearHealth                = 0.8;
        _health[JudgeType::PERFECT] = 0.01 * total / chart->getNoteCount();
        _health[JudgeType::GREAT]   = 0.01 * total / chart->getNoteCount();
        _health[JudgeType::GOOD]    = 0.01 * total / chart->getNoteCount() / 2;
        _health[JudgeType::BAD]     = -0.04;
        _health[JudgeType::MISS]    = -0.06;
        _health[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::EASY:
        //_basic.health             = 0.2;
        _minHealth                  = 0.02;
        _clearHealth                = 0.8;
        _health[JudgeType::PERFECT] = 0.01 * total / chart->getNoteCount() * 1.2;
        _health[JudgeType::GREAT]   = 0.01 * total / chart->getNoteCount() * 1.2;
        _health[JudgeType::GOOD]    = 0.01 * total / chart->getNoteCount() / 2 * 1.2;
        _health[JudgeType::BAD]     = -0.032;
        _health[JudgeType::MISS]    = -0.048;
        _health[JudgeType::BPOOR]   = -0.016;
        break;

    case GaugeType::ASSIST:
        //_basic.health             = 0.2;
        _minHealth                  = 0.02;
        _clearHealth                = 0.6;
        _health[JudgeType::PERFECT] = 0.01 * total / chart->getNoteCount() * 1.2;
        _health[JudgeType::GREAT]   = 0.01 * total / chart->getNoteCount() * 1.2;
        _health[JudgeType::GOOD]    = 0.01 * total / chart->getNoteCount() / 2 * 1.2;
        _health[JudgeType::BAD]     = -0.032;
        _health[JudgeType::MISS]    = -0.048;
        _health[JudgeType::BPOOR]   = -0.016;
        break;

    case GaugeType::GRADE:
        //_basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _health[JudgeType::PERFECT] = 1.0 / 1001.0;
        _health[JudgeType::GREAT]   = 1.0 / 1001.0;
        _health[JudgeType::GOOD]    = 1.0 / 1001.0 / 2;
        _health[JudgeType::BAD]     = -0.02;
        _health[JudgeType::MISS]    = -0.03;
        _health[JudgeType::BPOOR]   = -0.02;
        break;

    case GaugeType::EXGRADE:
        //_basic.health             = 1.0;
        _minHealth                  = 0;
        _clearHealth                = 0;
        _health[JudgeType::PERFECT] = 1.0 / 1001.0;
        _health[JudgeType::GREAT]   = 1.0 / 1001.0;
        _health[JudgeType::GOOD]    = 1.0 / 1001.0 / 2;
        _health[JudgeType::BAD]     = -0.12;
        _health[JudgeType::MISS]    = -0.2;     // FIXME not as hard as exhard
        _health[JudgeType::BPOOR]   = -0.1;
        break;

    default:
        break;
    }

	switch (side)
	{
	case RulesetBMS::PlaySide::SP:
	case RulesetBMS::PlaySide::DP:
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

    for (size_t k = Input::S1L; k <= Input::K2SPDDN; ++k)
    {
        auto [cat, idx] = _chart->getLaneFromKey((Input::Pad)k);
        if (cat == NoteLaneCategory::_) continue;
        if (idx == NoteLaneIndex::_) continue;
        _noteListIterators[{cat, idx}] = _chart->firstNote(cat, idx);
    }
}

RulesetBMS::judgeRes RulesetBMS::_judge(const Note& note, Time time)
{
    // spot judge area
    judgeArea a = judgeArea::NOTHING;
	Time error = time - note.time;
    if (error > -judgeTime[(size_t)_diff].BPOOR)
    {
        if (error < -judgeTime[(size_t)_diff].BAD)
            a = judgeArea::EARLY_BPOOR;
        else if (error < -judgeTime[(size_t)_diff].GOOD)
            a = judgeArea::EARLY_BAD;
        else if (error < -judgeTime[(size_t)_diff].GREAT)
            a = judgeArea::EARLY_GOOD;
        else if (error < -judgeTime[(size_t)_diff].PERFECT)
            a = judgeArea::EARLY_GREAT;
        else if (error < 0)
            a = judgeArea::EARLY_PERFECT;
        else if (error == 0)
            a = judgeArea::EXACT_PERFECT;
        else if (error < judgeTime[(size_t)_diff].PERFECT)
            a = judgeArea::LATE_PERFECT;
        else if (error < judgeTime[(size_t)_diff].GREAT)
            a = judgeArea::LATE_GREAT;
        else if (error < judgeTime[(size_t)_diff].GOOD)
            a = judgeArea::LATE_GOOD;
        else if (error < judgeTime[(size_t)_diff].BAD)
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

void RulesetBMS::_updateHp(const double delta)
{
    double tmp = _basic.health;
    switch (_gauge)
    {
    case RulesetBMS::GaugeType::HARD:
    case RulesetBMS::GaugeType::GRADE:
        if (tmp < 0.30 && delta < 0.0)
            tmp += delta * 0.6;
        else
            tmp += delta;
        break;
    default:
        tmp += delta;
        break;
    }
    _basic.health = std::max(_minHealth, std::min(1.0, tmp));

    if (_minHealth == 0.0 && _basic.health <= _minHealth)
        _isFailed = true;
}
void RulesetBMS::_updateHp(JudgeType judge)
{
    _updateHp(_health.at(judge));
}

void RulesetBMS::updateHit(Time& t, NoteLaneIndex ch, RulesetBMS::JudgeType judge, int slot)
{
    ++_count[judge];
    ++_basic.hit;
    ++_basic.combo;
    ++_basic.totaln;
    bool setTimer = false;
    switch (judge)
    {
    case JudgeType::PERFECT:
        inner_score += 1.0 * 150000 / _chart->getNoteCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteCount() - 55);
        _basic.score2 += 2;
        setTimer = true;
        break;
    case JudgeType::GREAT:
        inner_score += 1.0 * 100000 / _chart->getNoteCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteCount() - 55);
        _basic.score2 += 1;
        setTimer = true;
        break;
    case JudgeType::GOOD:
        inner_score += 1.0 * 20000 / _chart->getNoteCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _chart->getNoteCount() - 55);
        break;
    default:
        break;
    }
    if (setTimer && _bombTimerMap != nullptr && _bombTimerMap->find(ch) != _bombTimerMap->end())
        gTimers.set(_bombTimerMap->at(ch), t.norm());

    _updateHp(judge);
    if (_basic.combo > _basic.maxCombo)
        _basic.maxCombo = _basic.combo;

    if (slot == PLAYER_SLOT_1P)
    {
        setJudgeTimer1PInner(judge, t.norm());
        gNumbers.set(eNumber::_DISP_NOWCOMBO_1P, _basic.combo);
    }
    else
    {
        setJudgeTimer2PInner(judge, t.norm());
        gNumbers.set(eNumber::_DISP_NOWCOMBO_2P, _basic.combo);
    }
}

void RulesetBMS::updateMiss(Time& t, NoteLaneIndex ch, RulesetBMS::JudgeType judge, int slot)
{
    ++_count[judge];
    ++_basic.miss;
    _updateHp(judge);
    if (judge != JudgeType::BPOOR)
    {
        ++_basic.totaln;
        _basic.combo = 0;
    }

    if (slot == PLAYER_SLOT_1P)
    {
        setJudgeTimer1PInner(judge, t.norm());
        gNumbers.set(eNumber::_DISP_NOWCOMBO_1P, _basic.combo);
    }
    else
    {
        setJudgeTimer2PInner(judge, t.norm());
        gNumbers.set(eNumber::_DISP_NOWCOMBO_2P, _basic.combo);
    }
}

void RulesetBMS::updatePress(InputMask& pg, Time t)
{
	Time rt = t - gTimers.get(eTimer::PLAY_START);
    if (rt.norm() < 0) return;
    auto updatePressRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!pg[k]) continue;
            auto [cat, idx] = _chart->getLaneFromKey((Input::Pad)k);
            if (cat == NoteLaneCategory::_) return;
            auto n = _chart->incomingNote(cat, idx);
            auto j = _judge(*n, rt);
            switch (cat)
            {
            case NoteLaneCategory::Note:
                switch (j.area)
                {
                case judgeArea::NOTHING:
                    break;
                default:
                    gTimers.set(eTimer::PLAY_JUDGE_1P, t.norm());
                    break;
                }
                switch (j.area)
                {
                case judgeArea::EARLY_PERFECT:
                case judgeArea::EXACT_PERFECT:
                case judgeArea::LATE_PERFECT:
                    updateHit(t, idx, RulesetBMS::JudgeType::PERFECT, slot);
                    break;

                case judgeArea::EARLY_GREAT:
                case judgeArea::LATE_GREAT:
                    updateHit(t, idx, RulesetBMS::JudgeType::GREAT, slot);
                    break;

                case judgeArea::EARLY_GOOD:
                case judgeArea::LATE_GOOD:
                    updateHit(t, idx, RulesetBMS::JudgeType::GOOD, slot);
                    break;

                case judgeArea::EARLY_BAD:
                case judgeArea::LATE_BAD:
                    updateMiss(t, idx, RulesetBMS::JudgeType::BAD, slot);
                    break;

                case judgeArea::EARLY_BPOOR:
                    updateMiss(t, idx, RulesetBMS::JudgeType::BPOOR, slot);
                    break;
                }
                if (j.area > judgeArea::EARLY_BPOOR) n->hit = true;
                break;

            case NoteLaneCategory::Invs:
                break;

            case NoteLaneCategory::LN:
                if (!(n->flags & Note::LN_TAIL))
                {
                    switch (j.area)
                    {
                    case judgeArea::NOTHING:
                        _lnJudge[idx] = RulesetBMS::JudgeType::MISS;
                        break;
                    default:
                        if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                            gTimers.set(_bombLNTimerMap->at(idx), t.norm());
                        break;
                    }
                    switch (j.area)
                    {
                    case judgeArea::EARLY_PERFECT:
                    case judgeArea::EXACT_PERFECT:
                    case judgeArea::LATE_PERFECT:
                        _lnJudge[idx] = RulesetBMS::JudgeType::PERFECT;
                        break;

                    case judgeArea::EARLY_GREAT:
                    case judgeArea::LATE_GREAT:
                        _lnJudge[idx] = RulesetBMS::JudgeType::GREAT;
                        break;

                    case judgeArea::EARLY_GOOD:
                    case judgeArea::LATE_GOOD:
                        _lnJudge[idx] = RulesetBMS::JudgeType::GOOD;
                        break;

                    case judgeArea::EARLY_BAD:
                    case judgeArea::LATE_BAD:
                        _lnJudge[idx] = RulesetBMS::JudgeType::BAD;
                        break;

                    case judgeArea::EARLY_BPOOR:
                        _lnJudge[idx] = RulesetBMS::JudgeType::BPOOR;
                        break;
                    }
                    if (j.area > judgeArea::EARLY_BPOOR) n->hit = true;
                    break;
                }

                // TODO scratch LN miss
                break;
            }
        }
    };
    if (_k1P) updatePressRange(Input::S1L, Input::K1SPDDN, PLAYER_SLOT_1P);
    if (_k2P) updatePressRange(Input::S2L, Input::K2SPDDN, PLAYER_SLOT_2P);
}
void RulesetBMS::updateHold(InputMask& hg, Time t)
{
	Time rt = t - gTimers.get(eTimer::PLAY_START);
    if (rt < 0) return;

    auto updateHoldRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!hg[k]) continue;
            auto [cat, idx] = _chart->getLaneFromKey((Input::Pad)k);
            if (cat == NoteLaneCategory::_) return;
            auto n = _chart->incomingNote(cat, idx);
            switch (cat)
            {
            case NoteLaneCategory::Mine:
            {
                auto j = _judge(*n, rt);
                if (j.area == judgeArea::EXACT_PERFECT ||
                    j.area == judgeArea::EARLY_PERFECT && j.time < -2 ||
                    j.area == judgeArea::LATE_PERFECT && j.time < 2)
                {
                    n->hit = true;
                    _updateHp(-0.01 * std::get<long long>(n->value) / 2);
                    // TODO play mine sound + volume
                }
                break;
            }
            case NoteLaneCategory::LN:
                if (n->flags & Note::LN_TAIL)
                {
                    auto j = _judge(*n, rt);
                    if (j.area == judgeArea::EXACT_PERFECT ||
                        j.area == judgeArea::EARLY_PERFECT && j.time < -2 ||
                        j.area == judgeArea::LATE_PERFECT && j.time < 2)
                    {
                        n->hit = true;
                        updateHit(t, idx, _lnJudge[idx], slot);
                        if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                            gTimers.set(_bombLNTimerMap->at(idx), TIMER_NEVER);
                    }
                }
                break;

            default:
                break;
            }
        }
    };
    if (_k1P) updateHoldRange(Input::S1L, Input::K1SPDDN, PLAYER_SLOT_1P);
    if (_k2P) updateHoldRange(Input::S2L, Input::K2SPDDN, PLAYER_SLOT_2P);
}
void RulesetBMS::updateRelease(InputMask& rg, Time t)
{
	Time rt = t - gTimers.get(eTimer::PLAY_START);
    if (rt < 0) return;

    auto updateReleaseRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!rg[k]) continue;
            auto [cat, idx] = _chart->getLaneFromKey((Input::Pad)k);
            if (cat == NoteLaneCategory::_) continue;
            auto n = _chart->incomingNote(cat, idx);

            if (_bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                gTimers.set(_bombLNTimerMap->at(idx), TIMER_NEVER);

            //auto j = _judge(*n, rt);
            switch (cat)
            {
            case NoteLaneCategory::LN:
                // TODO LN miss
                if (_lnJudge[idx] != RulesetBMS::JudgeType::MISS && (n->flags & Note::LN_TAIL))
                {
                    auto j = _judge(*n, rt);
                    switch (j.area)
                    {
                    case judgeArea::EARLY_PERFECT:
                    case judgeArea::EXACT_PERFECT:
                        updateHit(t, idx, _lnJudge[idx], slot);
                        _lnJudge[idx] = RulesetBMS::JudgeType::MISS;
                        break;

                    case judgeArea::EARLY_GREAT:
                        updateHit(t, idx, std::max(JudgeType::GREAT, _lnJudge[idx]), slot);
                        _lnJudge[idx] = RulesetBMS::JudgeType::MISS;
                        break;

                    case judgeArea::EARLY_GOOD:
                        updateHit(t, idx, JudgeType::GOOD, slot);
                        _lnJudge[idx] = RulesetBMS::JudgeType::MISS;
                        break;

                    case judgeArea::EARLY_BAD:
                    default:
                        updateMiss(t, idx, JudgeType::BAD, slot);
                        _lnJudge[idx] = RulesetBMS::JudgeType::MISS;
                        break;
                    }
                    n->hit = true;
                    break;
                }
                break;

            default:
                break;
            }
        }
    };
    if (_k1P) updateReleaseRange(Input::S1L, Input::K1SPDDN, PLAYER_SLOT_1P);
    if (_k2P) updateReleaseRange(Input::S2L, Input::K2SPDDN, PLAYER_SLOT_2P);
}

void RulesetBMS::update(Time t)
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
                _basic.totalnr++;
                break;

            case NoteLaneCategory::LN:
                if (n->flags & Note::LN_TAIL)
                    _basic.totalnr++;
                break;
            }

            n++;
        }
    }

    auto updateRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            auto [cat, idx] = _chart->getLaneFromKey((Input::Pad)k);
            if (cat == NoteLaneCategory::_) continue;

            auto n = _chart->incomingNote(cat, idx);
            if (!_chart->isLastNote(cat, idx, n) && !n->hit)
            {
                if (!(n->flags & Note::LN_TAIL))
                {
                    if (rt - n->time >= judgeTime[(size_t)_diff].BAD)
                    {
                        switch (cat)
                        {
                        case NoteLaneCategory::Note:
                        case NoteLaneCategory::LN:
                            n->hit = true;
                            updateMiss(t, idx, RulesetBMS::JudgeType::MISS, slot);
                            //LOG_DEBUG << "LATE   POOR    "; break;
                            break;
                        }
                    }
                }
                else
                {
                    if (rt >= n->time)
                    {
                        switch (cat)
                        {
                        case NoteLaneCategory::Note:
                        case NoteLaneCategory::LN:
                            n->hit = true;
                            //LOG_DEBUG << "LATE   POOR    "; break;
                            break;
                        }
                    }
                }
            }
        }
    };
    if (_k1P) updateRange(Input::S1L, Input::K1SPDDN, PLAYER_SLOT_1P);
    if (_k2P) updateRange(Input::S2L, Input::K2SPDDN, PLAYER_SLOT_2P);

	unsigned max = _chart->getNoteCount() * 2;
	_basic.total_acc = 100.0 * _basic.score2 / max;
    _basic.acc = _basic.totaln ? (100.0 * _basic.score2 / _basic.totaln / 2) : 0;
    _basic.score = int(std::round(inner_score));
	gBargraphs.queue(eBargraph::PLAY_EXSCORE, _basic.total_acc);
	if (_k1P) // includes DP
	{
		gNumbers.queue(eNumber::PLAY_1P_EXSCORE, _basic.score2);
		gNumbers.queue(eNumber::PLAY_1P_SCORE, _basic.score);
		gNumbers.queue(eNumber::PLAY_1P_NOWCOMBO, _basic.combo);
		gNumbers.queue(eNumber::PLAY_1P_MAXCOMBO, _basic.maxCombo);
		gNumbers.queue(eNumber::PLAY_1P_RATE, int(std::floor(_basic.acc)));
        gNumbers.queue(eNumber::PLAY_1P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        gNumbers.queue(eNumber::PLAY_1P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        gNumbers.queue(eNumber::PLAY_1P_TOTAL_RATE_DECIMAL2, int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        gNumbers.queue(eNumber::PLAY_1P_PERFECT, _count[JudgeType::PERFECT]);
        gNumbers.queue(eNumber::PLAY_1P_GREAT, _count[JudgeType::GREAT]);
        gNumbers.queue(eNumber::PLAY_1P_GOOD, _count[JudgeType::GOOD]);
        gNumbers.queue(eNumber::PLAY_1P_BAD, _count[JudgeType::BAD]);
        gNumbers.queue(eNumber::PLAY_1P_POOR, _count[JudgeType::BPOOR] + _count[JudgeType::MISS]);
        gNumbers.queue(eNumber::PLAY_1P_GROOVEGAUGE, int(_basic.health * 100));

        {
            using namespace Option;
            if (_basic.acc >= 100.0)      gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_0);
            else if (_basic.acc >= 88.88) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_1);
            else if (_basic.acc >= 77.77) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_2);
            else if (_basic.acc >= 66.66) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_3);
            else if (_basic.acc >= 55.55) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_4);
            else if (_basic.acc >= 44.44) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_5);
            else if (_basic.acc >= 33.33) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_6);
            else if (_basic.acc >= 22.22) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_7);
            else                          gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_8);

            if (_basic.total_acc >= 100.0)      gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_0);
            else if (_basic.total_acc >= 88.88) gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_1);
            else if (_basic.total_acc >= 77.77) gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_2);
            else if (_basic.total_acc >= 66.66) gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_3);
            else if (_basic.total_acc >= 55.55) gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_4);
            else if (_basic.total_acc >= 44.44) gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_5);
            else if (_basic.total_acc >= 33.33) gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_6);
            else if (_basic.total_acc >= 22.22) gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_7);
            else                                gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_8);

            if (_basic.acc >= 100.0)     gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_100);
            else if (_basic.acc >= 90.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_90);
            else if (_basic.acc >= 80.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_80);
            else if (_basic.acc >= 70.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_70);
            else if (_basic.acc >= 60.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_60);
            else if (_basic.acc >= 50.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_50);
            else if (_basic.acc >= 40.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_40);
            else if (_basic.acc >= 30.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_30);
            else if (_basic.acc >= 20.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_20);
            else if (_basic.acc >= 10.0) gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_10);
            else                         gOptions.queue(eOption::PLAY_ACCURACY_1P, Option::ACC_0);
        }
	}
	else if (_k2P) // excludes DP
	{
		gNumbers.queue(eNumber::PLAY_2P_EXSCORE, _basic.score2);
		gNumbers.queue(eNumber::PLAY_2P_SCORE, _basic.score);
		gNumbers.queue(eNumber::PLAY_2P_NOWCOMBO, _basic.combo);
		gNumbers.queue(eNumber::PLAY_2P_MAXCOMBO, _basic.maxCombo);
        gNumbers.queue(eNumber::PLAY_2P_RATE, int(std::floor(_basic.acc)));
        gNumbers.queue(eNumber::PLAY_2P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        gNumbers.queue(eNumber::PLAY_2P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        gNumbers.queue(eNumber::PLAY_2P_TOTAL_RATE_DECIMAL2, int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        gNumbers.queue(eNumber::PLAY_2P_PERFECT, _count[JudgeType::PERFECT]);
        gNumbers.queue(eNumber::PLAY_2P_GREAT, _count[JudgeType::GREAT]);
        gNumbers.queue(eNumber::PLAY_2P_GOOD, _count[JudgeType::GOOD]);
        gNumbers.queue(eNumber::PLAY_2P_BAD, _count[JudgeType::BAD]);
        gNumbers.queue(eNumber::PLAY_2P_POOR, _count[JudgeType::BPOOR] + _count[JudgeType::MISS]);
        gNumbers.queue(eNumber::PLAY_2P_GROOVEGAUGE, int(_basic.health * 100));

        {
            using namespace Option;
            if (_basic.acc >= 100.0)      gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_0);
            else if (_basic.acc >= 88.88) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_1);
            else if (_basic.acc >= 77.77) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_2);
            else if (_basic.acc >= 66.66) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_3);
            else if (_basic.acc >= 55.55) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_4);
            else if (_basic.acc >= 44.44) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_5);
            else if (_basic.acc >= 33.33) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_6);
            else if (_basic.acc >= 22.22) gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_7);
            else                          gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_8);

            if (_basic.total_acc >= 100.0)      gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_0);
            else if (_basic.total_acc >= 88.88) gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_1);
            else if (_basic.total_acc >= 77.77) gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_2);
            else if (_basic.total_acc >= 66.66) gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_3);
            else if (_basic.total_acc >= 55.55) gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_4);
            else if (_basic.total_acc >= 44.44) gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_5);
            else if (_basic.total_acc >= 33.33) gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_6);
            else if (_basic.total_acc >= 22.22) gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_7);
            else                                gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_8);

            if (_basic.acc >= 100.0)     gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_100);
            else if (_basic.acc >= 90.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_90);
            else if (_basic.acc >= 80.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_80);
            else if (_basic.acc >= 70.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_70);
            else if (_basic.acc >= 60.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_60);
            else if (_basic.acc >= 50.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_50);
            else if (_basic.acc >= 40.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_40);
            else if (_basic.acc >= 30.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_30);
            else if (_basic.acc >= 20.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_20);
            else if (_basic.acc >= 10.0) gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_10);
            else                         gOptions.queue(eOption::PLAY_ACCURACY_2P, Option::ACC_0);

        }
	}
    gNumbers.flush();
    gOptions.flush();

    // TODO global num update
        
}
