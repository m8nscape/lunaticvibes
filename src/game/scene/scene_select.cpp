#include <algorithm>
#include <memory>

#include "scene_select.h"
#include "scene_context.h"
#include "common/chartformat/chartformat_types.h"
#include "common/entry/entry_song.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "config/config_mgr.h"

#include "game/skin/skin_lr2_button_callbacks.h"
#include "game/scene/scene_context.h"
#include "game/scene/scene_mgr.h"

////////////////////////////////////////////////////////////////////////////////

void config_sys()
{
    using namespace cfg;

    switch (gOptions.get(eOption::SYS_WINDOWED))
    {
    case Option::WIN_FULLSCREEN: ConfigMgr::set('C',V_WINMODE, V_WINMODE_FULL); break;
    case Option::WIN_BORDERLESS: ConfigMgr::set('C',V_WINMODE, V_WINMODE_BORDERLESS); break;
    case Option::WIN_WINDOWED: 
    default:                     ConfigMgr::set('C',V_WINMODE, V_WINMODE_WINDOWED); break;
    }

    
}

void config_player()
{
    using namespace cfg;

    ConfigMgr::set('P',P_HISPEED, gPlayContext.Hispeed);
    switch (gOptions.get(eOption::PLAY_HSFIX_TYPE_1P))
    {
    case Option::SPEED_FIX_MAX:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_MAX); break;
    case Option::SPEED_FIX_MIN:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_MIN); break;
    case Option::SPEED_FIX_AVG:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_AVG); break;
    case Option::SPEED_FIX_CONSTANT: ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_CONSTANT); break;
    default:                         ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_NORMAL); break;
    }

    ConfigMgr::set('P',P_LOAD_BGA, gOptions.get(eOption::PLAY_BGA_TYPE) != Option::BGA_OFF);
    ConfigMgr::set('P',P_LANECOVER_TOP, gNumbers.get(eNumber::LANECOVER_TOP_1P) / 10);


    switch (gOptions.get(eOption::PLAY_RANDOM_TYPE_1P))
    {
    case Option::RAN_MIRROR: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_MIRROR); break;
    case Option::RAN_RANDOM: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_RANDOM); break;
    case Option::RAN_SRAN:   ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_SRAN); break;
    case Option::RAN_HRAN:   ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_HRAN); break;
    case Option::RAN_ALLSCR: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_ALLSCR); break;
    default:                 ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_NORMAL); break;
    }

    switch (gOptions.get(eOption::PLAY_GAUGE_TYPE_1P))
    {
    case Option::GAUGE_HARD:   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_HARD); break;
    case Option::GAUGE_EASY:   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_EASY); break;
    case Option::GAUGE_DEATH:  ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_DEATH); break;
    case Option::GAUGE_EXHARD: ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_EXHARD); break;
    case Option::GAUGE_ASSIST: ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_ASSIST); break;
    default:                   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_NORMAL); break;
    }

    switch (gOptions.get(eOption::PLAY_GHOST_TYPE_1P))
    {
    case Option::GHOST_TOP:         ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_A); break;
    case Option::GHOST_SIDE:        ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_B); break;
    case Option::GHOST_SIDE_BOTTOM: ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_C); break;
    default:                        ConfigMgr::set('P',P_GHOST_TYPE, "OFF"); break;
    }

    ConfigMgr::set('P',P_JUDGE_OFFSET, gNumbers.get(eNumber::TIMING_ADJUST_VISUAL));
    ConfigMgr::set('P',P_GHOST_TARGET, gNumbers.get(eNumber::DEFAULT_TARGET_RATE));

    switch (gOptions.get(eOption::SELECT_FILTER_KEYS))
    {
    case Option::FILTER_KEYS_SINGLE:  ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_SINGLE); break;
    case Option::FILTER_KEYS_7:       ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_7K); break;
    case Option::FILTER_KEYS_5:       ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_5K); break;
    case Option::FILTER_KEYS_14:      ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_14K); break;
    case Option::FILTER_KEYS_DOUBLE:  ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_DOUBLE); break;
    case Option::FILTER_KEYS_10:      ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_10K); break;
    case Option::FILTER_KEYS_9:       ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_9K); break;
    default:                          ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_ALL); break;
    }

    switch (gOptions.get(eOption::SELECT_SORT))
    {
    case Option::SORT_TITLE: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_TITLE); break;
    case Option::SORT_LEVEL: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_LEVEL); break;
    case Option::SORT_CLEAR: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_CLEAR); break;
    case Option::SORT_RATE:  ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_RATE); break;
    default:                 ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_FOLDER); break;
    }

    switch (gOptions.get(eOption::SELECT_FILTER_DIFF))
    {
    case Option::DIFF_BEGINNER: ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_BEGINNER); break;
    case Option::DIFF_NORMAL:   ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_NORMAL); break;
    case Option::DIFF_HYPER:    ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_HYPER); break;
    case Option::DIFF_ANOTHER:  ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ANOTHER); break;
    case Option::DIFF_INSANE:   ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_INSANE); break;
    default:                    ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL); break;
    }

    ConfigMgr::set('P',P_BATTLE, gOptions.get(eOption::PLAY_BATTLE_TYPE) != 0);
    ConfigMgr::set('P',P_FLIP, gSwitches.get(eSwitch::PLAY_OPTION_DP_FLIP));
    ConfigMgr::set('P',P_SCORE_GRAPH, gSwitches.get(eSwitch::SYSTEM_SCOREGRAPH));
}

