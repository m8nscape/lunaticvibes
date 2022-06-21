#include "chartformat_bms.h"
#include "common/log.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <set>
#include <utility>
#include <exception>
#include <filesystem>
#include <numeric>
#include <random>
#include "db/db_song.h"

class noteLineException : public std::exception {};

int BMS::getExtendedProperty(const std::string& key, void* ret)
{
    if (strEqual(key, "PLAYER", true))
    {
        *(int*)ret = player;
    }
    if (strEqual(key, "RANK", true))
    {
        *(int*)ret = rank;
    }
    if (strEqual(key, "PLAYLEVEL", true))
    {
        *(int*)ret = playLevel;
    }
    if (strEqual(key, "TOTAL", true))
    {
        *(double*)ret = total;
    }
    if (strEqual(key, "DIFFICULTY", true))
    {
        *(int*)ret = difficulty;
    }
    return -1;
}

BMS::BMS() {
    wavFiles.resize(MAXSAMPLEIDX + 1);
    bgaFiles.resize(MAXSAMPLEIDX + 1);
    metres.resize(MAXBARIDX + 1);
}

BMS::BMS(const Path& file): BMS_prop() {
    wavFiles.resize(MAXSAMPLEIDX + 1);
    bgaFiles.resize(MAXSAMPLEIDX + 1);
    metres.resize(MAXBARIDX + 1);
    initWithFile(file);
}

int BMS::initWithPathParam(const SongDB& db)
{
    if (filePath.is_absolute())
        absolutePath = filePath;
    else
    {
        Path fp;
        db.getFolderPath(folderHash, fp);
        absolutePath = fp / filePath;
    }

    return initWithFile(absolutePath);
}

