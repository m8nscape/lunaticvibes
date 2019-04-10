#include "sprite_lane.h"
#include <cassert>

SpriteLaneVertical::SpriteLaneVertical(pTexture texture, Rect r,
    unsigned animRows, unsigned animCols, unsigned frameTime, eTimer timer,
    bool animVerticalIndexing,
    double basespeed, double lanespeed):
    SpriteStatic(nullptr, Rect(0)), pNote(new SpriteAnimated(texture, r, animRows, animCols, frameTime, timer, animVerticalIndexing))
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
    w = pNote->_aRect.w;
    h = pNote->_aRect.h;
}

void SpriteLaneVertical::updateNoteRect(hTime t, vScroll* s)
{
    rTime rt = h2r(t);

    // refresh note sprites
    pNote->updateByTimer(rt);
	pNote->updateAnimationByTimer(rt);

    // fetch note size
    auto c = _current.rect;
    auto r = pNote->getCurrentRenderParams().rect;

    // generate note rects and store to buffer
    int y = c.h;
    _outRect.clear();
	auto measure = s->getCurrentMeasure();
	auto beat = s->getCurrentBeat();
    auto it = s->incomingNoteOfChannel(_category, _index);
    while (!s->isLastNoteOfChannel(_category, _index, it) && y >= c.y)
    {
		if (measure < it->measure)
		{
			double extraBeats = s->getCurrentMeasureBeat() - beat;
			while (++measure < it->measure) extraBeats += s->getMeasureBeat(measure);
			y = c.h - (int)std::floor((extraBeats + it++->rawBeat) * c.h * _basespd * _hispeed);
		}
		else
		{
			y = c.h - (int)std::floor((it++->rawBeat - beat) * c.h * _basespd * _hispeed);
		}
        _outRect.push_front({ c.x, c.y + y, r.w, r.h });
    }
}

void SpriteLaneVertical::draw() const 
{
    if (pNote->_pTexture->_loaded)
	{
		auto dr = pNote->_drawRect;
		for (const auto& r : _outRect)
		{
			pNote->_pTexture->_draw(dr, r, _current.angle);
		}
	}
}

