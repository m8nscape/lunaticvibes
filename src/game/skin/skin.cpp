#include "skin.h"
#include "game/graphics/sprite_lane.h"
#include "game/scene/scene_context.h"
#include <execution>
#include <algorithm>

vSkin::vSkin()
{
    _texNameMap["Black"] = std::make_shared<TextureFull>(0x000000ff);
    _texNameMap["White"] = std::make_shared<TextureFull>(0xffffffff);
    _texNameMap["Error"] = std::make_shared<TextureFull>(0xff00ffff);
}

void vSkin::update()
{
    hTime ht = getHighresTimePoint();
    rTime t = h2r(ht);
    std::for_each(std::execution::par_unseq, _sprites.begin(), _sprites.end(), [&t, &ht](const auto& s)
    {
        switch (s->type())
        {
        case SpriteTypes::ANIMATED:
        {
            auto& ref = (std::shared_ptr<SpriteAnimated>&)s;
            ref->updateByTimer(t);
            //ref->updateSplitByTimer(t);
            ref->updateAnimationByTimer(t);
            break;
        }
        case SpriteTypes::NUMBER:
        {
            auto& ref = (std::shared_ptr<SpriteNumber>&)s;
            ref->updateByTimer(t);
            //ref->updateSplitByTimer(t);
            ref->updateAnimationByTimer(t);
            //ref->updateRectsByTimer(t);
            ref->updateNumberByInd();
            break;
        }
        case SpriteTypes::NOTE_VERT:
        {
            auto& ref = (std::shared_ptr<SpriteLaneVertical>&)s;
			if (ref->haveDst() && context_chart.scrollObj != nullptr && context_chart.started)
			{
				ref->update(t);
				ref->updateNoteRect(ht, &*context_chart.scrollObj);
			}
            break;
        }
        default:
            break;
        }
    });
}

void vSkin::draw() const
{
    for (auto& s : _sprites)
        s->draw();
}