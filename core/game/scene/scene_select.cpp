#include <algorithm>

#include "scene_select.h"
#include "scene_context.h"
#include "chartformat/chart_types.h"

#include "entry/entry_song.h"

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

    // TODO load song list
    for (auto & e: context_select.backtrace.top().list)
    {
        // TODO replace name/name2 by tag.db

        // apply filter
        if (e.type() == eEntryType::SONG)
        {
            auto& p = reinterpret_cast<Song&>(e)._file;
            switch (p->type())
            {
            case eChartFormat::BMS:
                // difficulty filter
                if (context_select.difficulty != 0 && 
                    std::reinterpret_pointer_cast<BMS>(p)->difficulty != context_select.difficulty)
                    continue;

                // gamemode filter
                if (context_select.gamemode != 0 && 
                    std::reinterpret_pointer_cast<BMS>(p)->gamemode != context_select.gamemode)
                    continue;
                break;

            default:
                break;
            }
        }

        context_select.entries.push_back(e);
    }

    // TODO sort song list
    switch (context_select.sort)
    {
    case SongListSort::DEFAULT:
    {
        auto& l = context_select.entries;
        std::sort(l.begin(), l.end(), [](const vEntry& lhs, const vEntry& rhs) 
        {
            if (lhs.type() != rhs.type())
                return lhs.type() > rhs.type();
            else
            {
                if (lhs.type() == eEntryType::SONG)
                {
                    const auto& l = reinterpret_cast<const Song&>(lhs)._file;
                    const auto& r = reinterpret_cast<const Song&>(rhs)._file;
                    if (l->_level != r->_level) return l->_level > r->_level;
                    if (l->_title != r->_title) return l->_title > r->_title;
                    if (l->_title2 != r->_title2) return l->_title2 > r->_title2;
                    if (l->_version != r->_version) return l->_version > r->_version;
                    return l->_fileHash > r->_fileHash;
                }
                else
                {
                    if (lhs._name != rhs._name) return lhs._name > rhs._name;
                    if (lhs._name2 != rhs._name2) return lhs._name2 > rhs._name2;
                    return false;
                }
            }
        });
        break;
    }
    case SongListSort::TITLE:
    {
        auto& l = context_select.entries;
        std::sort(l.begin(), l.end(), [](const vEntry& lhs, const vEntry& rhs)
        {
            if (lhs.type() != rhs.type())
                return lhs.type() > rhs.type();
            else
            {
                if (lhs.type() == eEntryType::SONG)
                {
                    const auto& l = reinterpret_cast<const Song&>(lhs)._file;
                    const auto& r = reinterpret_cast<const Song&>(rhs)._file;
                    if (l->_title != r->_title) return l->_title > r->_title;
                    if (l->_title2 != r->_title2) return l->_title2 > r->_title2;
                    if (l->_version != r->_version) return l->_version > r->_version;
                    return l->_fileHash > r->_fileHash;
                }
                else
                {
                    if (lhs._name != rhs._name) return lhs._name > rhs._name;
                    if (lhs._name2 != rhs._name2) return lhs._name2 > rhs._name2;
                    return false;
                }
            }
        });
        break;
    }
    default:
        break;
    }

    _state = eSelectState::PREPARE;

    loopStart();
}

////////////////////////////////////////////////////////////////////////////////

void SceneSelect::_updateAsync()
{
    std::unique_lock<decltype(_mutex)> _lock(_mutex, std::try_to_lock);
    if (!_lock.owns_lock()) return;

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

void setBarTitles()
{
    const auto& e = context_select.entries;
    const size_t idx = context_select.idx;
    for (size_t i = 0; i < 30; ++i)
    {
        gTexts.set(eText(int(eText::_SELECT_BAR_TITLE_FULL_0) + i), e[(idx + i) % e.size()]._name);
    }

    gTexts.set(eText::PLAY_TITLE, e[idx]._name);
    gTexts.set(eText::PLAY_SUBTITLE, e[idx]._name2);
    // TODO BMS text updates
}

void SceneSelect::updatePrepare()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);

    setBarTitles();

    if (rt.norm() >= _skin->info.timeIntro)
    {
        _state = eSelectState::SELECT;

        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneSelect::inputGamePress, this, _1, _2));
        _input.register_h("SCENE_HOLD", std::bind(&SceneSelect::inputGameHold, this, _1, _2));
        _input.register_r("SCENE_RELEASE", std::bind(&SceneSelect::inputGameRelease, this, _1, _2));
        _input.loopStart();

        LOG_DEBUG << "[Select] State changed to SELECT";
    }
}

void SceneSelect::updateSelect()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);

    setBarTitles();
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
        __next_scene = eScene::EXIT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneSelect::inputGamePress(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if ((_inputAvailable & m).any())
    {
        auto input = _inputAvailable & m;
        switch (_state)
        {
        case eSelectState::SELECT:
            switch (context_select.entries[context_select.idx].type())
            {
                case eEntryType::FOLDER:
                case eEntryType::CUSTOM_FOLDER:
                    if ((input & INPUT_MASK_DECIDE).any())
                        _navigateEnter();
                    if ((input & INPUT_MASK_CANCEL).any())
                        _navigateBack();
                    break;

                case eEntryType::SONG:
                case eEntryType::COURSE:
                    if ((input & INPUT_MASK_DECIDE).any())
                        _decide();
                    break;

                default:
                    // unsupported
                    break;
            }
            LOG_DEBUG << "[Select] State changed to STOP";
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
    auto& entry = context_select.entries[context_select.idx];
    //auto& chart = entry.charts[entry.chart_idx];
    auto& c = context_chart;
    auto& p = context_play;

    clearContextPlay();
    switch (entry.type())
    {
    case eEntryType::SONG:
    {
        c.chartObj = reinterpret_cast<Song&>(entry)._file;
        auto& chart = *c.chartObj;
        c.path = chart._filePath;
        c.hash = chart._fileHash;
        //c.chartObj = std::make_shared<vChartFormat>(chart);
        c.title = chart._title;
        c.title2 = chart._title2;
        c.artist = chart._artist;
        c.artist2 = chart._artist2;
        c.genre = chart._genre;
        c.version = chart._version;
        c.level = chart._level;
        c.minBPM = chart._minBPM;
        c.maxBPM = chart._maxBPM;
        c.itlBPM = chart._itlBPM;

        switch (c.chartObj->type())
        {
        case eChartFormat::BMS:
        {
            auto bms = std::reinterpret_pointer_cast<BMS>(c.chartObj);
            // TODO mods
            break;
        }

        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    
    __next_scene = eScene::DECIDE;
}


void SceneSelect::_navigateEnter()
{
    // TODO
}
void SceneSelect::_navigateBack()
{
    // TODO
}