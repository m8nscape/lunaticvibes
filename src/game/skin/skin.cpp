#include "skin.h"
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
    rTime t = getTimePoint();
    std::for_each(std::execution::par_unseq, _sprites.begin(), _sprites.end(), [&t](const auto& s)
    {
        switch (s->type())
        {
        case SpriteTypes::ANIMATED:
        {
            auto& ref = (std::shared_ptr<SpriteAnimated>&)s;
            ref->updateByTimer(t);
            ref->updateSplitByTimer(t);
            ref->updateAnimationByTimer(t);
            break;
        }
        case SpriteTypes::NUMBER:
        {
            auto& ref = (std::shared_ptr<SpriteNumber>&)s;
            ref->updateByTimer(t);
            ref->updateSplitByTimer(t);
            ref->updateAnimationByTimer(t);
            ref->updateRectsByTimer(t);
            ref->updateNumberByInd();
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