#include <algorithm>

#include "scene_select.h"
#include "scene_context.h"
#include "chartformat/chart_types.h"

#include "entry/entry_song.h"

////////////////////////////////////////////////////////////////////////////////

void setBarInfo()
{
    const auto& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;
    const size_t count = int(eText::_SELECT_BAR_TITLE_FULL_MAX) - int(eText::_SELECT_BAR_TITLE_FULL_0) + 1;
    for (size_t list_idx = (e.size() + idx - cursor) % e.size(), i = 0; i < count; list_idx = (list_idx + 1) % e.size(), ++i)
    {
        // set title
        gTexts.set(eText(int(eText::_SELECT_BAR_TITLE_FULL_0) + i), e[list_idx]->_name);

        // set level
        if (e[list_idx]->type() == eEntryType::SONG)
        {
            auto ps = std::reinterpret_pointer_cast<Song>(e[list_idx]);

            switch (ps->_file->type())
            {
            case eChartFormat::BMS:
            {
                const auto bms = std::reinterpret_pointer_cast<const BMS_prop>(ps->_file);
                gNumbers.set(eNumber(int(eNumber::_SELECT_BAR_LEVEL_0) + i), bms->playLevel);
                break;
            }

            default:
                gNumbers.set(eNumber(int(eNumber::_SELECT_BAR_LEVEL_0) + i), 0);
                break;
            }
        }
    }

}

