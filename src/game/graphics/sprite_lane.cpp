#include "sprite_lane.h"
#include <cassert>

SpriteLaneVertical::SpriteLaneVertical(pTexture texture, Rect r,
    unsigned animFrames, unsigned frameTime, eTimer timer,
    bool animVerticalIndexing,
    double basespeed, double lanespeed):
    SpriteStatic(nullptr, Rect(0)), pNote(new SpriteAnimated(texture, r, animFrames, frameTime, timer, animVerticalIndexing))
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
		return true;
	}
	return false;
}

void SpriteLaneVertical::updateNoteRect(timestamp t, vScroll* s)
{
    // refresh note sprites
	pNote->update(t);

    // fetch note size
    auto c = _current.rect;
    auto r = pNote->getCurrentRenderParams().rect;

    // generate note rects and store to buffer
    int y = c.h;
    _outRect.clear();
	//auto measure = s->getCurrentMeasure();
	auto beat = s->getCurrentTotalBeats();
    auto it = s->incomingNoteOfChannel(_category, _index);
    while (!s->isLastNoteOfChannel(_category, _index, it) && y >= c.y)
    {
		/*
		if (measure < it->measure)
		{
			double extraBeats = s->getMeasureBeat(measure) - beat;  // fill gap within current measure
			while (measure + 1 < it->measure)                       // fill gap between measures
			{
				++measure;
				extraBeats += s->getMeasureBeat(measure);
			}
			y = c.h - (int)std::floor((extraBeats + it++->rawBeat) * c.h * _basespd * _hispeed);
		}
		else
		{
			//y = c.h - (int)std::floor((it++->rawBeat - beat) * c.h * _basespd * _hispeed);
			y = -999;
		}
		*/

		y = c.h - (int)std::floor((it++->rawBeat - beat) * c.h * _basespd * _hispeed);
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
			pNote->_pTexture->_draw(dr, r, _current.color, _current.blend, _current.filter, _current.angle);
		}
	}
}

