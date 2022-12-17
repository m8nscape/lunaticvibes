#include "sprite_lane.h"
#include "game/runtime/state.h"
#include "game/scene/scene_context.h"
#include <cassert>

using namespace chart;

SpriteLaneVertical::SpriteLaneVertical(unsigned player, bool autoNotes, double basespeed, double lanespeed):
	SpriteStatic(nullptr, Rect(0)), playerSlot(player), _autoNotes(autoNotes)
{
	_type = SpriteTypes::NOTE_VERT;
	_basespd = basespeed * lanespeed;
	_hispeed = 1.0;
	_category = NoteLaneCategory::_;
	_index = NoteLaneIndex::_;

	_hiddenCompatibleTexture = std::make_shared<TextureFull>(Color(128, 128, 128, 255));
}

SpriteLaneVertical::SpriteLaneVertical(pTexture texture, Rect r,
    unsigned animFrames, unsigned frameTime, IndexTimer timer,
	unsigned animRows, unsigned animCols, bool animVerticalIndexing,
    unsigned player, bool autoNotes, double basespeed, double lanespeed):
	SpriteLaneVertical(player, autoNotes, basespeed, lanespeed)
{
	pNote = std::make_shared<SpriteAnimated>(texture, r, animFrames, frameTime, timer, animRows, animCols, animVerticalIndexing);
}


