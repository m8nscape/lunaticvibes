#include "skin.h"
#include "game/graphics/sprite_lane.h"
#include "game/graphics/sprite_video.h"
#include "game/scene/scene_context.h"
#include <execution>
#include <algorithm>
#include "common/utils.h"

vSkin::vSkin()
{
    _textureNameMap["Black"] = std::make_shared<TextureFull>(0x000000ff);
    _textureNameMap["White"] = std::make_shared<TextureFull>(0xffffffff);
    _textureNameMap["Error"] = std::make_shared<TextureFull>(0xff00ffff);
    _textureNameMap["STAGEFILE"] = std::shared_ptr<Texture>(&gChartContext.texStagefile, [](Texture*) {});
    _textureNameMap["BACKBMP"] = std::shared_ptr<Texture>(&gChartContext.texBackbmp, [](Texture*) {});
    _textureNameMap["BANNER"] = std::shared_ptr<Texture>(&gChartContext.texBanner, [](Texture*) {});
    _textureNameMap["THUMBNAIL"] = std::make_shared<Texture>(640, 480, Texture::PixelFormat::RGB24, true);
}

vSkin::~vSkin()
{
    if (_pEditing)
    {
        _pEditing->stopEditing(false);
        _pEditing = nullptr;
    }
}

void vSkin::update()
{
    // current beat, measure
    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        gUpdateContext.metre = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre();
        gUpdateContext.bar = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBar();
        State::set(IndexNumber::_TEST3, (int)(gUpdateContext.metre * 1000));
    }

    auto updateSpriteLambda = [](const pSprite& s)
    {
        // children are updated inside parent's update()
        if (s->hasParent()) return;

		s->update(gUpdateContext.updateTime);
    };

#ifdef _DEBUG
	std::for_each(std::execution::par_unseq, _sprites.begin(), _sprites.end(), updateSpriteLambda);
#else
    std::for_each(std::execution::par_unseq, _sprites.begin(), _sprites.end(), updateSpriteLambda);
#endif

    for (auto& s : _sprites)
    {
        if (auto pText = std::dynamic_pointer_cast<SpriteText>(s); pText != nullptr)
        {
            pText->updateText();
        }
    }

}

void vSkin::update_mouse(int x, int y)
{
    if (!_handleMouseEvents)
    {
        x = -99999999;
        y = -99999999;  // LUL
    }

    auto clickSpriteLambda = [x, y](const pSprite& s)
    {
        if (!s->isHidden())
        {
            auto pS = std::dynamic_pointer_cast<iSpriteMouse>(s);
            if (pS != nullptr)
            {
                pS->OnMouseMove(x, y);
            }
        }
    };

#ifdef _DEBUG
        std::for_each(std::execution::par_unseq, _sprites.begin(), _sprites.end(), clickSpriteLambda);
#else
        std::for_each(std::execution::par_unseq, _sprites.begin(), _sprites.end(), clickSpriteLambda);
#endif
}

void vSkin::update_mouse_click(int x, int y)
{
    if (!_handleMouseEvents) return;

    // sprite inserted last has priority
    bool invoked = false;
    bool invokedText = false;
    _pLastClick = nullptr;
    for (auto it = _sprites.rbegin(); it != _sprites.rend() && !invoked; ++it)
    {
        if (!(*it)->isHidden())
        {
            auto pS = std::dynamic_pointer_cast<iSpriteMouse>(*it);
            if (pS != nullptr)
            {
                if (pS->OnClick(x, y))
                {
                    if (std::dynamic_pointer_cast<SpriteText>(*it))
                    {
                        if (_pEditing)
                        {
                            _pEditing->stopEditing(false);
                        }
                        _pEditing = std::reinterpret_pointer_cast<SpriteText>(*it);
                    }
                    invoked = true;
                    _pDragging = pS;
                    _pLastClick = pS;
                }
            }
        }
    }
}

void vSkin::update_mouse_drag(int x, int y)
{
    if (!_handleMouseEvents) return;

    if (_pDragging != nullptr)
    {
        _pDragging->OnDrag(x, y);
    }
}

void vSkin::update_mouse_release()
{
    _pDragging = nullptr;
}

void vSkin::draw() const
{
    for (auto& s : _sprites)
        s->draw();
}

void vSkin::startSpriteVideoPlayback()
{
#ifndef VIDEO_DISABLED
    for (auto& p : _sprites)
    {
        if (p->type() == SpriteTypes::VIDEO)
        {
            auto v = std::reinterpret_pointer_cast<SpriteVideo>(p);
            v->startPlaying();
        }
    }
#endif
}

void vSkin::stopSpriteVideoPlayback()
{
#ifndef VIDEO_DISABLED
    for (auto& p : _sprites)
    {
        if (p->type() == SpriteTypes::VIDEO)
        {
            auto v = std::reinterpret_pointer_cast<SpriteVideo>(p);
            v->stopPlaying();
        }
    }
#endif
}

bool vSkin::textEditSpriteClicked() const
{
    return _pEditing != nullptr && _pEditing == _pLastClick;
}

IndexText vSkin::textEditType() const
{
    return _pEditing ? _pEditing->getInd() : IndexText::INVALID;
}

void vSkin::startTextEdit(bool clear)
{
    if (_pEditing)
    {
        _pEditing->startEditing(clear);
    }
}

void vSkin::stopTextEdit(bool modify)
{
    if (_pEditing)
    {
        _pEditing->stopEditing(modify);
        _pEditing = nullptr;
    }
}

pTexture vSkin::getTextureCustomizeThumbnail()
{
    return _textureNameMap["THUMBNAIL"];
}