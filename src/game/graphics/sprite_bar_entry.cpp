#include "common/pch.h"
#include "sprite_bar_entry.h"
#include "common/entry/entry_types.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/data/data_select.h"
#include "game/data/data_play.h"

namespace lunaticvibes
{

int SpriteBarEntry::setBody(BarType type, const SpriteAnimated::SpriteAnimatedBuilder& builder)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarType::TYPE_COUNT))
    {
        LOG_DEBUG << "[Sprite] BarBody type (" << int(type) << "Invalid!"
            << " (Line " << srcLine << ")";
        return 1;
    }

    auto idx = static_cast<size_t>(type);

    sBodyOff[idx] = builder.build();

    sBodyOn[idx] = builder.build();

    return 0;
}

int SpriteBarEntry::setFlash(const SpriteAnimated::SpriteAnimatedBuilder& builder)
{
    sFlash = builder.build();
    return 0;
}

int SpriteBarEntry::setLevel(BarLevelType type, const SpriteNumber::SpriteNumberBuilder& builder)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLevelType::LEVEL_TYPE_COUNT))
    {
        LOG_DEBUG << "[Sprite] BarEntry level type (" << int(type) << "Invalid!"
            << " (Line " << srcLine << ")";
        return 1;
    }

    if (builder.maxDigits < 3 && type == BarLevelType::IRRANK)
        LOG_DEBUG << "[Sprite] BarEntry level digit (" << builder.maxDigits << ") not enough for IRRANK "
        << " (Line " << srcLine << ")";
    else if (builder.maxDigits < 2)
        LOG_DEBUG << "[Sprite] BarEntry level digit (" << builder.maxDigits << ") not enough for idx " << int(type)
        << " (Line " << srcLine << ")";

    SpriteNumber::SpriteNumberBuilder tmpBuilder = builder;
    tmpBuilder.numberCallback = std::bind([](int index) { return SelectData.barLevel[index]; }, index);
    sLevel[static_cast<size_t>(type)] = tmpBuilder.build();
    return 0;
}

int SpriteBarEntry::setLamp(BarLampType type, const SpriteAnimated::SpriteAnimatedBuilder& builder)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        LOG_DEBUG << "[Sprite] BarEntry lamp type (" << int(type) << "Invalid!"
            << " (Line " << srcLine << ")";
        return 1;
    }

    sLamp[static_cast<size_t>(type)] = builder.build();
    return 0;
}

int SpriteBarEntry::setTitle(BarTitleType type, const SpriteText::SpriteTextBuilder& builder)
{
    SpriteText::SpriteTextBuilder tmpBuilder = builder;
    tmpBuilder.textCallback = std::bind([](int index) { return SelectData.barTitle[index]; }, index);
    sTitle[static_cast<size_t>(type)] = tmpBuilder.build();
    return 0;
}

int SpriteBarEntry::setTitle(BarTitleType type, const SpriteImageText::SpriteImageTextBuilder& builder)
{
    SpriteImageText::SpriteImageTextBuilder tmpBuilder = builder;
    tmpBuilder.textCallback = std::bind([](int index) { return SelectData.barTitle[index]; }, index);
    sTitle[static_cast<size_t>(type)] = tmpBuilder.build();
    return 0;
}

int SpriteBarEntry::setRank(BarRankType type, const SpriteAnimated::SpriteAnimatedBuilder& builder)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRankType::RANK_TYPE_COUNT))
    {
        if (srcLine >= 0)
        {
            LOG_DEBUG << "[Sprite] BarEntry rank type (" << int(type) << "Invalid!"
                << " (Line " << srcLine << ")";
        }
        return 1;
    }

    sRank[static_cast<size_t>(type)] = builder.build();
    return 0;
}

