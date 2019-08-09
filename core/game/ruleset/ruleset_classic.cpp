#include "ruleset_classic.h"
#include <plog/Log.h>
#include "common/chart/bms.h"
#include "game/data/timer.h"
#include "game/data/switch.h"
#include "game/data/option.h"
#include "game/data/number.h"
#include "game/data/bargraph.h"

using namespace rc;

void setJudgeTimer1PInner(int slot, long long t)
{
	gTimers.set(eTimer::_JUDGE_1P_0, -1);
	gTimers.set(eTimer::_JUDGE_1P_1, -1);
	gTimers.set(eTimer::_JUDGE_1P_2, -1);
	gTimers.set(eTimer::_JUDGE_1P_3, -1);
	gTimers.set(eTimer::_JUDGE_1P_4, -1);
	gTimers.set(eTimer::_JUDGE_1P_5, -1);
	switch (slot)
	{
	case 0: gTimers.set(eTimer::_JUDGE_1P_0, t); break;
	case 1: gTimers.set(eTimer::_JUDGE_1P_1, t); break;
	case 2: gTimers.set(eTimer::_JUDGE_1P_2, t); break;
	case 3: gTimers.set(eTimer::_JUDGE_1P_3, t); break;
	case 4: gTimers.set(eTimer::_JUDGE_1P_4, t); break;
	case 5: gTimers.set(eTimer::_JUDGE_1P_5, t); break;
	default: break;
	}
}

void setJudgeTimer2PInner(int slot, long long t)
{
	gTimers.set(eTimer::_JUDGE_2P_0, -1);
	gTimers.set(eTimer::_JUDGE_2P_1, -1);
	gTimers.set(eTimer::_JUDGE_2P_2, -1);
	gTimers.set(eTimer::_JUDGE_2P_3, -1);
	gTimers.set(eTimer::_JUDGE_2P_4, -1);
	gTimers.set(eTimer::_JUDGE_2P_5, -1);
	switch (slot)
	{
	case 0: gTimers.set(eTimer::_JUDGE_2P_0, t); break;
	case 1: gTimers.set(eTimer::_JUDGE_2P_1, t); break;
	case 2: gTimers.set(eTimer::_JUDGE_2P_2, t); break;
	case 3: gTimers.set(eTimer::_JUDGE_2P_3, t); break;
	case 4: gTimers.set(eTimer::_JUDGE_2P_4, t); break;
	case 5: gTimers.set(eTimer::_JUDGE_2P_5, t); break;
	default: break;
	}
}

