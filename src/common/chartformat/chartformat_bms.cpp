#include "chartformat_bms.h"
#include "common/log.h"
#include <iostream>
#include <fstream>
#include <set>
#include <utility>
#include <exception>
#include <filesystem>
#include <numeric>
#include <random>
#include "db/db_song.h"
#include "re2/re2.h"
#include <boost/algorithm/string.hpp>

class noteLineException : public std::exception {};

bool ChartFormatBMS::getExtendedProperty(const std::string& key, void* ret)
{
    if (strEqual(key, "PLAYER", true))
    {
        *(int*)ret = player;
    }
    else if (strEqual(key, "RANK", true))
    {
        *(int*)ret = rank;
    }
    else if (strEqual(key, "TOTAL", true))
    {
        *(int*)ret = total;
    }
    else
    {
        return false;
    }
    return true;
}

ChartFormatBMS::ChartFormatBMS() : ChartFormatBMSMeta() {
    wavFiles.resize(MAXSAMPLEIDX + 1);
    bgaFiles.resize(MAXSAMPLEIDX + 1);
    metres.resize(MAXBARIDX + 1);
}

ChartFormatBMS::ChartFormatBMS(const Path& filePath, uint64_t randomSeed): ChartFormatBMSMeta() {
    wavFiles.resize(MAXSAMPLEIDX + 1);
    bgaFiles.resize(MAXSAMPLEIDX + 1);
    metres.resize(MAXBARIDX + 1);
    initWithFile(filePath, randomSeed);
}

