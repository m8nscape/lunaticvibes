#include "sprite_bar_entry.h"
#include <plog/Log.h>
#include "game/scene/scene_context.h"

int SpriteBarEntry::setBody(BarType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarType::TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarBody type (" << int(type) << "Invalid!"
            << " (Line " << __line << ")";
        return 1;
    }

    auto idx = static_cast<size_t>(type);
    sBodyOff[idx] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sBodyOff[idx]->setParent(weak_from_this());
    sBodyOn[idx] = sBodyOff[idx];
    return 0;
}

int SpriteBarEntry::setFlash(pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    sFlash = std::make_shared<SpriteAnimated>(texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sFlash->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setLevel(BarLevelType type, pTexture texture, const Rect& rect, NumberAlign align, unsigned digits,
    unsigned numRows, unsigned numCols, unsigned frameTime, eTimer animtimer,
    unsigned animFrames, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLevelType::LEVEL_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry level type (" << int(type) << "Invalid!"
            << " (Line " << __line << ")";
        return 1;
    }

    if (digits < 3 && type == BarLevelType::IRRANK)
        LOG_WARNING << "[Sprite] BarEntry level digit (" << digits << ") not enough for idx " << int(type)
            << " (Line " << __line << ")";
    else if (digits < 2)
        LOG_WARNING << "[Sprite] BarEntry level digit (" << digits << ") not enough for idx " << int(type)
            << " (Line " << __line << ")";

    sLevel[static_cast<size_t>(type)] = std::make_shared<SpriteNumber>(
        texture, rect, align, digits, numRows, numCols, frameTime, eNumber(unsigned(eNumber::_SELECT_BAR_LEVEL_0) + index),
        animtimer, animFrames, texVertSplit);
    sLevel[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setLamp(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry lamp type (" << int(type) << "Invalid!"
            << " (Line " << __line << ")";
        return 1;
    }

    sLamp[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sLamp[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setTitle(pFont f, TextAlign align, unsigned ptsize, Color c)
{
    sTitle = std::make_shared<SpriteText>(f, eText(unsigned(eText::_SELECT_BAR_TITLE_FULL_0) + index), align, ptsize, c);
    sTitle->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setRank(BarRankType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRankType::RANK_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rank type (" << int(type) << "Invalid!"
            << " (Line " << __line << ")";
        return 1;
    }

    sRank[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sRank[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setRivalWinLose(BarRivalType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRivalType::RIVAL_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rival type (" << int(type) << "Invalid!"
            << " (Line " << __line << ")";
        return 1;
    }

    sRivalWinLose[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sRivalWinLose[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setRivalLampSelf(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rival lamp self type (" << int(type) << "Invalid!"
            << " (Line " << __line << ")";
        return 1;
    }

    sRivalLampSelf[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sRivalLampSelf[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setRivalLampRival(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rival lamp rival type (" << int(type) << "Invalid!"
            << " (Line " << __line << ")";
        return 1;
    }

    sRivalLampRival[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sRivalLampRival[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}


bool SpriteBarEntry::update(timestamp time)
{
    if (!context_select.info.empty())
    {
        _draw = true;
        const auto& info = context_select.info[index % context_select.info.size()];
        drawBodyOn = index == context_select.barIndex;
        if (!drawBodyOn && sBodyOff[info.type])
        {
            sBodyOff[info.type]->update(time);
            drawBody = info.type;
            setParent(sBodyOff[info.type]);
        }
        if (drawBodyOn && sBodyOn[info.type])
        {
            sBodyOn[info.type]->update(time);
            drawBody = info.type;
            setParent(sBodyOn[info.type]);
        }
        if (!_parent.expired())
        {
            auto parent = _parent.lock();
            _current.rect.x = parent->getCurrentRenderParams().rect.x;
            _current.rect.y = parent->getCurrentRenderParams().rect.y;
            //_current.color = parent->getCurrentRenderParams().color;
            //_current.angle += parent->getCurrentRenderParams().angle;
        }

        if (sTitle)
        {
            sTitle->update(time);
            drawTitle = true;
        }
        if (sFlash)
        {
            sFlash->update(time);
            drawFlash = true;
        }
        if (sLevel[info.level_type])
        {
            sLevel[info.level_type]->update(time); 
            drawLevel = info.level_type;
        }
        if (sRank[info.rank])
        {
            sRank[info.rank]->update(time);
            drawRank = info.rank;
        }
        if (sRivalWinLose[info.rival])
        {
            sRivalWinLose[info.rival]->update(time);
            drawRival = info.rival;
        }
        if (sRivalLampSelf[info.rival_lamp_self])
        {
            sRivalLampSelf[info.rival_lamp_self]->update(time);
            drawRivalLampSelf = info.rival_lamp_self;
        }
        if (sRivalLampRival[info.rival_lamp_rival])
        {
            sRivalLampRival[info.rival_lamp_rival]->update(time);
            drawRivalLampRival = info.rival_lamp_rival;
        }
    }
    else
    {
        _draw = false;
        //drawBody = drawTitle = false;
        //drawLevel = drawRank = drawRival = drawRivalLampSelf = drawRivalLampRival = -1u;
    }
    return _draw;
}


void SpriteBarEntry::setLoopTime(int t)
{
    LOG_ERROR << "[Sprite] setLoopTime(f) of SpriteBarEntry should not be used";
}

void SpriteBarEntry::setTrigTimer(eTimer t)
{
    LOG_ERROR << "[Sprite] setTrigTimer(f) of SpriteBarEntry should not be used";
}

void SpriteBarEntry::appendKeyFrame(RenderKeyFrame f)
{
    LOG_ERROR << "[Sprite] appendKeyFrame(f) of SpriteBarEntry should not be used";
}

void SpriteBarEntry::draw() const
{
    for (auto[type, subType] : drawQueue)
    {
        try {
            switch (type)
            {
            case BarPartsType::BODY_OFF:    if (subType == drawBody) sBodyOff[drawBody]->draw(); break;
            case BarPartsType::BODY_ON:     if (subType == drawBody) sBodyOn[drawBody]->draw(); break;
            case BarPartsType::TITLE:       if (drawTitle) sTitle->draw(); break;
            case BarPartsType::FLASH:       if (drawFlash) sFlash->draw(); break;
            case BarPartsType::LEVEL:       if (subType == drawLevel) sLevel[drawLevel]->draw(); break;
            case BarPartsType::LAMP:        if (subType == drawLamp) sLamp[drawLamp]->draw(); break;
            case BarPartsType::RANK:        if (subType == drawRank) sRank[drawRank]->draw(); break;
            case BarPartsType::RIVAL:       if (subType == drawRival) sRivalWinLose[drawRival]->draw(); break;
            case BarPartsType::MYLAMP:      if (subType == drawRivalLampSelf) sRivalLampSelf[drawRivalLampSelf]->draw(); break;
            case BarPartsType::RIVALLAMP:   if (subType == drawRivalLampRival) sRivalLampRival[drawRivalLampRival]->draw(); break;
            default: break;
            }
        }
        catch (std::out_of_range&) {}
    }
}