RulesetClassic::RulesetClassic(std::shared_ptr<vChart> chart, std::shared_ptr<vScroll> scroll, 
    rc::judgeDiff difficulty, rc::gauge_ty gauge, rc::player p) :
    vRuleset(chart, scroll, rc::JUDGE_COUNT), _diff(difficulty), _count{ 0 }, _player(p)
{
    using namespace std::string_literals;

    int total = 160;
    switch (chart->type())
    {
    case eChartType::BMS:
        chart->getExtendedProperty("TOTAL"s, (void*)total);
        if (total < 0) total = 160;
        break;

    case eChartType::BMSON:
    default:
        break;
    }

    switch (gauge)
    {
    case HARD:
        _basic.health = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _health[PERFECT] = 1000.0 / 1001.0 / 100.0;
        _health[GREAT] = 1000.0 / 1001.0 / 100.0;
        _health[GOOD] = 500.0 / 1001.0 / 100.0;
        _health[BAD] = -0.06;
        _health[MISS] = -0.1;
        _health[BPOOR] = -0.02;
        break;

    case EXHARD:
        _basic.health = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _health[PERFECT] = 1000.0 / 1001.0 / 100.0;
        _health[GREAT] = 1000.0 / 1001.0 / 100.0;
        _health[GOOD] = 500.0 / 1001.0 / 100.0;
        _health[BAD] = -0.12;
        _health[MISS] = -0.2;
        _health[BPOOR] = -0.1;
        break;

    case DEATH:
        _basic.health = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _health[PERFECT] = 1000.0 / 1001.0 / 100.0;
        _health[GREAT] = 500.0 / 1001.0 / 100.0;
        _health[GOOD] = 0.0;
        _health[BAD] = -1.0;
        _health[MISS] = -1.0;
        _health[BPOOR] = -0.02;
        break;

    case P_ATK:
        _basic.health = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _health[PERFECT] = 1000.0 / 1001.0 / 100.0;
        _health[GREAT] = -0.02;
        _health[GOOD] = -1.0;
        _health[BAD] = -1.0;
        _health[MISS] = -1.0;
        _health[BPOOR] = -0.02;
        break;

    case G_ATK:
        _basic.health = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _health[PERFECT] = -0.02;
        _health[GREAT] = -0.02;
        _health[GOOD] = 0.0;
        _health[BAD] = -1.0;
        _health[MISS] = -1.0;
        _health[BPOOR] = -0.02;
        break;

    case GROOVE:
        _basic.health = 0.2;
        _minHealth = 0.02;
        _clearHealth = 0.8;
        _health[PERFECT] = 1.0 * scroll->getNoteCount() / total / 100.0;
        _health[GREAT] = 1.0 * scroll->getNoteCount() / total / 100.0;
        _health[GOOD] = 1.0 * scroll->getNoteCount() / total / 200.0;
        _health[BAD] = -0.04;
        _health[MISS] = -0.06;
        _health[BPOOR] = -0.02;
        break;

    case EASY:
        _basic.health = 0.2;
        _minHealth = 0.02;
        _clearHealth = 0.8;
        _health[PERFECT] = 1.0 * scroll->getNoteCount() / total / 100.0 * 1.2;
        _health[GREAT] = 1.0 * scroll->getNoteCount() / total / 100.0 * 1.2;
        _health[GOOD] = 1.0 * scroll->getNoteCount() / total / 200.0 * 1.2;
        _health[BAD] = -0.032;
        _health[MISS] = -0.048;
        _health[BPOOR] = -0.016;
        break;

    case ASSIST:
        _basic.health = 0.2;
        _minHealth = 0.02;
        _clearHealth = 0.6;
        _health[PERFECT] = 1.0 * scroll->getNoteCount() / total / 100.0 * 1.2;
        _health[GREAT] = 1.0 * scroll->getNoteCount() / total / 100.0 * 1.2;
        _health[GOOD] = 1.0 * scroll->getNoteCount() / total / 200.0 * 1.2;
        _health[BAD] = -0.032;
        _health[MISS] = -0.048;
        _health[BPOOR] = -0.016;
        break;

    case GRADE:
        _basic.health = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _health[PERFECT] = 1000.0 / 1001.0 / 100.0;
        _health[GREAT] = 1000.0 / 1001.0 / 100.0;
        _health[GOOD] = 500.0 / 1001.0 / 100.0;
        _health[BAD] = -0.02;
        _health[MISS] = -0.03;
        _health[BPOOR] = -0.02;
        break;

    case EXGRADE:
        _basic.health = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _health[PERFECT] = 1000.0 / 1001.0 / 100.0;
        _health[GREAT] = 1000.0 / 1001.0 / 100.0;
        _health[GOOD] = 500.0 / 1001.0 / 100.0;
        _health[BAD] = -0.12;
        _health[MISS] = -0.2;
        _health[BPOOR] = -0.1;
        break;

    default:
        break;
    }

	switch (p)
	{
	case rc::player::SP_1P:
		_k1P = true;
		_k2P = false;
		break;
	case rc::player::SP_2P:
		_k1P = false;
		_k2P = true;
		break;
	case rc::player::DP:
		_k1P = true;
		_k2P = true;
		break;
	case rc::player::BATTLE_1P:
		_k1P = true;
		_k2P = false;
		// TODO: volume
		break;
	case rc::player::BATTLE_2P:
		_k1P = false;
		_k2P = true;
		// TODO: volume
		break;
	}
}