void SpriteLaneVertical::setLane(NoteLaneCategory cat, NoteLaneIndex idx)
{
	_category = cat;
	_index = idx;

	if (_category != chart::NoteLaneCategory::EXTRA)
	{
		switch (playerSlot)
		{
		case PLAYER_SLOT_PLAYER:
			if ((_index == chart::NoteLaneIndex::Sc1 && (gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR)) ||
				(_index == chart::NoteLaneIndex::Sc2 && !gPlayContext.isBattle))
			{
				if (!_autoNotes) _hideInternal = true;
			}
			else
			{
				if (_autoNotes) _hideInternal = true;
			}
			break;
		case PLAYER_SLOT_TARGET:
			if (_index == chart::NoteLaneIndex::Sc2 &&
				(gPlayContext.mods[gPlayContext.isBattle ? PLAYER_SLOT_TARGET : PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
			{
				if (!_autoNotes) _hideInternal = true;
			}
			else
			{
				if (_autoNotes) _hideInternal = true;
			}
			break;
		default:
			break;
		}
	}
}

void SpriteLaneVertical::setLoopTime(int t)
{
	SpriteStatic::setLoopTime(t);
	if (pNote) pNote->setLoopTime(t);
}

void SpriteLaneVertical::setTrigTimer(IndexTimer t)
{
	SpriteStatic::setTrigTimer(t);
	if (pNote) pNote->setTrigTimer(t);
}

void SpriteLaneVertical::appendKeyFrame(const RenderKeyFrame& f)
{
	SpriteStatic::appendKeyFrame(f);
	if (pNote) pNote->appendKeyFrame(f);
}

std::pair<NoteLaneCategory, NoteLaneIndex> SpriteLaneVertical::getLane() const
{
	return std::make_pair(_category, _index); 
}

void SpriteLaneVertical::getRectSize(int& w, int& h)
{
	if (!pNote || pNote->_texRect.empty())
	{
		w = h = 0;
	}
	else
	{
		w = pNote->_texRect[0].w;
		h = pNote->_texRect[0].h;
	}
}

bool SpriteLaneVertical::update(const Time& t)
{
	if (_hideInternal) return false;

	if (updateByKeyframes(t))
	{
		switch (playerSlot)
		{
		case 0:
			_hispeed = gPlayContext.HispeedGradientNow;
			break;
		case 1:
			_hispeed = gPlayContext.isBattle ? gPlayContext.battle2PHispeedGradientNow : gPlayContext.HispeedGradientNow;
			break;
		default:
			break;
		}
		updateNoteRect(t);
		updateHIDDENCompatible();
		return true;
	}
	return false;
}

void SpriteLaneVertical::updateNoteRect(const Time& t)
{
	_outRect.clear();
	auto pChart = gPlayContext.chartObj[gPlayContext.isBattle ? playerSlot : 0];
	if (pChart == nullptr || !gChartContext.started)
	{
		return;
	}
	auto metre = gUpdateContext.metre;
	auto bar = gUpdateContext.bar;

    // refresh note sprites
	if (!pNote) return;
	pNote->update(t);

	if (gPlayContext.mods[playerSlot].hispeedFix != eModHs::CONSTANT)
	{
		// fetch note size, c.y + c.h = judge line pos (top-left corner), -c.h = height start drawing
		auto c = _current.rect;
		auto currTotalMetre = pChart->getBarMetrePosition(bar).toDouble() + metre;

		// generate note rects and store to buffer
		// 120BPM with 1.0x HS is 2000ms (500ms/beat, green number 1200)
		// !!! scroll height should not affected by note height
		int y = (c.y + c.h);
		auto it = pChart->incomingNote(_category, _index);
		while (!pChart->isLastNote(_category, _index, it) && y >= 0)
		{
			auto noteMetreOffset = currTotalMetre - it->pos.toDouble();
			if (noteMetreOffset >= 0)
				y = (c.y + c.h); // expired notes stay on judge line, LR2 / pre RA behavior
			else
				y = (c.y + c.h) - static_cast<int>(std::floor(-noteMetreOffset * _noteAreaHeight * _basespd * _hispeed));
			it++;
			_outRect.push_front({ c.x, (float)y, c.w, -c.h });
		}
	}
	else
	{
		// fetch note size, c.y + c.h = judge line pos (top-left corner), -c.h = height start drawing
		auto c = _current.rect;
		long long currTimestamp = gChartContext.started ? (t - State::get(IndexTimer::PLAY_START)).norm() : 0;

		// generate note rects and store to buffer
		// CONSTANT: generate note rects with timestamp (BPM=150)
		// 150BPM with 1.0x HS is 1600ms (400ms/beat, green number 960)
		int y = (c.y + c.h);
		auto it = pChart->incomingNote(_category, _index);
		while (!pChart->isLastNote(_category, _index, it) && y >= 0)
		{
			auto noteTimeOffset = currTimestamp - it->time.norm();
			if (noteTimeOffset >= 0)
				y = (c.y + c.h); // expired notes stay on judge line, LR2 / pre RA behavior
			else
				y = (c.y + c.h) - static_cast<int>(std::floor(-noteTimeOffset / 1600.0 * _noteAreaHeight * _basespd * _hispeed));
			it++;
			_outRect.push_front({ c.x, (float)y, c.w, -c.h });
		}
	}
}

void SpriteLaneVertical::draw() const 
{
	if (isHidden()) return;

    if (pNote && pNote->_pTexture && pNote->_pTexture->_loaded)
	{
		for (const auto& r : _outRect)
		{
			pNote->_pTexture->draw(
				pNote->_texRect[pNote->_selectionIdx * pNote->_animFrames + pNote->_currAnimFrame],
				r,
				_current.color,
				_current.blend, 
				_current.filter,
				_current.angle,
				_current.center);
		}
	}

	if (_hiddenCompatibleDraw)
	{
		_hiddenCompatibleTexture->draw(
			RECT_FULL, _hiddenCompatibleArea,
			Color(0xffffffff), BlendMode::ALPHA, false, 0
		);
	}
}

void SpriteLaneVertical::adjustAfterUpdate(int x, int y, int w, int h)
{
	for (auto& r : _outRect)
	{
		r.x += x - w;
		r.y += y - h;
		r.w += w * 2;
		r.h += h * 2;
	}
}

void SpriteLaneVertical::updateHIDDENCompatible()
{
	if (_hiddenCompatible)
	{
		_hiddenCompatibleDraw = false;
		if (playerSlot == PLAYER_SLOT_PLAYER)
		{
			auto lcType = Option::e_lane_effect_type(State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P));
			if ((lcType == Option::LANE_HIDDEN || lcType == Option::LANE_SUDHID) &&
				State::get(IndexSwitch::P1_LANECOVER_ENABLED))
			{
				_hiddenCompatibleDraw = true;
				_hiddenCompatibleArea = _current.rect;
				double p = State::get(IndexNumber::LANECOVER_BOTTOM_1P) / 1000.0;
				int h = _noteAreaHeight;
				_hiddenCompatibleArea.y = h;
				_hiddenCompatibleArea.h = -h * p;
			}
		}
		else
		{
			Option::e_lane_effect_type lcType;
			bool sw;
			int lc;
			if (gPlayContext.isBattle)
			{
				lcType = Option::e_lane_effect_type(State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_2P));
				sw = State::get(IndexSwitch::P2_LANECOVER_ENABLED);
				lc = State::get(IndexNumber::LANECOVER_BOTTOM_2P);
			}
			else
			{
				lcType = Option::e_lane_effect_type(State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P));
				sw = State::get(IndexSwitch::P1_LANECOVER_ENABLED);
				lc = State::get(IndexNumber::LANECOVER_BOTTOM_1P);
			}
			if ((lcType == Option::LANE_HIDDEN || lcType == Option::LANE_SUDHID) && sw)
			{
				_hiddenCompatibleDraw = true;
				_hiddenCompatibleArea = _current.rect;
				double p = lc / 1000.0;
				int h = _noteAreaHeight;
				_hiddenCompatibleArea.y = h;
				_hiddenCompatibleArea.h = -h * p;
			}
		}
	}
}

