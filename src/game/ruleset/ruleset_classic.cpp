#include "ruleset_classic.h"
#include <plog/Log.h>
#include "game/data/timer.h"
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

RulesetClassic::RulesetClassic(vScroll* chart, rc::judgeDiff difficulty, rc::player p) : 
    vRuleset(chart, rc::JUDGE_COUNT), _diff(difficulty), _count{ 0 }, _player(p)
{
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

    // log
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

    return { a, error };
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
                    ++_count[PERFECT];
                    ++_basic.hit;
                    ++_basic.combo;
                    if (_basic.combo > _basic.maxCombo) _basic.maxCombo = _basic.combo;
                    _basic.score2 += 2;
					gTimers.set(bombTimer7k[c.second], t.norm());
					setJudgeTimer1PInner(5, t.norm());
                    break;

                case EARLY_GREAT:
                case LATE_GREAT:
                    ++_count[GREAT];
                    ++_basic.hit;
                    ++_basic.combo;
                    if (_basic.combo > _basic.maxCombo) _basic.maxCombo = _basic.combo;
                    _basic.score2 += 1;
					gTimers.set(bombTimer7k[c.second], t.norm());
					setJudgeTimer1PInner(4, t.norm());
                    break;

                case EARLY_GOOD:
                case LATE_GOOD:
                    ++_count[GOOD];
                    ++_basic.hit;
                    ++_basic.combo;
                    if (_basic.combo > _basic.maxCombo) _basic.maxCombo = _basic.combo;
					setJudgeTimer1PInner(3, t.norm());
                    break;

                case EARLY_BAD:
                case LATE_BAD:
                    ++_count[BAD];
                    ++_basic.miss;
                    _basic.combo = 0;
					setJudgeTimer1PInner(2, t.norm());
                    break;

                case EARLY_BPOOR:
                    ++_count[BPOOR];
                    ++_basic.miss;
					setJudgeTimer1PInner(0, t.norm());
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
                    ++_count[PERFECT];
                    ++_basic.hit;
                    ++_basic.combo;
                    if (_basic.combo > _basic.maxCombo) _basic.maxCombo = _basic.combo;
                    _basic.score2 += 2;
					gTimers.set(bombTimer7k[c.second], t.norm());
					setJudgeTimer2PInner(5, t.norm());
                    break;

                case EARLY_GREAT:
                case LATE_GREAT:
                    ++_count[GREAT];
                    ++_basic.hit;
                    ++_basic.combo;
                    if (_basic.combo > _basic.maxCombo) _basic.maxCombo = _basic.combo;
                    _basic.score2 += 1;
					gTimers.set(bombTimer7k[c.second], t.norm());
					setJudgeTimer2PInner(4, t.norm());
                    break;

                case EARLY_GOOD:
                case LATE_GOOD:
                    ++_count[GOOD];
                    ++_basic.hit;
                    ++_basic.combo;
                    if (_basic.combo > _basic.maxCombo) _basic.maxCombo = _basic.combo;
					setJudgeTimer2PInner(3, t.norm());
                    break;

                case EARLY_BAD:
                case LATE_BAD:
                    ++_count[BAD];
                    ++_basic.miss;
                    _basic.combo = 0;
					setJudgeTimer2PInner(2, t.norm());
                    break;

                case EARLY_BPOOR:
                    ++_count[BPOOR];
                    ++_basic.miss;
					setJudgeTimer2PInner(0, t.norm());
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
				++_count[MISS];
				_basic.combo = 0;
				gTimers.set(eTimer::PLAY_JUDGE_1P, t.norm());
				setJudgeTimer1PInner(1, t.norm());
				LOG_DEBUG << "LATE   POOR    "; break;
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
				++_count[MISS];
				_basic.combo = 0;
				gTimers.set(eTimer::PLAY_JUDGE_2P, t.norm());
				setJudgeTimer2PInner(1, t.norm());
				LOG_DEBUG << "LATE   POOR    "; break;
				break;
			}
		}
    }

	unsigned max = _scroll->getNoteCount() * 2;
	_basic.accuracy = (double)_basic.score2 / max;
	gBargraphs.set(eBargraph::PLAY_EXSCORE, _basic.accuracy);
	if (_k1P) // includes DP
	{
		gNumbers.set(eNumber::PLAY_1P_EXSCORE, _basic.score2);
		gNumbers.set(eNumber::PLAY_1P_SCORE, _basic.score);
		gNumbers.set(eNumber::PLAY_1P_NOWCOMBO, _basic.combo);
		gNumbers.set(eNumber::PLAY_1P_MAXCOMBO, _basic.maxCombo);
		gNumbers.set(eNumber::_DISP_NOWCOMBO_1P, _basic.combo);
	}
	else if (_k2P) // excludes DP
	{
		gNumbers.set(eNumber::PLAY_2P_EXSCORE, _basic.score2);
		gNumbers.set(eNumber::PLAY_2P_SCORE, _basic.score);
		gNumbers.set(eNumber::PLAY_2P_NOWCOMBO, _basic.combo);
		gNumbers.set(eNumber::PLAY_2P_MAXCOMBO, _basic.maxCombo);
		gNumbers.set(eNumber::_DISP_NOWCOMBO_2P, _basic.combo);
	}

    // TODO global num update
        
}
