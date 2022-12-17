#include "sprite_bar_entry.h"
#include "game/scene/scene_context.h"
#include "common/entry/entry_types.h"
#include "common/chartformat/chartformat_bms.h"

int SpriteBarEntry::setBody(BarType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    IndexTimer timer, int nRows, int nCols, bool texVertSplit)
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

    sBodyOn[idx] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);

    return 0;
}

int SpriteBarEntry::setFlash(pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    IndexTimer timer, int nRows, int nCols, bool texVertSplit)
{
    sFlash = std::make_shared<SpriteAnimated>(texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    return 0;
}

int SpriteBarEntry::setLevel(BarLevelType type, pTexture texture, const Rect& rect, NumberAlign align, unsigned digits,
    unsigned numRows, unsigned numCols, unsigned frameTime, IndexTimer animtimer,
    unsigned animFrames, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLevelType::LEVEL_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry level type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    if (digits < 3 && type == BarLevelType::IRRANK)
        LOG_WARNING << "[Sprite] BarEntry level digit (" << digits << ") not enough for IRRANK "
            << " (Line " << _srcLine << ")";
    else if (digits < 2)
        LOG_WARNING << "[Sprite] BarEntry level digit (" << digits << ") not enough for idx " << int(type)
            << " (Line " << _srcLine << ")";

    sLevel[static_cast<size_t>(type)] = std::make_shared<SpriteNumber>(
        texture, rect, align, digits, numRows, numCols, frameTime, IndexNumber(unsigned(IndexNumber::_SELECT_BAR_LEVEL_0) + index),
        animtimer, animFrames, texVertSplit);
    return 0;
}

int SpriteBarEntry::setLamp(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    IndexTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        LOG_WARNING << "[Sprite] BarEntry lamp type (" << int(type) << "Invalid!"
            << " (Line " << _srcLine << ")";
        return 1;
    }

    sLamp[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    return 0;
}

int SpriteBarEntry::setTitle(BarTitleType type, pFont f, TextAlign align, unsigned ptsize, Color c)
{
    const size_t i = int(IndexText::_SELECT_BAR_TITLE_FULL_0) + index;
    sTitle[static_cast<size_t>(type)] = std::make_shared<SpriteText>(f, IndexText(i), align, ptsize, c);
    return 0;
}

int SpriteBarEntry::setTitle(BarTitleType type, std::vector<pTexture>& textures, CharMappingList* chrList,
    TextAlign align, unsigned height, int margin)
{
    const size_t i = int(IndexText::_SELECT_BAR_TITLE_FULL_0) + index;
    sTitle[static_cast<size_t>(type)] = std::make_shared<SpriteImageText>(textures, chrList, IndexText(i), align, height, margin);
    return 0;
}

int SpriteBarEntry::setRank(BarRankType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    IndexTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRankType::RANK_TYPE_COUNT))
    {
        if (_srcLine >= 0)
        {
            LOG_WARNING << "[Sprite] BarEntry rank type (" << int(type) << "Invalid!"
                << " (Line " << _srcLine << ")";
        }
        return 1;
    }

    sRank[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    return 0;
}

int SpriteBarEntry::setRivalWinLose(BarRivalType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    IndexTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRivalType::RIVAL_TYPE_COUNT))
    {
        if (_srcLine >= 0)
        {
            LOG_WARNING << "[Sprite] BarEntry rival type (" << int(type) << ") Invalid!"
                << " (Line " << _srcLine << ")";
        }
        return 1;
    }

    sRivalWinLose[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    return 0;
}

int SpriteBarEntry::setRivalLampSelf(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    IndexTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        if (_srcLine >= 0)
        {
            LOG_WARNING << "[Sprite] BarEntry rival lamp self type (" << int(type) << "Invalid!"
                << " (Line " << _srcLine << ")";
        }
        return 1;
    }

    sRivalLampSelf[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    return 0;
}

int SpriteBarEntry::setRivalLampRival(BarLampType type, pTexture texture, const Rect& rect, unsigned animFrames, unsigned frameTime,
    IndexTimer timer, int nRows, int nCols, bool texVertSplit)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        if (_srcLine >= 0)
        {
            LOG_WARNING << "[Sprite] BarEntry rival lamp rival type (" << int(type) << "Invalid!"
                << " (Line " << _srcLine << ")";
        }
        return 1;
    }

    sRivalLampRival[static_cast<size_t>(type)] = std::make_shared<SpriteAnimated>(
        texture, rect, animFrames, frameTime, timer, nRows, nCols, texVertSplit);
    return 0;
}