void config_vol()
{
    using namespace cfg;

    ConfigMgr::set('P',P_VOL_MASTER, gSliders.get(eSlider::VOLUME_MASTER));
    ConfigMgr::set('P',P_VOL_KEY, gSliders.get(eSlider::VOLUME_KEY));
    ConfigMgr::set('P',P_VOL_BGM, gSliders.get(eSlider::VOLUME_BGM));
}

void config_eq()
{
    using namespace cfg;

    ConfigMgr::set('P',P_EQ, gSwitches.get(eSwitch::SOUND_EQ));
    ConfigMgr::set('P',P_EQ0, gNumbers.get(eNumber::EQ0));
    ConfigMgr::set('P',P_EQ1, gNumbers.get(eNumber::EQ1));
    ConfigMgr::set('P',P_EQ2, gNumbers.get(eNumber::EQ2));
    ConfigMgr::set('P',P_EQ3, gNumbers.get(eNumber::EQ3));
    ConfigMgr::set('P',P_EQ4, gNumbers.get(eNumber::EQ4));
    ConfigMgr::set('P',P_EQ5, gNumbers.get(eNumber::EQ5));
    ConfigMgr::set('P',P_EQ6, gNumbers.get(eNumber::EQ6));
}

void config_freq()
{
    using namespace cfg;

    ConfigMgr::set('P',P_FREQ, gSwitches.get(eSwitch::SOUND_PITCH));
    switch (gOptions.get(eOption::SOUND_PITCH_TYPE))
    {
    case Option::FREQ_FREQ: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_FREQ); break;
    case Option::FREQ_PITCH: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_PITCH); break;
    case Option::FREQ_SPEED: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_SPEED); break;
    default: break;
    }
    ConfigMgr::set('P',P_FREQ_VAL, gNumbers.get(eNumber::PITCH));
}

void config_fx()
{
    using namespace cfg;

    ConfigMgr::set('P',P_FX0, gSwitches.get(eSwitch::SOUND_FX0));
    switch (gOptions.get(eOption::SOUND_TARGET_FX0))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX0))
    {
    case Option::FX_OFF:        ConfigMgr::set('P',P_FX0_TYPE, "OFF"); break;
    case Option::FX_REVERB:     ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY:      ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS:    ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS:   ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER:    ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS:     ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    ConfigMgr::set('P',P_FX0_P1, gNumbers.get(eNumber::FX0_P1));
    ConfigMgr::set('P', P_FX0_P2, gNumbers.get(eNumber::FX0_P2));

    ConfigMgr::set('P',P_FX1, gSwitches.get(eSwitch::SOUND_FX1));
    switch (gOptions.get(eOption::SOUND_TARGET_FX1))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX1))
    {
    case Option::FX_OFF:        ConfigMgr::set('P',P_FX1_TYPE, "OFF"); break;
    case Option::FX_REVERB:     ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY:      ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS:    ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS:   ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER:    ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS:     ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    ConfigMgr::set('P',P_FX1_P1, gNumbers.get(eNumber::FX1_P1));
    ConfigMgr::set('P',P_FX1_P2, gNumbers.get(eNumber::FX1_P2));

    ConfigMgr::set('P',P_FX2, gSwitches.get(eSwitch::SOUND_FX2));
    switch (gOptions.get(eOption::SOUND_TARGET_FX2))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX2))
    {
    case Option::FX_OFF:        ConfigMgr::set('P',P_FX2_TYPE, "OFF"); break;
    case Option::FX_REVERB:     ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY:      ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS:    ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS:   ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER:    ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS:     ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    ConfigMgr::set('P',P_FX2_P1, gNumbers.get(eNumber::FX2_P1));
    ConfigMgr::set('P',P_FX2_P2, gNumbers.get(eNumber::FX2_P2));
}


////////////////////////////////////////////////////////////////////////////////

SceneSelect::SceneSelect() : vScene(eMode::MUSIC_SELECT, 1000)
{
    _scene = eScene::SELECT;

    _inputAvailable = INPUT_MASK_FUNC | INPUT_MASK_MOUSE;
    _inputAvailable |= INPUT_MASK_1P;
    _inputAvailable |= INPUT_MASK_2P;

    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
        loadSongList();
        sortSongList();
        setBarInfo();
        setEntryInfo();

        resetJukeboxText();
    }

    switch (gOptions.get(eOption::SELECT_FILTER_KEYS))
    {
    case Option::FILTER_KEYS_SINGLE: gSelectContext.filterKeys = 1; break;
    case Option::FILTER_KEYS_7:      gSelectContext.filterKeys = 7; break;
    case Option::FILTER_KEYS_5:      gSelectContext.filterKeys = 5; break;
    case Option::FILTER_KEYS_DOUBLE: gSelectContext.filterKeys = 2; break;
    case Option::FILTER_KEYS_14:     gSelectContext.filterKeys = 14; break;
    case Option::FILTER_KEYS_10:     gSelectContext.filterKeys = 10; break;
    case Option::FILTER_KEYS_9:      gSelectContext.filterKeys = 9; break;
    default:                         gSelectContext.filterKeys = 0; break;
    }

    gSelectContext.filterDifficulty = gOptions.get(eOption::SELECT_FILTER_DIFF);

    switch (gOptions.get(eOption::SELECT_SORT))
    {
    case Option::SORT_TITLE: gSelectContext.sort = SongListSort::TITLE; break;
    case Option::SORT_LEVEL: gSelectContext.sort = SongListSort::LEVEL; break;
    case Option::SORT_CLEAR: gSelectContext.sort = SongListSort::CLEAR; break;
    case Option::SORT_RATE:  gSelectContext.sort = SongListSort::RATE; break;
    default:                 gSelectContext.sort = SongListSort::DEFAULT; break;
    }

    gSelectContext.isGoingToKeyConfig = false;
    gSelectContext.isGoingToSkinSelect = false;
    gSelectContext.isGoingToAutoPlay = false;
    gSelectContext.isGoingToReplay = false;
    gSwitches.set(eSwitch::SYSTEM_AUTOPLAY, false);

    auto& [score, lamp] = getSaveScoreType();
    gSwitches.set(eSwitch::CHART_CAN_SAVE_SCORE, score);
    gOptions.set(eOption::CHART_SAVE_LAMP_TYPE, lamp);

    _state = eSelectState::PREPARE;
    _updateCallback = std::bind(&SceneSelect::updatePrepare, this);

    SoundMgr::stopNoteSamples();
    SoundMgr::stopSysSamples();
    SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);

    _imguiInit();
}

