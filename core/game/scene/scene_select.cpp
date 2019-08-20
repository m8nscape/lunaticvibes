#include "scene_select.h"
#include "scene_context.h"
#include "chart/chart_types.h"

SceneSelect::SceneSelect() : vScene(eMode::RESULT, 1000)
{
    _inputAvailable = INPUT_MASK_FUNC;

    //if (context_play.scrollObj[PLAYER_SLOT_1P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }
        
    //if (context_play.scrollObj[PLAYER_SLOT_2P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
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

void SceneSelect::updatePrepare()
{
    auto t = timestamp();
    auto rt = t - gTimers.get(eTimer::SCENE_START);

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
    auto t = timestamp();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updateSearch()
{
    auto t = timestamp();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updatePanel(unsigned idx)
{
    auto t = timestamp();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updateFadeout()
{
    auto t = timestamp();
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
void SceneSelect::inputGamePress(InputMask& m, timestamp t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if ((_inputAvailable & m).any())
    {
        auto input = _inputAvailable & m;
        switch (_state)
        {
        case eSelectState::SELECT:
            switch (_filteredSongList.entries[_currentSongIdx].type)
            {
                case EntryType::FOLDER:
                case EntryType::CUSTOM_FOLDER:
                    if ((input & INPUT_MASK_DECIDE).any())
                        _navigateEnter();
                    if ((input & INPUT_MASK_CANCEL).any())
                        _navigateBack();
                    break;

                case EntryType::SONG:
                case EntryType::COURSE:
                    if ((input & INPUT_MASK_DECIDE).any())
                        _decide();
                    break;

                case EntryType::SP_NEW_COURSE:
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
void SceneSelect::inputGameHold(InputMask& m, timestamp t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;
}

// CALLBACK
void SceneSelect::inputGameRelease(InputMask& m, timestamp t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;
}

void SceneSelect::_decide()
{
    auto& entry = _filteredSongList.entries[_currentSongIdx];
    auto& chart = entry.chart;
    auto& c = context_chart;
    auto& p = context_play;


    c.path = chart->_filePath;
    c.hash = chart->_fileHash;
    c.chartObj = chart;
    c.title = chart->_title;
    c.title2 = chart->_title2;
    c.artist = chart->_artist;
    c.artist2 = chart->_artist2;
    c.genre = chart->_genre;
    c.version = chart->_version;
    c.level = chart->_level;
    c.minBPM = chart->_minBPM;
    c.maxBPM = chart->_maxBPM;
    c.itlBPM = chart->_itlBPM;

    clearContextPlay();
    switch (chart->type())
    {
    case eChartType::BMS:
    {
        auto& bms = std::dynamic_pointer_cast<BMS>(chart);
        p.judgeLevel = bms->rank;
        // TODO mods
        break;
    }
    }
    
    __next_scene = eScene::DECIDE;
}