judgeRes RulesetClassic::_judge(const Note& note, timestamp time)
{
    using namespace judgeArea;

    // spot judge area
    area a = NOTHING;
	timestamp error = time - note.time;
    if (error > -judgeTime[(size_t)_diff].BPOOR)
    {
        if (error < -judgeTime[(size_t)_diff].BAD)
            a = EARLY_BPOOR;
        else if (error < -judgeTime[(size_t)_diff].GOOD)
            a = EARLY_BAD;
        else if (error < -judgeTime[(size_t)_diff].GREAT)
            a = EARLY_GOOD;
        else if (error < -judgeTime[(size_t)_diff].PERFECT)
            a = EARLY_GREAT;
        else if (error < 0)
            a = EARLY_PERFECT;
        else if (error == 0)
            a = EXACT_PERFECT;
        else if (error < judgeTime[(size_t)_diff].PERFECT)
            a = LATE_PERFECT;
        else if (error < judgeTime[(size_t)_diff].GREAT)
            a = LATE_GREAT;
        else if (error < judgeTime[(size_t)_diff].GOOD)
            a = LATE_GOOD;
        else if (error < judgeTime[(size_t)_diff].BAD)
            a = LATE_BAD;
    }

    // log
    /*
    switch (a)
    {
    case EARLY_BPOOR:   LOG_DEBUG << "EARLY  BPOOR   " << error; break;
    case EARLY_BAD:     LOG_DEBUG << "EARLY  BAD     " << error; break;
    case EARLY_GOOD:    LOG_DEBUG << "EARLY  GOOD    " << error; break;
    case EARLY_GREAT:   LOG_DEBUG << "EARLY  GREAT   " << error; break;
    case EARLY_PERFECT: LOG_DEBUG << "EARLY  PERFECT " << error; break;
    case LATE_PERFECT:  LOG_DEBUG << "LATE   PERFECT " << error; break;
    case LATE_GREAT:    LOG_DEBUG << "LATE   GREAT   " << error; break;
    case LATE_GOOD:     LOG_DEBUG << "LATE   GOOD    " << error; break;
    case LATE_BAD:      LOG_DEBUG << "LATE   BAD     " << error; break;
    }
    */

    return { a, error };
}

void RulesetClassic::_updateHp(const double delta)
{
    double tmp = _basic.health + delta;
    _basic.health = std::max(_minHealth, std::min(1.0, tmp));
}

void RulesetClassic::updateHit(timestamp& t, NoteChannelIndex ch, size_t judge, unsigned slot)
{
    ++_count[judge];
    ++_basic.hit;
    ++_basic.combo;
    ++_basic.totaln;
    switch (judge)
    {
    case PERFECT:
        inner_score += 1.0 * 150000 / _scroll->getNoteCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _scroll->getNoteCount() - 55);
        break;
    case GREAT:
        inner_score += 1.0 * 100000 / _scroll->getNoteCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _scroll->getNoteCount() - 55);
        break;
    case GOOD:
        inner_score += 1.0 * 20000 / _scroll->getNoteCount() + 
            1.0 * std::min(int(_basic.combo) - 1, 10) * 50000 / (10 * _scroll->getNoteCount() - 55);
        break;
    default:
        break;
    }
    _updateHp(_health[judge]);
    if (_basic.combo > _basic.maxCombo) _basic.maxCombo = _basic.combo;
    _basic.score2 += 2;
    gTimers.set(bombTimer7k[ch], t.norm());
    slot == 0 ? setJudgeTimer1PInner(5 - judge, t.norm()) : setJudgeTimer2PInner(5 - judge, t.norm());
    gNumbers.set(slot == 0 ? eNumber::_DISP_NOWCOMBO_1P : eNumber::_DISP_NOWCOMBO_2P, _basic.combo);
}