SceneSelect::~SceneSelect()
{
    config_sys();
    config_player();
    config_vol();
    config_eq();
    config_freq();
    config_fx();
    ConfigMgr::save();

    _input.loopEnd();
    loopEnd();
}

void SceneSelect::_updateAsync()
{
    if (gNextScene != eScene::SELECT) return;

    if (gAppIsExiting)
    {
        gNextScene = eScene::EXIT_TRANS;
    }

    _updateCallback();

    if (!gSelectContext.entries.empty() && scrollAccumulator != 0.0)
    {
        {
            std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

            Time t;
            if (!(isHoldingUp || isHoldingDown) &&
                (scrollAccumulator > 0 && scrollAccumulator - scrollAccumulatorAddUnit < 0 ||
                    scrollAccumulator < 0 && scrollAccumulator - scrollAccumulatorAddUnit > 0 ||
                    -0.000001 < scrollAccumulator && scrollAccumulator < 0.000001 ||
                    scrollAccumulator * scrollAccumulatorAddUnit < 0))
            {
                gSliders.set(eSlider::SELECT_LIST, gSelectContext.entries.empty() ? 0.0 : ((double)gSelectContext.idx / gSelectContext.entries.size()));
                scrollAccumulator = 0.0;
                scrollAccumulatorAddUnit = 0.0;
                gSelectContext.scrollDirection = 0;
                _skin->reset_bar_animation();
                gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());
                gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
            }
            else
            {
                if (gSelectContext.scrollDirection == 0)
                {
                    _skin->start_bar_animation();
                }

                double posOld = gSliders.get(eSlider::SELECT_LIST);
                double posNew = posOld + scrollAccumulatorAddUnit / gSelectContext.entries.size();

                int idxOld = (int)std::round(posOld * gSelectContext.entries.size());
                int idxNew = (int)std::round(posNew * gSelectContext.entries.size());
                if (idxOld != idxNew)
                {
                    if (idxOld < idxNew)
                        _navigateDownBy1(t);
                    else
                        _navigateUpBy1(t);
                }

                while (posNew < 0.) posNew += 1.;
                while (posNew >= 1.) posNew -= 1.;
                gSliders.set(eSlider::SELECT_LIST, posNew);

                scrollAccumulator -= scrollAccumulatorAddUnit;

                gSelectContext.scrollDirection = scrollAccumulator > 0. ? 1 : -1;
            }
        }

        setDynamicTextures();
    }

}

void SceneSelect::updatePrepare()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeIntro)
    {
        _state = eSelectState::SELECT;
        _updateCallback = std::bind(&SceneSelect::updateSelect, this);

        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneSelect::inputGamePress, this, _1, _2));
        _input.register_h("SCENE_HOLD", std::bind(&SceneSelect::inputGameHold, this, _1, _2));
        _input.register_r("SCENE_RELEASE", std::bind(&SceneSelect::inputGameRelease, this, _1, _2));
        _input.register_a("SCENE_AXIS", std::bind(&SceneSelect::inputGameAxisSelect, this, _1, _2, _3));
        _input.loopStart();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());

        // restore panel stat
        for (int i = 1; i <= 9; ++i)
        {
            eSwitch p = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + i);
            if (gSwitches.get(p))
            {
                eTimer tm = static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + i);
                gTimers.set(tm, t.norm());
                SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
            }
        }

        LOG_DEBUG << "[Select] State changed to SELECT";
    }
}