int SpriteBarEntry::setRivalWinLose(BarRivalType type, const SpriteAnimated::SpriteAnimatedBuilder& builder)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarRivalType::RIVAL_TYPE_COUNT))
    {
        if (srcLine >= 0)
        {
            LOG_DEBUG << "[Sprite] BarEntry rival type (" << int(type) << ") Invalid!"
                << " (Line " << srcLine << ")";
        }
        return 1;
    }

    sRivalWinLose[static_cast<size_t>(type)] = builder.build();
    return 0;
}

int SpriteBarEntry::setRivalLampSelf(BarLampType type, const SpriteAnimated::SpriteAnimatedBuilder& builder)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        if (srcLine >= 0)
        {
            LOG_DEBUG << "[Sprite] BarEntry rival lamp self type (" << int(type) << "Invalid!"
                << " (Line " << srcLine << ")";
        }
        return 1;
    }

    sRivalLampSelf[static_cast<size_t>(type)] = builder.build();
    return 0;
}

int SpriteBarEntry::setRivalLampRival(BarLampType type, const SpriteAnimated::SpriteAnimatedBuilder& builder)
{
    if (static_cast<size_t>(type) >= static_cast<size_t>(BarLampType::LAMP_TYPE_COUNT))
    {
        if (srcLine >= 0)
        {
            LOG_DEBUG << "[Sprite] BarEntry rival lamp rival type (" << int(type) << "Invalid!"
                << " (Line " << srcLine << ")";
        }
        return 1;
    }

    sRivalLampRival[static_cast<size_t>(type)] = builder.build();
    return 0;
}


void SpriteBarEntry::pushPartsOrder(BarPartsType type)
{
    if (std::find(partsOrder.begin(), partsOrder.end(), type) == partsOrder.end())
        partsOrder.emplace_back(type);
}