int ChartFormatBMS::initWithFile(const Path& filePath, uint64_t randomSeed)
{
    using err = ErrorCode;
    if (loaded)
    {
        //errorCode = err::ALREADY_INITIALIZED;
        //errorLine = 0;
        return 1;
    }

    fileName = filePath.filename();
    absolutePath = std::filesystem::absolute(filePath);
    LOG_DEBUG << "[BMS] " << absolutePath.u8string();
    std::ifstream ifsFile(absolutePath.c_str());
    if (ifsFile.fail())
    {
        errorCode = err::FILE_ERROR;
        errorLine = 0;
        LOG_WARNING << "[BMS] " << absolutePath.u8string() << " File ERROR";
        return 1;
    }

    // copy the whole file into ram, once for all
    std::stringstream bmsFile;
    bmsFile << ifsFile.rdbuf();
    bmsFile.sync();
    ifsFile.close();

    auto encoding = getFileEncoding(bmsFile);

    if (toLower(filePath.extension().u8string()) == ".pms")
    {
        isPMS = true;
    }

    // 拉面早就看出bms有多难读，直接鸽了我5年

    unsigned srcLine = 0;

    // #RANDOM related parameters
    std::stack<int> randomValueMax;
    std::stack<int> randomValue;
    std::stack<std::set<int>> randomUsedValues;
    std::stack<std::stack<int>> ifStack;
    std::stack<int> ifValue;

    // implicit parameters
    bool hasDifficulty = false;

    while (!bmsFile.eof())
    {
        StringContent lineBuf;
        std::getline(bmsFile, lineBuf, '\n');
        srcLine++;
        if (lineBuf.length() <= 1) continue;

        // remove not needed spaces
#ifdef WIN32
        static const auto localeUTF8 = std::locale(".65001");
#else
        static const auto localeUTF8 = std::locale("en_US.UTF-8");
#endif
        boost::trim_right(lineBuf, localeUTF8);

        // convert codepage
        lineBuf = to_utf8(lineBuf, encoding);

        StringContentView buf(lineBuf);
        if (buf[0] != '#') continue;

        // parsing
        try
        {
            auto spacePos = std::min(buf.length(), buf.find_first_of(' '));

            // supporting single level control flow (#RANDOM, #IF, etc.), matching with LR2's capability
            if (!randomUsedValues.empty() && randomUsedValues.top().size() < randomValue.top())
            {
                StringContentView v = buf;
                if (ifValue.empty() && !v.empty() && v[0] == '#' &&
                    !strEqual(v.substr(0, 3), "#IF", true) && 
                    !strEqual(v.substr(0, 6), "#ENDIF", true) && 
                    !strEqual(v.substr(0, 10), "#ENDRANDOM", true))
                {
                    LOG_WARNING << "[BMS] definition found after all IF blocks finished, assuming #ENDRANDOM is missing. " << absolutePath.u8string() << "@" << srcLine;
                    randomValueMax.pop();
                    randomValue.pop();
                    randomUsedValues.pop();
                    ifStack.pop();
                }
            }

            if (spacePos > 1 && strEqual(buf.substr(0, 7), "#RANDOM", true))
            {
                StringContentView value = spacePos < buf.length() ? buf.substr(spacePos + 1) : "";
                int iValue = toInt(value);
                if (iValue == 0)
                {
                    LOG_WARNING << "[BMS] Invalid #RANDOM value found. " << absolutePath.u8string()  << "@" << srcLine;
                    continue;
                }

                haveRandom = true;
                std::mt19937_64 rng(randomSeed);
                int rngValue = (rng() % toInt(value)) + 1;
                randomValueMax.push(iValue);
                randomValue.push(rngValue);
                randomUsedValues.emplace();

                ifStack.push(ifValue);
                ifValue = std::stack<int>();

                continue;
            }

            if (!randomValue.empty())
            {
                // read IF headers outside of blocks
                if (spacePos > 1)
                {
                    StringContentView key = buf.substr(1, spacePos - 1);
                    StringContentView value = spacePos < buf.length() ? buf.substr(spacePos + 1) : "";
                    if (strEqual(key, "IF", true))
                    {
                        int ifBlockValue = toInt(value);
                        if (randomUsedValues.top().find(ifBlockValue) != randomUsedValues.top().end())
                        {
                            LOG_WARNING << "[BMS] duplicate #IF value found. " << absolutePath.u8string() << "@" << srcLine;
                        }

                        // one level control flow
                        if (!ifValue.empty())
                        {
                            LOG_WARNING << "[BMS] unexpected #IF found, assuming #ENDIF is missing. " << absolutePath.u8string() << "@" << srcLine;
                            randomUsedValues.top().emplace(ifValue.top());
                            ifValue.pop();
                        }

                        ifValue.push(ifBlockValue);
                    }
                    else if (strEqual(key, "ENDIF", true))
                    {
                        if (!ifValue.empty())
                        {
                            randomUsedValues.top().emplace(ifValue.top());
                            ifValue.pop();
                        }
                        else
                        {
                            LOG_WARNING << "[BMS] unexpected #ENDIF found. " << absolutePath.u8string() << "@" << srcLine;
                        }
                    }
                    else if (strEqual(key, "ENDRANDOM", true))
                    {
                        if (!ifValue.empty())
                        {
                            LOG_WARNING << "[BMS] #ENDRANDOM found before #ENDIF. " << absolutePath.u8string() << "@" << srcLine;
                        }
                        randomValueMax.pop();
                        randomValue.pop();
                        randomUsedValues.pop();

                        if (!ifStack.empty())
                        {
                            ifValue = ifStack.top();
                            ifStack.pop();
                        }
                        else
                        {
                            ifValue = std::stack<int>();
                        }

                        continue;
                    }
                }

                if (!randomUsedValues.empty() && randomUsedValues.top().size() == randomValueMax.top())
                {
                    randomValueMax.pop();
                    randomValue.pop();
                    randomUsedValues.pop();

                    if (!ifStack.empty())
                    {
                        ifValue = ifStack.top();
                        ifStack.pop();
                    }
                    else
                    {
                        ifValue = std::stack<int>();
                    }
                }
                else if (ifValue.empty())
                {
                    // skip orphan blocks
                    continue;
                }
                else if (ifValue.top() != randomValue.top())
                {
                    // skip mismatch IF value blocks
                    continue;
                }
            }

            bool isNoteDef = false;
            static LazyRE2 regexNote{ R"(#[\d]{3}[0-9A-Za-z]{2}:.*)" };
            if (!RE2::FullMatch(re2::StringPiece(buf.data(), buf.length()), *regexNote))
            {
                auto spacePos = std::min(buf.length(), buf.find_first_of(' '));
                if (spacePos <= 1) continue;

                StringContentView key = buf.substr(1, spacePos - 1);
                StringContentView value = spacePos < buf.length() ? buf.substr(spacePos + 1) : "";

                static LazyRE2 regexWav{ R"((?i)WAV[0-9A-Za-z]{1,2})" };
                static LazyRE2 regexBga{ R"((?i)BMP[0-9A-Za-z]{1,2})" };
                static LazyRE2 regexBpm{ R"((?i)BPM[0-9A-Za-z]{1,2})" };
                static LazyRE2 regexStop{ R"((?i)STOP[0-9A-Za-z]{1,2})" };

                if (key.empty()) continue;
                if (value.empty()) continue;

                // digits
                if (strEqual(key, "PLAYER", true))
                    player = toInt(value);
                else if (strEqual(key, "RANK", true))
                    rank = toInt(value);
                else if (strEqual(key, "TOTAL", true))
                    total = toInt(value);
                else if (strEqual(key, "PLAYLEVEL", true))
                {
                    playLevel = toInt(value);
                    levelEstimated = double(playLevel);
                }
                else if (strEqual(key, "DIFFICULTY", true))
                {
                    difficulty = toInt(value);
                    hasDifficulty = true;
                }
                else if (strEqual(key, "BPM", true))
                    bpm = toDouble(value);

                // strings
                else if (strEqual(key, "TITLE", true))
                    title.assign(value.begin(), value.end());
                else if (strEqual(key, "SUBTITLE", true))
                    title2.assign(value.begin(), value.end());
                else if (strEqual(key, "ARTIST", true))
                    artist.assign(value.begin(), value.end());
                else if (strEqual(key, "SUBARTIST", true))
                    artist2.assign(value.begin(), value.end());
                else if (strEqual(key, "GENRE", true))
                    genre.assign(value.begin(), value.end());
                else if (strEqual(key, "STAGEFILE", true))
                    stagefile.assign(value.begin(), value.end());
                else if (strEqual(key, "BANNER", true))
                    banner.assign(value.begin(), value.end());
                else if (strEqual(key, "LNOBJ", true) && value.length() >= 2)
                {
                    if (!lnobjSet.empty())
                    {
                        LOG_WARNING << "[BMS] Multiple #LNOBJ found. " << absolutePath.u8string() << "@" << srcLine;
                        lnobjSet.clear();
                    }
                    lnobjSet.insert(base36(value[0], value[1]));
                }

                // #???xx
                else if (RE2::FullMatch(re2::StringPiece(key.data(), key.length()), *regexWav))
                {
                    int idx = base36(key[3], key[4]);
                    wavFiles[idx].assign(value.begin(), value.end());
                    if (!ifStack.empty()) resourceStable = false;
                }
                else if (RE2::FullMatch(re2::StringPiece(key.data(), key.length()), *regexBga))
                {
                    int idx = base36(key[3], key[4]);
                    if (idx != 0)
                    {
                        bgaFiles[idx].assign(value.begin(), value.end());
                        if (!ifStack.empty()) resourceStable = false;
                    }
                }
                else if (RE2::FullMatch(re2::StringPiece(key.data(), key.length()), *regexBpm))
                {
                    int idx = base36(key[3], key[4]);
                    if (idx != 0)
                        exBPM[idx] = toDouble(value);
                }
                else if (RE2::FullMatch(re2::StringPiece(key.data(), key.length()), *regexStop))
                {
                    int idx = base36(key[4], key[5]);
                    if (idx != 0)
                        stop[idx] = toDouble(value);
                }

                // unknown
                else
                    extraCommands[std::string(key)] = StringContent(value.begin(), value.end());
            }
            else // #zzzxy:......
            {
                auto colon_idx = buf.find_first_of(':');
                StringContentView key = buf.substr(1, 5);
                StringContentView value = buf.substr(7);

                if (value.empty())
                {
                    LOG_WARNING << "[BMS] Empty note line detected. " << absolutePath.u8string() << "@" << srcLine;
                    errorLine = srcLine;
                    errorCode = err::NOTE_LINE_ERROR;
                    return 1;
                }

                unsigned bar = toInt(key.substr(0, 3));
                if (lastBarIdx < bar)
                {
                    lastBarIdx = bar;
                }

                try
                {
                    int x_ = base36(key[3]);
                    int _y = base36(key[4]);

                    if (x_ == 0) // 0x: basic info
                    {
                        switch (_y)
                        {
                        case 1:            // 01: BGM
                            seqToLane36(chBGM[bgmLayersCount[bar]][bar], value);
                            ++bgmLayersCount[bar];
                            break;

                        case 2:            // 02: Bar Length
                            metres[bar] = toDouble(value);
                            haveMetricMod = true;
                            break;

                        case 3:            // 03: BPM change
                            seqToLane16(chBPMChange[bar], value);
                            haveBPMChange = true;
                            break;

                        case 4:            // 04: BGA Base
                            seqToLane36(chBGABase[bar], value);
                            haveBGA = true;
                            break;

                        case 6:            // 06: BGA Poor
                            seqToLane36(chBGAPoor[bar], value);
                            haveBGA = true;
                            break;

                        case 7:            // 07: BGA Layer
                            seqToLane36(chBGALayer[bar], value);
                            haveBGA = true;
                            break;

                        case 8:            // 08: ExBPM
                            seqToLane36(chExBPMChange[bar], value);
                            haveBPMChange = true;
                            break;

                        case 9:            // 09: Stop
                            seqToLane36(chStop[bar], value);
                            haveStop = true;
                            break;
                        }
                    }
                    else // not 0x
                    {
                        if (!isPMS && _y == 7)
                        {
                            LOG_WARNING << "[BMS] #xxxX7 lanes are not supported. " << absolutePath.u8string() << "@" << srcLine;
                            continue;
                        }

                        auto [side, idx] = isPMS ? getLaneIndexPMS(x_, _y) : getLaneIndexBME(x_, _y);
                        unsigned chIdx = idx + (side == 1 ? 10 : 0);
                        if (side >= 0)
                        {
                            switch (x_)
                            {
                            case 1:            // 1x: 1P visible
                            case 2:            // 2x: 2P visible
                                seqToLane36(chNotesRegular[chIdx][bar], value);
                                haveNote = true;
                                if (side == 1) haveAny_2 = true;
                                break;
                            case 3:            // 3x: 1P invisible
                            case 4:            // 4x: 2P invisible
                                seqToLane36(chNotesInvisible[chIdx][bar], value);
                                haveInvisible = true;
                                if (side == 1) haveAny_2 = true;
                                break;
                            case 5:            // 5x: 1P LN
                            case 6:            // 6x: 2P LN
                                haveLNchannels = true;
                                if (!lnobjSet.empty())
                                {
                                    // Note: there is so many possibilities of conflicting LN definition. Add all LN channel notes as regular notes
                                    channel noteLane;
                                    seqToLane36(noteLane, value, channel::NoteParseValue::LN);
                                    unsigned scale = chNotesRegular[chIdx][bar].relax(noteLane.resolution) / noteLane.resolution;
                                    for (auto& note : noteLane.notes)
                                    {
                                        note.segment *= scale;
                                        bool noteInserted = false;
                                        channel& chTarget = chNotesRegular[chIdx][bar];
                                        for (auto it = chTarget.notes.begin(); it != chTarget.notes.end(); ++it)
                                        {
                                            if (it->segment > note.segment || (it->segment == note.segment && it->value > note.value))
                                            {
                                                chTarget.notes.insert(it, note);
                                                noteInserted = true;
                                                break;
                                            }
                                        }
                                        if (!noteInserted)
                                        {
                                            chTarget.notes.push_back(note);
                                        }
                                    }
                                }
                                else
                                {
                                    // #LNTYPE 1
                                    seqToLane36(chNotesLN[chIdx][bar], value, channel::NoteParseValue::LN);
                                    haveLN = true;
                                    if (side == 1) haveAny_2 = true;
                                }
                                break;
                            case 0xD:        // Dx: 1P mine
                            case 0xE:        // Ex: 2P mine
                                seqToLane36(chMines[chIdx][bar], value);
                                haveMine = true;
                                break;
                            }

                            switch (idx)
                            {
                            case 6:
                            case 7:
                                if (side == 1)
                                    have67_2 = true;
                                else
                                    have67 = true;
                                break;
                            case 8:
                            case 9:
                                if (side == 1)
                                    have89_2 = true;
                                else
                                    have89 = true;
                                break;
                            }
                        }
                    }
                }
                catch (noteLineException& e)
                {
                    LOG_WARNING << "[BMS] Line error. " << absolutePath.u8string() << "@" << srcLine;
                }
            }
        }
        catch (noteLineException)
        {
            errorCode = err::NOTE_LINE_ERROR;
            throw;
        }
        catch (std::invalid_argument)
        {
            errorCode = err::TYPE_MISMATCH;
            throw;
        }
        catch (std::out_of_range)
        {
            errorCode = err::VALUE_ERROR;
            throw;
        }
        catch (std::exception)
        {
            errorLine = srcLine;
            return 1;
        }
    }

    // implicit subtitle
    if (title2.empty())
    {
        static const LazyRE2 subTitleRegex[]
        {
            { R"((.+?) *(-.*?-))" },
            { R"((.+?) *(〜.*?〜))" },
            { R"((.+?) *(\(.*?\)))" },
            { R"((.+?) *(\[.*?\]))" },
            { R"((.+?) *(<.*?>))" },
        };
        for (auto& reg : subTitleRegex)
        {
            std::string title1, title2;
            if (RE2::FullMatch(title, *reg, &title1, &title2))
            {
                this->title = title1;
                this->title2 = title2;
                break;
            }
        }
    }

    // implicit difficulty
    if (!hasDifficulty)
    {
        static const LazyRE2 difficultyRegex[]
        {
            { "" },
            { R"((?i)(easy|beginner|light))" },
            { R"((?i)(normal|standard))" },
            { R"((?i)(hard|hyper))" },
            { R"((?i)(ex|another|insane|lunatic|maniac))" },
        };
        difficulty = -1;
        for (int i = 4; i >= 1; --i)
        {
            if (RE2::PartialMatch(version, *difficultyRegex[i]))
            {
                difficulty = i;
                break;
            }
            if (RE2::PartialMatch(title2, *difficultyRegex[i]))
            {
                difficulty = i;
                break;
            }
            if (RE2::PartialMatch(title, *difficultyRegex[i]))
            {
                difficulty = i;
                break;
            }
        }
        if (difficulty == -1)
            difficulty = 2; // defaults to normal
    }

    for (size_t i = 0; i <= lastBarIdx; i++)
        if (metres[i].toDouble() == 0.0)
            metres[i] = Metre(4, 4);

    // pick LNs out of notes for each lane
    if (!lnobjSet.empty() || haveLNchannels)
    {
        for (unsigned chIdx = 0; chIdx < 20; ++chIdx)
        {
            if (chNotesRegular.find(chIdx) == chNotesRegular.end()) continue;

            std::list<size_t> modifiedChannels;
            decltype(std::declval<channel>().notes.begin()) LNhead;
            unsigned bar_head = 0;
            unsigned resolution_head = 1;
            unsigned bar_curr = 0;
            bool hasHead = false;

            // find next LN head
            for (; bar_curr <= lastBarIdx; bar_curr++)
            {
                if (chNotesRegular[chIdx][bar_curr].notes.empty()) continue;

                auto& noteList = chNotesRegular[chIdx][bar_curr].notes;
                auto itNote = noteList.begin();
                while (itNote != noteList.end())
                {
                    // Regular note inside a LN (can be seen with o2mania + #LNTYPE 1) is not allowed. Handle any following note as LN tail.
                    if (hasHead && (lnobjSet.count(itNote->value) || (LNhead->flags & channel::NoteParseValue::LN)))
                    {
                        unsigned segment = LNhead->segment * chNotesLN[chIdx][bar_head].relax(resolution_head) / resolution_head;
                        unsigned value = LNhead->value;
                        unsigned resolution_tail = chNotesRegular[chIdx][bar_curr].resolution;
                        unsigned segment2 = itNote->segment * chNotesLN[chIdx][bar_curr].relax(resolution_tail) / resolution_tail;
                        unsigned value2 = itNote->value;
                        chNotesLN[chIdx][bar_head].notes.push_back({ segment, value });
                        chNotesLN[chIdx][bar_curr].notes.push_back({ segment2, value2 });

                        haveLN = true;

                        chNotesRegular[chIdx][bar_head].notes.erase(LNhead);
                        auto itPrev = itNote;
                        bool resetItNote = (itNote == noteList.begin());
                        if (!resetItNote) itPrev--;
                        chNotesRegular[chIdx][bar_curr].notes.erase(itNote);
                        itNote = resetItNote ? noteList.begin() : ++itPrev;

                        modifiedChannels.push_back(bar_head);
                        modifiedChannels.push_back(bar_curr);

                        bar_head = 0;
                        resolution_head = 1;
                        hasHead = false;
                    }
                    else
                    {
                        LNhead = itNote;
                        bar_head = bar_curr;
                        resolution_head = chNotesRegular[chIdx][bar_curr].resolution;
                        hasHead = true;

                        ++itNote;
                    }
                }
            }

            if (bar_head <= lastBarIdx) chNotesLN[chIdx][bar_head].sortNotes();
            if (bar_curr <= lastBarIdx) chNotesLN[chIdx][bar_curr].sortNotes();
        }
    }

    // Get statistics
    if (haveNote)
    {
        for (unsigned bar = 0; bar <= lastBarIdx; bar++)
        {
            notes_scratch += chNotesRegular[0][bar].notes.size() + chNotesRegular[10][bar].notes.size();
        }

        for (unsigned lane = 0; lane < chNotesRegular.size(); ++lane)
        {
            if (lane == 0 || lane == 10) continue;
            for (const auto& [barIdx, ch]: chNotesRegular[lane])
            {
                notes_key += ch.notes.size();
            }
        }
        notes_total += notes_scratch + notes_key;
    }

    if (haveLN)
    {
        for (unsigned bar = 0; bar <= lastBarIdx; bar++)
        {
            notes_scratch_ln += chNotesLN[0][bar].notes.size() + chNotesLN[10][bar].notes.size();
        }
        notes_scratch_ln /= 2;

        for (unsigned lane = 0; lane < chNotesLN.size(); ++lane)
        {
            if (lane == 0 || lane == 10) continue;
            for (const auto& [barIdx, ch] : chNotesLN[lane])
            {
                notes_key_ln += ch.notes.size();
            }
        }
        notes_key_ln /= 2;

        notes_total += notes_scratch_ln + notes_key_ln;
    }

    if (haveMine)
    {
        for (const auto& [chIdx, chLane] : chMines)
        {
            for (const auto& [barIdx, ch] : chLane)
            {
                notes_mine += ch.notes.size();
            }
        }
    }

    minBPM = bpm;
    maxBPM = bpm;
    startBPM = bpm;
    if (haveBPMChange)
    {
        for (unsigned m = 0; m <= lastBarIdx; m++)
        {
            for (const auto& ns : chBPMChange[m].notes)
            {
                if (ns.value > maxBPM) maxBPM = ns.value;
                if (ns.value < minBPM) minBPM = ns.value;
            }
            for (const auto& ns : chExBPMChange[m].notes)
            {
                if (exBPM[ns.value] > maxBPM) maxBPM = exBPM[ns.value];
                if (exBPM[ns.value] < minBPM) minBPM = exBPM[ns.value];
            }
        }
    }

    if (isPMS)
    {
        gamemode = 9;
        if (have89)
        {
            // 11	12	13	14	15	18	19	16	17	not known or well known
            player = 1;
            std::swap(chNotesRegular[6], chNotesRegular[8]);
            std::swap(chNotesRegular[7], chNotesRegular[9]);
            std::swap(chNotesInvisible[6], chNotesInvisible[8]);
            std::swap(chNotesInvisible[7], chNotesInvisible[9]);
            std::swap(chNotesLN[6], chNotesLN[8]);
            std::swap(chNotesLN[7], chNotesLN[9]);
			std::swap(chMines[6], chMines[8]);
			std::swap(chMines[7], chMines[9]);
            have67 = true;

            if (have67_2)
            {
                // 21	22	23	24	25	28	29	26	27	2P-side (right)
                // 18KEYS is not supported. Parse as 9KEYS
                gamemode = 9;
                player = 1;
                std::swap(chNotesRegular[16], chNotesRegular[18]);
                std::swap(chNotesRegular[17], chNotesRegular[19]);
                std::swap(chNotesInvisible[16], chNotesInvisible[18]);
                std::swap(chNotesInvisible[17], chNotesInvisible[19]);
                std::swap(chNotesLN[16], chNotesLN[18]);
                std::swap(chNotesLN[17], chNotesLN[19]);
                have67_2 = true;
            }
        }
        else
        {
            // 11	12	13	14	15	22	23	24	25	standard PMS
            player = 1;
            std::swap(chNotesRegular[6], chNotesRegular[12]);
            std::swap(chNotesRegular[7], chNotesRegular[13]);
            std::swap(chNotesRegular[8], chNotesRegular[14]);
            std::swap(chNotesRegular[9], chNotesRegular[15]);
            std::swap(chNotesInvisible[6], chNotesInvisible[12]);
            std::swap(chNotesInvisible[7], chNotesInvisible[13]);
            std::swap(chNotesInvisible[8], chNotesInvisible[14]);
            std::swap(chNotesInvisible[9], chNotesInvisible[15]);
            std::swap(chNotesLN[6], chNotesLN[12]);
            std::swap(chNotesLN[7], chNotesLN[13]);
            std::swap(chNotesLN[8], chNotesLN[14]);
            std::swap(chNotesLN[9], chNotesLN[15]);
			std::swap(chMines[6], chMines[12]);
            std::swap(chMines[7], chMines[13]);
            std::swap(chMines[8], chMines[14]);
            std::swap(chMines[9], chMines[15]);
            have67 = true;
            have89 = true;
            haveAny_2 = false;
            have67_2 = false;
            have89_2 = false;
        }
    }
    else
    {
        if (player == 0)
        {
            player = (have67_2 || haveAny_2) ? 3 : 1;
        }
        if (have67 || have67_2)
        {
            gamemode = (player == 1 ? 7 : 14);
        }
        else
        {
            gamemode = (player == 1 ? 5 : 10);
        }
    }

    fileHash = md5file(absolutePath);
    LOG_INFO << "[BMS] " << absolutePath.u8string() << " MD5: " << fileHash.hexdigest();

    loaded = true;

    return 0;
}

