#include "sprite_lane.h"
#include <cassert>

SpriteLaneVertical::SpriteLaneVertical(pTexture texture, Rect r,
    unsigned animRows, unsigned animCols, bool animVerticalIndexing,
    double basespeed, double lanespeed, unsigned frameTime, eTimer timer):
    SpriteStatic(nullptr, r), _note(new SpriteAnimated(texture, animRows, animCols, frameTime, timer, animVerticalIndexing))
{
    _type = SpriteTypes::NOTE;
    _basespd = basespeed * lanespeed;
    _hispeed = 1.0;
}

void SpriteLaneVertical::updateNoteRect(hTime t, vScroll* s, size_t channel)
{
    rTime rt = h2r(t);

    // refresh lane sprite
    update(rt);

    // refresh notes sprites
    _note->update(rt);

    // fetch note size
    auto c = _current.rect;
    auto r = _note->getCurrentRenderParams().rect;

    // generate note rects and store to buffer
    int y = 0;
    _outRect.clear();
    auto currentRenderPos = s->getCurrentBeat();
    auto it = s->lastNoteOfChannel((NoteChannelCategory)0, (NoteChannelIndex)channel);
    while (!s->isLastNoteOfChannel((NoteChannelCategory)0, (NoteChannelIndex)channel) && y <= c.h)
    {
        int y = (it++->renderPos - currentRenderPos) * c.h * _basespd * _hispeed;
        _outRect.push_front({ c.x + r.x, c.y + r.y - y, r.w, r.h });
    }
}

void SpriteLaneVertical::draw() const 
{
    if (_note->_pTexture->_loaded)
        for (const auto& r : _outRect)
            _note->_pTexture->_draw(_note->_drawRect, r, _current.angle);
}

