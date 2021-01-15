#include "sprite_bar_entry.h"
#include <plog/Log.h>
#include "game/scene/scene_context.h"
#include "entry/entry_song.h"
#include "chartformat/format_bms.h"

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


bool SpriteBarEntry::update(Time time)
{
    size_t listidx = context_select.idx;
    auto& list = context_select.entries;
    if (!list.empty())
    {
        _draw = true;
        const auto& info = list[listidx % list.size()];
        drawBodyOn = (index == listidx);

        size_t typeidx = (size_t)BarType::SONG;
        switch (info.type())
        {
        case eEntryType::UNKNOWN:
            break;
        case eEntryType::FOLDER:
            typeidx = (size_t)BarType::FOLDER;
            break;
        case eEntryType::CUSTOM_FOLDER:
            typeidx = (size_t)BarType::CUSTOM_FOLDER;
            break;
        case eEntryType::SONG:
            // TODO addtime check -> NEW_SONG
            typeidx = (size_t)BarType::SONG;
            break;
        case eEntryType::RIVAL:
            typeidx = (size_t)BarType::RIVAL;
            break;
        case eEntryType::RIVAL_SONG:
            typeidx = (size_t)BarType::SONG_RIVAL;
            break;
        case eEntryType::NEW_COURSE:
            typeidx = (size_t)BarType::NEW_COURSE;
            break;
        case eEntryType::COURSE:
            typeidx = (size_t)BarType::COURSE;
            break;
        case eEntryType::RANDOM_COURSE:
            typeidx = (size_t)BarType::RANDOM_COURSE;
            break;
        }

        if (!drawBodyOn && sBodyOff[typeidx])
        {
            sBodyOff[typeidx]->update(time);
            drawBodyType = typeidx;
            setParent(sBodyOff[typeidx]);
        }
        if (drawBodyOn && sBodyOn[typeidx])
        {
            sBodyOn[typeidx]->update(time);
            drawBodyType = typeidx;
            setParent(sBodyOn[typeidx]);
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

        if (info.type() == eEntryType::SONG)
        {
            const auto& e = reinterpret_cast<const Song&>(info);

            switch (e._file->type())
            {
            case eChartFormat::BMS:
            {
                const auto& bms = reinterpret_cast<const BMS&>(e._file);
                if ((size_t)bms.difficulty < sLevel.size() && sLevel[bms.difficulty])
                {
                    sLevel[bms.difficulty]->update(time);
                    drawLevelType = bms.difficulty;
                }
                if ((size_t)bms.rank < sRank.size() && sRank[bms.rank])
                {
                    sRank[bms.rank]->update(time);
                    drawRankType = bms.rank;
                }
                break;
            }
            default:
                break;
            }

            // rival things
            if ((size_t)e.rival < sRivalWinLose.size() && sRivalWinLose[e.rival])
            {
                sRivalWinLose[e.rival]->update(time);
                drawRivalType = e.rival;
            }
            if ((size_t)e.rival_lamp_self < sRivalLampSelf.size() && sRivalLampSelf[e.rival_lamp_self])
            {
                sRivalLampSelf[e.rival_lamp_self]->update(time);
                drawRivalLampSelfType = e.rival_lamp_self;
            }
            if ((size_t)e.rival_lamp_rival < sRivalLampRival.size() && sRivalLampRival[e.rival_lamp_rival])
            {
                sRivalLampRival[e.rival_lamp_rival]->update(time);
                drawRivalLampRivalType = e.rival_lamp_rival;
            }
        }

        drawQueue.clear();
        for (const auto& p : partsOrder)
        {
            switch (p)
            {
            case BarPartsType::BODY_OFF:
            case BarPartsType::BODY_ON:
                drawQueue.emplace_back(p, drawBodyType);
                break;

            case BarPartsType::TITLE:
            case BarPartsType::FLASH:
                drawQueue.emplace_back(p, 0);
                break;

            case BarPartsType::LEVEL:
                drawQueue.emplace_back(p, drawLevelType);
                break;
            case BarPartsType::LAMP:
                drawQueue.emplace_back(p, drawLampType);
                break;
            case BarPartsType::RANK:
                drawQueue.emplace_back(p, drawRankType);
                break;
            case BarPartsType::RIVAL:
                drawQueue.emplace_back(p, drawRivalType);
                break;
            case BarPartsType::MYLAMP:
                drawQueue.emplace_back(p, drawRivalLampSelfType);
                break;
            case BarPartsType::RIVALLAMP:
                drawQueue.emplace_back(p, drawRivalLampRivalType);
                break;
            default:
                break;
            }
        }

    }
    else
    {
        _draw = false;
        //drawBodyType = drawTitle = false;
        //drawLevelType = drawRankType = drawRivalType = drawRivalLampSelfType = drawRivalLampRivalType = -1u;
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
        switch (type)
        {
        case BarPartsType::BODY_OFF:    sBodyOff[drawBodyType]->draw(); break;
        case BarPartsType::BODY_ON:     sBodyOn[drawBodyType]->draw(); break;
        case BarPartsType::TITLE:       sTitle->draw(); break;
        case BarPartsType::FLASH:       sFlash->draw(); break;
        case BarPartsType::LEVEL:       sLevel[drawLevelType]->draw(); break;
        case BarPartsType::LAMP:        sLamp[drawLampType]->draw(); break;
        case BarPartsType::RANK:        sRank[drawRankType]->draw(); break;
        case BarPartsType::RIVAL:       sRivalWinLose[drawRivalType]->draw(); break;
        case BarPartsType::MYLAMP:      sRivalLampSelf[drawRivalLampSelfType]->draw(); break;
        case BarPartsType::RIVALLAMP:   sRivalLampRival[drawRivalLampRivalType]->draw(); break;
        default: break;
        }
    }
}