void setEntryInfo()
{
    const auto& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;

    // chart parameters
    if (e[idx]->type() == eEntryType::SONG || e[idx]->type() == eEntryType::RIVAL_SONG)
    {
        auto ps = std::reinterpret_pointer_cast<Song>(e[idx]);
        auto pf = std::reinterpret_pointer_cast<vChartFormat>(ps->_file);

        gSwitches.set(eSwitch::CHART_HAVE_README, 
            !(pf->text1.empty() && pf->text2.empty() && pf->text3.empty()));
        gSwitches.set(eSwitch::CHART_HAVE_BANNER, !pf->banner.empty());
        gSwitches.set(eSwitch::CHART_HAVE_STAGEFILE, !pf->stagefile.empty());

        gTexts.set(eText::PLAY_TITLE, pf->title);
        gTexts.set(eText::PLAY_SUBTITLE, pf->title2);
        gTexts.set(eText::PLAY_ARTIST, pf->artist);
        gTexts.set(eText::PLAY_SUBARTIST, pf->artist2);
        gTexts.set(eText::PLAY_GENRE, pf->genre);
        gTexts.set(eText::PLAY_DIFFICULTY, pf->version);
        // _level

        // _totalLength_sec
        gNumbers.set(eNumber::INFO_TOTALNOTE, pf->totalNotes);

        // _BG
        // _banner

        gNumbers.set(eNumber::PLAY_BPM, static_cast<int>(std::round(pf->startBPM)));
        gNumbers.set(eNumber::INFO_BPM_MIN, static_cast<int>(std::round(pf->minBPM)));
        gNumbers.set(eNumber::INFO_BPM_MAX, static_cast<int>(std::round(pf->maxBPM)));

        switch (ps->_file->type())
        {
        case eChartFormat::BMS:
        {
            const auto bms = std::reinterpret_pointer_cast<const BMS_prop>(pf);

            // gamemode
            switch (bms->player)
            {
            case 5:
            case 7:
            case 9:
            case 24:
                gOptions.set(eOption::CHART_PLAY_MODE, Option::PLAY_SINGLE);
                break;

            case 10:
            case 14:
            case 48:
                gOptions.set(eOption::CHART_PLAY_MODE, Option::PLAY_DOUBLE);
                break;

            default:
                break;
            }

            gNumbers.set(eNumber::PLAY_BPM, static_cast<int>(std::round(bms->bpm)));

            // gamemode
            unsigned op_keys = Option::KEYS_NOT_PLAYABLE;
            switch (bms->gamemode)
            {
            case 7:  op_keys = Option::KEYS_7; break;
            case 5:  op_keys = Option::KEYS_5; break;
            case 14: op_keys = Option::KEYS_14; break;
            case 10: op_keys = Option::KEYS_10; break;
            case 9:  op_keys = Option::KEYS_9; break;
            case 24: op_keys = Option::KEYS_24; break;
            case 48: op_keys = Option::KEYS_48; break;
            default: break;
            }
            gOptions.set(eOption::CHART_PLAY_KEYS, op_keys);

            // judge
            unsigned op_judgerank = Option::JUDGE_NORMAL;
            switch (bms->rank)
            {
            case 0: op_judgerank = Option::JUDGE_VHARD; break;
            case 1: op_judgerank = Option::JUDGE_HARD; break;
            case 2: op_judgerank = Option::JUDGE_NORMAL; break;
            case 3: op_judgerank = Option::JUDGE_EASY; break;
            default: break;
            }
            gOptions.set(eOption::CHART_JUDGE_TYPE, op_judgerank);

            // difficulty
            unsigned op_difficulty = Option::DIFF_0;
            gNumbers.set(eNumber::MUSIC_BEGINNER_LEVEL, 0);
            gNumbers.set(eNumber::MUSIC_NORMAL_LEVEL, 0);
            gNumbers.set(eNumber::MUSIC_HYPER_LEVEL, 0);
            gNumbers.set(eNumber::MUSIC_ANOTHER_LEVEL, 0);
            gNumbers.set(eNumber::MUSIC_INSANE_LEVEL, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_BEGINNER, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_NORMAL, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_HYPER, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_ANOTHER, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_INSANE, 0);
            switch (bms->difficulty)
            {
            case 0:
                op_difficulty = Option::DIFF_0; 
                break;
            case 1: 
                op_difficulty = Option::DIFF_1; 
                gNumbers.set(eNumber::MUSIC_BEGINNER_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_BEGINNER, bms->playLevel / 12.0); 
                break;
            case 2: 
                op_difficulty = Option::DIFF_2; 
                gNumbers.set(eNumber::MUSIC_NORMAL_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_NORMAL, bms->playLevel / 12.0);
                break;
            case 3: 
                op_difficulty = Option::DIFF_3; 
                gNumbers.set(eNumber::MUSIC_HYPER_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_HYPER, bms->playLevel / 12.0); 
                break;
            case 4: 
                op_difficulty = Option::DIFF_4; 
                gNumbers.set(eNumber::MUSIC_ANOTHER_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_ANOTHER, bms->playLevel / 12.0); 
                break;
            case 5: 
                op_difficulty = Option::DIFF_5; 
                gNumbers.set(eNumber::MUSIC_INSANE_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_INSANE, bms->playLevel / 12.0); 
                break;
            }
            gOptions.set(eOption::CHART_DIFFICULTY, op_difficulty);

            // TODO TOTAL

            gSwitches.set(eSwitch::CHART_HAVE_BGA, bms->haveBGA);
            gSwitches.set(eSwitch::CHART_HAVE_BPMCHANGE, bms->haveBPMChange);
            gSwitches.set(eSwitch::CHART_HAVE_LN, bms->haveLN);
            gSwitches.set(eSwitch::CHART_HAVE_RANDOM, bms->haveRandom);

            //gSwitches.set(eSwitch::CHART_HAVE_BACKBMP, ?);

            //gSwitches.set(eSwitch::CHART_HAVE_SPEEDCHANGE, ?);

            break;
        }

        default:
            break;
        }
    }
    else
    {
        gTexts.set(eText::PLAY_TITLE, e[idx]->_name);
        gTexts.set(eText::PLAY_SUBTITLE, e[idx]->_name2);
        gTexts.set(eText::PLAY_ARTIST, "");
        gTexts.set(eText::PLAY_SUBARTIST, "");
        gTexts.set(eText::PLAY_GENRE, "");
        gTexts.set(eText::PLAY_DIFFICULTY, "");
     }

    // score.db
    switch (e[idx]->type())
    {
    case eEntryType::SONG:
    case eEntryType::RIVAL_SONG:
        gOptions.set(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_SONG);
        gOptions.set(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        gOptions.set(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        break;

    case eEntryType::COURSE:
        gOptions.set(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_COURSE);
        gOptions.set(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        gOptions.set(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        break;

    case eEntryType::NEW_COURSE:
        gOptions.set(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_NEW_COURSE);
        gOptions.set(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOT_APPLICIABLE);
        gOptions.set(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        gOptions.set(eOption::CHART_PLAY_KEYS, Option::KEYS_NOT_PLAYABLE);
        break;

    case eEntryType::FOLDER:
    case eEntryType::CUSTOM_FOLDER:
    case eEntryType::RIVAL:
    default:
        gOptions.set(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_FOLDER);
        gOptions.set(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOT_APPLICIABLE);
        gOptions.set(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        gOptions.set(eOption::CHART_PLAY_KEYS, Option::KEYS_NOT_PLAYABLE);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////

SceneSelect::SceneSelect() : vScene(eMode::MUSIC_SELECT, 1000)
{
    _inputAvailable = INPUT_MASK_FUNC;

    //if (context_play.chartObj[PLAYER_SLOT_1P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }

    //if (context_play.chartObj[PLAYER_SLOT_2P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    {
        std::lock_guard<std::mutex> u(gSelectContext._mutex);
        loadSongList();
        setBarInfo();
        setEntryInfo();
    }

    _state = eSelectState::PREPARE;

    loopStart();

}

void SceneSelect::_updateAsync()
{
    switch (_state)
    {
    case eSelectState::PREPARE:
        updatePrepare();
        break;
    case eSelectState::SELECT:
        updateSelect();
        break;
    case eSelectState::SEARCH:
        updateSearch();
        break;
    case eSelectState::PANEL1:
    case eSelectState::PANEL2:
    case eSelectState::PANEL3:
    case eSelectState::PANEL4:
    case eSelectState::PANEL5:
    case eSelectState::PANEL6:
    case eSelectState::PANEL7:
    case eSelectState::PANEL8:
    case eSelectState::PANEL9:
        updatePanel(unsigned(_state) - unsigned(eSelectState::PANEL1) + 1);
        break;
    case eSelectState::FADEOUT:
        updateFadeout();
        break;
    }
}

void SceneSelect::updatePrepare()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeIntro)
    {
        _state = eSelectState::SELECT;

        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneSelect::inputGamePress, this, _1, _2));
        _input.register_h("SCENE_HOLD", std::bind(&SceneSelect::inputGameHold, this, _1, _2));
        _input.register_r("SCENE_RELEASE", std::bind(&SceneSelect::inputGameRelease, this, _1, _2));
        _input.loopStart();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());

        LOG_DEBUG << "[Select] State changed to SELECT";
    }
}

void SceneSelect::updateSelect()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updateSearch()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updatePanel(unsigned idx)
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updateFadeout()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (ft >= _skin->info.timeOutro)
    {
        loopEnd();
        _input.loopEnd();
        gNextScene = eScene::EXIT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneSelect::inputGamePress(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        if (input[ESC])
        {
            LOG_DEBUG << "[Select] ESC";
            gNextScene = eScene::EXIT;
            return;
        }

        switch (_state)
        {
        case eSelectState::SELECT:
            switch (gSelectContext.entries[gSelectContext.idx]->type())
            {
                case eEntryType::FOLDER:
                case eEntryType::CUSTOM_FOLDER:
                    if ((input & INPUT_MASK_DECIDE).any())
                        _navigateEnter(t);
                    break;

                case eEntryType::SONG:
                case eEntryType::COURSE:
                    if ((input & INPUT_MASK_DECIDE).any())
                        _decide();
                    break;

                default:
                    break;
            }
            if ((input & INPUT_MASK_CANCEL).any())
                _navigateBack(t);
            if ((input & INPUT_MASK_NAV_UP).any())
                _navigateUpBy1(t);
            if ((input & INPUT_MASK_NAV_DN).any())
                _navigateDownBy1(t);

            break;

        case eSelectState::PANEL1:
            break;

        case eSelectState::FADEOUT:
            break;

        default:
            break;
        }
    }
}

// CALLBACK
void SceneSelect::inputGameHold(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;
}

// CALLBACK
void SceneSelect::inputGameRelease(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;
}

void SceneSelect::_decide()
{
    std::lock_guard<std::mutex> u(gSelectContext._mutex);

    auto entry = gSelectContext.entries[gSelectContext.idx];
    //auto& chart = entry.charts[entry.chart_idx];
    auto& c = gChartContext;
    auto& p = gPlayContext;

    clearContextPlay();
    switch (entry->type())
    {
    case eEntryType::SONG:
    {
        c.chartObj = std::reinterpret_pointer_cast<Song>(entry)->_file;

        auto& chart = *c.chartObj;
        //c.path = chart._filePath;
        c.path = chart.absolutePath;
        c.hash = chart.fileHash;
        //c.chartObj = std::make_shared<vChartFormat>(chart);
        c.title = chart.title;
        c.title2 = chart.title2;
        c.artist = chart.artist;
        c.artist2 = chart.artist2;
        c.genre = chart.genre;
        c.version = chart.version;
        c.level = chart.levelEstimated;
        c.minBPM = chart.minBPM;
        c.maxBPM = chart.maxBPM;
        c.startBPM = chart.startBPM;

        break;
    }
    default:
        break;
    }
    
    // FIXME change to decide scene
    gNextScene = eScene::DECIDE;
}

void SceneSelect::loadSongList()
{
    // TODO load song list
    gSelectContext.entries.clear();
    for (auto& e : gSelectContext.backtrace.top().list)
    {
        // TODO replace name/name2 by tag.db

        // apply filter
        bool skip = false;
        switch (e->type())
        {
        case eEntryType::SONG:
        {
            auto f = std::reinterpret_pointer_cast<FolderSong>(e);
            for (size_t idx = 0; idx < f->getContentsCount() && !skip; ++idx)
            {
                auto p = std::reinterpret_pointer_cast<BMS_prop>(f->getChart(idx));
                switch (p->type())
                {
                case eChartFormat::BMS:
                    // difficulty filter
                    if (gSelectContext.difficulty != 0 &&
                        std::reinterpret_pointer_cast<BMS_prop>(p)->difficulty != gSelectContext.difficulty)
                    {
                        continue;
                    }
                    // gamemode filter
                    if (gSelectContext.gamemode != 0 &&
                        std::reinterpret_pointer_cast<BMS_prop>(p)->gamemode != gSelectContext.gamemode)
                    {
                        continue;
                    }
                    break;

                default:
                    break;
                }

                // currently add all charts into list
                gSelectContext.entries.push_back(std::make_shared<Song>(p));
            }
            break;
        }
        default:
            gSelectContext.entries.push_back(e);
            break;
        }
    }

    // TODO sort song list
    switch (gSelectContext.sort)
    {
    case SongListSort::DEFAULT:
    {
        auto& l = gSelectContext.entries;
        std::sort(l.begin(), l.end(), [](const std::shared_ptr<vEntry>& lhs, const std::shared_ptr<vEntry>& rhs)
        {
            if (lhs->type() != rhs->type())
                return lhs->type() > rhs->type();
            else
            {
                if (lhs->type() == eEntryType::SONG)
                {
                    const auto& l = std::reinterpret_pointer_cast<const Song>(lhs)->_file;
                    const auto& r = std::reinterpret_pointer_cast<const Song>(rhs)->_file;
                    if (l->levelEstimated != r->levelEstimated) return l->levelEstimated > r->levelEstimated;
                    if (l->title != r->title) return l->title > r->title;
                    if (l->title2 != r->title2) return l->title2 > r->title2;
                    if (l->version != r->version) return l->version > r->version;
                    return l->fileHash > r->fileHash;
                }
                else
                {
                    if (lhs->_name != rhs->_name) return lhs->_name > rhs->_name;
                    if (lhs->_name2 != rhs->_name2) return lhs->_name2 > rhs->_name2;
                    return false;
                }
            }
        });
        break;
    }
    case SongListSort::TITLE:
    {
        auto& l = gSelectContext.entries;
        std::sort(l.begin(), l.end(), [](const std::shared_ptr<vEntry>& lhs, const std::shared_ptr<vEntry>& rhs)
        {
            if (lhs->type() != rhs->type())
                return lhs->type() > rhs->type();
            else
            {
                if (lhs->type() == eEntryType::SONG)
                {
                    const auto& l = std::reinterpret_pointer_cast<const Song>(lhs)->_file;
                    const auto& r = std::reinterpret_pointer_cast<const Song>(rhs)->_file;
                    if (l->title != r->title) return l->title > r->title;
                    if (l->title2 != r->title2) return l->title2 > r->title2;
                    if (l->version != r->version) return l->version > r->version;
                    return l->fileHash > r->fileHash;
                }
                else
                {
                    if (lhs->_name != rhs->_name) return lhs->_name > rhs->_name;
                    if (lhs->_name2 != rhs->_name2) return lhs->_name2 > rhs->_name2;
                    return false;
                }
            }
        });
        break;
    }
    default:
        break;
    }

}

void SceneSelect::_navigateUpBy1(Time t)
{
    std::lock_guard<std::mutex> u(gSelectContext._mutex);

    gSelectContext.idx = (gSelectContext.entries.size() + gSelectContext.idx - 1) % gSelectContext.entries.size();
    // TODO animation
    gTimers.set(eTimer::LIST_MOVE, t.norm());
    gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());

    setBarInfo();
    setEntryInfo();
}

void SceneSelect::_navigateDownBy1(Time t)
{
    std::lock_guard<std::mutex> u(gSelectContext._mutex);

    gSelectContext.idx = (gSelectContext.idx + 1) % gSelectContext.entries.size();
    // TODO animation
    gTimers.set(eTimer::LIST_MOVE, t.norm());
    gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());

    setBarInfo();
    setEntryInfo();
}