void SceneSelect::updateSelect()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (gSwitches.get(eSwitch::SELECT_PANEL1) && (!_skin->isSupportGreenNumber))
    {
        std::stringstream ss;
        bool lock1 = ConfigMgr::get('P', cfg::P_LOCK_SPEED, false);

        if (lock1) ss << "G(1P): FIX " << ConfigMgr::get('P', cfg::P_GREENNUMBER, 0);

        bool lock2 = gPlayContext.battle2PLockSpeed;
        if (lock2) ss << (lock1 ? "\n" : "") << "G(2P): " << gPlayContext.battle2PGreenNumber;

        gTexts.set(eText::_OVERLAY_TOPLEFT, ss.str());
    }
    else
    {
        gTexts.set(eText::_OVERLAY_TOPLEFT, "");
    }

    if (gSelectContext.isGoingToKeyConfig || gSelectContext.isGoingToSkinSelect)
    {
        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        _state = eSelectState::FADEOUT;
        _updateCallback = std::bind(&SceneSelect::updateFadeout, this);
    }
    if (gSelectContext.isGoingToAutoPlay)
    {
        gSelectContext.isGoingToAutoPlay = false;
        if (!gSelectContext.entries.empty())
        {
            switch (gSelectContext.entries[gSelectContext.idx].first->type())
            {
            case eEntryType::SONG:
            case eEntryType::CHART:
            case eEntryType::RIVAL_SONG:
            case eEntryType::RIVAL_CHART:
            case eEntryType::COURSE:
                gPlayContext.isAuto = true;
                gSwitches.set(eSwitch::SYSTEM_AUTOPLAY, true);
                _decide();
                break;
            }
        }
    }
    if (gSelectContext.isGoingToReplay)
    {
        gSelectContext.isGoingToReplay = false;
        if (!gSelectContext.entries.empty())
        {
            switch (gSelectContext.entries[gSelectContext.idx].first->type())
            {
            case eEntryType::SONG:
            case eEntryType::CHART:
            case eEntryType::RIVAL_SONG:
            case eEntryType::RIVAL_CHART:
            case eEntryType::COURSE:
                if (false /* current chart has a replay */)
                _decide();
                break;
            }
        }
    }
}

void SceneSelect::updateSearch()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updatePanel(unsigned idx)
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updateFadeout()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);
    Time ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (ft >= _skin->info.timeOutro)
    {
        if (gSelectContext.isGoingToKeyConfig)
        {
            SoundMgr::stopSysSamples();
            gNextScene = eScene::KEYCONFIG;
        }
        else if (gSelectContext.isGoingToSkinSelect)
        {
            SoundMgr::stopSysSamples();
            gNextScene = eScene::CUSTOMIZE;
        }
        else
        {
            SoundMgr::stopSysSamples();
            gNextScene = eScene::EXIT_TRANS;
        }
    }
}

void SceneSelect::_updateImgui()
{
    vScene::_updateImgui();

    _imguiSettings();
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneSelect::inputGamePress(InputMask& m, const Time& t)
{
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    using namespace Input;

    if (isInTextEdit())
    {
        inputGamePressTextEdit(m, t);
        return;
    }

    if (m[Input::Pad::F9])
    {
        imguiShow = !imguiShow;
        _skin->setHandleMouseEvents(!imguiShow);
    }
    if (imguiShow)
    {
        if (m[Input::Pad::ESC])
        {
            imguiShow = false;
            _skin->setHandleMouseEvents(true);
        }
        return;
    }

    if (m[Input::Pad::ESC])
    {
        // close panels if opened. exit if no panel is opened
        bool hasPanelOpened = _closeAllPanels(t);
        if (!hasPanelOpened)
        {
            LOG_DEBUG << "[Select] ESC";
            gNextScene = eScene::EXIT_TRANS;
            return;
        }
    }

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // sub callbacks
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            inputGamePressPanel(input, t);
        }
        else
        {
            switch (_state)
            {
            case eSelectState::SELECT:
                inputGamePressSelect(input, t);
                break;

            case eSelectState::FADEOUT:
                break;

            default:
                break;
            }
        }

        if (input[Pad::M2])
        {
            // close panels if opened
            _closeAllPanels(t);
        }

        // lights
        for (size_t k = Pad::K11; k <= Pad::K19; ++k)
        {
            if (input[k])
            {
                gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K11), t.norm());
                gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K11), TIMER_NEVER);
            }
        }
        for (size_t k = Pad::K21; k <= Pad::K29; ++k)
        {
            if (input[k])
            {
                if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_DOWN) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_UP) + k - Pad::K21), TIMER_NEVER);
                }
                else
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }
    }
}

// CALLBACK
void SceneSelect::inputGameHold(InputMask& m, const Time& t)
{
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // sub callbacks
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            inputGameHoldPanel(input, t);
        }
        else
        {
            switch (_state)
            {
            case eSelectState::SELECT:
                inputGameHoldSelect(input, t);
                break;

            default:
                break;
            }
        }
    }
}

// CALLBACK
void SceneSelect::inputGameRelease(InputMask& m, const Time& t)
{
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // sub callbacks
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            inputGameReleasePanel(input, t);
        }
        else
        {

            switch (_state)
            {
            case eSelectState::SELECT:
                inputGameReleaseSelect(input, t);
                break;

            default:
                break;
            }
        }

        // lights
        for (size_t k = Pad::K11; k <= Pad::K19; ++k)
        {
            if (input[k])
            {
                gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K11), t.norm());
                gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K11), TIMER_NEVER);
            }
        }
        for (size_t k = Pad::K21; k <= Pad::K29; ++k)
        {
            if (input[k])
            {
                if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_UP) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_DOWN) + k - Pad::K21), TIMER_NEVER);
                }
                else
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }
    }
}


