#include "sprite_lane.h"
#include "game/data/number.h"
#include <cassert>

SpriteLaneVertical::SpriteLaneVertical(pTexture texture, Rect r,
    unsigned animFrames, unsigned frameTime, eTimer timer,
    bool animVerticalIndexing,
    unsigned player, double basespeed, double lanespeed):
    SpriteStatic(nullptr, Rect(0)), pNote(new SpriteAnimated(texture, r, animFrames, frameTime, timer, animVerticalIndexing)), _playerSlot(player)
{
    _type = SpriteTypes::NOTE_VERT;
    _basespd = basespeed * lanespeed;
    _hispeed = 1.0;
	_category = NoteLaneCategory::_;
	_index = NoteLaneIndex::_;
}


void SpriteLaneVertical::setLane(NoteLaneCategory cat, NoteLaneIndex idx)
{
	_category = cat;
	_index = idx;
	_haveDst = true;
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

bool SpriteLaneVertical::update(Time t)
{
	if (updateByKeyframes(t))
	{
        _hispeed = double(gNumbers.get(eNumber::HS_1P)) / 100.0;
		return true;
	}
	return false;
}

void SpriteLaneVertical::updateNoteRect(Time t, vChart* s, double beat, unsigned measure)
{
    // refresh note sprites
	pNote->update(t);

    // fetch note size, c.h = whole lane height, c.y = height start drawing
    auto c = _current.rect;
    auto r = pNote->getCurrentRenderParams().rect;
    auto currTotalBeat = s->getBarBeatstamp(measure) + beat;
    gNumbers.set(eNumber::_TEST5, (int)(currTotalBeat * 100.0));

    // generate note rects and store to buffer
	// 150BPM with 1.0x HS is 1600ms
    int y = c.h;
    _outRect.clear();
    auto it = s->incomingNoteOfLane(_category, _index);
    while (!s->isLastNoteOfLane(_category, _index, it) && y >= c.y)
    {
		auto noteBeatOffset = currTotalBeat - it->totalbeat;
        if (noteBeatOffset >= 0)
			y = (c.y + c.h); // expired notes stay on judge line, LR2 / pre RA behavior
        else
            y = (c.y + c.h) - static_cast<int>( std::floor((-noteBeatOffset * 4 / 4) * c.h * _basespd * _hispeed) );
        it++;
        _outRect.push_front({ c.x, y, r.w, r.h });
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