std::string ChartFormatBMS::getError()
{
    using err = ErrorCode;
    switch (errorCode)
    {
    case err::OK:    return "No errors.";
        //TODO return translated strings 
    }

    return "?";
}

int ChartFormatBMS::seqToLane36(channel& ch, StringContentView str, unsigned flags)
{
    //if (str.length() % 2 != 0)
    //    throw new noteLineException;

    size_t length = 0;

    for (auto c : str)
    {
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
        {
            //throw new noteLineException;
            break;
        }
        length++;
    }
    if (length / 2 == 0) return 1;

    unsigned resolution = static_cast<unsigned>(length / 2);
    unsigned scale = ch.relax(resolution) / resolution;
    for (unsigned i = 0; i < resolution; i++)
    {
        unsigned segment = i * scale;
        unsigned value = base36(str[i * 2], str[i * 2 + 1]);
        if (value == 0) continue;

        ch.notes.push_back({ segment, value, flags });
    }
    ch.sortNotes();

    return 0;
}
int ChartFormatBMS::seqToLane36(channel& ch, const StringContent& str, unsigned flags)
{
    return seqToLane36(ch, StringContentView(str), flags);
}

int ChartFormatBMS::seqToLane16(channel& ch, StringContentView str)
{
    //if (str.length() % 2 != 0)
    //    throw new noteLineException;

    size_t length = 0;

    for (auto c : str)
    {
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')))
        {
            //throw new noteLineException;
            break;
        }
        length++;
    }
    if (length / 2 == 0) return 1;

    unsigned resolution = static_cast<unsigned>(length / 2);
    unsigned scale = ch.relax(resolution) / resolution;
    for (unsigned i = 0; i < resolution; i++)
    {
        unsigned segment = i * scale;
        unsigned value = base16(str[i * 2], str[i * 2 + 1]);
        if (value == 0) continue;

        ch.notes.push_back({ segment, value });
    }
    ch.sortNotes();

    return 0;
}
int ChartFormatBMS::seqToLane16(channel& ch, const StringContent& str)
{
    return seqToLane16(ch, StringContentView(str));
}