void SpriteBarEntry::pushPartsOrder(BarPartsType type)
{
    if (std::find(partsOrder.begin(), partsOrder.end(), type) == partsOrder.end())
        partsOrder.emplace_back(type);
}


bool SpriteBarEntry::update(Time time)
{
    for (auto& s: sBodyOff) if (s) s->setHideInternal(true);
    for (auto& s: sBodyOn) if (s) s->setHideInternal(true);
    for (auto& s: sTitle) if (s) s->setHideInternal(true);
    for (auto& s: sLevel) if (s) s->setHideInternal(true);
    for (auto& s: sLamp) if (s) s->setHideInternal(true);
    for (auto& s: sRank) if (s) s->setHideInternal(true);
    for (auto& s: sRivalWinLose) if (s) s->setHideInternal(true);
    for (auto& s: sRivalLampSelf) if (s) s->setHideInternal(true);
    for (auto& s: sRivalLampRival) if (s) s->setHideInternal(true);
    if (sFlash) sFlash->setHideInternal(true);

    auto& list = gSelectContext.entries;
    if (!list.empty())
    {
        size_t listidx = gSelectContext.idx + index;
        if (listidx < gSelectContext.cursor)
            listidx += list.size() * ((gSelectContext.cursor - listidx) / list.size() + 1);
        listidx -= gSelectContext.cursor;
        listidx %= list.size();

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
        bool isNewEntry = false;
        if (pEntry->type() == eEntryType::NEW_SONG_FOLDER)
            isNewEntry = true;
        else
            isNewEntry = (pEntry->_addTime > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - State::get(IndexNumber::NEW_ENTRY_SECONDS));

        static const std::map<eEntryType, size_t> BAR_TYPE_MAP =
        {
            {eEntryType::UNKNOWN, (size_t)BarType::SONG},
            {eEntryType::FOLDER, (size_t)BarType::FOLDER},
            {eEntryType::NEW_SONG_FOLDER, (size_t)BarType::NEW_SONG_FOLDER},
            {eEntryType::CUSTOM_FOLDER, (size_t)BarType::CUSTOM_FOLDER},
            {eEntryType::COURSE_FOLDER, (size_t)BarType::COURSE_FOLDER},
            {eEntryType::SONG, (size_t)BarType::SONG},
            {eEntryType::CHART, (size_t)BarType::SONG},
            {eEntryType::RIVAL, (size_t)BarType::RIVAL},
            {eEntryType::RIVAL_SONG, (size_t)BarType::SONG_RIVAL},
            {eEntryType::RIVAL_CHART, (size_t)BarType::SONG_RIVAL},
            {eEntryType::NEW_COURSE, (size_t)BarType::NEW_COURSE},
            {eEntryType::COURSE, (size_t)BarType::COURSE},
            {eEntryType::RANDOM_COURSE, (size_t)BarType::RANDOM_COURSE},
            {eEntryType::ARENA_FOLDER, (size_t)BarType::RIVAL},
            {eEntryType::ARENA_COMMAND, (size_t)BarType::RIVAL},
            {eEntryType::ARENA_LOBBY, (size_t)BarType::SONG},
        };
        size_t barTypeIdx = (size_t)BarType::SONG;
        if (BAR_TYPE_MAP.find(pEntry->type()) != BAR_TYPE_MAP.end())
            barTypeIdx = BAR_TYPE_MAP.at(pEntry->type());
        if (isNewEntry && (BarType)barTypeIdx == BarType::SONG)
        {
            barTypeIdx = (size_t)BarType::NEW_SONG;
        }
        pSprite pBody = nullptr;
        if (!drawBodyOn && sBodyOff[barTypeIdx])
        {
            if (!sBodyOff[barTypeIdx]->update(time))
            {
                _draw = false;
                return false;
            }
            sBodyOff[barTypeIdx]->setHideInternal(false);
            drawBodyType = barTypeIdx;
            pBody = sBodyOff[barTypeIdx];
        }
        if (drawBodyOn && sBodyOn[barTypeIdx])
        {
            if (!sBodyOn[barTypeIdx]->update(time))
            {
                _draw = false;
                return false;
            }
            sBodyOn[barTypeIdx]->setHideInternal(false);
            drawBodyType = barTypeIdx;
            pBody = sBodyOn[barTypeIdx];
        }
        if (pBody == nullptr)
        {
            _draw = false;
            return false;
        }

        const auto&& parentRenderParam = pBody->getCurrentRenderParams();
        _current.rect = parentRenderParam.rect;
        _current.color = parentRenderParam.color;
        _current.angle = parentRenderParam.angle;

        drawTitle = false;
        drawTitleType = size_t(isNewEntry ? BarTitleType::NEW_SONG : BarTitleType::NORMAL);
        if (sTitle[drawTitleType])
        {
            sTitle[drawTitleType]->update(time);
            sTitle[drawTitleType]->setHideInternal(false);
            drawTitle = true;
        }

        drawFlash = gSelectContext.cursor == index;
        if (drawFlash && sFlash)
        {
            sFlash->update(time);
            sFlash->setHideInternal(false);
        }

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
                        sLevel[bms->difficulty]->setHideInternal(false);
                        drawLevelType = bms->difficulty;
                        drawLevel = true;
                    }

                    auto score = std::reinterpret_pointer_cast<ScoreBMS>(pScore);
                    if (score)
                    {
                        // lamp
                        // TODO rival entry has two lamps
                        static const std::map<ScoreBMS::Lamp, BarLampType> BMS_LAMP_TYPE_MAP_OLD =
                        {
                            {ScoreBMS::Lamp::NOPLAY,        BarLampType::NOPLAY      },
                            {ScoreBMS::Lamp::FAILED,        BarLampType::FAILED      },
                            {ScoreBMS::Lamp::ASSIST,        BarLampType::FAILED      },
                            {ScoreBMS::Lamp::EASY,          BarLampType::EASY        },
                            {ScoreBMS::Lamp::NORMAL,        BarLampType::NORMAL      },
                            {ScoreBMS::Lamp::HARD,          BarLampType::HARD        },
                            {ScoreBMS::Lamp::EXHARD,        BarLampType::HARD        },
                            {ScoreBMS::Lamp::FULLCOMBO,     BarLampType::FULLCOMBO   },
                            {ScoreBMS::Lamp::PERFECT,       BarLampType::FULLCOMBO   },
                            {ScoreBMS::Lamp::MAX,           BarLampType::FULLCOMBO   }
                        };
                        static const std::map<ScoreBMS::Lamp, BarLampType> BMS_LAMP_TYPE_MAP =
                        {
                            {ScoreBMS::Lamp::NOPLAY,        BarLampType::NOPLAY      },
                            {ScoreBMS::Lamp::FAILED,        BarLampType::FAILED      },
                            {ScoreBMS::Lamp::ASSIST,        BarLampType::ASSIST_EASY },
                            {ScoreBMS::Lamp::EASY,          BarLampType::EASY        },
                            {ScoreBMS::Lamp::NORMAL,        BarLampType::NORMAL      },
                            {ScoreBMS::Lamp::HARD,          BarLampType::HARD        },
                            {ScoreBMS::Lamp::EXHARD,        BarLampType::EXHARD      }, // FIXME EXHARD
                            {ScoreBMS::Lamp::FULLCOMBO,     BarLampType::FULLCOMBO   },
                            {ScoreBMS::Lamp::PERFECT,       BarLampType::FULLCOMBO   }, // FIXME PERFECT
                            {ScoreBMS::Lamp::MAX,           BarLampType::FULLCOMBO   }  // FIXME MAX
                        };
                        size_t lampTypeIdx = (BMS_LAMP_TYPE_MAP.find(score->lamp) != BMS_LAMP_TYPE_MAP.end()) ?
                            (size_t)BMS_LAMP_TYPE_MAP.at(score->lamp) : (size_t)BarLampType::NOPLAY;
                        if (sLamp[lampTypeIdx])
                        {
                            sLamp[lampTypeIdx]->update(time);
                            sLamp[lampTypeIdx]->setHideInternal(false);
                            drawLampType = lampTypeIdx;
                            drawLamp = true;
                        }
                        else
                        {
                            lampTypeIdx = (size_t)BMS_LAMP_TYPE_MAP_OLD.at(score->lamp);
                            if (sLamp[lampTypeIdx])
                            {
                                sLamp[lampTypeIdx]->update(time);
                                sLamp[lampTypeIdx]->setHideInternal(false);
                                drawLampType = lampTypeIdx;
                                drawLamp = true;
                            }
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
                                sRank[drawRankType]->setHideInternal(false);
                                drawRank = true;
                            }
                            // win/lose/draw
                            if ((size_t)score->rival < sRivalWinLose.size() && sRivalWinLose[score->rival])
                            {
                                sRivalWinLose[score->rival]->update(time);
                                sRivalWinLose[score->rival]->setHideInternal(false);
                                drawRivalType = score->rival;
                                drawRival = true;
                            }
                            // rival lamp
                            if (drawLamp && sRivalLampSelf[drawRivalLampSelfType])
                            {
                                drawRivalLampSelfType = drawLampType;
                                drawRivalLampSelf = true;
                                sRivalLampSelf[drawRivalLampSelfType]->update(time);
                                sRivalLampSelf[drawRivalLampSelfType]->setHideInternal(false);
                            }
                            // rival lamp
                            size_t rivalLampTypeIdx = (BMS_LAMP_TYPE_MAP.find(score->rival_lamp) != BMS_LAMP_TYPE_MAP.end()) ?
                                (size_t)BMS_LAMP_TYPE_MAP.at(score->rival_lamp) : (size_t)BarLampType::NOPLAY;
                            if (sRivalLampRival[rivalLampTypeIdx])
                            {
                                sRivalLampRival[rivalLampTypeIdx]->update(time);
                                sRivalLampRival[rivalLampTypeIdx]->setHideInternal(false);
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
        else if ((BarType)barTypeIdx == BarType::COURSE)
        {
            auto ps = std::reinterpret_pointer_cast<EntryCourse>(pEntry);

            auto score = std::dynamic_pointer_cast<ScoreBMS>(pScore);
            if (score)
            {
                static const std::map<ScoreBMS::Lamp, BarLampType> BMS_LAMP_TYPE_MAP_OLD =
                {
                    {ScoreBMS::Lamp::NOPLAY,        BarLampType::NOPLAY      },
                    {ScoreBMS::Lamp::FAILED,        BarLampType::FAILED      },
                    {ScoreBMS::Lamp::ASSIST,        BarLampType::FAILED      },
                    {ScoreBMS::Lamp::EASY,          BarLampType::EASY        },
                    {ScoreBMS::Lamp::NORMAL,        BarLampType::NORMAL      },
                    {ScoreBMS::Lamp::HARD,          BarLampType::HARD        },
                    {ScoreBMS::Lamp::EXHARD,        BarLampType::HARD        },
                    {ScoreBMS::Lamp::FULLCOMBO,     BarLampType::FULLCOMBO   },
                    {ScoreBMS::Lamp::PERFECT,       BarLampType::FULLCOMBO   },
                    {ScoreBMS::Lamp::MAX,           BarLampType::FULLCOMBO   } 
                };
                static const std::map<ScoreBMS::Lamp, BarLampType> BMS_LAMP_TYPE_MAP =
                {
                    {ScoreBMS::Lamp::NOPLAY,        BarLampType::NOPLAY      },
                    {ScoreBMS::Lamp::FAILED,        BarLampType::FAILED      },
                    {ScoreBMS::Lamp::ASSIST,        BarLampType::ASSIST_EASY },
                    {ScoreBMS::Lamp::EASY,          BarLampType::EASY        },
                    {ScoreBMS::Lamp::NORMAL,        BarLampType::NORMAL      },
                    {ScoreBMS::Lamp::HARD,          BarLampType::HARD        },
                    {ScoreBMS::Lamp::EXHARD,        BarLampType::EXHARD      },
                    {ScoreBMS::Lamp::FULLCOMBO,     BarLampType::FULLCOMBO   },
                    {ScoreBMS::Lamp::PERFECT,       BarLampType::FULLCOMBO   }, // FIXME PERFECT
                    {ScoreBMS::Lamp::MAX,           BarLampType::FULLCOMBO   }  // FIXME MAX
                };
                size_t lampTypeIdx = (BMS_LAMP_TYPE_MAP.find(score->lamp) != BMS_LAMP_TYPE_MAP.end()) ?
                    (size_t)BMS_LAMP_TYPE_MAP.at(score->lamp) : (size_t)BarLampType::NOPLAY;
                if (sLamp[lampTypeIdx])
                {
                    sLamp[lampTypeIdx]->update(time);
                    sLamp[lampTypeIdx]->setHideInternal(false);
                    drawLampType = lampTypeIdx;
                    drawLamp = true;
                }
                else
                {
                    lampTypeIdx = (size_t)BMS_LAMP_TYPE_MAP_OLD.at(score->lamp);
                    if (sLamp[lampTypeIdx])
                    {
                        sLamp[lampTypeIdx]->update(time);
                        sLamp[lampTypeIdx]->setHideInternal(false);
                        drawLampType = lampTypeIdx;
                        drawLamp = true;
                    }
                }
            }
        }
        // TODO folder lamp

        setRectOffsetBarIndex(parentRenderParam.rect.x, parentRenderParam.rect.y);
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

void SpriteBarEntry::setTrigTimer(IndexTimer t)
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
}

void SpriteBarEntry::setRectOffsetAnim(float x, float y)
{
    auto adjust_rect = [](RectF& r, float x, float y)
    {
        r.x += x;
        r.y += y;
    };

    if (drawBodyOn)
        adjust_rect(sBodyOn[drawBodyType]->getCurrentRenderParamsRef().rect, x, y);
    else
        adjust_rect(sBodyOff[drawBodyType]->getCurrentRenderParamsRef().rect, x, y);

    if (drawTitle)
    {
        adjust_rect(sTitle[drawTitleType]->getCurrentRenderParamsRef().rect, x, y);
        sTitle[drawTitleType]->updateTextRect();
    }
    if (drawLevel)
    {
        adjust_rect(sLevel[drawLevelType]->getCurrentRenderParamsRef().rect, x, y);
        sLevel[drawLevelType]->updateNumberRect();
    }
    if (drawLamp) adjust_rect(sLamp[drawLampType]->getCurrentRenderParamsRef().rect, x, y);
    if (drawRank) adjust_rect(sRank[drawRankType]->getCurrentRenderParamsRef().rect, x, y);
    if (drawRival) adjust_rect(sRivalWinLose[drawRivalType]->getCurrentRenderParamsRef().rect, x, y);
    if (drawRivalLampSelf) adjust_rect(sRivalLampSelf[drawRivalLampSelfType]->getCurrentRenderParamsRef().rect, x, y);
    if (drawRivalLampRival) adjust_rect(sRivalLampRival[drawRivalLampRivalType]->getCurrentRenderParamsRef().rect, x, y);
}

void SpriteBarEntry::setRectOffsetBarIndex(float x, float y)
{
    auto adjust_rect = [](RectF& r, float x, float y)
    {
        r.x += x;
        r.y += y;
    };

    if (drawTitle)
    {
        adjust_rect(sTitle[drawTitleType]->getCurrentRenderParamsRef().rect, x, y);
        sTitle[drawTitleType]->updateTextRect();
    }
    if (drawLevel)
    {
        adjust_rect(sLevel[drawLevelType]->getCurrentRenderParamsRef().rect, x, y);
        sLevel[drawLevelType]->updateNumberRect();
    }
    if (drawFlash) adjust_rect(sFlash->getCurrentRenderParamsRef().rect, x, y);
    if (drawLamp) adjust_rect(sLamp[drawLampType]->getCurrentRenderParamsRef().rect, x, y);
    if (drawRank) adjust_rect(sRank[drawRankType]->getCurrentRenderParamsRef().rect, x, y);
    if (drawRival) adjust_rect(sRivalWinLose[drawRivalType]->getCurrentRenderParamsRef().rect, x, y);
    if (drawRivalLampSelf) adjust_rect(sRivalLampSelf[drawRivalLampSelfType]->getCurrentRenderParamsRef().rect, x, y);
    if (drawRivalLampRival) adjust_rect(sRivalLampRival[drawRivalLampRivalType]->getCurrentRenderParamsRef().rect, x, y);
}

bool SpriteBarEntry::OnClick(int x, int y)
{
    if (!_draw) return false;

    if (_current.rect.x <= x && x < _current.rect.x + _current.rect.w &&
        _current.rect.y <= y && y < _current.rect.y + _current.rect.h)
    {
        if (gSelectContext.cursor == index)
        {
            gSelectContext.cursorEnter = true;
        }
        else
        {
            if (available)
            {
                gSelectContext.cursorClick = index;
            }
            else
            {
                if (gSelectContext.cursor > index)
                    gSelectContext.cursorClickScroll = (gSelectContext.cursor - index);
                else
                    gSelectContext.cursorClickScroll = -(index - gSelectContext.cursor);
            }
        }
        return true;
    }
    return false;
}