void SceneSelect::inputGamePressSelect(InputMask& input, const Time& t)
{
    if (_skin->type() == eSkinType::LR2)
    {
        if (input[Input::Pad::K1START] || input[Input::Pad::K2START])
        {
            // close other panels
            _closeAllPanels(t);

            // open panel 1
            gSwitches.set(eSwitch::SELECT_PANEL1, true);
            gTimers.set(eTimer::PANEL1_START, t.norm());
            gTimers.set(eTimer::PANEL1_END, TIMER_NEVER);
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
            return;
        }
        if (input[Input::M2])
        {
            bool hasPanelOpened = false;
            for (int i = 1; i <= 9; ++i)
            {
                eSwitch p = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + i);
                if (gSwitches.get(p))
                {
                    hasPanelOpened = true;
                    break;
                }
            }
            if (!hasPanelOpened)
            {
                _navigateBack(t);
                return;
            }
        }
        if (selectDownTimestamp == -1 && (input[Input::Pad::K1SELECT || input[Input::Pad::K2SELECT]]))
        {
            switch (gSelectContext.entries[gSelectContext.idx].first->type())
            {
            case eEntryType::SONG:
            case eEntryType::RIVAL_SONG:
            {
                selectDownTimestamp = t;
                break;
            }
            }
        }
    }

    // navigate
    if (!gSelectContext.entries.empty())
    {
        switch (gSelectContext.entries[gSelectContext.idx].first->type())
        {
        case eEntryType::FOLDER:
        case eEntryType::CUSTOM_FOLDER:
            if ((input & INPUT_MASK_DECIDE).any())
                return _navigateEnter(t);
            break;

        case eEntryType::SONG:
        case eEntryType::CHART:
        case eEntryType::RIVAL_SONG:
        case eEntryType::RIVAL_CHART:
        case eEntryType::COURSE:
            if ((input & INPUT_MASK_DECIDE).any())
                return _decide();
            break;

        default:
            break;
        }
        if ((input & INPUT_MASK_CANCEL).any())
            return _navigateBack(t);

        if ((input & INPUT_MASK_NAV_UP).any())
        {
            isHoldingUp = true;
            scrollAccumulator -= 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (getRate() / 1000);
        }
        if ((input & INPUT_MASK_NAV_DN).any())
        {
            isHoldingDown = true;
            scrollAccumulator += 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (getRate() / 1000);
        }

        if (input[Input::MWHEELUP])
        {
            if (scrollAccumulator != 0.0)
            {
                gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
            }
            scrollAccumulator -= 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (getRate() / 1000);
        }
        if (input[Input::MWHEELDOWN])
        {
            if (scrollAccumulator != 0.0)
            {
                gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
            }
            scrollAccumulator += 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (getRate() / 1000);
        }
    }
    else
    {
        if ((input & INPUT_MASK_CANCEL).any())
            return _navigateBack(t);
    }
}

void SceneSelect::inputGameHoldSelect(InputMask& input, const Time& t)
{
    // navigate
    if (isHoldingUp && (t - scrollButtonTimestamp).norm() >= gSelectContext.scrollTimeLength)
    {
        gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
        scrollButtonTimestamp = t;
        scrollAccumulator -= 1.0;
        scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (getRate() / 1000);
    }
    if (isHoldingDown && (t - scrollButtonTimestamp).norm() >= gSelectContext.scrollTimeLength)
    {
        gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
        scrollButtonTimestamp = t;
        scrollAccumulator += 1.0;
        scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (getRate() / 1000);
    }
    if ((t - selectDownTimestamp).norm() >= 233 && !isInVersionList && (input[Input::Pad::K1SELECT] || input[Input::Pad::K2SELECT]))
    {
        _navigateSongEnter(t);
    }
}

void SceneSelect::inputGameReleaseSelect(InputMask& input, const Time& t)
{
    if (_skin->type() == eSkinType::LR2)
    {
        if (selectDownTimestamp != -1 && (input[Input::Pad::K1SELECT] || input[Input::Pad::K2SELECT]))
        {
            if (isInVersionList)
            {
                _navigateSongBack(t);
            }
            else
            {
                // short press on song, inc version by 1
                // TODO play some animation
                auto pSong = std::dynamic_pointer_cast<FolderSong>(gSelectContext.entries[gSelectContext.idx].first);
                pSong->incCurrentChart();
                setBarInfo();
                SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
            }
            selectDownTimestamp = -1;
            return;
        }
    }

    // navigate
    if ((input & INPUT_MASK_NAV_UP).any())
    {
        isHoldingUp = false;
    }
    if ((input & INPUT_MASK_NAV_DN).any())
    {
        isHoldingDown = false;
    }
}

void SceneSelect::inputGameAxisSelect(double s1, double s2, const Time& t)
{
    double s = (s1 + s2) * 75.0;
    if (s <= -0.01 || 0.01 <= s)
    {
        scrollAccumulator += s;
        if (scrollAccumulator > 0)
            scrollAccumulatorAddUnit = std::max(0.001, scrollAccumulator / 100.0);
        else
            scrollAccumulatorAddUnit = std::min(-0.001, scrollAccumulator / 100.0);
    }
}

void SceneSelect::inputGamePressPanel(InputMask& input, const Time& t)
{
    using namespace Input;

    if (_skin->type() == eSkinType::LR2)
    {
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            // 1: KEYS
            if (input[Pad::K12]) lr2skin::button::random_type(PLAYER_SLOT_1P, 1);
            if (input[Pad::K13]) lr2skin::button::battle(1);
            if (input[Pad::K14]) lr2skin::button::gauge_type(PLAYER_SLOT_1P, 1);
            if (input[Pad::K15]) lr2skin::button::hs(PLAYER_SLOT_1P, -1);
            if (input[Pad::K16]) lr2skin::button::autoscr(PLAYER_SLOT_1P, 1);
            if (input[Pad::K17]) lr2skin::button::hs(PLAYER_SLOT_1P, 1);

            if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
            {
                // 1: KEYS
                if (input[Pad::K22]) lr2skin::button::random_type(PLAYER_SLOT_2P, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(PLAYER_SLOT_2P, 1);
                if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_2P, -1);
                if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_2P, 1);
                if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_2P, 1);
            }
            else
            {
                // 1: KEYS
                if (input[Pad::K22]) lr2skin::button::random_type(PLAYER_SLOT_1P, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(PLAYER_SLOT_1P, 1);
                if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_1P, -1);
                if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_1P, 1);
                if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_1P, 1);
            }
        }
    }
}

