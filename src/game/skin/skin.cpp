#include "skin.h"
#include "game/graphics/sprite_lane.h"
#include "game/graphics/sprite_video.h"
#include "game/scene/scene_context.h"
#include <execution>
#include <algorithm>
#include "common/utils.h"

std::map<std::string, std::shared_ptr<Texture>> SkinBase::preDefinedTextures;
std::map<std::string, std::shared_ptr<Texture>> SkinBase::textureNameMap;

SkinBase::SkinBase()
{
    _version = SkinVersion::UNDEF;
    if (preDefinedTextures.empty())
    {
        preDefinedTextures["Black"] = std::make_shared<TextureFull>(0x000000ff);
        preDefinedTextures["White"] = std::make_shared<TextureFull>(0xffffffff);
        preDefinedTextures["Error"] = std::make_shared<TextureFull>(0xff00ffff);
        preDefinedTextures["STAGEFILE"] = std::shared_ptr<Texture>(&gChartContext.texStagefile, [](Texture*) {});
        preDefinedTextures["BACKBMP"] = std::shared_ptr<Texture>(&gChartContext.texBackbmp, [](Texture*) {});
        preDefinedTextures["BANNER"] = std::shared_ptr<Texture>(&gChartContext.texBanner, [](Texture*) {});
        preDefinedTextures["THUMBNAIL"] = std::make_shared<Texture>(1920, 1080, Texture::PixelFormat::RGB24, true);
    }
    for (auto& [key, texture] : preDefinedTextures)
    {
        textureNameMap[key] = texture;
    }
}

SkinBase::~SkinBase()
{
    if (pSpriteTextEditing)
    {
        pSpriteTextEditing->stopEditing(false);
        pSpriteTextEditing = nullptr;
    }
}

void SkinBase::update()
{
    // current beat, measure
    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        gUpdateContext.metre = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre();
        gUpdateContext.bar = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBar();
        State::set(IndexNumber::_TEST3, (int)(gUpdateContext.metre * 1000));
    }

    auto updateSpriteLambda = [](const std::shared_ptr<SpriteBase>& s)
    {
        // reset
        s->_draw = false;

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

void SkinBase::update_mouse(int x, int y)
{
    if (!handleMouseEvents)
    {
        x = -99999999;
        y = -99999999;  // LUL
    }

    auto clickSpriteLambda = [x, y](const std::shared_ptr<SpriteBase>& s)
    {
        if (s->isDraw() && !s->isHidden())
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

void SkinBase::update_mouse_click(int x, int y)
{
    if (!handleMouseEvents) return;

    // sprite inserted last has priority
    bool invoked = false;
    bool invokedText = false;
    pSpriteLastClicked = nullptr;
#if _DEBUG
    for (auto it = _sprites.rbegin(); it != _sprites.rend() && !invoked; ++it)
    {
        if ((*it)->type() != SpriteTypes::MOUSE_CURSOR && (*it)->isDraw() && !(*it)->isHidden())
        {
            const RectF& rc = (*it)->_current.rect;
            if (x >= rc.x && y >= rc.y && x < rc.x + rc.w && y < rc.y + rc.h)
            {
                createNotification((boost::format("Clicked sprite #%d (%d,%d)[%dx%d] (Line:%d)") %
                    (int)std::distance(it, _sprites.rend()) %
                    (*it)->_current.rect.x % (*it)->_current.rect.y % (*it)->_current.rect.w % (*it)->_current.rect.h % (*it)->srcLine).str());
                break;
            }
        }
    }
#endif
    for (auto it = _sprites.rbegin(); it != _sprites.rend() && !invoked; ++it)
    {
        if ((*it)->isDraw() && !(*it)->isHidden())
        {
            auto pS = std::dynamic_pointer_cast<iSpriteMouse>(*it);
            if (pS != nullptr)
            {
                if (pS->OnClick(x, y))
                {
                    if (std::dynamic_pointer_cast<SpriteText>(*it))
                    {
                        if (pSpriteTextEditing)
                        {
                            pSpriteTextEditing->stopEditing(false);
                        }
                        pSpriteTextEditing = std::reinterpret_pointer_cast<SpriteText>(*it);
                    }
                    invoked = true;
                    pSpriteDragging = pS;
                    pSpriteLastClicked = pS;
                }
            }
        }
    }
}

void SkinBase::update_mouse_drag(int x, int y)
{
    if (!handleMouseEvents) return;

    if (pSpriteDragging != nullptr)
    {
        pSpriteDragging->OnDrag(x, y);
    }
}

void SkinBase::update_mouse_release()
{
    pSpriteDragging = nullptr;
}

void SkinBase::draw() const
{
    for (auto& s : _sprites)
        s->draw();
}

void SkinBase::startSpriteVideoPlayback()
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

void SkinBase::stopSpriteVideoPlayback()
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

bool SkinBase::textEditSpriteClicked() const
{
    return pSpriteTextEditing != nullptr && pSpriteTextEditing == pSpriteLastClicked;
}

IndexText SkinBase::textEditType() const
{
    return pSpriteTextEditing ? pSpriteTextEditing->getInd() : IndexText::INVALID;
}

void SkinBase::startTextEdit(bool clear)
{
    if (pSpriteTextEditing)
    {
        pSpriteTextEditing->startEditing(clear);
    }
}

void SkinBase::stopTextEdit(bool modify)
{
    if (pSpriteTextEditing)
    {
        pSpriteTextEditing->stopEditing(modify);
        pSpriteTextEditing = nullptr;
    }
}

std::shared_ptr<Texture> SkinBase::getTextureCustomizeThumbnail()
{
    return textureNameMap["THUMBNAIL"];
}