#include "skin.h"

vSkin::vSkin()
{
    _texNameMap["Black"] = std::make_shared<TextureFull>(0x000000ff);
    _texNameMap["White"] = std::make_shared<TextureFull>(0xffffffff);
    _texNameMap["Error"] = std::make_shared<TextureFull>(0xff00ffff);
}

void vSkin::update()
{
    rTime t = getTimePoint();
    for (auto& s : _sprites)
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
    }
}

void vSkin::draw() const
{
    for (auto& s : _sprites)
        s->draw();
}