std::pair<int, int> ChartFormatBMS::getLaneIndexBME(int x_, int _y)
{
    int side = 0;
    int idx = 0;
    switch (x_)
    {
    case 1:            // 1x: 1P visible
    case 3:            // 3x: 1P invisible
    case 5:            // 5x: 1P LN
    case 0xD:          // Dx: 1P mine
        side = 0;
        break;
    case 2:            // 2x: 2P visible
    case 4:            // 4x: 2P invisible
    case 6:            // 6x: 2P LN
    case 0xE:          // Ex: 2P mine
        side = 1;
        break;
    default:
        side = -1;
        break;
    }
    if (side >= 0)
    {
        switch (_y)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            idx = _y;
            break;
        case 6:        //SCR
            idx = 0;
            break;
        case 8:        //6
            idx = 6;
            break;
        case 9:        //7
            idx = 7;
            break;
        case 7:        //Free zone / pedal
            idx = 9;
            break;
        }
    }
    return { side, idx };
}

std::pair<int, int> ChartFormatBMS::getLaneIndexPMS(int x_, int _y)
{
    int side = 0;
    int idx = 0;
    switch (x_)
    {
    case 1:            // 1x: 1P visible
    case 3:            // 3x: 1P invisible
    case 5:            // 5x: 1P LN
    case 0xD:          // Dx: 1P mine
        side = 0;
        break;
    case 2:            // 2x: 2P visible
    case 4:            // 4x: 2P invisible
    case 6:            // 6x: 2P LN
    case 0xE:          // Ex: 2P mine
        side = 1;
        break;
    default:
        side = -1;
        break;
    }
    if (side >= 0)
    {
        // return as-is and handle after parsing completed. PMS 6-9 lanes definition may vary
        idx = _y;
    }
    return { side, idx };
}