void SpriteLaneVerticalLN::setTrigTimer(IndexTimer t)
{
	SpriteLaneVertical::setTrigTimer(t);

	switch (t)
	{
	case IndexTimer::S1_LN_BOMB:
	case IndexTimer::K11_LN_BOMB:
	case IndexTimer::K12_LN_BOMB:
	case IndexTimer::K13_LN_BOMB:
	case IndexTimer::K14_LN_BOMB:
	case IndexTimer::K15_LN_BOMB:
	case IndexTimer::K16_LN_BOMB:
	case IndexTimer::K17_LN_BOMB:
	case IndexTimer::K18_LN_BOMB:
	case IndexTimer::K19_LN_BOMB:
	case IndexTimer::S2_LN_BOMB:
	case IndexTimer::K21_LN_BOMB:
	case IndexTimer::K22_LN_BOMB:
	case IndexTimer::K23_LN_BOMB:
	case IndexTimer::K24_LN_BOMB:
	case IndexTimer::K25_LN_BOMB:
	case IndexTimer::K26_LN_BOMB:
	case IndexTimer::K27_LN_BOMB:
	case IndexTimer::K28_LN_BOMB:
	case IndexTimer::K29_LN_BOMB:
		animLimited = true;
		break;

	default:
		animLimited = false;
		break;
	}
}


