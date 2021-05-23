#include "skin.h"
#include "game/graphics/sprite_lane.h"
#include "game/graphics/sprite_video.h"
#include "game/scene/scene_context.h"
#include <execution>
#include <algorithm>
#include "utils.h"

vSkin::vSkin()
{
    _textureNameMap["Black"] = std::make_shared<TextureFull>(0x000000ff);
    _textureNameMap["White"] = std::make_shared<TextureFull>(0xffffffff);
    _textureNameMap["Error"] = std::make_shared<TextureFull>(0xff00ffff);
}

void vSkin::update()
{
	Time t;
    double beat;
    unsigned measure;

    // current beat, measure
    if (gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr)
    {
        beat = gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBeat();
        measure = gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentMeasure();
        gNumbers.set(eNumber::_TEST3, (int)(beat * 1000));
    }

    auto updateSpriteLambda = [&t, beat, measure](const pSprite& s)
    {
		switch (s->type())
		{
        case SpriteTypes::GLOBAL:
        {
            auto ps = std::dynamic_pointer_cast<SpriteGlobal>(s);
            if (gSprites[ps->getIdx()]) ps->set(gSprites[ps->getIdx()]);
            ps->update(t);
            break;
        }
		case SpriteTypes::NOTE_VERT:
		{
            auto ps = std::dynamic_pointer_cast<SpriteLaneVertical>(s);
            if (gPlayContext.chartObj[ps->playerSlot] != nullptr && gChartContext.started)
			{
				ps->update(t);
				ps->updateNoteRect(t, &*gPlayContext.chartObj[ps->playerSlot], beat, measure);
			}
			break;
		}
		default:
			s->update(t);
			break;
		}
    };

#ifdef _DEBUG
	std::for_each(std::execution::seq, _sprites_parent.begin(), _sprites_parent.end(), updateSpriteLambda);
    std::for_each(std::execution::seq, _sprites_child.begin(), _sprites_child.end(), updateSpriteLambda);
#else
    std::for_each(std::execution::par, _sprites_parent.begin(), _sprites_parent.end(), updateSpriteLambda);
    std::for_each(std::execution::par, _sprites_child.begin(), _sprites_child.end(), updateSpriteLambda);
#endif
}

void vSkin::update_mouse(int x, int y)
{
    auto clickSpriteLambda = [x, y](const pSprite& s)
    {
        switch (s->type())
        {
        case SpriteTypes::ONMOUSE:
        {
            auto ps = std::dynamic_pointer_cast<SpriteOnMouse>(s);
            ps->checkMouseArea(x, y);
            break;
        }
        case SpriteTypes::MOUSE_CURSOR:
        {
            auto ps = std::dynamic_pointer_cast<SpriteCursor>(s);
            ps->moveToPos(x, y);
            break;
        }
        default:
            break;
        }
    };

#ifdef _DEBUG
        std::for_each(std::execution::seq, _sprites.begin(), _sprites.end(), clickSpriteLambda);
#else
        std::for_each(std::execution::par, _sprites.begin(), _sprites.end(), clickSpriteLambda);
#endif
}

void vSkin::update_mouse_click(int x, int y)
{
    bool invoked = false;
    for (auto it = _sprites.rend(); it != _sprites.rbegin() && !invoked;)
    {
        --it;
        auto s = *it;
        switch (s->type())
        {
        case SpriteTypes::BUTTON:
        {
            auto ps = std::dynamic_pointer_cast<SpriteButton>(s);
            if (ps->doIfInRange(x, y))
                invoked = true;
            break;
        }
        default:
            break;
        }
    }

}

void vSkin::draw() const
{
    for (auto& s : _sprites)
        s->draw();
}