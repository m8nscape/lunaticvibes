#include "sprite_imagetext.h"

#include "common/encoding.h"

SpriteImageText::SpriteImageText(std::vector<pTexture>& textures, CharMappingList& chrList, eText textInd, TextAlign align, unsigned height, int margin):
    SpriteText(nullptr, textInd, align), _textures(textures), _chrList(chrList), _height(height), _margin(margin)
{
    _type = SpriteTypes::IMAGE_TEXT;
}

void SpriteImageText::setInputBindingText(std::string&& text)
{
    if (text.empty())
    {
        _draw = false;
        return;
    }

    _currText = text;

    /*
    // convert UTF-8 to SHIFT-JIS
    std::u16string sjisText = utf8_to_sjis(_currText)

    // save characters
    int x_margin = 0;
    for (auto c : sjisText)
    {
        if (_chrList.find(c) != _chrList.end())
        {
            _drawList.push_back({c, });
        }
    }
    */

    // convert UTF-8 to UTF-32
    std::u32string u32Text = utf8_to_utf32(_currText);

    // save characters
    int x = 0;
    int w = 0, h = 0;
    _drawListOrig.clear();
    for (auto c : u32Text)
    {
        if (_chrList.find(c) != _chrList.end() && _chrList[c].textureIdx < _textures.size())
        {
            auto r = _chrList[c].textureRect;
            _drawListOrig.push_back({ c, {x, 0, r.w, r.h} });
            w = x + r.w;
            x += r.w + _margin;
            h = std::max(h, r.h);
        }
    }
    //_drawList = _drawListOrig;
    _drawRect = { 0, 0, w, h };
}

void SpriteImageText::updateTextRect()
{
    _drawList = _drawListOrig;

    // size
    double sizeFactor = 1.0;
    if (_current.rect.h != _drawRect.h)
    {
        sizeFactor = (double)_current.rect.h / _drawRect.h;
        for (auto& [c, r] : _drawList)
        {
            r.x *= sizeFactor;
            r.w *= sizeFactor;
            r.h *= sizeFactor;
        }
    }

    // shrink
    int text_w = static_cast<int>(std::round(_drawRect.w * sizeFactor));
    if (text_w > _current.rect.w)
    {
        double widthFactor = (double)_current.rect.w / text_w;
        for (auto& [c, r] : _drawList)
        {
            r.x *= widthFactor;
            r.w *= widthFactor;
        }
        text_w = _current.rect.w;
    }

    // align
    switch (_align)
    {
    case TEXT_ALIGN_LEFT:
        break;
    case TEXT_ALIGN_CENTER:
        for (auto& [c, r] : _drawList) r.x -= text_w / 2;
        break;
    case TEXT_ALIGN_RIGHT:
        for (auto& [c, r] : _drawList) r.x -= text_w;
        break;
    }

    // move
    for (auto& [c, r] : _drawList)
    {
        r.x += _current.rect.x;
        r.y += _current.rect.y;
    }

    /*
    if (_haveParent && !_parent.expired())
    {
        auto parent = _parent.lock();
        auto r = parent->getCurrentRenderParams().rect;
        if (r.w == -1 && r.h == -1)
        {
            _current.rect.x = 0;
            _current.rect.y = 0;
        }
        else
        {
            _current.rect.x += parent->getCurrentRenderParams().rect.x;
            _current.rect.y += parent->getCurrentRenderParams().rect.y;
        }
    }
    */

}

bool SpriteImageText::update(const Time& t)
{
    if (_draw = updateByKeyframes(t))
    {
        setInputBindingText(gTexts.get(_textInd));
        if (_draw) updateTextRect();
    }
    return _draw;
}

void SpriteImageText::draw() const
{
    if (!_hide && _draw)
    {
        for (auto [c, r] : _drawList)
        {
            size_t idx = _chrList.at(c).textureIdx;
            _textures[idx]->draw(_chrList.at(c).textureRect, r, _current.color, _current.blend, _current.filter, _current.angle);
        }
    }
}