void SpriteLaneVerticalLN::updateNoteRect(const Time& t)
{
	_outRect.clear();
	_outRectBody.clear();
	_outRectTail.clear();

	auto pChart = gPlayContext.chartObj[gPlayContext.isBattle ? playerSlot : 0];
	if (pChart == nullptr || !gChartContext.started)
	{
		return;
	}
	auto metre = gUpdateContext.metre;
	auto bar = gUpdateContext.bar;

	// refresh note sprites
	if (!pNote) return;
	pNote->update(t);

	if (pNoteTail) pNoteTail->update(t);


	if (gPlayContext.mods[playerSlot].hispeedFix != eModHs::CONSTANT)
	{
		// fetch note size, c.y + c.h = judge line pos (top-left corner), -c.h = height start drawing
		auto c = _current.rect;
		auto currTotalMetre = pChart->getBarMetrePosition(bar).toDouble() + metre;

		// generate note rects and store to buffer
		// 120BPM with 1.0x HS is 2000ms (500ms/beat, green number 1200)
		int head_y_actual = c.y + c.h;
		auto it = pChart->incomingNote(_category, _index);

		if (!pChart->isLastNote(_category, _index, it))
		{
			// set hit status based on the first LN
			headExpired = false;
			tailExpired = false;
			headHit = false;
			tailHit = false;
			if (it->flags & Note::LN_TAIL)
			{
				auto itHead = it;
				itHead--;
				headExpired = itHead->expired;
				headHit = itHead->hit;
				tailExpired = it->expired;
				tailHit = it->hit;
			}
			else
			{
				auto itTail = it;
				itTail++;
				headExpired = it->expired;
				headHit = it->hit;
				if (!pChart->isLastNote(_category, _index, itTail))
				{
					tailExpired = itTail->expired;
					tailHit = itTail->hit;
				}
			}
		}

		while (!pChart->isLastNote(_category, _index, it) && head_y_actual >= 0)
		{
			int head_y = c.y + c.h;
			int tail_y = 0;

			if (it->flags & Note::LN_TAIL)
			{
				head_y_actual = c.y + c.h;

				const auto& tail = *it;
				auto tailMetreOffset = currTotalMetre - tail.pos.toDouble();
				if (tailMetreOffset >= 0)
					tail_y = (c.y + c.h); // expired notes stay on judge line, LR2 / pre RA behavior
				else
					tail_y = (c.y + c.h) - static_cast<int>(std::floor(-tailMetreOffset * _noteAreaHeight * _basespd * _hispeed));

				++it;
			}
			else
			{
				const auto& head = *it;
				++it;
				if (pChart->isLastNote(_category, _index, it)) break;

				const auto& tail = *it;

				auto headMetreOffset = currTotalMetre - head.pos.toDouble();
				head_y_actual = (c.y + c.h) - static_cast<int>(std::floor(-headMetreOffset * _noteAreaHeight * _basespd * _hispeed));
				if (head_y_actual < head_y) head_y = head_y_actual;

				auto tailMetreOffset = currTotalMetre - tail.pos.toDouble();
				if (tailMetreOffset >= 0)
					tail_y = (c.y + c.h); // expired notes stay on judge line, LR2 / pre RA behavior
				else
					tail_y = (c.y + c.h) - static_cast<int>(std::floor(-tailMetreOffset * _noteAreaHeight * _basespd * _hispeed));

				++it;
			}

			_outRect.push_front({ c.x, (float)head_y, c.w, -c.h });
			_outRectBody.push_front({ c.x, (float)tail_y, c.w, float(head_y - tail_y - c.h) });
			_outRectTail.push_front({ c.x, (float)tail_y, c.w, -c.h });
		}
	}
	else
	{
		// fetch note size, c.y + c.h = judge line pos (top-left corner), -c.h = height start drawing
		auto c = _current.rect;
		long long currTimestamp = gChartContext.started ? (t - State::get(IndexTimer::PLAY_START)).norm() : 0;

		// generate note rects and store to buffer
		// CONSTANT: generate note rects with timestamp (BPM=150)
		// 150BPM with 1.0x HS is 1600ms (400ms/beat, green number 960)
		int head_y_actual = c.y + c.h;
		auto it = pChart->incomingNote(_category, _index);

		if (!pChart->isLastNote(_category, _index, it))
		{
			// set hit status based on the first LN
			headExpired = false;
			tailExpired = false;
			headHit = false;
			tailHit = false;
			if (it->flags & Note::LN_TAIL)
			{
				auto itHead = it;
				itHead--;
				headExpired = itHead->expired;
				headHit = itHead->hit;
				tailExpired = it->expired;
				tailHit = it->hit;
			}
			else
			{
				auto itTail = it;
				itTail++;
				headExpired = it->expired;
				headHit = it->hit;
				if (!pChart->isLastNote(_category, _index, itTail))
				{
					tailExpired = itTail->expired;
					tailHit = itTail->hit;
				}
			}
		}

		while (!pChart->isLastNote(_category, _index, it) && head_y_actual >= 0)
		{
			int head_y = c.y + c.h;
			int tail_y = 0;

			if (it->flags & Note::LN_TAIL)
			{
				head_y_actual = c.y + c.h;

				const auto& tail = *it;
				auto tailTimeOffset = currTimestamp - tail.time.norm();
				tail_y = (c.y + c.h) - static_cast<int>(std::floor(-tailTimeOffset / 1600.0 * _noteAreaHeight * _basespd * _hispeed));

				++it;
			}
			else
			{
				const auto& head = *it;
				++it;
				if (pChart->isLastNote(_category, _index, it)) break;

				const auto& tail = *it;

				auto headTimeOffset = currTimestamp - head.time.norm();
				head_y_actual = (c.y + c.h) - static_cast<int>(std::floor(-headTimeOffset / 1600.0 * _noteAreaHeight * _basespd * _hispeed));
				if (head_y_actual < head_y) head_y = head_y_actual;

				auto tailTimeOffset = currTimestamp - tail.time.norm();
				tail_y = (c.y + c.h) - static_cast<int>(std::floor(-tailTimeOffset / 1600.0 * _noteAreaHeight * _basespd * _hispeed));

				++it;
			}

			_outRect.push_front({ c.x, (float)head_y, c.w, -c.h });
			_outRectBody.push_front({ c.x, (float)tail_y, c.w, float(head_y - tail_y - c.h) });
			_outRectTail.push_front({ c.x, (float)tail_y, c.w, -c.h });
		}
	}

	if (pNoteBody)
	{
		pNoteBody->update(t);
		if (pNoteBody->_animFrames > 1 && pNoteBody->_currAnimFrame == 0)
		{
			pNoteBody->_currAnimFrame = 1;
		}
		if ((headExpired && !headHit) || (tailExpired && !tailHit) || !(headHit && !tailHit))
		{
			pNoteBody->_currAnimFrame = 0;
		}
	}
}

