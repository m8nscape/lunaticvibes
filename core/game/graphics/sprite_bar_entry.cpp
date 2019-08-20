#include "sprite_bar_entry.h"
#include <plog/Log.h>
#include "game/scene/scene_context.h"

int SpriteBarEntry::setBody(pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer = eTimer::SCENE_START, bool texVertSplit = false)
{
    sBody = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    return 0;
}

int SpriteBarEntry::setLevel(BarLevelType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime, unsigned digits, NumberAlign align,
    eTimer timer = eTimer::SCENE_START, bool texVertSplit = false)
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
        texture, rect, animFrames, frameTime, align, digits, timer, 1, 1, texVertSplit);
    return 0;
}

int SpriteBarEntry::setLamp(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer = eTimer::SCENE_START, bool texVertSplit = false)
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

int SpriteBarEntry::setTitle(pFont f,
    TextAlign align = TEXT_ALIGN_LEFT, unsigned ptsize = 72, Color c = 0xffffffff)
{
    sTitle = std::make_shared<SpriteText>(f, eText(unsigned(eText::_SELECT_BAR_TITLE_FULL_0) + index), align, ptsize, c);
    return 0;
}

int SpriteBarEntry::setRank(BarRankType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer = eTimer::SCENE_START, bool texVertSplit = false)
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
    eTimer timer = eTimer::SCENE_START, bool texVertSplit = false)
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
    eTimer timer = eTimer::SCENE_START, bool texVertSplit = false)
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
    eTimer timer = eTimer::SCENE_START, bool texVertSplit = false)
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
    if (sBody) sBody->update(time);
    if (sTitle) sTitle->update(time);
    for (auto& p : sLevel) if (p) p->update(time);
    for (auto& p : sLamp) if (p) p->update(time);
    for (auto& p : sRank) if (p) p->update(time);
    for (auto& p : sRivalWinLose) if (p) p->update(time);
    for (auto& p : sRivalLampSelf) if (p) p->update(time);
    for (auto& p : sRivalLampRival) if (p) p->update(time);
}


void SpriteBarEntry::setLoopTime(int t)
{
    if (sBody) sBody->setLoopTime(t);
    if (sTitle) sTitle->setLoopTime(t);
    for (auto& p : sLevel) if (p) p->setLoopTime(t);
    for (auto& p : sLamp) if (p) p->setLoopTime(t);
    for (auto& p : sRank) if (p) p->setLoopTime(t);
    for (auto& p : sRivalWinLose) if (p) p->setLoopTime(t);
    for (auto& p : sRivalLampSelf) if (p) p->setLoopTime(t);
    for (auto& p : sRivalLampRival) if (p) p->setLoopTime(t);
}


void SpriteBarEntry::setTimer(eTimer t)
{
    if (sBody) sBody->setTimer(t);
    if (sTitle) sTitle->setTimer(t);
    for (auto& p : sLevel) if (p) p->setTimer(t);
    for (auto& p : sLamp) if (p) p->setTimer(t);
    for (auto& p : sRank) if (p) p->setTimer(t);
    for (auto& p : sRivalWinLose) if (p) p->setTimer(t);
    for (auto& p : sRivalLampSelf) if (p) p->setTimer(t);
    for (auto& p : sRivalLampRival) if (p) p->setTimer(t);
}


void SpriteBarEntry::appendKeyFrame(RenderKeyFrame f)
{
    LOG_ERROR << "[Sprite] appendKeyFrame(f) of SpriteBarEntry should not be used";
}

// FIXME: should draw subparts following order in definition.
void SpriteBarEntry::draw() const
{
    if (!context_select.info.empty())
    {
        const auto& info = context_select.info[index % context_select.info.size()];
        if (sBody) sBody->draw();
        if (sTitle) sTitle->draw();
        if (sLevel[info.level_type]) sLevel[info.level_type]->draw();
        if (sRank[info.rank]) sRank[info.rank]->draw();
        if (sRivalWinLose[info.rival]) sRivalWinLose[info.rival]->draw();
        if (sRivalLampSelf[info.rival_lamp_self]) sRivalLampSelf[info.rival_lamp_self]->draw();
        if (sRivalLampRival[info.rival_lamp_rival]) sRivalLampRival[info.rival_lamp_rival]->draw();
    }
}

void SpriteBarEntry::appendKeyFrameBody(RenderKeyFrame f)
{
    sBody->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameTitle(RenderKeyFrame f)
{
    sTitle->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameLevel(BarLevelType type, RenderKeyFrame f)
{
    if (sLevel[static_cast<size_t>(type)]) sLevel[static_cast<size_t>(type)]->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameLamp(BarLampType type, RenderKeyFrame f)
{
    if (sLamp[static_cast<size_t>(type)]) sLamp[static_cast<size_t>(type)]->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameRank(BarRankType type, RenderKeyFrame f)
{
    if (sRank[static_cast<size_t>(type)]) sRank[static_cast<size_t>(type)]->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameRivalWinLose(BarRivalType type, RenderKeyFrame f)
{
    if (sRivalWinLose[static_cast<size_t>(type)]) sRivalWinLose[static_cast<size_t>(type)]->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameRivalLampSelf(BarLampType type, RenderKeyFrame f)
{
    if (sRivalLampSelf[static_cast<size_t>(type)]) sRivalLampSelf[static_cast<size_t>(type)]->appendKeyFrame(f);
}

void SpriteBarEntry::appendKeyFrameRivalLampRival(BarLampType type, RenderKeyFrame f)
{
    if (sRivalLampRival[static_cast<size_t>(type)]) sRivalLampRival[static_cast<size_t>(type)]->appendKeyFrame(f);
}