void SceneSelect::_navigateEnter(Time t)
{
    std::lock_guard<std::mutex> u(gSelectContext._mutex);

    // TODO
    const auto& e = gSelectContext.entries[gSelectContext.idx];
    switch (e->type())
    {
    case eEntryType::FOLDER:
    case eEntryType::CUSTOM_FOLDER:
    {
        SongListProperties prop{
            gSelectContext.backtrace.top().folder,
            e->md5,
            e->_name,
            {},
            gSelectContext.idx
        };
        auto top = g_pSongDB->browse(e->md5, false);
        for (size_t i = 0; i < top.getContentsCount(); ++i)
            prop.list.push_back(top.getEntry(i));

        gSelectContext.backtrace.push(prop);
        gSelectContext.entries.clear();
        gSelectContext.idx = 0;
        loadSongList();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());

        setBarInfo();
        setEntryInfo();
        break;
    }
    default:
        break;
    }
}
void SceneSelect::_navigateBack(Time t)
{
    std::lock_guard<std::mutex> u(gSelectContext._mutex);

    // TODO
    auto top = gSelectContext.backtrace.top();
    if (!top.parent.empty())
    {
        gSelectContext.idx = 0;
        gSelectContext.backtrace.pop();
        top = gSelectContext.backtrace.top();
        gSelectContext.entries = top.list;
        gSelectContext.idx = top.index;

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());

        setBarInfo();
        setEntryInfo();
    }
}