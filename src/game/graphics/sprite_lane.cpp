#include "sprite_lane.h"
#include "game/data/number.h"
#include "game/scene/scene_context.h"
#include <cassert>

SpriteLaneVertical::SpriteLaneVertical(unsigned player, double basespeed, double lanespeed):
	SpriteStatic(nullptr, Rect(0)), playerSlot(player)
{
	_type = SpriteTypes::NOTE_VERT;
	_basespd = basespeed * lanespeed;
	_hispeed = 1.0;
	_category = NoteLaneCategory::_;
	_index = NoteLaneIndex::_;
}

SpriteLaneVertical::SpriteLaneVertical(pTexture texture, Rect r,
    unsigned animFrames, unsigned frameTime, eTimer timer,
	unsigned animRows, unsigned animCols, bool animVerticalIndexing,
    unsigned player, double basespeed, double lanespeed):
	SpriteLaneVertical(player, basespeed, lanespeed)
{
	pNote = std::make_shared<SpriteAnimated>(texture, r, animFrames, frameTime, timer, animRows, animCols, animVerticalIndexing);
}


void SpriteLaneVertical::setLane(NoteLaneCategory cat, NoteLaneIndex idx)
{
	_category = cat;
	_index = idx;
}

void SpriteLaneVertical::getRectSize(int& w, int& h)
{
	if (pNote->_texRect.empty())
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
	if (updateByKeyframes(t))
	{
		switch (playerSlot)
		{
		case 0:
			_hispeed = double(gNumbers.get(eNumber::HS_1P)) / 100.0;
			break;
		case 1:
			_hispeed = double(gNumbers.get(eNumber::HS_2P)) / 100.0;
			break;
		default:
			break;
		}
		updateNoteRect(t);
		return true;
	}
	return false;
}

void SpriteLaneVertical::updateNoteRect(const Time& t)
{
	_outRect.clear();
	auto pChart = gPlayContext.chartObj[playerSlot];
	if (pChart != nullptr && gChartContext.started)
	{
		return;
	}
	auto beat = gUpdateContext.beat;
	auto measure = gUpdateContext.measure;

    // refresh note sprites
	pNote->update(t);

    // fetch note size, c.y + c.h = judge line pos (top-left corner), -c.h = height start drawing
    auto c = _current.rect;
    auto currTotalBeat = pChart->getBarBeatstamp(beat) + measure;

    // generate note rects and store to buffer
	// 150BPM with 1.0x HS is 1600ms
    int y = (c.y + c.h);
    auto it = pChart->incomingNote(_category, _index);
    while (!pChart->isLastNote(_category, _index, it) && y >= -c.h)
    {
		auto noteBeatOffset = currTotalBeat - it->totalbeat;
        if (noteBeatOffset >= 0)
			y = (c.y + c.h); // expired notes stay on judge line, LR2 / pre RA behavior
        else
            y = (c.y + c.h) - static_cast<int>( std::floor((-noteBeatOffset * 4 / 4) * (c.y + c.h) * _basespd * _hispeed) );
        it++;
        _outRect.push_front({ c.x, y, c.w, -c.h });
    }
}

void SpriteLaneVertical::draw() const 
{
    if (pNote->_pTexture && pNote->_pTexture->_loaded)
	{
		for (const auto& r : _outRect)
		{
			pNote->_pTexture->draw(
				pNote->_texRect[pNote->_selectionIdx], 
				r,
				_current.color,
				_current.blend, 
				_current.filter,
				_current.angle,
				_current.center);
		}
	}
}


void SpriteLaneVerticalLN::updateNoteRect(const Time& t)
{
	_outRect.clear();
	_outRectBody.clear();
	_outRectTail.clear();

	auto pChart = gPlayContext.chartObj[playerSlot];
	if (pChart != nullptr && gChartContext.started)
	{
		return;
	}
	auto beat = gUpdateContext.beat;
	auto measure = gUpdateContext.measure;

	// refresh note sprites
	pNote->update(t);

	// fetch note size, c.y + c.h = judge line pos (top-left corner), -c.h = height start drawing
	auto c = _current.rect;
	auto currTotalBeat = pChart->getBarBeatstamp(measure) + beat;

	// generate note rects and store to buffer
	// 150BPM with 1.0x HS is 1600ms
	int head_y_actual = c.y + c.h;
	auto it = pChart->incomingNote(_category, _index);
	while (!pChart->isLastNote(_category, _index, it) && head_y_actual >= -c.h)
	{
		int head_y = c.y + c.h;
		int tail_y = 0;

		if (it->flags & Note::LN_TAIL)
		{
			head_y_actual = c.y + c.h;

			const auto& tail = *it;
			auto tailBeatOffset = currTotalBeat - tail.totalbeat;
			tail_y = (c.y + c.h) - static_cast<int>(std::floor((-tailBeatOffset * 4 / 4) * (c.y + c.h) * _basespd * _hispeed));

			++it;
		}
		else
		{
			const auto& head = *it;
			++it;
			if (pChart->isLastNote(_category, _index, it)) break;

			const auto& tail = *it;

			auto headBeatOffset = currTotalBeat - head.totalbeat;
			head_y_actual = (c.y + c.h) - static_cast<int>(std::floor((-headBeatOffset * 4 / 4) * (c.y + c.h) * _basespd * _hispeed));
			if (head_y_actual < head_y) head_y = head_y_actual;

			auto tailBeatOffset = currTotalBeat - tail.totalbeat;
			tail_y = (c.y + c.h) - static_cast<int>(std::floor((-tailBeatOffset * 4 / 4) * (c.y + c.h) * _basespd * _hispeed));

			++it;
		}

		_outRect.push_front({ c.x, head_y, c.w, -c.h });
		_outRectBody.push_front({ c.x, tail_y, c.w, head_y - tail_y - c.h });
		_outRectTail.push_front({ c.x, tail_y, c.w, -c.h });
	}
}

void SpriteLaneVerticalLN::draw() const
{
	// body
	if (pNoteBody->_pTexture && pNoteBody->_pTexture->_loaded)
	{
		for (const auto& r : _outRectBody)
		{
			pNoteBody->_pTexture->draw(
				pNoteBody->_texRect[pNoteBody->_selectionIdx],
				r,
				_current.color,
				_current.blend,
				_current.filter,
				_current.angle,
				_current.center);
		}
	}

	// head
	SpriteLaneVertical::draw();

	// tail
	if (pNoteTail->_pTexture && pNoteTail->_pTexture->_loaded)
	{
		for (const auto& r : _outRectTail)
		{
			pNoteTail->_pTexture->draw(
				pNoteTail->_texRect[pNoteTail->_selectionIdx],
				r,
				_current.color,
				_current.blend,
				_current.filter,
				_current.angle,
				_current.center);
		}
	}
}
