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
    if (gPlayContext.chartObj[gPlayContext.playerSlot] != nullptr)
    {
        beat = gPlayContext.chartObj[gPlayContext.playerSlot]->getCurrentBeat();
        measure = gPlayContext.chartObj[gPlayContext.playerSlot]->getCurrentMeasure();
        gNumbers.set(eNumber::_TEST3, (int)(beat * 1000));
    }

    auto updateSpriteLambda = [&t, beat, measure](const auto& s)
    {
		switch (s->type())
		{
        case SpriteTypes::GLOBAL:
        {
            auto ref = std::reinterpret_pointer_cast<SpriteGlobal>(s);
            if (gSprites[ref->getIdx()]) ref->set(gSprites[ref->getIdx()]);
            ref->update(t);
            break;
        }
		case SpriteTypes::NOTE_VERT:
		{
            auto ref = std::reinterpret_pointer_cast<SpriteLaneVertical>(s);
            if (gPlayContext.chartObj[ref->playerSlot] != nullptr && gChartContext.started)
			{
				ref->update(t);
				ref->updateNoteRect(t, &*gPlayContext.chartObj[ref->playerSlot], beat, measure);
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

void vSkin::draw() const
{
    for (auto& s : _sprites)
        s->draw();
}