bool SpriteBarEntry::update(Time time)
{
    for (auto& s : sBodyOff) if (s) s->setHideInternal(true);
    for (auto& s : sBodyOn) if (s) s->setHideInternal(true);
    for (auto& s : sTitle) if (s) s->setHideInternal(true);
    for (auto& s : sLevel) if (s) s->setHideInternal(true);
    for (auto& s : sLamp) if (s) s->setHideInternal(true);
    for (auto& s : sRank) if (s) s->setHideInternal(true);
    for (auto& s : sRivalWinLose) if (s) s->setHideInternal(true);
    for (auto& s : sRivalLampSelf) if (s) s->setHideInternal(true);
    for (auto& s : sRivalLampRival) if (s) s->setHideInternal(true);
    if (sFlash) sFlash->setHideInternal(true);

    auto& list = SelectData.entries;
    if (!list.empty())
    {
        size_t listidx = SelectData.selectedEntryIndex + index;
        if (listidx < SelectData.highlightBarIndex)
            listidx += list.size() * ((SelectData.highlightBarIndex - listidx) / list.size() + 1);
        listidx -= SelectData.highlightBarIndex;
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
        drawBodyOn = (index == SelectData.highlightBarIndex);

        // check new song
        bool isNewEntry = false;
        if (pEntry->type() == eEntryType::NEW_SONG_FOLDER)
            isNewEntry = true;
        else
        {
            using namespace std::chrono;
            isNewEntry = (pEntry->_addTime > duration_cast<seconds>(
                system_clock::now().time_since_epoch()).count() - SelectData.newEntrySeconds);
        }

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
        std::shared_ptr<SpriteBase> pBody = nullptr;
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

        const auto& parentRenderParam = pBody->_current;
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

        drawFlash = SelectData.highlightBarIndex == index;
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
                        static const std::map<LampType, BarLampType> BMS_LAMP_TYPE_MAP_OLD =
                        {
                            {LampType::NOPLAY,        BarLampType::NOPLAY      },
                            {LampType::FAILED,        BarLampType::FAILED      },
                            {LampType::ASSIST,        BarLampType::FAILED      },
                            {LampType::EASY,          BarLampType::EASY        },
                            {LampType::NORMAL,        BarLampType::NORMAL      },
                            {LampType::HARD,          BarLampType::HARD        },
                            {LampType::EXHARD,        BarLampType::HARD        },
                            {LampType::FULLCOMBO,     BarLampType::FULLCOMBO   },
                            {LampType::PERFECT,       BarLampType::FULLCOMBO   },
                            {LampType::MAX,           BarLampType::FULLCOMBO   }
                        };
                        static const std::map<LampType, BarLampType> BMS_LAMP_TYPE_MAP =
                        {
                            {LampType::NOPLAY,        BarLampType::NOPLAY      },
                            {LampType::FAILED,        BarLampType::FAILED      },
                            {LampType::ASSIST,        BarLampType::ASSIST_EASY },
                            {LampType::EASY,          BarLampType::EASY        },
                            {LampType::NORMAL,        BarLampType::NORMAL      },
                            {LampType::HARD,          BarLampType::HARD        },
                            {LampType::EXHARD,        BarLampType::EXHARD      }, // FIXME EXHARD
                            {LampType::FULLCOMBO,     BarLampType::FULLCOMBO   },
                            {LampType::PERFECT,       BarLampType::FULLCOMBO   }, // FIXME PERFECT
                            {LampType::MAX,           BarLampType::FULLCOMBO   }  // FIXME MAX
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
                            auto t = getRankType(score->rival_rate);
                            switch (t)
                            {
                            case RankType::MAX:  drawRankType = (size_t)BarRankType::MAX;  break;
                            case RankType::AAA:  drawRankType = (size_t)BarRankType::AAA;  break;
                            case RankType::AA:   drawRankType = (size_t)BarRankType::AA;   break;
                            case RankType::A:    drawRankType = (size_t)BarRankType::A;    break;
                            case RankType::B:    drawRankType = (size_t)BarRankType::B;    break;
                            case RankType::C:    drawRankType = (size_t)BarRankType::C;    break;
                            case RankType::D:    drawRankType = (size_t)BarRankType::D;    break;
                            case RankType::E:    drawRankType = (size_t)BarRankType::E;    break;
                            case RankType::F:    drawRankType = (size_t)BarRankType::F;    break;
                            case RankType::_:    drawRankType = (size_t)BarRankType::NONE; break;
                            }
                            if (sRank[drawRankType])
                            {
                                sRank[drawRankType]->update(time);
                                sRank[drawRankType]->setHideInternal(false);
                                drawRank = true;
                            }
                            // win/lose/draw
                            if ((size_t)score->rival_win < sRivalWinLose.size() && sRivalWinLose[score->rival_win])
                            {
                                sRivalWinLose[score->rival_win]->update(time);
                                sRivalWinLose[score->rival_win]->setHideInternal(false);
                                drawRivalType = score->rival_win;
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
                static const std::map<LampType, BarLampType> BMS_LAMP_TYPE_MAP_OLD =
                {
                    {LampType::NOPLAY,        BarLampType::NOPLAY      },
                    {LampType::FAILED,        BarLampType::FAILED      },
                    {LampType::ASSIST,        BarLampType::FAILED      },
                    {LampType::EASY,          BarLampType::EASY        },
                    {LampType::NORMAL,        BarLampType::NORMAL      },
                    {LampType::HARD,          BarLampType::HARD        },
                    {LampType::EXHARD,        BarLampType::HARD        },
                    {LampType::FULLCOMBO,     BarLampType::FULLCOMBO   },
                    {LampType::PERFECT,       BarLampType::FULLCOMBO   },
                    {LampType::MAX,           BarLampType::FULLCOMBO   }
                };
                static const std::map<LampType, BarLampType> BMS_LAMP_TYPE_MAP =
                {
                    {LampType::NOPLAY,        BarLampType::NOPLAY      },
                    {LampType::FAILED,        BarLampType::FAILED      },
                    {LampType::ASSIST,        BarLampType::ASSIST_EASY },
                    {LampType::EASY,          BarLampType::EASY        },
                    {LampType::NORMAL,        BarLampType::NORMAL      },
                    {LampType::HARD,          BarLampType::HARD        },
                    {LampType::EXHARD,        BarLampType::EXHARD      },
                    {LampType::FULLCOMBO,     BarLampType::FULLCOMBO   },
                    {LampType::PERFECT,       BarLampType::FULLCOMBO   }, // FIXME PERFECT
                    {LampType::MAX,           BarLampType::FULLCOMBO   }  // FIXME MAX
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


void SpriteBarEntry::setMotionLoopTo(int t)
{
    LOG_ERROR << "[Sprite] setMotionLoopTo(f) of SpriteBarEntry should not be used";
    assert(false);
}

void SpriteBarEntry::setMotionStartTimer(const std::string& t)
{
    LOG_ERROR << "[Sprite] setMotionStartTimer(f) of SpriteBarEntry should not be used";
    assert(false);
}

void SpriteBarEntry::appendMotionKeyFrame(const MotionKeyFrame& f)
{
    LOG_ERROR << "[Sprite] appendMotionKeyFrame(f) of SpriteBarEntry should not be used";
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
        adjust_rect(sBodyOn[drawBodyType]->_current.rect, x, y);
    else
        adjust_rect(sBodyOff[drawBodyType]->_current.rect, x, y);

    if (drawTitle)
    {
        adjust_rect(sTitle[drawTitleType]->_current.rect, x, y);
        sTitle[drawTitleType]->updateTextRect();
    }
    if (drawLevel)
    {
        adjust_rect(sLevel[drawLevelType]->_current.rect, x, y);
        sLevel[drawLevelType]->updateNumberRect();
    }
    if (drawLamp) adjust_rect(sLamp[drawLampType]->_current.rect, x, y);
    if (drawRank) adjust_rect(sRank[drawRankType]->_current.rect, x, y);
    if (drawRival) adjust_rect(sRivalWinLose[drawRivalType]->_current.rect, x, y);
    if (drawRivalLampSelf) adjust_rect(sRivalLampSelf[drawRivalLampSelfType]->_current.rect, x, y);
    if (drawRivalLampRival) adjust_rect(sRivalLampRival[drawRivalLampRivalType]->_current.rect, x, y);
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
        adjust_rect(sTitle[drawTitleType]->_current.rect, x, y);
        sTitle[drawTitleType]->updateTextRect();
    }
    if (drawLevel)
    {
        adjust_rect(sLevel[drawLevelType]->_current.rect, x, y);
        sLevel[drawLevelType]->updateNumberRect();
    }
    if (drawFlash) adjust_rect(sFlash->_current.rect, x, y);
    if (drawLamp) adjust_rect(sLamp[drawLampType]->_current.rect, x, y);
    if (drawRank) adjust_rect(sRank[drawRankType]->_current.rect, x, y);
    if (drawRival) adjust_rect(sRivalWinLose[drawRivalType]->_current.rect, x, y);
    if (drawRivalLampSelf) adjust_rect(sRivalLampSelf[drawRivalLampSelfType]->_current.rect, x, y);
    if (drawRivalLampRival) adjust_rect(sRivalLampRival[drawRivalLampRivalType]->_current.rect, x, y);
}

bool SpriteBarEntry::OnClick(int x, int y)
{
    if (!_draw) return false;

    if (_current.rect.x <= x && x < _current.rect.x + _current.rect.w &&
        _current.rect.y <= y && y < _current.rect.y + _current.rect.h)
    {
        if (SelectData.highlightBarIndex == index)
        {
            SelectData.cursorEnterPending = true;
        }
        else
        {
            if (available)
            {
                SelectData.cursorClick = index;
            }
            else
            {
                if (SelectData.highlightBarIndex > index)
                    SelectData.cursorClickScroll = (SelectData.highlightBarIndex - index);
                else
                    SelectData.cursorClickScroll = -(index - SelectData.highlightBarIndex);
            }
        }
        return true;
    }
    return false;
}

}
