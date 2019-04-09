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

    // refresh lane sprite
    update(rt);

    // refresh note sprites
    pNote->update(rt);

    // fetch note size
    auto c = _current.rect;
    auto r = pNote->getCurrentRenderParams().rect;

    // generate note rects and store to buffer
    int y = 0;
    _outRect.clear();
    auto currentRenderPos = s->getCurrentBeat();
    auto it = s->incomingNoteOfChannel(_category, _index);
    while (!s->isLastNoteOfChannel(_category, _index, it) && y <= c.h)
    {
        y = (int)std::floor((it++->renderPos - currentRenderPos) * c.h * _basespd * _hispeed);
        _outRect.push_front({ c.x + r.x, c.y + r.y - y, r.w, r.h });
    }
}

void SpriteLaneVertical::draw() const 
{
    if (pNote->_pTexture->_loaded)
		for (const auto& r : _outRect)
		{
#if _DEBUG
			char buf[256];
			auto dr = pNote->_drawRect;
			sprintf(buf, "texture[%dx%d, %dx%d] output[%dx%d, %dx%d]", dr.x, dr.y, dr.w, dr.h, r.x, r.y, r.w, r.h);
			LOG_DEBUG << "[Skin] Lane rect: " << buf;
#endif
			pNote->_pTexture->_draw(pNote->_drawRect, r, _current.angle);
		}
}