void RulesetClassic::updateMiss(timestamp& t, NoteChannelIndex ch, size_t judge, unsigned slot)
{
    ++_count[judge];
    ++_basic.miss;
    if (judge != BPOOR) ++_basic.totaln;
    _updateHp(_health[judge]);
    if (judge != BPOOR) _basic.combo = 0;
    slot == 0 ? setJudgeTimer1PInner(5 - judge, t.norm()) : setJudgeTimer2PInner(5 - judge, t.norm());
    gNumbers.set(slot == 0 ? eNumber::_DISP_NOWCOMBO_1P : eNumber::_DISP_NOWCOMBO_2P, _basic.combo);
}

void RulesetClassic::updatePress(InputMask& pg, timestamp t)
{
	timestamp rt = t - gTimers.get(eTimer::PLAY_START);
    if (rt.norm() < 0) return;
	if (_k1P) for (size_t k = Input::S1L; k <= Input::K1SPDDN; ++k)
    {
        if (!pg[k]) continue;
        auto c = _scroll->getChannelFromKey((Input::Ingame)k);
		if (c.first == NoteChannelCategory::_) return;
        auto n = _scroll->incomingNoteOfChannel(c.first, c.second);
        auto j = _judge(*n, rt);
        switch (c.first)
        {
        case NoteChannelCategory::Note:
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
                using namespace judgeArea;
                case EARLY_PERFECT:
                case EXACT_PERFECT:
                case LATE_PERFECT:
                    updateHit(t, c.second, PERFECT, 0);
                    break;

                case EARLY_GREAT:
                case LATE_GREAT:
                    updateHit(t, c.second, GREAT, 0);
                    break;

                case EARLY_GOOD:
                case LATE_GOOD:
                    updateHit(t, c.second, GOOD, 0);
                    break;

                case EARLY_BAD:
                case LATE_BAD:
                    updateMiss(t, c.second, BAD, 0);
                    break;

                case EARLY_BPOOR:
                    updateMiss(t, c.second, BPOOR, 0);
                    break;
            }
            if (j.area > judgeArea::EARLY_BPOOR) n->hit = true;
            break;

        case NoteChannelCategory::Invs:
            break;

        case NoteChannelCategory::LN:
            // TODO LN
            // TODO scratch LN miss
            break;
        }
    }
	if (_k2P) for (size_t k = Input::S2L; k <= Input::K2SPDDN; ++k)
    {
        if (!pg[k]) continue;
        auto c = _scroll->getChannelFromKey((Input::Ingame)k);
		if (c.first == NoteChannelCategory::_) return;
        auto n = _scroll->incomingNoteOfChannel(c.first, c.second);
        auto j = _judge(*n, rt);
        switch (c.first)
        {
        case NoteChannelCategory::Note:
			switch (j.area)
			{
			case judgeArea::NOTHING:
				break;
			default:
				gTimers.set(eTimer::PLAY_JUDGE_2P, t.norm());
				break;
			}
            switch (j.area)
            {
                using namespace judgeArea;
            case EARLY_PERFECT:
            case EXACT_PERFECT:
            case LATE_PERFECT:
                updateHit(t, c.second, PERFECT, 1);
                break;

            case EARLY_GREAT:
            case LATE_GREAT:
                updateHit(t, c.second, GREAT, 1);
                break;

            case EARLY_GOOD:
            case LATE_GOOD:
                updateHit(t, c.second, GOOD, 1);
                break;

            case EARLY_BAD:
            case LATE_BAD:
                updateMiss(t, c.second, BAD, 1);
                break;

            case EARLY_BPOOR:
                updateMiss(t, c.second, BPOOR, 1);
                break;
            }
            if (j.area > judgeArea::EARLY_BPOOR) n->hit = true;
            break;

        case NoteChannelCategory::Invs:
            break;

        case NoteChannelCategory::LN:
            // TODO LN
            // TODO scratch LN miss
            break;
        }
    }
}
void RulesetClassic::updateHold(InputMask& hg, timestamp t)
{
	timestamp rt = t - gTimers.get(eTimer::PLAY_START);
    if (rt < 0) return;

    if (_k1P) for (size_t k = Input::S1L; k <= Input::K1SPDDN; ++k)
    {
        if (!hg[k]) continue;
        auto c = _scroll->getChannelFromKey((Input::Ingame)k);
		if (c.first == NoteChannelCategory::_) return;
        auto n = _scroll->incomingNoteOfChannel(c.first, c.second);
        switch (c.first)
        {
        case NoteChannelCategory::Mine:
		{
			auto j = _judge(*n, rt);
			if (j.area == judgeArea::EXACT_PERFECT ||
				j.area == judgeArea::EARLY_PERFECT && j.time < -2 ||
				j.area == judgeArea::LATE_PERFECT && j.time < 2)
			{
				n->hit = true;
                _updateHp(-1.0 * std::get<long long>(n->value) / base36("ZZ"));
				// TODO play mine sound + volume
			}
			break;
		}

        default:
            break;
        }
    }
    if (_k2P) for (size_t k = Input::S2L; k <= Input::K2SPDDN; ++k)
    {
        if (!hg[k]) continue;
        auto c = _scroll->getChannelFromKey((Input::Ingame)k);
		if (c.first == NoteChannelCategory::_) return;
        auto n = _scroll->incomingNoteOfChannel(c.first, c.second);
        switch (c.first)
        {
        case NoteChannelCategory::Mine:
		{
			auto j = _judge(*n, rt);
			if (j.area == judgeArea::EXACT_PERFECT ||
				j.area == judgeArea::EARLY_PERFECT && j.time < -2 ||
				j.area == judgeArea::LATE_PERFECT && j.time < 2)
			{
				n->hit = true;
                _updateHp(-1.0 * std::get<long long>(n->value) / base36("ZZ"));
				// TODO play mine sound + volume
			}
			break;
		}

        default:
            break;
        }
    }
}
void RulesetClassic::updateRelease(InputMask& rg, timestamp t)
{
	timestamp rt = t - gTimers.get(eTimer::PLAY_START);
    if (rt < 0) return;
    if (_k1P) for (size_t k = Input::S1L; k <= Input::K1SPDDN; ++k)
    {
        if (!rg[k]) continue;
        auto c = _scroll->getChannelFromKey((Input::Ingame)k);
		if (c.first == NoteChannelCategory::_) return;
        auto n = _scroll->incomingNoteOfChannel(c.first, c.second);
        //auto j = _judge(*n, rt);
        switch (c.first)
        {
        case NoteChannelCategory::LN:
            // TODO LN miss
            break;

        default:
            break;
        }
    }
    if (_k2P) for (size_t k = Input::S2L; k <= Input::K2SPDDN; ++k)
    {
        if (!rg[k]) continue;
        auto c = _scroll->getChannelFromKey((Input::Ingame)k);
		if (c.first == NoteChannelCategory::_) return;
        auto n = _scroll->incomingNoteOfChannel(c.first, c.second);
        //auto j = _judge(*n, rt);
        switch (c.first)
        {
        case NoteChannelCategory::LN:
            // TODO LN miss
            break;

        default:
            break;
        }
    }
}