int BMS::initWithFile(const Path& file)
{
    using err = ErrorCode;
    if (_loaded)
    {
        //errorCode = err::ALREADY_INITIALIZED;
        //errorLine = 0;
        return 1;
    }

    filePath = file.filename();
    absolutePath = std::filesystem::absolute(file);
    LOG_INFO << "[BMS] File: " << absolutePath.u8string();
    std::ifstream fs(absolutePath.c_str());
    if (fs.fail())
    {
        errorCode = err::FILE_ERROR;
        errorLine = 0;
        LOG_INFO << "[BMS] File ERROR";
        return 1;
    }

    auto encoding = getFileEncoding(absolutePath);

    // TODO 天国的PARSER！！！
    // SUPER LAZY PARSER FOR TESTING

    StringContent buf;
    unsigned srcLine = 0;

    // #RANDOM related parameters
    int randomValue = 0;
    int ifBlock = 0;
    std::stack<int> ifValue;

    // implicit parameters
    bool hasDifficulty = false;

    while (!fs.eof())
    {
        std::getline(fs, buf, '\n');
        srcLine++;
        if (buf.length() <= 1) continue;

        // remove not needed spaces
        buf = buf.substr(buf.find_first_not_of(' '),  buf.find_first_of('\r') - buf.find_first_not_of(' '));

        // convert codepage
        buf = to_utf8(buf, encoding);

        if (buf[0] != '#') continue;

        // skip comments
        bool skip = false;
        static const std::vector<std::regex> skipRegex
        {
            std::regex(R"(\*-.*)", prebuiltRegexFlags),     // *-
            std::regex(R"(\/\/.*)", prebuiltRegexFlags),    // //
            std::regex(R"(;.*)", prebuiltRegexFlags),       // ;
            std::regex(R"(#BPM00 .*)", prebuiltRegexFlags),
            std::regex(R"(#STOP00 .*)", prebuiltRegexFlags),
            std::regex(R"(#BMP00 .*)", prebuiltRegexFlags),
        };
        for (auto& reg : skipRegex)
        {
            if (std::regex_match(buf, reg))
            {
                skip = true;
                break;
            }
        }
        if (skip) continue;

        // parsing
        try
        {
            if (randomValue != 0)
            {
                // skip orphan blocks
                if (ifValue.empty())
                    continue;

                // skip mismatch IF value blocks
                if (ifValue.top() != randomValue)
                    continue;
            }

            auto colon_idx = buf.find_first_of(':');

            if (colon_idx != buf.npos)
            {
                StringContent key = buf.substr(1, colon_idx - 1);
                StringContent value = buf.substr(colon_idx + 1);
                if (value.empty())
                {
                    LOG_WARNING << "[BMS] Empty note line detected: line " << srcLine;
                    errorLine = srcLine;
                    errorCode = err::NOTE_LINE_ERROR;
                    return 1;
                }

                static const std::regex regexNotes = std::regex(R"(\d{3}[0-9A-Za-z]{2})", prebuiltRegexFlags);

                // lastBarIdx & channels
                if (std::regex_match(key, regexNotes))
                {
                    unsigned measure = toInt(key.substr(0, 3));
                    if (lastBarIdx < measure) lastBarIdx = measure;

                    int layer = base36(key[3]);
                    int ch = base36(key[4]);

                    if (layer == 0) // 0x: basic info
                    {
                        switch (ch)
                        {
                        case 1:            // 01: BGM
                            if (bgmLayersCount[measure] >= chBGM.size())
                            {
                                chBGM.emplace_back();
                                strToLane36(chBGM.back()[measure], value);
                            }
                            else
                            {
                                strToLane36(chBGM[bgmLayersCount[measure]][measure], value);
                            }
                            ++bgmLayersCount[measure];
                            break;

                        case 2:            // 02: Bar Length
                            metres[measure] = toDouble(value);
                            haveMetricMod = true;
                            break;

                        case 3:            // 03: BPM change
                            strToLane16(chBPMChange[measure], value);
                            haveBPMChange = true;
                            break;

                        case 4:            // 04: BGA Base
                            strToLane36(chBGABase[measure], value);
                            haveBGA = true;
                            break;

                        case 6:            // 06: BGA Poor
                            strToLane36(chBGAPoor[measure], value);
                            haveBGA = true;
                            break;

                        case 7:            // 07: BGA Layer
                            strToLane36(chBGALayer[measure], value);
                            haveBGA = true;
                            break;

                        case 8:            // 08: ExBPM
                            strToLane36(chExBPMChange[measure], value);
                            haveStop = true;
                            break;

                        case 9:            // 09: Stop
                            strToLane36(chStop[measure], value);
                            haveStop = true;
                            break;
                        }
                    }
                    else // layer != 0
                    {
                        auto [area, idx] = normalizeIndexesBME(layer, ch);
                        assert(area < sizeof(chNotesRegular.lanes) / sizeof(chNotesRegular.lanes[0]));
                        assert(idx < sizeof(chNotesRegular.lanes[0]) / sizeof(chNotesRegular.lanes[0][0]));
                        assert(measure < sizeof(chNotesRegular.lanes[0][0]) / sizeof(chNotesRegular.lanes[0][0][0]));
                        switch (layer)
                        {
                        case 1:            // 1x: 1P visible
                        case 2:            // 2x: 2P visible
                            strToLane36(chNotesRegular.lanes[area][idx][measure], value);
                            haveNote = true;
                            break;
                        case 3:            // 3x: 1P invisible
                        case 4:            // 4x: 2P invisible
                            strToLane36(chNotesInvisible.lanes[area][idx][measure], value);
                            haveInvisible = true;
                            break;
                        case 5:            // 5x: 1P LN
                        case 6:            // 6x: 2P LN
                            strToLane36(chNotesLN.lanes[area][idx][measure], value);
                            haveLN = true;
                            break;
                        case 0xD:        // Dx: 1P mine
                        case 0xE:        // Ex: 2P mine
                            strToLane36(chMines.lanes[area][idx][measure], value);
                            haveMine = true;
                            break;
                        }
                    }
                }
            }
            else
            {
                auto space_idx = std::min(buf.length(), buf.find_first_of(' '));
                if (space_idx <= 1) continue;

                StringContent key = buf.substr(1, space_idx - 1);
                StringContent value = space_idx < buf.length() ? buf.substr(space_idx + 1) : "";

                static const std::regex regexWav = std::regex(R"(WAV[0-9A-Za-z]{1,2})", prebuiltRegexFlags);
                static const std::regex regexBga = std::regex(R"(BMP[0-9A-Za-z]{1,2})", prebuiltRegexFlags);
                static const std::regex regexBpm = std::regex(R"(BPM[0-9A-Za-z]{1,2})", prebuiltRegexFlags);
                static const std::regex regexStop = std::regex(R"(STOP[0-9A-Za-z]{1,2})", prebuiltRegexFlags);

                // RANDOM
                if (strEqual(key, "RANDOM", true))
                {
                    haveRandom = true;
                    static std::mt19937_64 rng(std::time(nullptr));
                    randomValue = (rng() % toInt(value)) + 1;
                }
                else if (strEqual(key, "IF", true))
                {
                    ifBlock = toInt(value);
                    if (randomValue != 0)
                    {
                        if (randomValue == ifBlock)
                        {
                            ifValue.push(ifBlock);
                        }
                    }
                    else
                    {
                        LOG_WARNING << "[BMS] orphan #IF found in line " << srcLine;
                    }
                }
                else if (strEqual(key, "ENDIF", true))
                {
                    if (!ifValue.empty())
                    {
                        ifValue.pop();
                    }
                    else if (ifBlock == 0)
                    {
                        LOG_WARNING << "[BMS] unexpected #ENDIF fond at line " << srcLine;
                    }
                }
                else if (strEqual(key, "ENDRANDOM", true))
                {
                    if (!ifValue.empty())
                    {
                        LOG_WARNING << "[BMS] #ENDRANDOM found before #ENDIF at line " << srcLine;
                        randomValue = 0;
                        ifBlock = 0;
                        ifValue = {};
                    }
                    else
                    {
                        randomValue = 0;
                    }
                }
                else
                {
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
                        lnobjSet.insert(base36(value[0], value[1]));

                    // #xxx00
                    else if (std::regex_match(key, regexWav))
                    {
                        int idx = base36(key[3], key[4]);
                        wavFiles[idx].assign(value.begin(), value.end());
                    }
                    else if (std::regex_match(key, regexBga))
                    {
                        int idx = base36(key[3], key[4]);
                        bgaFiles[idx].assign(value.begin(), value.end());
                    }
                    else if (std::regex_match(key, regexBpm))
                    {
                        int idx = base36(key[3], key[4]);
                        exBPM[idx] = toDouble(value);
                    }
                    else if (std::regex_match(key, regexStop))
                    {
                        int idx = base36(key[4], key[5]);
                        stop[idx] = toDouble(value);
                    }

                    // unknown
                    else
                        extraCommands[key] = StringContent(value.begin(), value.end());
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
            fs.close();
            errorLine = srcLine;
            return 1;
        }
    }

    fs.close();

    // implicit subtitle
    if (title2.empty())
    {
        static const std::vector<std::regex> subTitleRegex
        {
            std::regex(R"((.+) *(-.*?-))", prebuiltRegexFlags),
            std::regex(R"((.+) *(〜.*?〜))", prebuiltRegexFlags),
            std::regex(R"((.+) *(\(.*?\)))", prebuiltRegexFlags),
            std::regex(R"((.+) *(\[.*?\]))", prebuiltRegexFlags),
            std::regex(R"((.+) *(<.*?>))", prebuiltRegexFlags),
        };
        for (auto& reg : subTitleRegex)
        {
            std::smatch sm;
            if (std::regex_match(title, sm, reg))
            {
                title2 = sm[2].str();
                title = sm[1].str();
                break;
            }
        }
    }

    // implicit difficulty
    if (!hasDifficulty)
    {
        static const std::vector<std::regex> difficultyRegex
        {
            std::regex(""),
            std::regex(R"((easy|beginner|light))", prebuiltRegexFlags),
            std::regex(R"((normal|standard))", prebuiltRegexFlags),
            std::regex(R"((hard|hyper))", prebuiltRegexFlags),
            std::regex(R"((ex|another|insane))", prebuiltRegexFlags),
        };
        difficulty = -1;
        for (int i = 1; i <= 4; ++i)
        {
            if (std::regex_search(title2, difficultyRegex[i]))
                difficulty = i;
        }
        if (difficulty == -1)
            difficulty = 2; // defaults to normal
    }

    for (size_t i = 0; i <= lastBarIdx; i++)
        if (metres[i].toDouble() == 0.0)
            metres[i] = Metre(4, 4);

    // pick LNs out of notes for each lane
    for (int area = 0; area < 2; ++area)
    {
        for (size_t ch = 0; ch < PlayAreaLanes::LANE_COUNT; ++ch)
        {
            decltype(chNotesLN.lanes[0][ch][0].notes.begin()) it_head;
            decltype(&chNotesLN.lanes[0][ch][0].notes) notes_head = nullptr;
            unsigned resolution_head = 1;
            unsigned bar_head = 0;
            unsigned bar_curr = 0;

            // find next LN head
            for (; bar_curr <= lastBarIdx; bar_curr++)
            {
                if (!chNotesLN.lanes[area][ch][bar_curr].notes.empty())
                {
                    notes_head = &chNotesRegular.lanes[area][ch][bar_curr].notes;
                    it_head = notes_head->begin();
                    resolution_head = chNotesRegular.lanes[area][ch][bar_curr].resolution;
                    bar_head = bar_curr;
                    break;
                }
            }
            if (!notes_head) continue;

            // find next LN note as LN tail
            for (; bar_curr <= lastBarIdx; bar_curr++)
            {
                auto& notes_tail = chNotesRegular.lanes[area][ch][bar_curr].notes;
                unsigned res_tail = chNotesRegular.lanes[area][ch][bar_curr].resolution;
                if (notes_tail.empty()) continue;

                auto it_tail = notes_tail.begin();
                while (it_tail != notes_tail.end())
                {
                    if (notes_head && it_head == notes_head->end())
                    {
                        notes_head = &notes_tail;
                        it_head = notes_head->begin();
                        resolution_head = res_tail;
                        bar_head = bar_curr;
                    }

                    if (notes_head == &notes_tail && it_head == it_tail)
                    {
                        ++it_tail;
                        continue;
                    }

                    if (lnobjSet.count(it_tail->value) > 0)
                    {
                        it_head->segment *= chNotesLN.lanes[area][ch][bar_head].relax(resolution_head) / resolution_head;
                        chNotesLN.lanes[area][ch][bar_head].notes.push_back(*it_head);
                        auto tmp1 = it_head++;
                        notes_head->erase(tmp1);

                        if (notes_head == &notes_tail && it_head == it_tail)
                            ++it_head;

                        it_tail->segment *= chNotesLN.lanes[area][ch][bar_curr].relax(res_tail) / res_tail;
                        chNotesLN.lanes[area][ch][bar_curr].notes.push_back(*it_tail);
                        auto tmp2 = it_tail++;
                        notes_tail.erase(tmp2);

                        haveLN = true;
                    }
                    else
                    {
                        ++it_head;
                        ++it_tail;
                    }
                }
            }
        }
    }

    // Get statistics
    if (haveNote)
    {
        for (int area = 0; area < 2; ++area)
            for (const auto& ch : chNotesRegular.lanes[area])
                for (unsigned bar = 0; bar <= lastBarIdx; bar++)
                    notes += ch[bar].notes.size();
    }

    if (haveLN)
    {
        for (int area = 0; area < 2; ++area)
            for (const auto& ch : chNotesLN.lanes[area])
                for (unsigned bar = 0; bar <= lastBarIdx; bar++)
                    notes_ln += ch[bar].notes.size();
        notes += notes_ln / 2;
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

    if (have89)
    {
        gamemode = 9;   // 18?
    }
    else if (have67)
    {
        gamemode = (player == 1 ? 7 : 14);
    }
    else
    {
        gamemode = (player == 1 ? 5 : 10);
    }

    fileHash = md5file(absolutePath);
    LOG_INFO << "[BMS] MD5: " << fileHash.hexdigest();

    _loaded = true;
    LOG_INFO << "[BMS] Parsing BMS complete.";

    return 0;
}

int BMS::strToLane36(channel& ch, const StringContent& str)
{
    //if (str.length() % 2 != 0)
    //    throw new noteLineException;

    for (auto c: str)
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
            throw new noteLineException;

    ch.resolution = static_cast<unsigned>(str.length() / 2);
    for (unsigned i = 0; i < ch.resolution; i++)
        if (unsigned sample = base36(str[i * 2], str[i * 2 + 1]))
            ch.notes.push_back({ i, sample });
    return 0;
}

int BMS::strToLane16(channel& ch, const StringContent& str)
{
    //if (str.length() % 2 != 0)
    //    throw new noteLineException;

    for (auto c : str)
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')))
            throw new noteLineException;

    ch.resolution = static_cast<unsigned>(str.length() / 2);
    for (unsigned i = 0; i < ch.resolution; i++)
        if (unsigned sample = base16(str[i * 2], str[i * 2 + 1]))
            ch.notes.push_back({ i, sample });
    return 0;
}

std::pair<int, int> BMS::normalizeIndexesBME(int layer, int ch)
{
    int area = 0;
    int idx = 0;
    switch (layer)
    {
    case 1:            // 1x: 1P visible
    case 3:            // 3x: 1P invisible
    case 5:            // 5x: 1P LN
    case 0xD:          // Dx: 1P mine
        area = 0;
        break;
    case 2:            // 2x: 2P visible
    case 4:            // 4x: 2P invisible
    case 6:            // 6x: 2P LN
    case 0xE:          // Ex: 2P mine
        area = 1;
        break;
    }
    switch (ch)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        idx = ch;
        break;
    case 6:        //SCR
        idx = 0;
        break;
    case 8:        //6
    case 9:        //7
        have67 = true;
        idx = 6 + (ch - 8);
        break;
    case 7:        //9 of PMS BME
        have89 = true;
        idx = 9;
        break;
    }
    return { area, idx };
}

std::string BMS::getError()
{
    using err = ErrorCode;
    switch (errorCode)
    {
    case err::OK:    return "No errors.";
        //TODO return translated strings 
    }

    return "?";
}

int BMS::getMode() const
{
    if (player != 1)
        return (have67) ? MODE_14KEYS : MODE_10KEYS;
    else
        return (have89) ? MODE_9KEYS : (have67) ? MODE_7KEYS : MODE_5KEYS;
}

auto BMS::getLane(LaneCode code, unsigned chIdx, unsigned measureIdx) const -> const decltype(chBGM[0][0])&
{
    using eC = LaneCode;
    switch (code)
    {
    case eC::BGM:          return chBGM[chIdx][measureIdx]; break;
    case eC::BPM:          return chBPMChange[measureIdx]; break;
    case eC::EXBPM:        return chExBPMChange[measureIdx]; break;
    case eC::STOP:         return chStop[measureIdx]; break;
    case eC::BGABASE:      return chBGABase[measureIdx]; break;
    case eC::BGALAYER:     return chBGALayer[measureIdx]; break;
    case eC::BGAPOOR:      return chBGAPoor[measureIdx]; break;;
    case eC::NOTE1:        return chNotesRegular.lanes[0][chIdx][measureIdx]; break;
    case eC::NOTE2:        return chNotesRegular.lanes[1][chIdx][measureIdx]; break;
    case eC::NOTEINV1:     return chNotesInvisible.lanes[0][chIdx][measureIdx]; break;
    case eC::NOTEINV2:     return chNotesInvisible.lanes[1][chIdx][measureIdx]; break;
    case eC::NOTELN1:      return chNotesLN.lanes[0][chIdx][measureIdx]; break;
    case eC::NOTELN2:      return chNotesLN.lanes[1][chIdx][measureIdx]; break;
    case eC::NOTEMINE1:    return chMines.lanes[0][chIdx][measureIdx]; break;
    case eC::NOTEMINE2:    return chMines.lanes[1][chIdx][measureIdx]; break;
    default: break;
    }

    assert(false);
    return chBGM[0][0];
}

unsigned BMS::channel::relax(unsigned target_resolution)
{
    if (target_resolution < resolution) return 0;
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