void SceneSelect::inputGameHoldPanel(InputMask& input, const Time& t)
{
}

void SceneSelect::inputGameReleasePanel(InputMask& input, const Time& t)
{
    if (gSwitches.get(eSwitch::SELECT_PANEL1) && (input[Input::Pad::K1START] || input[Input::Pad::K2START]))
    {
        // close panel 1
        gSwitches.set(eSwitch::SELECT_PANEL1, false);
        gTimers.set(eTimer::PANEL1_START, TIMER_NEVER);
        gTimers.set(eTimer::PANEL1_END, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
        return;
    }
}


void SceneSelect::_decide()
{
    std::shared_lock<std::shared_mutex> u(gSelectContext._mutex);

    auto entry = gSelectContext.entries[gSelectContext.idx].first;
    //auto& chart = entry.charts[entry.chart_idx];
    auto& c = gChartContext;
    auto& p = gPlayContext;

    clearContextPlay();

    if (gSwitches.get(eSwitch::SYSTEM_AUTOPLAY))
    {
        gPlayContext.canRetry = false;
        gPlayContext.isAuto = true;
    }
    switch (gOptions.get(eOption::PLAY_BATTLE_TYPE))
    {
    case Option::BATTLE_OFF:
    case Option::BATTLE_DB:     gPlayContext.isBattle = false; break;
    case Option::BATTLE_LOCAL:
    case Option::BATTLE_GHOST:  gPlayContext.isBattle = true; break;
    }

    if (entry->type() == eEntryType::COURSE)
    {
        gPlayContext.canRetry = false;
        gPlayContext.isCourse = true;
        gPlayContext.isCourseFirstStage = true;
    }

    // gauge
    auto convertGaugeType = [](int nType) -> eModGauge
    {
        switch (nType)
        {
        case 1: return eModGauge::HARD;
        case 2: return eModGauge::DEATH;
        case 3: return eModGauge::EASY;
        case 4: return eModGauge::PATTACK;
        case 5: return eModGauge::GATTACK;
        case 6: return eModGauge::ASSISTEASY;
        case 7: return eModGauge::EXHARD;
        case 0:
        default: return eModGauge::NORMAL;
        };
    };
    gPlayContext.mods[PLAYER_SLOT_1P].gauge = convertGaugeType(gOptions.get(eOption::PLAY_GAUGE_TYPE_1P));
    gPlayContext.mods[PLAYER_SLOT_2P].gauge = convertGaugeType(gOptions.get(eOption::PLAY_GAUGE_TYPE_2P));

    // random
    auto convertRandomType = [](int nType) -> eModChart
    {
        switch (nType)
        {
        case 1: return eModChart::MIRROR;
        case 2: return eModChart::RANDOM;
        case 3: return eModChart::SRAN;
        case 4: return eModChart::HRAN;
        case 5: return eModChart::ALLSCR;
        case 0:
        default: return eModChart::NONE;
        };
    };
    gPlayContext.mods[PLAYER_SLOT_1P].chart = convertRandomType(gOptions.get(eOption::PLAY_RANDOM_TYPE_1P));
    gPlayContext.mods[PLAYER_SLOT_2P].chart = convertRandomType(gOptions.get(eOption::PLAY_RANDOM_TYPE_2P));

    // assist
    gPlayContext.mods[PLAYER_SLOT_1P].assist_mask |= gSwitches.get(eSwitch::PLAY_OPTION_AUTOSCR_1P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;
    gPlayContext.mods[PLAYER_SLOT_2P].assist_mask |= gSwitches.get(eSwitch::PLAY_OPTION_AUTOSCR_2P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;

    // HS fix
    auto convertHSType = [](int nType) -> eModHs
    {
        switch (nType)
        {
        case 1: return eModHs::MAXBPM;
        case 2: return eModHs::MINBPM;
        case 3: return eModHs::AVERAGE;
        case 4: return eModHs::CONSTANT;
        case 0:
        default: return eModHs::NONE;
        };
    };
    gPlayContext.mods[PLAYER_SLOT_1P].hs = convertHSType(gOptions.get(eOption::PLAY_HSFIX_TYPE_1P));
    gPlayContext.mods[PLAYER_SLOT_2P].hs = convertHSType(gOptions.get(eOption::PLAY_HSFIX_TYPE_2P));

    // chart
    c.started = false;
    switch (entry->type())
    {
    case eEntryType::SONG:
    case eEntryType::RIVAL_SONG:
    case eEntryType::CHART:
    case eEntryType::RIVAL_CHART:
    {
        // set metadata
        if (entry->type() == eEntryType::SONG || entry->type() == eEntryType::RIVAL_SONG)
        {
            auto pFile = std::reinterpret_pointer_cast<FolderSong>(entry)->getCurrentChart();
            c.chartObj = pFile;
        }
        else
        {
            auto pFile = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
            c.chartObj = pFile;
        }

        auto& chart = *c.chartObj;
        //c.path = chart._filePath;
        c.path = chart.absolutePath;

        if (c.hash != chart.fileHash)
        {
            c.isBgaLoaded = false;
            c.isSampleLoaded = false;
        }
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

        // set gamemode
        if (c.chartObj->type() == eChartFormat::BMS)
        {
            auto pBMS = std::reinterpret_pointer_cast<BMS_prop>(c.chartObj);
            switch (pBMS->gamemode)
            {
            case 5:  gPlayContext.mode = eMode::PLAY5;  break;
            case 7:  gPlayContext.mode = eMode::PLAY7;  break;
            case 9:  gPlayContext.mode = eMode::PLAY9;  break;
            case 10: gPlayContext.mode = eMode::PLAY10; break;
            case 14: gPlayContext.mode = eMode::PLAY14; break;
            default: gPlayContext.mode = eMode::PLAY7;  break;
            }
            switch (gOptions.get(eOption::PLAY_BATTLE_TYPE))
            {
            case Option::BATTLE_OFF:   gOptions.set(eOption::PLAY_MODE, (unsigned)ePlayMode::SINGLE_PLAYER); break;
            case Option::BATTLE_LOCAL: gOptions.set(eOption::PLAY_MODE, (unsigned)ePlayMode::LOCAL_BATTLE); break;
            case Option::BATTLE_GHOST: gOptions.set(eOption::PLAY_MODE, (unsigned)ePlayMode::GHOST_BATTLE); break;
            case Option::BATTLE_DB:    gOptions.set(eOption::PLAY_MODE, (unsigned)ePlayMode::SINGLE_PLAYER); break;
            default: assert(false);    gOptions.set(eOption::PLAY_MODE, (unsigned)ePlayMode::SINGLE_PLAYER); break;
            }
            if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == Option::BATTLE_DB)
            {
                switch (pBMS->gamemode)
                {
                case 5:  gPlayContext.mode = eMode::PLAY10;  break;
                case 7:  gPlayContext.mode = eMode::PLAY14;  break;
                default: break;
                }
            }
        }

        break;
    }
    case eEntryType::COURSE:
    {
        // reset mods
        static const std::set<eModGauge> courseGaugeModsAllowed = { eModGauge::NORMAL , eModGauge::HARD };
        if (courseGaugeModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_1P].gauge) == courseGaugeModsAllowed.end())
        {
            gOptions.set(eOption::PLAY_GAUGE_TYPE_1P, 0);
            gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::NORMAL;
        }
        if (courseGaugeModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_2P].gauge) == courseGaugeModsAllowed.end())
        {
            gOptions.set(eOption::PLAY_GAUGE_TYPE_2P, 0);
            gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::NORMAL;
        }
        static const std::set<eModChart> courseChartModsAllowed = { eModChart::NONE , eModChart::MIRROR };
        if (courseChartModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_1P].chart) == courseChartModsAllowed.end())
        {
            gOptions.set(eOption::PLAY_RANDOM_TYPE_1P, 0);
            gPlayContext.mods[PLAYER_SLOT_1P].chart = eModChart::NONE;
        }
        if (courseChartModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_2P].chart) == courseChartModsAllowed.end())
        {
            gOptions.set(eOption::PLAY_RANDOM_TYPE_2P, 0);
            gPlayContext.mods[PLAYER_SLOT_2P].chart = eModChart::NONE;
        }
        gSwitches.set(eSwitch::PLAY_OPTION_AUTOSCR_1P, false);
        gPlayContext.mods[PLAYER_SLOT_1P].assist_mask = 0; 
        gSwitches.set(eSwitch::PLAY_OPTION_AUTOSCR_2P, false);
        gPlayContext.mods[PLAYER_SLOT_2P].assist_mask = 0; 

        // set metadata

        break;
    }
    default:
        break;
    }

    gNextScene = eScene::DECIDE;
}