auto ChartFormatBMS::getLane(LaneCode code, unsigned chIdx, unsigned barIdx) const -> const channel&
{
    static const channel emptyChannel;
    if (barIdx <= lastBarIdx)
    {
        auto getCommonLane = [](const LaneMap& ch, unsigned barIdx) -> const channel&
        {
            if (ch.find(barIdx) != ch.end())
            {
                return ch.at(barIdx);
            }
            else
            {
                return emptyChannel;
            }
        };

        auto getNoteLane = [&](const std::map<unsigned, LaneMap>& ch, unsigned chIdx, unsigned barIdx) -> const channel&
        {
            if (ch.find(chIdx) != ch.end())
            {
                return getCommonLane(ch.at(chIdx), barIdx);
            }
            else
            {
                return emptyChannel;
            }
        };

        using eC = LaneCode;
        switch (code)
        {
        case eC::BGM:          return getNoteLane(chBGM, chIdx, barIdx); 
        case eC::BPM:          return getCommonLane(chBPMChange, barIdx); 
        case eC::EXBPM:        return getCommonLane(chExBPMChange, barIdx); 
        case eC::STOP:         return getCommonLane(chStop, barIdx); 
        case eC::BGABASE:      return getCommonLane(chBGABase, barIdx); 
        case eC::BGALAYER:     return getCommonLane(chBGALayer, barIdx); 
        case eC::BGAPOOR:      return getCommonLane(chBGAPoor, barIdx); 
        case eC::NOTE1:        return getNoteLane(chNotesRegular, chIdx, barIdx); 
        case eC::NOTE2:        return getNoteLane(chNotesRegular, chIdx + 10, barIdx); 
        case eC::NOTEINV1:     return getNoteLane(chNotesInvisible, chIdx, barIdx); 
        case eC::NOTEINV2:     return getNoteLane(chNotesInvisible, chIdx + 10, barIdx);
        case eC::NOTELN1:      return getNoteLane(chNotesLN, chIdx, barIdx); 
        case eC::NOTELN2:      return getNoteLane(chNotesLN, chIdx + 10, barIdx); 
        case eC::NOTEMINE1:    return getNoteLane(chMines, chIdx, barIdx); 
        case eC::NOTEMINE2:    return getNoteLane(chMines, chIdx + 10, barIdx); 
        default: break;
        }
    }

    assert(false);
    return emptyChannel;
}

unsigned ChartFormatBMS::channel::relax(unsigned target_resolution)
{
    unsigned target = std::lcm(target_resolution, resolution);
    unsigned pow = target / resolution;
    if (pow == 1) return target;

    resolution = target;
    for (auto& n : notes)
    {
        n.segment *= pow;
    }
    return target;
}

void ChartFormatBMS::channel::sortNotes()
{
    std::vector<NoteParseValue> vec(notes.begin(), notes.end());
    std::stable_sort(vec.begin(), vec.end(), [](const NoteParseValue& lhs, const NoteParseValue& rhs)
        {
            return lhs.segment < rhs.segment;
        });
    notes.assign(vec.begin(), vec.end());
}