void RulesetClassic::update(timestamp t)
{
	auto rt = t - gTimers.get(eTimer::PLAY_START);
    if (_k1P) for (size_t k = Input::S1L; k <= Input::K1SPDDN; ++k)
	{
		auto c = _scroll->getChannelFromKey((Input::Ingame)k);
		if (c.first == NoteChannelCategory::_) continue;

		auto n = _scroll->incomingNoteOfChannel(c.first, c.second);
		if (!_scroll->isLastNoteOfChannel(c.first, c.second, n) && !n->hit &&
			rt - n->time >= judgeTime[(size_t)_diff].BAD)
		{
			switch (c.first)
			{
			case NoteChannelCategory::Note:
			case NoteChannelCategory::LN:
				n->hit = true;
                updateMiss(t, c.second, MISS, 0);
				//LOG_DEBUG << "LATE   POOR    "; break;
				break;
			}
		}
    }
    if (_k2P) for (size_t k = Input::S2L; k <= Input::K2SPDDN; ++k)
	{
		auto c = _scroll->getChannelFromKey((Input::Ingame)k);
		if (c.first == NoteChannelCategory::_) continue;

		auto n = _scroll->incomingNoteOfChannel(c.first, c.second);
		if (!_scroll->isLastNoteOfChannel(c.first, c.second, n) && !n->hit &&
			rt - n->time >= judgeTime[(size_t)_diff].BAD)
		{
			switch (c.first)
			{
			case NoteChannelCategory::Note:
			case NoteChannelCategory::LN:
				n->hit = true;
                updateMiss(t, c.second, MISS, 1);
				//LOG_DEBUG << "LATE   POOR    "; break;
				break;
			}
		}
    }

	unsigned max = _scroll->getNoteCount() * 2;
	_basic.total_acc = 100.0 * _basic.score2 / max;
    _basic.acc = _basic.totaln ? (100.0 * _basic.score2 / _basic.totaln / 2) : 0;
    _basic.score = int(std::round(inner_score));
	gBargraphs.set(eBargraph::PLAY_EXSCORE, _basic.total_acc);
	if (_k1P) // includes DP
	{
		gNumbers.set(eNumber::PLAY_1P_EXSCORE, _basic.score2);
		gNumbers.set(eNumber::PLAY_1P_SCORE, _basic.score);
		gNumbers.set(eNumber::PLAY_1P_NOWCOMBO, _basic.combo);
		gNumbers.set(eNumber::PLAY_1P_MAXCOMBO, _basic.maxCombo);
		gNumbers.set(eNumber::PLAY_1P_RATE, int(std::floor(_basic.acc)));
        gNumbers.set(eNumber::PLAY_1P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        gNumbers.set(eNumber::PLAY_1P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        gNumbers.set(eNumber::PLAY_1P_TOTAL_RATE_DECIMAL2, int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        gNumbers.set(eNumber::PLAY_1P_PERFECT, _count[PERFECT]);
        gNumbers.set(eNumber::PLAY_1P_GREAT, _count[GREAT]);
        gNumbers.set(eNumber::PLAY_1P_GOOD, _count[GOOD]);
        gNumbers.set(eNumber::PLAY_1P_BAD, _count[BAD]);
        gNumbers.set(eNumber::PLAY_1P_POOR, _count[BPOOR]+_count[MISS]);
        gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, int(_basic.health * 100));

        {
            using namespace Option;
            if (_basic.acc >= 100.0)      gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_0);
            else if (_basic.acc >= 88.88) gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_1);
            else if (_basic.acc >= 77.77) gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_2);
            else if (_basic.acc >= 66.66) gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_3);
            else if (_basic.acc >= 55.55) gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_4);
            else if (_basic.acc >= 44.44) gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_5);
            else if (_basic.acc >= 33.33) gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_6);
            else if (_basic.acc >= 22.22) gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_7);
            else                          gOptions.set(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_8);

            if (_basic.total_acc >= 100.0)      gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_0);
            else if (_basic.total_acc >= 88.88) gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_1);
            else if (_basic.total_acc >= 77.77) gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_2);
            else if (_basic.total_acc >= 66.66) gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_3);
            else if (_basic.total_acc >= 55.55) gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_4);
            else if (_basic.total_acc >= 44.44) gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_5);
            else if (_basic.total_acc >= 33.33) gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_6);
            else if (_basic.total_acc >= 22.22) gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_7);
            else                                gOptions.set(eOption::PLAY_RANK_BORDER_1P, Option::RANK_8);

            if (_basic.acc >= 100.0)     gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_100);
            else if (_basic.acc >= 90.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_90);
            else if (_basic.acc >= 80.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_80);
            else if (_basic.acc >= 70.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_70);
            else if (_basic.acc >= 60.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_60);
            else if (_basic.acc >= 50.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_50);
            else if (_basic.acc >= 40.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_40);
            else if (_basic.acc >= 30.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_30);
            else if (_basic.acc >= 20.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_20);
            else if (_basic.acc >= 10.0) gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_10);
            else                         gOptions.set(eOption::PLAY_ACCURACY_1P, Option::ACC_0);
        }
	}
	else if (_k2P) // excludes DP
	{
		gNumbers.set(eNumber::PLAY_2P_EXSCORE, _basic.score2);
		gNumbers.set(eNumber::PLAY_2P_SCORE, _basic.score);
		gNumbers.set(eNumber::PLAY_2P_NOWCOMBO, _basic.combo);
		gNumbers.set(eNumber::PLAY_2P_MAXCOMBO, _basic.maxCombo);
        gNumbers.set(eNumber::PLAY_2P_RATE, int(std::floor(_basic.acc)));
        gNumbers.set(eNumber::PLAY_2P_RATEDECIMAL, int(std::floor((_basic.acc - int(_basic.acc)) * 100)));
        gNumbers.set(eNumber::PLAY_2P_TOTAL_RATE, int(std::floor(_basic.total_acc)));
        gNumbers.set(eNumber::PLAY_2P_TOTAL_RATE_DECIMAL2, int(std::floor((_basic.total_acc - int(_basic.total_acc)) * 100)));
        gNumbers.set(eNumber::PLAY_2P_PERFECT, _count[PERFECT]);
        gNumbers.set(eNumber::PLAY_2P_GREAT, _count[GREAT]);
        gNumbers.set(eNumber::PLAY_2P_GOOD, _count[GOOD]);
        gNumbers.set(eNumber::PLAY_2P_BAD, _count[BAD]);
        gNumbers.set(eNumber::PLAY_2P_POOR, _count[BPOOR] + _count[MISS]);
        gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, int(_basic.health * 100));

        {
            using namespace Option;
            if (_basic.acc >= 100.0)      gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_0);
            else if (_basic.acc >= 88.88) gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_1);
            else if (_basic.acc >= 77.77) gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_2);
            else if (_basic.acc >= 66.66) gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_3);
            else if (_basic.acc >= 55.55) gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_4);
            else if (_basic.acc >= 44.44) gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_5);
            else if (_basic.acc >= 33.33) gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_6);
            else if (_basic.acc >= 22.22) gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_7);
            else                          gOptions.set(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_8);

            if (_basic.total_acc >= 100.0)      gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_0);
            else if (_basic.total_acc >= 88.88) gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_1);
            else if (_basic.total_acc >= 77.77) gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_2);
            else if (_basic.total_acc >= 66.66) gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_3);
            else if (_basic.total_acc >= 55.55) gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_4);
            else if (_basic.total_acc >= 44.44) gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_5);
            else if (_basic.total_acc >= 33.33) gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_6);
            else if (_basic.total_acc >= 22.22) gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_7);
            else                                gOptions.set(eOption::PLAY_RANK_BORDER_2P, Option::RANK_8);

            if (_basic.acc >= 100.0)     gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_100);
            else if (_basic.acc >= 90.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_90);
            else if (_basic.acc >= 80.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_80);
            else if (_basic.acc >= 70.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_70);
            else if (_basic.acc >= 60.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_60);
            else if (_basic.acc >= 50.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_50);
            else if (_basic.acc >= 40.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_40);
            else if (_basic.acc >= 30.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_30);
            else if (_basic.acc >= 20.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_20);
            else if (_basic.acc >= 10.0) gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_10);
            else                         gOptions.set(eOption::PLAY_ACCURACY_2P, Option::ACC_0);
        }
	}

    // TODO global num update
        
}