void SceneSelect::_navigateUpBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        gSelectContext.idx = (gSelectContext.entries.size() + gSelectContext.idx - 1) % gSelectContext.entries.size();

        setBarInfo();
        setEntryInfo();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void SceneSelect::_navigateDownBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        gSelectContext.idx = (gSelectContext.idx + 1) % gSelectContext.entries.size();

        setBarInfo();
        setEntryInfo();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void SceneSelect::_navigateEnter(const Time& t)
{
    if (!gSelectContext.entries.empty())
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        const auto& [e, s] = gSelectContext.entries[gSelectContext.idx];
        switch (e->type())
        {
        case eEntryType::FOLDER:
        {
            SongListProperties prop{
                gSelectContext.backtrace.top().folder,
                e->md5,
                e->_name,
                {},
                {},
                0
            };
            auto top = g_pSongDB->browse(e->md5, false);
            for (size_t i = 0; i < top.getContentsCount(); ++i)
                prop.dbBrowseEntries.push_back({ top.getEntry(i), nullptr });

            gSelectContext.backtrace.top().index = gSelectContext.idx;
            gSelectContext.backtrace.top().displayEntries = gSelectContext.entries;
            gSelectContext.backtrace.push(prop);
            gSelectContext.entries.clear();
            gSelectContext.idx = 0;
            loadSongList();
            sortSongList();

            setBarInfo();
            setEntryInfo();

            if (!gSelectContext.entries.empty())
            {
                gSliders.set(eSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
            }
            else
            {
                gSliders.set(eSlider::SELECT_LIST, 0.0);
            }

            resetJukeboxText();

            scrollAccumulator = 0.;
            scrollAccumulatorAddUnit = 0.;

            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
            break;
        }

        case eEntryType::CUSTOM_FOLDER:
        {
            SongListProperties prop{
                gSelectContext.backtrace.top().folder,
                e->md5,
                e->_name,
                {},
                {},
                0
            };
            auto top = std::dynamic_pointer_cast<FolderTable>(e);
            assert(top != nullptr);
            for (size_t i = 0; i < top->getContentsCount(); ++i)
                prop.dbBrowseEntries.push_back({ top->getEntry(i), nullptr });

            gSelectContext.backtrace.top().index = gSelectContext.idx;
            gSelectContext.backtrace.top().displayEntries = gSelectContext.entries;
            gSelectContext.backtrace.push(prop);
            gSelectContext.entries.clear();
            gSelectContext.idx = 0;
            loadSongList();
            sortSongList();

            setBarInfo();
            setEntryInfo();

            if (!gSelectContext.entries.empty())
            {
                gSliders.set(eSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
            }
            else
            {
                gSliders.set(eSlider::SELECT_LIST, 0.0);
            }

            resetJukeboxText();

            scrollAccumulator = 0.;
            scrollAccumulatorAddUnit = 0.;

            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
            break;
        }

        default:
            break;
        }
    }
    setDynamicTextures();
}
void SceneSelect::_navigateBack(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (gSelectContext.backtrace.size() >= 2)
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        auto& top = gSelectContext.backtrace.top();

        gSelectContext.idx = 0;
        gSelectContext.backtrace.pop();
        auto& parent = gSelectContext.backtrace.top();
        gSelectContext.entries = parent.displayEntries;
        gSelectContext.idx = parent.index;

        setBarInfo();
        setEntryInfo();

        if (!gSelectContext.entries.empty())
        {
            gSliders.set(eSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
        }
        else
        {
            gSliders.set(eSlider::SELECT_LIST, 0.0);
        }

        resetJukeboxText();

        scrollAccumulator = 0.;
        scrollAccumulatorAddUnit = 0.;

        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_CLOSE);
    }
    setDynamicTextures();
}

