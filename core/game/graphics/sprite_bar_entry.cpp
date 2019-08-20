#include "sprite_bar_entry.h"
#include <plog/Log.h>
#include "game/scene/scene_context.h"

int SpriteBarEntry::setBody(BarType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    sBodyOff[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    return 0;
}

int SpriteBarEntry::setLevel(BarLevelType type, pTexture texture, const Rect& rect, NumberAlign align, unsigned digits,
    unsigned numRows, unsigned numCols, unsigned frameTime, eNumber num, eTimer animtimer,
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
        texture, rect, align, digits, numRows, numCols, frameTime, num, animtimer, animFrames, texVertSplit);
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
    return 0;
}

int SpriteBarEntry::setTitle(pFont f, TextAlign align, unsigned ptsize, Color c)
{
    sTitle = std::make_shared<SpriteText>(f, eText(unsigned(eText::_SELECT_BAR_TITLE_FULL_0) + index), align, ptsize, c);
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
    return 0;
}


bool SpriteBarEntry::update(timestamp time)
{
    _draw = false;
    if (!context_select.info.empty())
    {
        const auto& info = context_select.info[index % context_select.info.size()];
        drawBodyOn = index == context_select.barIndex;
        if (!drawBodyOn && sBodyOff[info.type])
        {
            sBodyOff[info.type]->update(time);
            drawBody = true;
            setParent(sBodyOff[info.type]);
        }
        if (drawBodyOn && sBodyOn[info.type])
        {
            sBodyOn[info.type]->update(time);
            drawBody = true;
            setParent(sBodyOn[info.type]);
        }
        if (!_parent.expired())
        {
            _draw = true;
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
        drawBody = drawTitle = false;
        drawLevel = drawRank = drawRival = drawRivalLampSelf = drawRivalLampRival = -1u;
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

// FIXME: should draw subparts following order in definition.
void SpriteBarEntry::draw() const
{
    if (drawBody != -1u) drawBodyOn ? sBodyOn[drawBody]->draw() : sBodyOff[drawBody]->draw();
    if (drawTitle) sTitle->draw();
    if (drawLevel != -1u) sLevel[drawLevel]->draw();
    if (drawRank != -1u) sRank[drawRank]->draw();
    if (drawRival != -1u) sRivalWinLose[drawRival]->draw();
    if (drawRivalLampSelf != -1u) sRivalLampSelf[drawRivalLampSelf]->draw();
    if (drawRivalLampRival != -1u) sRivalLampRival[drawRivalLampRival]->draw();
}

void SpriteBarEntry::setTrigTimerBodyOn(BarType type, eTimer t)
{
    if (sBodyOn[static_cast<size_t>(type)]) sBodyOn[static_cast<size_t>(type)]->setTrigTimer(t);
}

void SpriteBarEntry::setTrigTimerBodyOff(BarType type, eTimer t)
{
    if (sBodyOff[static_cast<size_t>(type)]) sBodyOff[static_cast<size_t>(type)]->setTrigTimer(t);
}

void SpriteBarEntry::setTrigTimerTitle(eTimer t)
{
    if (sTitle)
    {
        sTitle->setTrigTimer(t);
        sTitle->setParent(weak_from_this());
    }
}

void SpriteBarEntry::setTrigTimerLevel(BarLevelType type, eTimer t)
{
    auto& ps = sLevel[static_cast<size_t>(type)];
    if (ps)
    {
        ps->setTrigTimer(t);
        ps->setParent(weak_from_this());
    }
}

void SpriteBarEntry::setTrigTimerLamp(BarLampType type, eTimer t)
{
    auto& ps = sLamp[static_cast<size_t>(type)];
    if (ps)
    {
        ps->setTrigTimer(t);
        ps->setParent(weak_from_this());
    }
}

void SpriteBarEntry::setTrigTimerRank(BarRankType type, eTimer t)
{
    auto& ps = sRank[static_cast<size_t>(type)];
    if (ps)
    {
        ps->setTrigTimer(t);
        ps->setParent(weak_from_this());
    }
}

void SpriteBarEntry::setTrigTimerRivalWinLose(BarRivalType type, eTimer t)
{
    auto& ps = sRivalWinLose[static_cast<size_t>(type)];
    if (ps)
    {
        ps->setTrigTimer(t);
        ps->setParent(weak_from_this());
    }
}

void SpriteBarEntry::setTrigTimerRivalLampSelf(BarLampType type, eTimer t)
{
    auto& ps = sRivalLampSelf[static_cast<size_t>(type)];
    if (ps)
    {
        ps->setTrigTimer(t);
        ps->setParent(weak_from_this());
    }
}

void SpriteBarEntry::setTrigTimerRivalLampRival(BarLampType type, eTimer t)
{
    auto& ps = sRivalLampRival[static_cast<size_t>(type)];
    if (ps)
    {
        ps->setTrigTimer(t);
        ps->setParent(weak_from_this());
    }
}

void SpriteBarEntry::setLoopTimeBodyOff(BarType type, int t)
{
    auto& ps = sBodyOff[static_cast<size_t>(type)];
    if (ps) ps->setLoopTime(t);
}

void SpriteBarEntry::setLoopTimeBodyOn(BarType type, int t)
{
    auto& ps = sBodyOn[static_cast<size_t>(type)];
    if (ps) ps->setLoopTime(t);
}

void SpriteBarEntry::setLoopTimeTitle(int t)
{
    sTitle->setLoopTime(t);
}

void SpriteBarEntry::setLoopTimeLevel(BarLevelType type, int t)
{
    auto& ps = sLevel[static_cast<size_t>(type)];
    if (ps) ps->setLoopTime(t);
}

void SpriteBarEntry::setLoopTimeLamp(BarLampType type, int t)
{
    auto& ps = sLamp[static_cast<size_t>(type)];
    if (ps) ps->setLoopTime(t);
}

void SpriteBarEntry::setLoopTimeRank(BarRankType type, int t)
{
    auto& ps = sRank[static_cast<size_t>(type)];
    if (ps) ps->setLoopTime(t);
}

void SpriteBarEntry::setLoopTimeRivalWinLose(BarRivalType type, int t)
{
    auto& ps = sRivalWinLose[static_cast<size_t>(type)];
    if (ps) ps->setLoopTime(t);
}

void SpriteBarEntry::setLoopTimeRivalLampSelf(BarLampType type, int t)
{
    auto& ps = sRivalLampSelf[static_cast<size_t>(type)];
    if (ps) ps->setLoopTime(t);
}

void SpriteBarEntry::setLoopTimeRivalLampRival(BarLampType type, int t)
{
    auto& ps = sRivalLampRival[static_cast<size_t>(type)];
    if (ps) ps->setLoopTime(t);
}

void SpriteBarEntry::appendKeyFrameBodyOff(BarType type, RenderKeyFrame f)
{
    auto& ps = sBodyOff[static_cast<size_t>(type)];
    if (ps) ps->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameBodyOn(BarType type, RenderKeyFrame f)
{
    auto& ps = sBodyOn[static_cast<size_t>(type)];
    if (ps) ps->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameTitle(RenderKeyFrame f)
{
    sTitle->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameLevel(BarLevelType type, RenderKeyFrame f)
{
    auto& ps = sLevel[static_cast<size_t>(type)];
    if (ps) ps->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameLamp(BarLampType type, RenderKeyFrame f)
{
    auto& ps = sLamp[static_cast<size_t>(type)];
    if (ps) ps->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameRank(BarRankType type, RenderKeyFrame f)
{
    auto& ps = sRank[static_cast<size_t>(type)];
    if (ps) ps->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameRivalWinLose(BarRivalType type, RenderKeyFrame f)
{
    auto& ps = sRivalWinLose[static_cast<size_t>(type)];
    if (ps) ps->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameRivalLampSelf(BarLampType type, RenderKeyFrame f)
{
    auto& ps = sRivalLampSelf[static_cast<size_t>(type)];
    if (ps) ps->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameRivalLampRival(BarLampType type, RenderKeyFrame f)
{
    auto& ps = sRivalLampRival[static_cast<size_t>(type)];
    if (ps) ps->appendKeyFrame(f);
}
