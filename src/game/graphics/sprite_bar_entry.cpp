#include "sprite_bar_entry.h"
#include "game/scene/scene_context.h"
#include "common/entry/entry_song.h"
#include "common/chartformat/format_bms.h"

int SpriteBarEntry::setBody(BarType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarType::TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarBody type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    auto idx = static_cast<size_t>(type);

    sBodyOff[idx] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    //sBodyOff[idx]->setParent(weak_from_this());

    sBodyOn[idx] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    //sBodyOn[idx]->setParent(weak_from_this());

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
            << " (Line " << _srcLine << ")";
        return 1;
    }

    if (digits < 3 && type == BarLevelType::IRRANK)
        LOG_WARNING << "[Sprite] BarEntry level digit (" << digits << ") not enough for idx " << int(type)
            << " (Line " << _srcLine << ")";
    else if (digits < 2)
        LOG_WARNING << "[Sprite] BarEntry level digit (" << digits << ") not enough for idx " << int(type)
            << " (Line " << _srcLine << ")";

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
            << " (Line " << _srcLine << ")";
        return 1;
    }

    sLamp[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sLamp[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setTitle(BarTitleType type, pFont f, TextAlign align, unsigned ptsize, Color c)
{
    const size_t i = int(eText::_SELECT_BAR_TITLE_FULL_0) + index;
    sTitle[static_cast<size_t>(type)] = std::make_shared<SpriteText>(f, eText(i), align, ptsize, c);
    sTitle[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setTitle(BarTitleType type, std::vector<pTexture>& textures, CharMappingList& chrList,
    TextAlign align, unsigned height, int margin)
{
    const size_t i = int(eText::_SELECT_BAR_TITLE_FULL_0) + index;
    sTitle[static_cast<size_t>(type)] = std::make_shared<SpriteImageText>(textures, chrList, eText(i), align, height, margin);
    sTitle[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setRank(BarRankType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRankType::RANK_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rank type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
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
            << " (Line " << _srcLine << ")";
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
            << " (Line " << _srcLine << ")";
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
            << " (Line " << _srcLine << ")";
        return 1;
    }

    sRivalLampRival[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, 1, 1, texVertSplit);
    sRivalLampRival[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}


void SpriteBarEntry::pushPartsOrder(BarPartsType type)
{
    if (std::find(partsOrder.begin(), partsOrder.end(), type) == partsOrder.end())
        partsOrder.emplace_back(type);
}


bool SpriteBarEntry::update(Time time)
{
    auto& list = gSelectContext.entries;
    if (!list.empty())
    {
        size_t listidx = (gSelectContext.idx + list.size() - gSelectContext.cursor + index) % list.size();

        _draw = true;

        drawLevel = false;
        drawRank = false;
        drawLamp = false;
        drawRival = false;
        drawRivalLampSelf = false;
        drawRivalLampRival = false;
        drawBodyType = 0;
        drawTitleType = 0;
        drawLevelType = 0;
        drawLampType = 0;
        drawRankType = 0;
        drawRivalType = 0;
        drawRivalLampSelfType = 0;
        drawRivalLampRivalType = 0;

        auto pinfo = list[listidx];
        drawBodyOn = (index == gSelectContext.cursor);

        static const std::map<eEntryType, size_t> BAR_TYPE_MAP =
        {
            {eEntryType::UNKNOWN, (size_t)BarType::SONG},
            {eEntryType::FOLDER, (size_t)BarType::FOLDER},
            {eEntryType::CUSTOM_FOLDER, (size_t)BarType::CUSTOM_FOLDER},
            {eEntryType::SONG, (size_t)BarType::SONG},
            {eEntryType::RIVAL, (size_t)BarType::RIVAL},
            {eEntryType::RIVAL_SONG, (size_t)BarType::SONG_RIVAL},
            {eEntryType::NEW_COURSE, (size_t)BarType::NEW_COURSE},
            {eEntryType::COURSE, (size_t)BarType::COURSE},
            {eEntryType::RANDOM_COURSE, (size_t)BarType::RANDOM_COURSE},
        };
        size_t typeidx = (size_t)BarType::SONG;
        if (BAR_TYPE_MAP.find(pinfo->type()) != BAR_TYPE_MAP.end())
            typeidx = BAR_TYPE_MAP.at(pinfo->type());

        if (!drawBodyOn && sBodyOff[typeidx])
        {
            if (!sBodyOff[typeidx]->update(time))
            {
                _draw = false;
                return false;
            }
            drawBodyType = typeidx;
            setParent(sBodyOff[typeidx]);
        }
        if (drawBodyOn && sBodyOn[typeidx])
        {
            if (!sBodyOn[typeidx]->update(time))
            {
                _draw = false;
                return false;
            }
            drawBodyType = typeidx;
            setParent(sBodyOn[typeidx]);
        }
        if (!_parent.expired())
        {
            auto parent = _parent.lock();
            _current.rect.x = parent->getCurrentRenderParams().rect.x;
            _current.rect.y = parent->getCurrentRenderParams().rect.y;
            _current.color = parent->getCurrentRenderParams().color;
            _current.angle = parent->getCurrentRenderParams().angle;
        }

        drawTitle = false;
        if (/* TODO NEW SONG */true)
        {
            drawTitleType = size_t(BarTitleType::NORMAL);
            if (sTitle[drawTitleType])
            {
                sTitle[drawTitleType]->update(time);
                drawTitle = true;
            }
        }

        /*
        drawFlash = false;
        if (sFlash)
        {
            sFlash->update(time);
            drawFlash = true;
        }
        */
        if (drawFlash && sFlash)
            sFlash->update(time);

        if (pinfo->type() == eEntryType::SONG)
        {
            auto e = std::reinterpret_pointer_cast<Song>(pinfo);

            switch (e->_file->type())
            {
            case eChartFormat::BMS:
            {
                const auto bms = std::reinterpret_pointer_cast<const BMS_prop>(e->_file);
                if ((size_t)bms->difficulty < sLevel.size() && sLevel[bms->difficulty])
                {
                    sLevel[bms->difficulty]->update(time);
                    drawLevelType = bms->difficulty;
                    drawLevel = true;
                }
                /* bms.rank is judge rank, not play rank
                if ((size_t)bms.rank < sRank.size() && sRank[bms.rank])
                {
                    sRank[bms.rank]->update(time);
                    drawRankType = bms.rank;
                    drawRank = true;
                }
                */
                // TODO lamp
                break;
            }
            default:
                break;
            }

            // rival things
            if ((size_t)e->rival < sRivalWinLose.size() && sRivalWinLose[e->rival])
            {
                sRivalWinLose[e->rival]->update(time);
                drawRivalType = e->rival;
                drawRival = true;
            }
            if ((size_t)e->rival_lamp_self < sRivalLampSelf.size() && sRivalLampSelf[e->rival_lamp_self])
            {
                sRivalLampSelf[e->rival_lamp_self]->update(time);
                drawRivalLampSelfType = e->rival_lamp_self;
                drawRivalLampSelf = true;
            }
            if ((size_t)e->rival_lamp_rival < sRivalLampRival.size() && sRivalLampRival[e->rival_lamp_rival])
            {
                sRivalLampRival[e->rival_lamp_rival]->update(time);
                drawRivalLampRivalType = e->rival_lamp_rival;
                drawRivalLampRival = true;
            }
        }
        // TODO folder lamp

        drawQueue.clear();
        for (const auto& p : partsOrder)
        {
            switch (p)
            {
            case BarPartsType::BODY_OFF:
                if (!drawBodyOn && sBodyOff[drawBodyType])
                    drawQueue.emplace_back(p, (unsigned)drawBodyType);
                break;
            case BarPartsType::BODY_ON:
                if (drawBodyOn && sBodyOn[drawBodyType])
                    drawQueue.emplace_back(p, (unsigned)drawBodyType);
                break;
            case BarPartsType::TITLE:
                if (drawTitle && sTitle[drawTitleType])
                    drawQueue.emplace_back(p, (unsigned)drawTitleType);
                break;
            case BarPartsType::FLASH:
                if (drawFlash && sFlash)
                    drawQueue.emplace_back(p, 0);
                break;
            case BarPartsType::LEVEL:
                if (drawLevel && sLevel[drawLevelType])
                    drawQueue.emplace_back(p, (unsigned)drawLevelType);
                break;
            case BarPartsType::LAMP:
                if (drawLamp && sLamp[drawLampType])
                    drawQueue.emplace_back(p, (unsigned)drawLampType);
                break;
            case BarPartsType::RANK:
                if (drawRank && sRank[drawRankType])
                    drawQueue.emplace_back(p, (unsigned)drawRankType);
                break;
            case BarPartsType::RIVAL:
                if (drawRival && sRivalWinLose[drawRivalType])
                    drawQueue.emplace_back(p, (unsigned)drawRivalType);
                break;
            case BarPartsType::MYLAMP:
                if (drawRivalLampSelf && sRivalLampSelf[drawRivalLampSelfType])
                    drawQueue.emplace_back(p, (unsigned)drawRivalLampSelfType);
                break;
            case BarPartsType::RIVALLAMP:
                if (drawRivalLampRival && sRivalLampRival[drawRivalLampRivalType])
                    drawQueue.emplace_back(p, (unsigned)drawRivalLampRivalType);
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
    if (!_draw) return;
    for (auto[type, subType] : drawQueue)
    {
        switch (type)
        {
        case BarPartsType::BODY_OFF:    sBodyOff[drawBodyType]->draw(); break;
        case BarPartsType::BODY_ON:     sBodyOn[drawBodyType]->draw(); break;
        case BarPartsType::TITLE:       sTitle[drawTitleType]->draw(); break;
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

void SpriteBarEntry::setRectOffset(const Rect& dr)
{
    auto adjust_rect = [](Rect& r, const Rect& dr)
    {
        r.x += dr.x;
        r.y += dr.y;
    };

    if (drawBodyOn) adjust_rect(sBodyOn[drawBodyType]->getCurrentRenderParamsRef().rect, dr);
    else adjust_rect(sBodyOff[drawBodyType]->getCurrentRenderParamsRef().rect, dr);
    if (drawTitle)
    {
        adjust_rect(sTitle[drawTitleType]->getCurrentRenderParamsRef().rect, dr);
        sTitle[drawTitleType]->updateTextRect();
    }
    if (drawLevel)
    {
        adjust_rect(sLevel[drawLevelType]->getCurrentRenderParamsRef().rect, dr);
        sLevel[drawLevelType]->updateNumberRect();
    }
    if (drawLamp) adjust_rect(sLamp[drawLampType]->getCurrentRenderParamsRef().rect, dr);
    if (drawRank) adjust_rect(sRank[drawRankType]->getCurrentRenderParamsRef().rect, dr);
    if (drawRival) adjust_rect(sRivalWinLose[drawRivalType]->getCurrentRenderParamsRef().rect, dr);
    if (drawRivalLampSelf) adjust_rect(sRivalLampSelf[drawRivalLampSelfType]->getCurrentRenderParamsRef().rect, dr);
    if (drawRivalLampRival) adjust_rect(sRivalLampRival[drawRivalLampRivalType]->getCurrentRenderParamsRef().rect, dr);
}