void SceneSelect::_navigateSongEnter(const Time& t)
{
    isInVersionList = true;

    // TODO
    // play some sound
    // play some animation
    // push current list into buffer
    // create version list
    // show list
}

void SceneSelect::_navigateSongBack(const Time& t)
{
    // TODO
    // play some sound
    // play some animation
    // behavior like _navigateBack

    isInVersionList = false;
}

bool SceneSelect::_closeAllPanels(const Time& t)
{
    bool hasPanelOpened = false;
    for (int i = 1; i <= 9; ++i)
    {
        eSwitch p = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + i);
        if (gSwitches.get(p))
        {
            hasPanelOpened = true;
            gSwitches.set(p, false);
            gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + i), TIMER_NEVER);
            gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + i), t.norm());
        }
    }
    if (hasPanelOpened)
    {
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
    }
    return hasPanelOpened;
}


bool SceneSelect::checkAndStartTextEdit()
{
    if (_skin)
    {
        if (_skin->textEditSpriteClicked())
        {
            if (_skin->textEditType() == eText::EDIT_JUKEBOX_NAME)
            {
                startTextEdit(true);
                return true;
            }
        }
        else if (isInTextEdit())
        {
            stopTextEdit(false);
            return false;
        }
    }
    return false;
}

void SceneSelect::inputGamePressTextEdit(InputMask& input, const Time& t)
{
    if (input[Input::Pad::ESC])
    {
        stopTextEdit(false);
    }
    else if (input[Input::Pad::RETURN])
    {
        if (textEditType() == eText::EDIT_JUKEBOX_NAME)
        {
            stopTextEdit(true);
            std::string searchText = gTexts.get(eText::EDIT_JUKEBOX_NAME);
            searchSong(searchText);
        }
    }
}

void SceneSelect::stopTextEdit(bool modify)
{
    vScene::stopTextEdit(modify);
    if (!modify)
        resetJukeboxText();
}

void SceneSelect::resetJukeboxText()
{
    if (gSelectContext.backtrace.top().name.empty())
        gTexts.set(eText::EDIT_JUKEBOX_NAME, "SEARCH SONG");
    else
        gTexts.set(eText::EDIT_JUKEBOX_NAME, gSelectContext.backtrace.top().name);
}

void SceneSelect::searchSong(const std::string& text)
{
    LOG_DEBUG << "Search: " << text;

    auto top = g_pSongDB->search(ROOT_FOLDER_HASH, text);
    if (top.empty())
    {
        gTexts.set(eText::EDIT_JUKEBOX_NAME, "SEARCH FAILED");
        return;
    }

    std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

    std::string name = "SEARCH: "s + text + " ("s + std::to_string(top.getContentsCount()) + " entries)"s;
    SongListProperties prop{
        "",
        "",
        name,
        {},
        {},
        0
    };
    for (size_t i = 0; i < top.getContentsCount(); ++i)
        prop.dbBrowseEntries.push_back({ top.getEntry(i), nullptr });

    gSelectContext.backtrace.top().index = gSelectContext.idx;
    gSelectContext.backtrace.top().displayEntries = gSelectContext.entries;
    gSelectContext.backtrace.push(prop);
    gSelectContext.entries.clear();
    gSelectContext.idx = 0;
    loadSongList();
    sortSongList();

    setBarInfo();
    setEntryInfo();

    if (!gSelectContext.entries.empty())
    {
        gSliders.set(eSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
    }
    else
    {
        gSliders.set(eSlider::SELECT_LIST, 0.0);
    }

    resetJukeboxText();

    scrollAccumulator = 0.;
    scrollAccumulatorAddUnit = 0.;

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
}