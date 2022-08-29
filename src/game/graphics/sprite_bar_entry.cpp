#include "sprite_bar_entry.h"
#include "game/scene/scene_context.h"
#include "common/entry/entry_song.h"
#include "common/chartformat/chartformat_bms.h"

int SpriteBarEntry::setBody(BarType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarType::TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarBody type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    auto idx = static_cast<size_t>(type);

    sBodyOff[idx] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    //sBodyOff[idx]->setParent(weak_from_this());

    sBodyOn[idx] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    //sBodyOn[idx]->setParent(weak_from_this());

    return 0;
}

int SpriteBarEntry::setFlash(pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, int nRows, int nCols, bool texVertSplit)
{
    sFlash = std::make_shared<SpriteAnimated>(texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
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
    eTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry lamp type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    sLamp[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
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
    eTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRankType::RANK_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rank type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    sRank[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    sRank[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setRivalWinLose(BarRivalType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRivalType::RIVAL_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rival type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    sRivalWinLose[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    sRivalWinLose[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setRivalLampSelf(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rival lamp self type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    sRivalLampSelf[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    sRivalLampSelf[static_cast<size_t>(type)]->setParent(weak_from_this());
    return 0;
}

int SpriteBarEntry::setRivalLampRival(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    eTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry rival lamp rival type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    sRivalLampRival[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
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

        auto [pEntry, pScore] = list[listidx];
        drawBodyOn = (index == gSelectContext.cursor);

        // check new song
        bool isNewEntry = (pEntry->_addTime < std::time(nullptr) + gNumbers.get(eNumber::NEW_ENTRY_SECONDS));

        static const std::map<eEntryType, size_t> BAR_TYPE_MAP =
        {
            {eEntryType::UNKNOWN, (size_t)BarType::SONG},
            {eEntryType::FOLDER, (size_t)BarType::FOLDER},
            {eEntryType::CUSTOM_FOLDER, (size_t)BarType::CUSTOM_FOLDER},
            {eEntryType::SONG, (size_t)BarType::SONG},
            {eEntryType::CHART, (size_t)BarType::SONG},
            {eEntryType::RIVAL, (size_t)BarType::RIVAL},
            {eEntryType::RIVAL_SONG, (size_t)BarType::SONG_RIVAL},
            {eEntryType::RIVAL_CHART, (size_t)BarType::SONG_RIVAL},
            {eEntryType::NEW_COURSE, (size_t)BarType::NEW_COURSE},
            {eEntryType::COURSE, (size_t)BarType::COURSE},
            {eEntryType::RANDOM_COURSE, (size_t)BarType::RANDOM_COURSE},
        };
        size_t barTypeIdx = (size_t)BarType::SONG;
        if (BAR_TYPE_MAP.find(pEntry->type()) != BAR_TYPE_MAP.end())
            barTypeIdx = BAR_TYPE_MAP.at(pEntry->type());
        if (isNewEntry)
        {
            switch ((BarType)barTypeIdx)
            {
            case BarType::SONG:
                barTypeIdx = (size_t)BarType::NEW_SONG;
                break;

            default:
                break;
            }
        }
        if (!drawBodyOn && sBodyOff[barTypeIdx])
        {
            if (!sBodyOff[barTypeIdx]->update(time))
            {
                _draw = false;
                return false;
            }
            drawBodyType = barTypeIdx;
            setParent(sBodyOff[barTypeIdx]);
        }
        if (drawBodyOn && sBodyOn[barTypeIdx])
        {
            if (!sBodyOn[barTypeIdx]->update(time))
            {
                _draw = false;
                return false;
            }
            drawBodyType = barTypeIdx;
            setParent(sBodyOn[barTypeIdx]);
        }
        if (_parent.expired())
        {
            _draw = false;
            return false;
        }
        else
        {
            auto parent = _parent.lock();
            const auto&& parentRenderParam = parent->getCurrentRenderParams();
            _current.rect = parentRenderParam.rect;
            _current.color = parentRenderParam.color;
            _current.angle = parentRenderParam.angle;
        }

        drawTitle = false;
        drawTitleType = size_t(isNewEntry ? BarTitleType::NEW_SONG : BarTitleType::NORMAL);
        if (sTitle[drawTitleType])
        {
            sTitle[drawTitleType]->update(time);
            drawTitle = true;
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

        if ((BarType)barTypeIdx == BarType::SONG || 
            (BarType)barTypeIdx == BarType::NEW_SONG || 
            (BarType)barTypeIdx == BarType::SONG_RIVAL)
        {
            std::shared_ptr<ChartFormatBase> pf;
            if (pEntry->type() == eEntryType::SONG || pEntry->type() == eEntryType::RIVAL_SONG)
            {
                pf = std::reinterpret_pointer_cast<EntryFolderSong>(pEntry)->getCurrentChart();
            }
            else if (pEntry->type() == eEntryType::CHART || pEntry->type() == eEntryType::RIVAL_CHART)
            {
                pf = std::reinterpret_pointer_cast<EntryChart>(pEntry)->_file;
            }
            if (pf)
            {
                switch (pf->type())
                {
                case eChartFormat::BMS:
                {
                    const auto bms = std::reinterpret_pointer_cast<const ChartFormatBMSMeta>(pf);

                    // level
                    if ((size_t)bms->difficulty < sLevel.size() && sLevel[bms->difficulty])
                    {
                        sLevel[bms->difficulty]->update(time);
                        drawLevelType = bms->difficulty;
                        drawLevel = true;
                    }

                    auto score = std::reinterpret_pointer_cast<ScoreBMS>(pScore);
                    if (score)
                    {
                        // lamp
                        // TODO rival entry has two lamps
                        static const std::map<ScoreBMS::Lamp, BarLampType> BMS_LAMP_TYPE_MAP =
                        {
                            {ScoreBMS::Lamp::NOPLAY,        BarLampType::NOPLAY      },
                            {ScoreBMS::Lamp::FAILED,        BarLampType::FAILED      },
                            {ScoreBMS::Lamp::ASSIST,        BarLampType::ASSIST_EASY },
                            {ScoreBMS::Lamp::EASY,          BarLampType::EASY        },
                            {ScoreBMS::Lamp::NORMAL,        BarLampType::NORMAL      },
                            {ScoreBMS::Lamp::HARD,          BarLampType::HARD        },
                            {ScoreBMS::Lamp::EXHARD,        BarLampType::HARD        }, // FIXME EXHARD
                            {ScoreBMS::Lamp::FULLCOMBO,     BarLampType::FULLCOMBO   },
                            {ScoreBMS::Lamp::PERFECT,       BarLampType::FULLCOMBO   }, // FIXME PERFECT
                            {ScoreBMS::Lamp::MAX,           BarLampType::FULLCOMBO   }  // FIXME MAX
                        };
                        size_t lampTypeIdx = (BMS_LAMP_TYPE_MAP.find(score->lamp) != BMS_LAMP_TYPE_MAP.end()) ?
                            (size_t)BMS_LAMP_TYPE_MAP.at(score->lamp) : (size_t)BarLampType::NOPLAY;
                        if (sLamp[lampTypeIdx])
                        {
                            sLamp[lampTypeIdx]->update(time);
                            drawLampType = lampTypeIdx;
                            drawLamp = true;
                        }

                        if ((BarType)barTypeIdx == BarType::SONG_RIVAL)
                        {
                            // rank
                            auto t = Option::getRankType(score->rival_rate);
                            switch (t)
                            {
                            case Option::RANK_0: drawRankType = (size_t)BarRankType::MAX;  break;
                            case Option::RANK_1: drawRankType = (size_t)BarRankType::AAA;  break;
                            case Option::RANK_2: drawRankType = (size_t)BarRankType::AA;   break;
                            case Option::RANK_3: drawRankType = (size_t)BarRankType::A;    break;
                            case Option::RANK_4: drawRankType = (size_t)BarRankType::B;    break;
                            case Option::RANK_5: drawRankType = (size_t)BarRankType::C;    break;
                            case Option::RANK_6: drawRankType = (size_t)BarRankType::D;    break;
                            case Option::RANK_7: drawRankType = (size_t)BarRankType::E;    break;
                            case Option::RANK_8: drawRankType = (size_t)BarRankType::F;    break;
                            case Option::RANK_NONE: drawRankType = (size_t)BarRankType::NONE; break;
                            }
                            if (sRank[drawRankType])
                            {
                                sRank[drawRankType]->update(time);
                                drawRank = true;
                            }
                            // win/lose/draw
                            if ((size_t)score->rival < sRivalWinLose.size() && sRivalWinLose[score->rival])
                            {
                                sRivalWinLose[score->rival]->update(time);
                                drawRivalType = score->rival;
                                drawRival = true;
                            }
                            // rival lamp
                            if (drawLamp && sRivalLampSelf[drawRivalLampSelfType])
                            {
                                drawRivalLampSelfType = drawLampType;
                                drawRivalLampSelf = true;
                                sRivalLampSelf[drawRivalLampSelfType]->update(time);
                            }
                            // rival lamp
                            size_t rivalLampTypeIdx = (BMS_LAMP_TYPE_MAP.find(score->rival_lamp) != BMS_LAMP_TYPE_MAP.end()) ?
                                (size_t)BMS_LAMP_TYPE_MAP.at(score->rival_lamp) : (size_t)BarLampType::NOPLAY;
                            if (sRivalLampRival[rivalLampTypeIdx])
                            {
                                sRivalLampRival[rivalLampTypeIdx]->update(time);
                                drawRivalLampRivalType = rivalLampTypeIdx;
                                drawRivalLampRival = true;
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
                }
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
    assert(false);
}

void SpriteBarEntry::setTrigTimer(eTimer t)
{
    LOG_ERROR << "[Sprite] setTrigTimer(f) of SpriteBarEntry should not be used";
    assert(false);
}

void SpriteBarEntry::appendKeyFrame(const RenderKeyFrame& f)
{
    LOG_ERROR << "[Sprite] appendKeyFrame(f) of SpriteBarEntry should not be used";
    assert(false);
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

    if (drawBodyOn) 
        adjust_rect(sBodyOn[drawBodyType]->getCurrentRenderParamsRef().rect, dr);
    else 
        adjust_rect(sBodyOff[drawBodyType]->getCurrentRenderParamsRef().rect, dr);

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
