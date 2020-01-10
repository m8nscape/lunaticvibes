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
}


void SpriteLaneVertical::setChannel(NoteChannelCategory cat, NoteChannelIndex idx)
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

bool SpriteLaneVertical::update(timestamp t)
{
	if (updateByKeyframes(t))
	{
        _hispeed = double(gNumbers.get(eNumber::HS_1P)) / 100.0;
		return true;
	}
	return false;
}

void SpriteLaneVertical::updateNoteRect(timestamp t, vScroll* s, double beat, unsigned measure)
{
    // refresh note sprites
	pNote->update(t);

    // fetch note size, c.h = whole lane height, c.y = height start drawing -c.y = note height
    auto c = _current.rect;
    auto r = pNote->getCurrentRenderParams().rect;
    auto currTotalBeat = s->getMeasureTotalBeats(measure) + beat;
    gNumbers.set(eNumber::_TEST5, (int)(currTotalBeat * 100.0));

    // generate note rects and store to buffer
    int y = c.h;
    _outRect.clear();
    auto it = s->incomingNoteOfChannel(_category, _index);
    while (!s->isLastNoteOfChannel(_category, _index, it) && y >= c.y)
    {
        if (currTotalBeat >= it->totalbeat)
            y = c.h;
        else
            y = c.h - (int)std::floor((it->totalbeat - currTotalBeat) * (c.h - c.y) * _basespd * _hispeed);
        it++;
        _outRect.push_front({ c.x, y, r.w, r.h });
    }
}

void SpriteLaneVertical::draw() const 
{
    if (pNote->_pTexture->_loaded)
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