void SpriteLaneVerticalLN::draw() const
{
	if (isHidden()) return;

	Color colorMiss = _current.color;
	colorMiss.a *= 0.5;

	// body
	if (pNoteBody && pNoteBody->_pTexture && pNoteBody->_pTexture->_loaded)
	{
		for (auto it = _outRectBody.begin(); it != _outRectBody.end(); ++it)
		{
			auto itNext = it;
			itNext++;
			bool miss = (itNext == _outRectBody.end() && ((headExpired && !headHit) || (tailExpired && !tailHit)));
			if (itNext == _outRectBody.end())
			{
				pNoteBody->_pTexture->draw(
					pNoteBody->_texRect[pNoteBody->_selectionIdx * pNoteBody->_animFrames + pNoteBody->_currAnimFrame],
					*it,
					miss ? colorMiss : _current.color,
					miss ? BlendMode::ALPHA : _current.blend,
					_current.filter,
					_current.angle,
					_current.center);
			}
			else
			{
				pNoteBody->_pTexture->draw(
					pNoteBody->_texRect[pNoteBody->_selectionIdx],
					*it,
					miss ? colorMiss : _current.color,
					miss ? BlendMode::ALPHA : _current.blend,
					_current.filter,
					_current.angle,
					_current.center);
			}
		}
	}

	// head
	if (pNote && pNote->_pTexture && pNote->_pTexture->_loaded)
	{
		for (auto it = _outRect.begin(); it != _outRect.end(); ++it)
		{
			auto itNext = it;
			itNext++;
			bool miss = (itNext == _outRect.end() && ((headExpired && !headHit) || (tailExpired && !tailHit)));
			if (itNext == _outRect.end())
			{
				pNote->_pTexture->draw(
					pNote->_texRect[pNote->_selectionIdx * pNote->_animFrames + pNote->_currAnimFrame],
					*it,
					miss ? colorMiss : _current.color,
					miss ? BlendMode::ALPHA : _current.blend,
					_current.filter,
					_current.angle,
					_current.center);
			}
			else
			{
				pNote->_pTexture->draw(
					pNote->_texRect[pNoteBody->_selectionIdx],
					*it,
					miss ? colorMiss : _current.color,
					miss ? BlendMode::ALPHA : _current.blend,
					_current.filter,
					_current.angle,
					_current.center);
			}
		}
	}

	// tail
	if (pNoteTail && pNoteTail->_pTexture && pNoteTail->_pTexture->_loaded)
	{
		for (auto it = _outRectTail.begin(); it != _outRectTail.end(); ++it)
		{
			auto itNext = it;
			itNext++;
			bool miss = (itNext == _outRectTail.end() && ((headExpired && !headHit) || (tailExpired && !tailHit)));
			if (itNext == _outRectTail.end())
			{
				pNoteTail->_pTexture->draw(
					pNoteTail->_texRect[pNoteTail->_selectionIdx * pNoteTail->_animFrames + pNoteTail->_currAnimFrame],
					*it,
					miss ? colorMiss : _current.color,
					miss ? BlendMode::ALPHA : _current.blend,
					_current.filter,
					_current.angle,
					_current.center);
			}
			else
			{
				pNoteTail->_pTexture->draw(
					pNoteTail->_texRect[pNoteBody->_selectionIdx],
					*it,
					miss ? colorMiss : _current.color,
					miss ? BlendMode::ALPHA : _current.blend,
					_current.filter,
					_current.angle,
					_current.center);
			}
		}
	}

	// HIDDEN
	if (_hiddenCompatibleDraw)
	{
		_hiddenCompatibleTexture->draw(
			RECT_FULL, _hiddenCompatibleArea,
			Color(0xffffffff), BlendMode::ALPHA, false, 0
		);
	}
}

void SpriteLaneVerticalLN::adjustAfterUpdate(int x, int y, int w, int h)
{
	for (auto& r : _outRect)
	{
		r.x += x - w;
		r.y += y - h;
		r.w += w * 2;
		r.h += h * 2;
	}
	for (auto& r : _outRectBody)
	{
		r.x += x - w;
		r.y += y;
		r.w += w * 2;
	}
	for (auto& r : _outRectTail)
	{
		r.x += x - w;
		r.y += y - h;
		r.w += w * 2;
		r.h += h * 2;
	}
}
