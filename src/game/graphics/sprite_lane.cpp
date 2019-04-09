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
	gNumbers.set(eNumber::_TEST2, (int)measure);
	auto beat = s->getCurrentBeat();
    auto it = s->incomingNoteOfChannel(_category, _index);
    while (!s->isLastNoteOfChannel(_category, _index, it) && y >= c.y)
    {
		if (measure < it->measure)
		{
			double extraBeats = s->getCurrentMeasureBeat() - beat;
			while (measure < it->measure) extraBeats += s->getMeasureBeat(measure++);
			y = c.h - (int)std::floor((extraBeats + it++->rawBeat) * c.h * _basespd * _hispeed);
		}
		else
		{
			y = c.h - (int)std::floor((it++->rawBeat - beat) * c.h * _basespd * _hispeed);
		}
		gNumbers.set(eNumber::_TEST1, y);
		gNumbers.set(eNumber::_TEST3, beat * 1000);
        _outRect.push_front({ c.x + r.x, c.y + r.y - y, r.w, r.h });
    }
}

void SpriteLaneVertical::draw() const 
{
    if (pNote->_pTexture->_loaded)
	{
		auto dr = pNote->_drawRect;
		for (const auto& r : _outRect)
		{
#if _DEBUG
			char buf[256];
			sprintf(buf, "texture[%dx%d, %dx%d] output[%dx%d, %dx%d]", dr.x, dr.y, dr.w, dr.h, r.x, r.y, r.w, r.h);
			//LOG_DEBUG << "[Skin] Lane rect: " << buf;
#endif
			pNote->_pTexture->_draw(dr, r, _current.angle);
		}
	}
}

