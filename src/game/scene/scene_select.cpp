#include <algorithm>
#include <memory>

#include "scene_select.h"
#include "scene_mgr.h"
#include "scene_context.h"
#include "scene_pre_select.h"
#include "scene_customize.h"

#include "common/chartformat/chartformat_types.h"
#include "common/entry/entry_types.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "config/config_mgr.h"

#include "game/skin/skin_lr2_button_callbacks.h"
#include "game/skin/skin_lr2_slider_callbacks.h"
#include "game/scene/scene_mgr.h"

#include "game/chart/chart_bms.h"
#include "game/ruleset/ruleset_bms_auto.h"

////////////////////////////////////////////////////////////////////////////////

#pragma region save config

void config_sys()
{
    using namespace cfg;

    switch (State::get(IndexOption::SYS_WINDOWED))
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

    ConfigMgr::set('P', P_HISPEED, gPlayContext.Hispeed);
    ConfigMgr::set('P', P_LANECOVER_ENABLE, State::get(IndexSwitch::P1_LANECOVER_ENABLED));
    ConfigMgr::set('P', P_LOCK_SPEED, State::get(IndexSwitch::P1_LOCK_SPEED));

    switch (State::get(IndexOption::PLAY_HSFIX_TYPE))
    {
    case Option::SPEED_FIX_MAX:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_MAX); break;
    case Option::SPEED_FIX_MIN:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_MIN); break;
    case Option::SPEED_FIX_AVG:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_AVG); break;
    case Option::SPEED_FIX_CONSTANT: ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_CONSTANT); break;
    default:                         ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_NORMAL); break;
    }

    switch (State::get(IndexOption::PLAY_TARGET_TYPE))
    {
    case Option::TARGET_0:          ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_0); break;
    case Option::TARGET_MYBEST:     ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_MYBEST); break;
    case Option::TARGET_AAA:        ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_AAA); break;
    case Option::TARGET_AA:         ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_AA); break;
    case Option::TARGET_A:          ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_A); break;
    case Option::TARGET_DEFAULT:    ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_DEFAULT); break;
    case Option::TARGET_IR_TOP:     ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_IR_TOP); break;
    case Option::TARGET_IR_NEXT:    ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_IR_NEXT); break;
    case Option::TARGET_IR_AVERAGE: ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_IR_AVERAGE); break;
    default:                        ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_DEFAULT); break;
    }

    switch (State::get(IndexOption::PLAY_BGA_TYPE))
    {
    case Option::BGA_OFF:      ConfigMgr::set('P', P_BGA_TYPE, P_BGA_TYPE_OFF); break;
    case Option::BGA_ON:       ConfigMgr::set('P', P_BGA_TYPE, P_BGA_TYPE_ON); break;
    case Option::BGA_AUTOPLAY: ConfigMgr::set('P', P_BGA_TYPE, P_BGA_TYPE_AUTOPLAY); break;
    default:                   ConfigMgr::set('P', P_BGA_TYPE, P_BGA_TYPE_ON); break;
    }
    switch (State::get(IndexOption::PLAY_BGA_SIZE))
    {
    case Option::BGA_NORMAL:   ConfigMgr::set('P', P_BGA_SIZE, P_BGA_SIZE_NORMAL); break;
    case Option::BGA_EXTEND:   ConfigMgr::set('P', P_BGA_SIZE, P_BGA_SIZE_EXTEND); break;
    default:                   ConfigMgr::set('P', P_BGA_SIZE, P_BGA_SIZE_NORMAL); break;
    }

    switch (State::get(IndexOption::PLAY_RANDOM_TYPE_1P))
    {
    case Option::RAN_MIRROR: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_MIRROR); break;
    case Option::RAN_RANDOM: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_RANDOM); break;
    case Option::RAN_SRAN:   ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_SRAN); break;
    case Option::RAN_HRAN:   ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_HRAN); break;
    case Option::RAN_ALLSCR: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_ALLSCR); break;
    default:                 ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_NORMAL); break;
    }

    switch (State::get(IndexOption::PLAY_GAUGE_TYPE_1P))
    {
    case Option::GAUGE_HARD:   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_HARD); break;
    case Option::GAUGE_EASY:   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_EASY); break;
    case Option::GAUGE_DEATH:  ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_DEATH); break;
    case Option::GAUGE_EXHARD: ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_EXHARD); break;
    case Option::GAUGE_ASSIST: ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_ASSIST); break;
    default:                   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_NORMAL); break;
    }

    switch (State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P))
    {
    case Option::LANE_OFF:     ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_OFF); break;
    case Option::LANE_HIDDEN:  ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_HIDDEN); break;
    case Option::LANE_SUDDEN:  ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_SUDDEN); break;
    case Option::LANE_SUDHID:  ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_SUDHID); break;
    case Option::LANE_LIFT:    ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_LIFT); break;
    case Option::LANE_LIFTSUD: ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_LIFTSUD); break;
    default:                   ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_OFF); break;
    }

    switch (State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_1P))
    {
    case false: ConfigMgr::set('P', P_CHART_ASSIST_OP, P_CHART_ASSIST_OP_NONE); break;
    case true:  ConfigMgr::set('P', P_CHART_ASSIST_OP, P_CHART_ASSIST_OP_AUTOSCR); break;
    }

    switch (State::get(IndexOption::PLAY_GHOST_TYPE_1P))
    {
    case Option::GHOST_TOP:         ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_A); break;
    case Option::GHOST_SIDE:        ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_B); break;
    case Option::GHOST_SIDE_BOTTOM: ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_C); break;
    default:                        ConfigMgr::set('P',P_GHOST_TYPE, "OFF"); break;
    }

    ConfigMgr::set('P',P_JUDGE_OFFSET, State::get(IndexNumber::TIMING_ADJUST_VISUAL));
    ConfigMgr::set('P',P_GHOST_TARGET, State::get(IndexNumber::DEFAULT_TARGET_RATE));

    switch (State::get(IndexOption::SELECT_FILTER_KEYS))
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

    switch (State::get(IndexOption::SELECT_SORT))
    {
    case Option::SORT_TITLE: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_TITLE); break;
    case Option::SORT_LEVEL: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_LEVEL); break;
    case Option::SORT_CLEAR: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_CLEAR); break;
    case Option::SORT_RATE:  ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_RATE); break;
    default:                 ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_FOLDER); break;
    }

    switch (State::get(IndexOption::SELECT_FILTER_DIFF))
    {
    case Option::DIFF_BEGINNER: ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_BEGINNER); break;
    case Option::DIFF_NORMAL:   ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_NORMAL); break;
    case Option::DIFF_HYPER:    ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_HYPER); break;
    case Option::DIFF_ANOTHER:  ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ANOTHER); break;
    case Option::DIFF_INSANE:   ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_INSANE); break;
    default:                    ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL); break;
    }

    ConfigMgr::set('P',P_FLIP, State::get(IndexSwitch::PLAY_OPTION_DP_FLIP));
    ConfigMgr::set('P',P_SCORE_GRAPH, State::get(IndexSwitch::SYSTEM_SCOREGRAPH));
}

void config_vol()
{
    using namespace cfg;

    ConfigMgr::set('P',P_VOL_MASTER, State::get(IndexSlider::VOLUME_MASTER));
    ConfigMgr::set('P',P_VOL_KEY, State::get(IndexSlider::VOLUME_KEY));
    ConfigMgr::set('P',P_VOL_BGM, State::get(IndexSlider::VOLUME_BGM));
}

void config_eq()
{
    using namespace cfg;

    ConfigMgr::set('P',P_EQ, State::get(IndexSwitch::SOUND_EQ));
    ConfigMgr::set('P',P_EQ0, State::get(IndexNumber::EQ0));
    ConfigMgr::set('P',P_EQ1, State::get(IndexNumber::EQ1));
    ConfigMgr::set('P',P_EQ2, State::get(IndexNumber::EQ2));
    ConfigMgr::set('P',P_EQ3, State::get(IndexNumber::EQ3));
    ConfigMgr::set('P',P_EQ4, State::get(IndexNumber::EQ4));
    ConfigMgr::set('P',P_EQ5, State::get(IndexNumber::EQ5));
    ConfigMgr::set('P',P_EQ6, State::get(IndexNumber::EQ6));
}

void config_freq()
{
    using namespace cfg;

    ConfigMgr::set('P',P_FREQ, State::get(IndexSwitch::SOUND_PITCH));
    switch (State::get(IndexOption::SOUND_PITCH_TYPE))
    {
    case Option::FREQ_FREQ: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_FREQ); break;
    case Option::FREQ_PITCH: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_PITCH); break;
    case Option::FREQ_SPEED: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_SPEED); break;
    default: break;
    }
    ConfigMgr::set('P',P_FREQ_VAL, State::get(IndexNumber::PITCH));
}

void config_fx()
{
    using namespace cfg;

    ConfigMgr::set('P',P_FX0, State::get(IndexSwitch::SOUND_FX0));
    switch (State::get(IndexOption::SOUND_TARGET_FX0))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (State::get(IndexOption::SOUND_FX0))
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
    ConfigMgr::set('P',P_FX0_P1, State::get(IndexNumber::FX0_P1));
    ConfigMgr::set('P', P_FX0_P2, State::get(IndexNumber::FX0_P2));

    ConfigMgr::set('P',P_FX1, State::get(IndexSwitch::SOUND_FX1));
    switch (State::get(IndexOption::SOUND_TARGET_FX1))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (State::get(IndexOption::SOUND_FX1))
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
    ConfigMgr::set('P',P_FX1_P1, State::get(IndexNumber::FX1_P1));
    ConfigMgr::set('P',P_FX1_P2, State::get(IndexNumber::FX1_P2));

    ConfigMgr::set('P',P_FX2, State::get(IndexSwitch::SOUND_FX2));
    switch (State::get(IndexOption::SOUND_TARGET_FX2))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (State::get(IndexOption::SOUND_FX2))
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
    ConfigMgr::set('P',P_FX2_P1, State::get(IndexNumber::FX2_P1));
    ConfigMgr::set('P',P_FX2_P2, State::get(IndexNumber::FX2_P2));
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<SceneCustomize> SceneSelect::_virtualSceneCustomize = nullptr;

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

    switch (State::get(IndexOption::SELECT_FILTER_KEYS))
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

    gSelectContext.filterDifficulty = State::get(IndexOption::SELECT_FILTER_DIFF);

    switch (State::get(IndexOption::SELECT_SORT))
    {
    case Option::SORT_TITLE: gSelectContext.sort = SongListSort::TITLE; break;
    case Option::SORT_LEVEL: gSelectContext.sort = SongListSort::LEVEL; break;
    case Option::SORT_CLEAR: gSelectContext.sort = SongListSort::CLEAR; break;
    case Option::SORT_RATE:  gSelectContext.sort = SongListSort::RATE; break;
    default:                 gSelectContext.sort = SongListSort::DEFAULT; break;
    }

    gPlayContext.isAuto = false;
    gPlayContext.isReplay = false;
    gSelectContext.isGoingToKeyConfig = false;
    gSelectContext.isGoingToSkinSelect = false;
    gSelectContext.isGoingToAutoPlay = false;
    gSelectContext.isGoingToReplay = false;
    gSelectContext.isGoingToReboot = false;
    State::set(IndexSwitch::SYSTEM_AUTOPLAY, false);

    auto& [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);

    State::set(IndexText::_OVERLAY_TOPLEFT, "");
    State::set(IndexText::_OVERLAY_TOPLEFT2, "");

    State::set(IndexSwitch::SOUND_PITCH, true);
    lr2skin::slider::pitch(0.5);
    State::set(IndexSwitch::SOUND_PITCH, ConfigMgr::get('P', cfg::P_FREQ_TYPE_PITCH, false));
    lr2skin::slider::pitch((ConfigMgr::get('P', cfg::P_FREQ_VAL, 0) + 12) / 24.0);

    gPlayContext.Hispeed = ConfigMgr::get('P', cfg::P_HISPEED, 1.0);

    lr2skin::button::target_type(0);

    if (!gInCustomize)
    {
        InputMgr::updateBindings(7);
    }

    _state = eSelectState::PREPARE;
    _updateCallback = std::bind(&SceneSelect::updatePrepare, this);

    // update random options
    loadLR2Sound();

    gCustomizeContext.modeUpdate = false;
    if (_virtualSceneCustomize != nullptr)
    {
        _virtualSceneCustomize->loopStart();
    }

    if (!gInCustomize)
    {
        SoundMgr::stopNoteSamples();
        SoundMgr::stopSysSamples();
        SoundMgr::setSysVolume(1.0);
        SoundMgr::setNoteVolume(1.0);
        SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);
    }

    // do not play preview right after scene is loaded
    previewState = PREVIEW_FINISH;

    _imguiInit();
}

SceneSelect::~SceneSelect()
{
    if (_virtualSceneCustomize != nullptr)
    {
        _virtualSceneCustomize->loopEnd();
        if (gNextScene == eScene::CUSTOMIZE || gNextScene == eScene::EXIT_TRANS || gNextScene == eScene::EXIT)
        {
            _virtualSceneCustomize.reset();
        }
    }

    postStopPreview();
    {
        // safe end loading
        std::unique_lock l(previewMutex);
        previewState = PREVIEW_FINISH;
    }

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

    Time t;

    if (gAppIsExiting)
    {
        gNextScene = eScene::EXIT_TRANS;
    }

    _updateCallback();

    bool idxUpdated = false;

    if (gSelectContext.optionChanged)
    {
        gSelectContext.optionChanged = false;

        State::set(IndexTimer::LIST_MOVE, t.norm());
        navigateTimestamp = t;
        postStopPreview();
    }

    // update by slider
    if (gSelectContext.entryDragging)
    {
        gSelectContext.entryDragging = false;

        size_t idx_new = (size_t)std::floor(State::get(IndexSlider::SELECT_LIST) * gSelectContext.entries.size());
        if (idx_new == gSelectContext.entries.size())
            idx_new = 0;

        if (gSelectContext.idx != idx_new)
        {
            idxUpdated = true;

            navigateTimestamp = t;
            postStopPreview();

            std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

            gSelectContext.idx = idx_new;
            setBarInfo();
            setEntryInfo();

            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
        }
    }

    if (!gSelectContext.entries.empty())
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        if (!(isHoldingUp || isHoldingDown) && 
            (scrollAccumulator > 0 && scrollAccumulator - scrollAccumulatorAddUnit < 0 ||
                scrollAccumulator < 0 && scrollAccumulator - scrollAccumulatorAddUnit > 0 ||
                -0.000001 < scrollAccumulator && scrollAccumulator < 0.000001 ||
                scrollAccumulator * scrollAccumulatorAddUnit < 0))
        {
            bool scrollModified = false;

            if (gSelectContext.scrollDirection != 0)
            {
                double posOld = State::get(IndexSlider::SELECT_LIST);
                double idxOld = posOld * gSelectContext.entries.size();

                int idxNew = (int)idxOld;
                if (gSelectContext.scrollDirection > 0)
                {
                    double idxOldTmp = idxOld;
                    if (idxOldTmp - (int)idxOldTmp < 0.0001)
                        idxOldTmp -= 0.0001;
                    idxNew = std::floor(idxOldTmp) + 1;
                }
                else
                {
                    double idxOldTmp = idxOld;
                    if (idxOldTmp - (int)idxOldTmp > 0.9999)
                        idxOldTmp += 0.0001;
                    idxNew = std::ceil(idxOldTmp) - 1;
                }

                double margin = idxNew - idxOld;
                if (margin < -0.05 || 0.05 < margin)
                {
                    scrollAccumulator = margin;
                    scrollAccumulatorAddUnit = scrollAccumulator / 100 * (getRate() / 1000);
                    scrollModified = true;
                }
            }

            if (!scrollModified)
            {
                State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
                scrollAccumulator = 0.0;
                scrollAccumulatorAddUnit = 0.0;
                gSelectContext.scrollDirection = 0;
                _skin->reset_bar_animation();
                State::set(IndexTimer::LIST_MOVE_STOP, t.norm());
                gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
            }
        }
        else if(gSelectContext.scrollDirection != 0 || scrollAccumulatorAddUnit < -0.003 || scrollAccumulatorAddUnit > 0.003)
        {
            if (gSelectContext.scrollDirection == 0)
            {
                _skin->start_bar_animation();
            }

            double posOld = State::get(IndexSlider::SELECT_LIST);
            double posNew = posOld + scrollAccumulatorAddUnit / gSelectContext.entries.size();

            int idxOld = (int)std::round(posOld * gSelectContext.entries.size());
            int idxNew = (int)std::round(posNew * gSelectContext.entries.size());
            if (idxOld != idxNew)
            {
                if (idxOld < idxNew)
                    _navigateDownBy1(t);
                else
                    _navigateUpBy1(t);

                idxUpdated = true;
            }

            while (posNew < 0.) posNew += 1.;
            while (posNew >= 1.) posNew -= 1.;
            State::set(IndexSlider::SELECT_LIST, posNew);

            scrollAccumulator -= scrollAccumulatorAddUnit;
            if (scrollAccumulator < -0.000001 || scrollAccumulator > 0.000001)
                gSelectContext.scrollDirection = scrollAccumulator > 0. ? 1 : -1;
        }
    }

    if (idxUpdated)
        setDynamicTextures();

    if (!gInCustomize && gCustomizeContext.modeUpdate)
    {
        gCustomizeContext.modeUpdate = false;

        if (_virtualSceneCustomize == nullptr)
        {
            createNotification("Loading skin options...");

            _skin->setHandleMouseEvents(false);
            _virtualSceneCustomize = std::make_shared<SceneCustomize>();
            _virtualSceneCustomize->loopStart();
            _skin->setHandleMouseEvents(true);

            createNotification("Load finished.");
        }
    }

    // load preview
    if (!gInCustomize && (t - navigateTimestamp).norm() > 500)
    {
        updatePreview();
    }
}

void SceneSelect::updatePrepare()
{
    Time t;
    Time rt = t - State::get(IndexTimer::SCENE_START);

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

        State::set(IndexTimer::LIST_MOVE, t.norm());
        State::set(IndexTimer::LIST_MOVE_STOP, t.norm());

        // restore panel stat
        for (int i = 1; i <= 9; ++i)
        {
            IndexSwitch p = static_cast<IndexSwitch>(int(IndexSwitch::SELECT_PANEL1) - 1 + i);
            if (State::get(p))
            {
                IndexTimer tm = static_cast<IndexTimer>(int(IndexTimer::PANEL1_START) - 1 + i);
                State::set(tm, t.norm());
                SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
            }
        }

        LOG_DEBUG << "[Select] State changed to SELECT";
    }
}

void SceneSelect::updateSelect()
{
    Time t;
    Time rt = t - State::get(IndexTimer::SCENE_START);

    if (!refreshingSongList)
    {
        int line = (int)IndexText::_OVERLAY_TOPLEFT;
        if (State::get(IndexSwitch::SELECT_PANEL1))
        {
            if (!_skin->isSupportGreenNumber)
            {
                std::stringstream ss;
                bool lock1 = State::get(IndexSwitch::P1_LOCK_SPEED);
                if (lock1) ss << "G(1P): FIX " << ConfigMgr::get('P', cfg::P_GREENNUMBER, 0);

                bool lock2 = State::get(IndexSwitch::P2_LOCK_SPEED);
                if (lock2) ss << (lock1 ? " | " : "") << "G(2P): FIX " << gPlayContext.battle2PGreenNumber;

                std::string s = ss.str();
                if (!s.empty())
                {
                    State::set((IndexText)line++, ss.str());
                }
            }
            if (!_skin->isSupportLift)
            {
                std::stringstream ss;
                int lane1 = State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P);
                switch (lane1)
                {
                case Option::LANE_OFF:      ss << "Lane(1P): OFF"; break;
                case Option::LANE_HIDDEN:   ss << "Lane(1P): HIDDEN+"; break;
                case Option::LANE_SUDDEN:   ss << "Lane(1P): SUDDEN+"; break;
                case Option::LANE_SUDHID:   ss << "Lane(1P): SUD+ & HID+"; break;
                case Option::LANE_LIFT:     ss << "Lane(1P): LIFT"; break;
                case Option::LANE_LIFTSUD:  ss << "Lane(1P): LIFT & SUD+"; break;
                }

                if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_LOCAL)
                {
                    int lane2 = State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_2P);
                    switch (lane2)
                    {
                    case Option::LANE_OFF:      ss << " | Lane(2P): OFF"; break;
                    case Option::LANE_HIDDEN:   ss << " | Lane(2P): HIDDEN+"; break;
                    case Option::LANE_SUDDEN:   ss << " | Lane(2P): SUDDEN+"; break;
                    case Option::LANE_SUDHID:   ss << " | Lane(2P): SUD+ & HID+"; break;
                    case Option::LANE_LIFT:     ss << " | Lane(2P): LIFT"; break;
                    case Option::LANE_LIFTSUD:  ss << " | Lane(2P): LIFT & SUD+"; break;
                    }
                }

                std::string s = ss.str();
                if (!s.empty())
                {
                    State::set((IndexText)line++, ss.str());
                }
            }
        }
        while (line <= (int)IndexText::_OVERLAY_TOPLEFT2)
            State::set((IndexText)line++, "");
    }

    if (gSelectContext.isGoingToKeyConfig || gSelectContext.isGoingToSkinSelect || gSelectContext.isGoingToReboot)
    {
        if (!gInCustomize) SoundMgr::setSysVolume(0.0, 500);
        State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
        _state = eSelectState::FADEOUT;
        _updateCallback = std::bind(&SceneSelect::updateFadeout, this);
    }
    else if (gSelectContext.isGoingToAutoPlay)
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
                State::set(IndexSwitch::SYSTEM_AUTOPLAY, true);
                _decide();
                break;
            }
        }
    }
    else if (gSelectContext.isGoingToReplay)
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
            {
                if (State::get(IndexSwitch::CHART_HAVE_REPLAY))
                {
                    gPlayContext.isReplay = true;
                    _decide();
                }
                break;
            }
            }
        }
    }
}

void SceneSelect::updateSearch()
{
    Time t;
    Time rt = t - State::get(IndexTimer::SCENE_START);
}

void SceneSelect::updatePanel(unsigned idx)
{
    Time t;
    Time rt = t - State::get(IndexTimer::SCENE_START);
}

void SceneSelect::updateFadeout()
{
    Time t;
    Time rt = t - State::get(IndexTimer::SCENE_START);
    Time ft = t - State::get(IndexTimer::FADEOUT_BEGIN);

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
            gInCustomize = true;
        }
        else if (gSelectContext.isGoingToReboot)
        {
            SoundMgr::stopSysSamples();
            gNextScene = eScene::PRE_SELECT;
        }
        else
        {
            SoundMgr::stopSysSamples();
            gNextScene = eScene::EXIT_TRANS;
        }
    }
}

void SceneSelect::update()
{
    vScene::update();

    if (_virtualSceneLoadSongs)
        _virtualSceneLoadSongs->update();
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
    Time rt = t - State::get(IndexTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    if (refreshingSongList) return;

    using namespace Input;

    if (isInTextEdit())
    {
        inputGamePressTextEdit(m, t);
        return;
    }

    if (imguiShow)
    {
        if (m[Input::Pad::F9] || m[Input::Pad::ESC])
        {
            imguiShow = false;
            imgui_add_profile_popup = false;
            _skin->setHandleMouseEvents(true);
        }
        return;
    }
    else
    {
        if (m[Input::Pad::F9] || m[Input::Pad::ESC])
        {
            imguiShow = !imguiShow;
            _skin->setHandleMouseEvents(!imguiShow);
        }
    }

    //if (m[Input::Pad::ESC])
    //{
    //    // close panels if opened. exit if no panel is opened
    //    bool hasPanelOpened = _closeAllPanels(t);
    //    if (!hasPanelOpened)
    //    {
    //        LOG_DEBUG << "[Select] ESC";
    //        gNextScene = eScene::EXIT_TRANS;
    //        return;
    //    }
    //}

    auto input = _inputAvailable & m;
    if (input.any())
    {
        if (input[Pad::K15]) isHoldingK15 = true;
        if (input[Pad::K16]) isHoldingK16 = true;
        if (input[Pad::K17]) isHoldingK17 = true;
        if (input[Pad::K25]) isHoldingK25 = true;
        if (input[Pad::K26]) isHoldingK26 = true;
        if (input[Pad::K27]) isHoldingK27 = true;

        // sub callbacks
        if (State::get(IndexSwitch::SELECT_PANEL1))
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
                State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K11_DOWN) + k - Pad::K11), t.norm());
                State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K11_UP) + k - Pad::K11), TIMER_NEVER);
            }
        }
        for (size_t k = Pad::K21; k <= Pad::K29; ++k)
        {
            if (input[k])
            {
                if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_LOCAL)
                {
                    State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K21_DOWN) + k - Pad::K21), t.norm());
                    State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K21_UP) + k - Pad::K21), TIMER_NEVER);
                }
                else
                {
                    State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K11_DOWN) + k - Pad::K21), t.norm());
                    State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K11_UP) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }
    }
}

// CALLBACK
void SceneSelect::inputGameHold(InputMask& m, const Time& t)
{
    Time rt = t - State::get(IndexTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // sub callbacks
        if (State::get(IndexSwitch::SELECT_PANEL1))
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
    Time rt = t - State::get(IndexTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        if (input[Pad::K15]) isHoldingK15 = false;
        if (input[Pad::K16]) isHoldingK16 = false;
        if (input[Pad::K17]) isHoldingK17 = false;
        if (input[Pad::K25]) isHoldingK25 = false;
        if (input[Pad::K26]) isHoldingK26 = false;
        if (input[Pad::K27]) isHoldingK27 = false;

        // sub callbacks
        if (State::get(IndexSwitch::SELECT_PANEL1))
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
                State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K11_UP) + k - Pad::K11), t.norm());
                State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K11_DOWN) + k - Pad::K11), TIMER_NEVER);
            }
        }
        for (size_t k = Pad::K21; k <= Pad::K29; ++k)
        {
            if (input[k])
            {
                if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_LOCAL)
                {
                    State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K21_UP) + k - Pad::K21), t.norm());
                    State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K21_DOWN) + k - Pad::K21), TIMER_NEVER);
                }
                else
                {
                    State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K11_UP) + k - Pad::K21), t.norm());
                    State::set(static_cast<IndexTimer>(static_cast<size_t>(IndexTimer::K11_DOWN) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }
    }
}


void SceneSelect::inputGamePressSelect(InputMask& input, const Time& t)
{
    if (input[Input::Pad::F8])
    {
        // refresh folder
        // LR2 behavior: refresh all folders regardless in which folder
        // May optimize at some time

        refreshingSongList = true;

        if (gSelectContext.backtrace.size() >= 2)
        {
            // only update current folder
            auto& [hasPath, path] = g_pSongDB->getFolderPath(gSelectContext.backtrace.top().folder);
            if (hasPath)
            {
                LOG_INFO << "[List] Refreshing folder " << path.u8string();
                State::set(IndexText::_OVERLAY_TOPLEFT, (boost::format("Refresh folder: %s") % path.u8string()).str());

                g_pSongDB->resetAddSummary();
                int count = g_pSongDB->addSubFolder(path, gSelectContext.backtrace.top().parent);
                g_pSongDB->waitLoadingFinish();

                int added = g_pSongDB->addChartSuccess - g_pSongDB->addChartModified;
                int updated = g_pSongDB->addChartModified;
                int deleted = g_pSongDB->addChartDeleted;
                if (added || updated || deleted)
                {
                    createNotification((boost::format("%s: Added %d, Updated %d, Deleted %d") % path.u8string() % added % updated % deleted).str());
                }
                State::set(IndexText::_OVERLAY_TOPLEFT, "");
                State::set(IndexText::_OVERLAY_TOPLEFT2, "");
            }

            // re-browse
            if (!isInVersionList)
                selectDownTimestamp = -1;

            // simplified _navigateBack(t)
            {
                std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
                auto& top = gSelectContext.backtrace.top();

                gSelectContext.idx = 0;
                gSelectContext.backtrace.pop();
                auto& parent = gSelectContext.backtrace.top();
                gSelectContext.entries = parent.displayEntries;
                gSelectContext.idx = parent.index;
            }

            // reset infos, play sound
            _navigateEnter(Time());
        }
        else
        {
            // Pressed at root, refresh all
            LOG_INFO << "[List] Refreshing all folders";

            // re-browse
            if (!isInVersionList)
                selectDownTimestamp = -1;

            // Make a virtual preload scene to rebuild song list. This is very tricky...
            assert(_virtualSceneLoadSongs == nullptr);
            if (_virtualSceneLoadSongs == nullptr)
            {
                _virtualSceneLoadSongs = std::make_shared<ScenePreSelect>();
                _virtualSceneLoadSongs->loopStart();
                while (!_virtualSceneLoadSongs->isLoadingFinished())
                {
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(33ms);
                }
                _virtualSceneLoadSongs->loopEnd();
                _virtualSceneLoadSongs.reset();
            }

            {
                std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
                gSelectContext.idx = 0;
                loadSongList();
                sortSongList();
                setBarInfo();
                setEntryInfo();

                resetJukeboxText();
            }

            State::set(IndexTimer::LIST_MOVE, Time().norm());
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_F_OPEN);
        }

        setDynamicTextures();

        refreshingSongList = false;
        return;
    }

    if (_skin->type() == eSkinType::LR2)
    {
        if (input[Input::Pad::K1START] || input[Input::Pad::K2START])
        {
            // close other panels
            _closeAllPanels(t);

            // open panel 1
            State::set(IndexSwitch::SELECT_PANEL1, true);
            State::set(IndexTimer::PANEL1_START, t.norm());
            State::set(IndexTimer::PANEL1_END, TIMER_NEVER);
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
            return;
        }
        if (input[Input::M2])
        {
            bool hasPanelOpened = false;
            for (int i = 1; i <= 9; ++i)
            {
                IndexSwitch p = static_cast<IndexSwitch>(int(IndexSwitch::SELECT_PANEL1) - 1 + i);
                if (State::get(p))
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
            switch (!gSelectContext.entries.empty() && gSelectContext.entries[gSelectContext.idx].first->type())
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
        case eEntryType::COURSE_FOLDER:
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
                auto pSong = std::dynamic_pointer_cast<EntryFolderSong>(gSelectContext.entries[gSelectContext.idx].first);
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
        if (State::get(IndexSwitch::SELECT_PANEL1))
        {
            if (input[Pad::K1SELECT] || input[Pad::K2SELECT])
            {
                // SELECT: TARGET
                lr2skin::button::target_type(1);
            }

            // 1: KEYS
            if (input[Pad::K11]) lr2skin::button::select_keys_filter(1);
            if (input[Pad::K12]) lr2skin::button::random_type(PLAYER_SLOT_PLAYER, 1);
            if (input[Pad::K13]) lr2skin::button::battle(1);
            if (input[Pad::K14]) lr2skin::button::gauge_type(PLAYER_SLOT_PLAYER, 1);

            if (isHoldingK16 && input[Pad::K17] || isHoldingK17 && input[Pad::K16])
            {
                // 6+7: LANE EFFECT
                lr2skin::button::lane_effect(PLAYER_SLOT_PLAYER, 1);
            }
            else if (isHoldingK15 && input[Pad::K17] || isHoldingK17 && input[Pad::K15])
            {
                // 5+7: HS FIX
                lr2skin::button::hs_fix(1);
            }
            else
            {
                if (input[Pad::K15]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, -1);
                if (input[Pad::K16]) lr2skin::button::autoscr(PLAYER_SLOT_PLAYER, 1);
                if (input[Pad::K17]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, 1);
            }

            if (State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DOUBLE ||
                State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DOUBLE_BATTLE ||
                State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DP_GHOST_BATTLE)
            {
                // 1: KEYS
                if (input[Pad::K21]) lr2skin::button::select_keys_filter(1);
                if (input[Pad::K22]) lr2skin::button::random_type(PLAYER_SLOT_TARGET, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(PLAYER_SLOT_TARGET, 1);

                if (isHoldingK26 && input[Pad::K27] || isHoldingK27 && input[Pad::K26])
                {
                    // 6+7: LANE EFFECT
                    lr2skin::button::lane_effect(PLAYER_SLOT_TARGET, 1);
                }
                else if (isHoldingK25 && input[Pad::K27] || isHoldingK27 && input[Pad::K25])
                {
                    // 5+7: HS FIX
                    lr2skin::button::hs_fix(1);
                }
                else
                {
                    if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_TARGET, -1);
                    if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_TARGET, 1);
                    if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_TARGET, 1);
                }
            }
            else
            {
                // 1: KEYS
                if (input[Pad::K21]) lr2skin::button::select_keys_filter(1);
                if (input[Pad::K22]) lr2skin::button::random_type(PLAYER_SLOT_PLAYER, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(PLAYER_SLOT_PLAYER, 1);

                if (isHoldingK26 && input[Pad::K27] || isHoldingK27 && input[Pad::K26] ||
                    isHoldingK16 && input[Pad::K27] || isHoldingK17 && input[Pad::K26] ||
                    isHoldingK26 && input[Pad::K17] || isHoldingK27 && input[Pad::K16])
                {
                    // 6+7: LANE EFFECT
                    lr2skin::button::lane_effect(PLAYER_SLOT_PLAYER, 1);
                }
                else if (isHoldingK25 && input[Pad::K27] || isHoldingK27 && input[Pad::K25] ||
                    isHoldingK15 && input[Pad::K27] || isHoldingK17 && input[Pad::K25] ||
                    isHoldingK25 && input[Pad::K17] || isHoldingK27 && input[Pad::K15])
                {
                    // 5+7: HS FIX
                    lr2skin::button::hs_fix(1);
                }
                else
                {
                    if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, -1);
                    if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_PLAYER, 1);
                    if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, 1);
                }
            }
        }
    }
}

void SceneSelect::inputGameHoldPanel(InputMask& input, const Time& t)
{
}

void SceneSelect::inputGameReleasePanel(InputMask& input, const Time& t)
{
    if (State::get(IndexSwitch::SELECT_PANEL1) && (input[Input::Pad::K1START] || input[Input::Pad::K2START]))
    {
        // close panel 1
        State::set(IndexSwitch::SELECT_PANEL1, false);
        State::set(IndexTimer::PANEL1_START, TIMER_NEVER);
        State::set(IndexTimer::PANEL1_END, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
        return;
    }
}


void SceneSelect::_decide()
{
    std::shared_lock<std::shared_mutex> u(gSelectContext._mutex);

    if (State::get(IndexOption::SELECT_ENTRY_TYPE) == Option::ENTRY_COURSE && State::get(IndexSwitch::COURSE_NOT_PLAYABLE))
        return;

    auto& [entry, score] = gSelectContext.entries[gSelectContext.idx];
    //auto& chart = entry.charts[entry.chart_idx];

    clearContextPlay();

    gPlayContext.canRetry = !(gPlayContext.isAuto || gPlayContext.isReplay);

    switch (State::get(IndexOption::PLAY_BATTLE_TYPE))
    {
    case Option::BATTLE_OFF:
    case Option::BATTLE_DB:     gPlayContext.isBattle = false; break;
    case Option::BATTLE_LOCAL:  gPlayContext.isBattle = true; break;
    case Option::BATTLE_GHOST:  gPlayContext.isBattle = !gPlayContext.isAuto && State::get(IndexSwitch::CHART_HAVE_REPLAY); break;
    }

    if (entry->type() == eEntryType::COURSE)
    {
        gPlayContext.canRetry = false;
        gPlayContext.isBattle = false;
        gPlayContext.isCourse = true;
        gPlayContext.courseStage = 0;
    }

    // chart
    gChartContext.started = false;
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
            auto pFile = std::reinterpret_pointer_cast<EntryFolderSong>(entry)->getCurrentChart();
            gChartContext.chartObj = pFile;
        }
        else
        {
            auto pFile = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
            gChartContext.chartObj = pFile;
        }

        auto& chart = *gChartContext.chartObj;
        //gChartContext.path = chart._filePath;
        gChartContext.path = chart.absolutePath;

        // only reload resources if selected chart is different
        if (gChartContext.hash != chart.fileHash)
        {
            gChartContext.isBgaLoaded = false;
            gChartContext.isSampleLoaded = false;
        }
        gChartContext.hash = chart.fileHash;

        //gChartContext.chartObj = std::make_shared<ChartFormatBase>(chart);
        gChartContext.title = chart.title;
        gChartContext.title2 = chart.title2;
        gChartContext.artist = chart.artist;
        gChartContext.artist2 = chart.artist2;
        gChartContext.genre = chart.genre;
        gChartContext.version = chart.version;
        gChartContext.level = chart.levelEstimated;
        gChartContext.minBPM = chart.minBPM;
        gChartContext.maxBPM = chart.maxBPM;
        gChartContext.startBPM = chart.startBPM;


        // set gamemode
        gChartContext.isDoubleBattle = false;
        if (gChartContext.chartObj->type() == eChartFormat::BMS)
        {
            auto pBMS = std::reinterpret_pointer_cast<ChartFormatBMSMeta>(gChartContext.chartObj);
            switch (pBMS->gamemode)
            {
            case 5:  gPlayContext.mode = eMode::PLAY5;  break;
            case 7:  gPlayContext.mode = eMode::PLAY7;  break;
            case 9:  gPlayContext.mode = eMode::PLAY9;  break;
            case 10: gPlayContext.mode = eMode::PLAY10; break;
            case 14: gPlayContext.mode = eMode::PLAY14; break;
            default: gPlayContext.mode = eMode::PLAY7;  break;
            }
            if (gPlayContext.isBattle)
            {
                if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_LOCAL || 
                    State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_GHOST)
                {
                    switch (pBMS->gamemode)
                    {
                    case 5:  gPlayContext.mode = eMode::PLAY5_2;  break;
                    case 7:  gPlayContext.mode = eMode::PLAY7_2;  break;
                    case 9:  gPlayContext.mode = eMode::PLAY9;  gPlayContext.isBattle = false; break;
                    case 10: gPlayContext.mode = eMode::PLAY10; gPlayContext.isBattle = false; break;
                    case 14: gPlayContext.mode = eMode::PLAY14; gPlayContext.isBattle = false; break;
                    default: assert(false); break;
                    }
                }
                else if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_DB)
                {
                    switch (pBMS->gamemode)
                    {
                    case 5:  gPlayContext.mode = eMode::PLAY10;  break;
                    case 7:  gPlayContext.mode = eMode::PLAY14;  break;
                    default: assert(false); break;
                    }
                    gChartContext.isDoubleBattle = true;
                }
            }
        }

        switch (gPlayContext.mode)
        {
        case eMode::PLAY5:
        case eMode::PLAY7:
        case eMode::PLAY9:
            assert(!gPlayContext.isBattle);
            assert(!gChartContext.isDoubleBattle);
            break;

        case eMode::PLAY5_2:
        case eMode::PLAY7_2:
        case eMode::PLAY9_2:
            assert(gPlayContext.isBattle);
            assert(!gChartContext.isDoubleBattle);
            break;

        case eMode::PLAY10:
        case eMode::PLAY14:
            assert(!gPlayContext.isBattle);
            break;
        }

        break;
    }
    case eEntryType::COURSE:
    {
        // reset mods
        static const std::set<eModGauge> courseGaugeModsAllowed = { eModGauge::NORMAL , eModGauge::HARD };
        if (courseGaugeModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge) == courseGaugeModsAllowed.end())
        {
            State::set(IndexOption::PLAY_GAUGE_TYPE_1P, 0);
            gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge = eModGauge::NORMAL;
        }
        if (courseGaugeModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_TARGET].gauge) == courseGaugeModsAllowed.end())
        {
            State::set(IndexOption::PLAY_GAUGE_TYPE_2P, 0);
            gPlayContext.mods[PLAYER_SLOT_TARGET].gauge = eModGauge::NORMAL;
        }
        static const std::set<eModRandom> courseChartModsAllowed = { eModRandom::NONE , eModRandom::MIRROR };
        if (courseChartModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft) == courseChartModsAllowed.end())
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_1P, 0);
            gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft = eModRandom::NONE;
        }
        if (courseChartModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_TARGET].randomLeft) == courseChartModsAllowed.end())
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_2P, 0);
            gPlayContext.mods[PLAYER_SLOT_TARGET].randomLeft = eModRandom::NONE;
        }
        State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_1P, false);
        gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask = 0;
        State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_2P, false);
        gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask = 0;

        // set metadata
        auto pCourse = std::dynamic_pointer_cast<EntryCourse>(entry);
        gPlayContext.courseHash = pCourse->md5;
        gPlayContext.courseCharts = pCourse->charts;
        gPlayContext.courseStageRulesetCopy[0].clear();
        gPlayContext.courseStageRulesetCopy[1].clear();
        gPlayContext.courseStageReplayPath.clear();

        auto pChart = *g_pSongDB->findChartByHash(*pCourse->charts.begin()).begin();
        gChartContext.chartObj = pChart;

        auto& chart = *gChartContext.chartObj;
        //gChartContext.path = chart._filePath;
        gChartContext.path = chart.absolutePath;

        // only reload resources if selected chart is different
        if (gChartContext.hash != chart.fileHash)
        {
            gChartContext.isBgaLoaded = false;
            gChartContext.isSampleLoaded = false;
        }
        gChartContext.hash = chart.fileHash;

        //gChartContext.chartObj = std::make_shared<ChartFormatBase>(chart);
        gChartContext.title = chart.title;
        gChartContext.title2 = chart.title2;
        gChartContext.artist = chart.artist;
        gChartContext.artist2 = chart.artist2;
        gChartContext.genre = chart.genre;
        gChartContext.version = chart.version;
        gChartContext.level = chart.levelEstimated;
        gChartContext.minBPM = chart.minBPM;
        gChartContext.maxBPM = chart.maxBPM;
        gChartContext.startBPM = chart.startBPM;

        break;
    }
    default:
        break;
    }

    if (!gPlayContext.isReplay)
    {
        // gauge
        auto convertGaugeType = [](int nType) -> eModGauge
        {
            if (gPlayContext.isCourse)
            {
                if (State::get(IndexOption::COURSE_TYPE) == Option::COURSE_GRADE)
                {
                    switch (nType)
                    {
                    case 1:
                    case 4:
                    case 5:
                    case 7: return eModGauge::GRADE_HARD;
                    case 2: return eModGauge::GRADE_DEATH;
                    case 0:
                    default: return eModGauge::GRADE_NORMAL;
                    };
                }
            }

            switch (nType)
            {
            case 1: return eModGauge::HARD;
            case 2: return eModGauge::DEATH;
            case 3: return eModGauge::EASY;
                // case 4: return eModGauge::PATTACK;
                // case 5: return eModGauge::GATTACK;
            case 4: return eModGauge::HARD;
            case 5: return eModGauge::HARD;
            case 6: return eModGauge::ASSISTEASY;
            case 7: return eModGauge::EXHARD;
            case 0:
            default: return eModGauge::NORMAL;
            };
        };
        gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge = convertGaugeType(State::get(IndexOption::PLAY_GAUGE_TYPE_1P));

        // random
        auto convertRandomType = [](int nType) -> eModRandom
        {
            switch (nType)
            {
            case 1: return eModRandom::MIRROR;
            case 2: return eModRandom::RANDOM;
            case 3: return eModRandom::SRAN;
            case 4: return eModRandom::HRAN;
            case 5: return eModRandom::ALLSCR;
            case 0:
            default: return eModRandom::NONE;
            };
        };
        gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft = convertRandomType(State::get(IndexOption::PLAY_RANDOM_TYPE_1P));

        if (gPlayContext.mode == eMode::PLAY10 || gPlayContext.mode == eMode::PLAY14)
        {
            gPlayContext.mods[PLAYER_SLOT_PLAYER].randomRight = convertRandomType(State::get(IndexOption::PLAY_RANDOM_TYPE_2P));
            gPlayContext.mods[PLAYER_SLOT_TARGET].randomRight = convertRandomType(State::get(IndexOption::PLAY_RANDOM_TYPE_2P));
            gPlayContext.mods[PLAYER_SLOT_PLAYER].DPFlip = State::get(IndexSwitch::PLAY_OPTION_DP_FLIP);
            gPlayContext.mods[PLAYER_SLOT_TARGET].DPFlip = State::get(IndexSwitch::PLAY_OPTION_DP_FLIP);
        }

        // assist
        gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask |= State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_1P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;

        // lane
        gPlayContext.mods[PLAYER_SLOT_PLAYER].laneEffect = (eModLaneEffect)State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P);
        gPlayContext.mods[PLAYER_SLOT_TARGET].laneEffect = (eModLaneEffect)State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_2P);

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
        gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix = convertHSType(State::get(IndexOption::PLAY_HSFIX_TYPE));
        gPlayContext.mods[PLAYER_SLOT_TARGET].hispeedFix = convertHSType(State::get(IndexOption::PLAY_HSFIX_TYPE));

        if (gPlayContext.isBattle)
        {
            if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_GHOST && gPlayContext.replay != nullptr)
            {
                // notes are loaded in 2P area, we should check randomRight instead of randomLeft
                gPlayContext.mods[PLAYER_SLOT_TARGET].gauge = gPlayContext.replay->gaugeType;
                gPlayContext.mods[PLAYER_SLOT_TARGET].randomRight = gPlayContext.replay->randomTypeLeft;
                gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask = gPlayContext.replay->assistMask;
                gPlayContext.mods[PLAYER_SLOT_TARGET].laneEffect = (eModLaneEffect)gPlayContext.replay->laneEffectType;
            }
            else
            {
                // notes are loaded in 2P area, we should check randomRight instead of randomLeft
                gPlayContext.mods[PLAYER_SLOT_TARGET].gauge = convertGaugeType(State::get(IndexOption::PLAY_GAUGE_TYPE_2P));
                gPlayContext.mods[PLAYER_SLOT_TARGET].randomRight = convertRandomType(State::get(IndexOption::PLAY_RANDOM_TYPE_2P));
                gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask |= State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_2P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;
            }
        }
        else
        {
            // copy 1P setting for target
            gPlayContext.mods[PLAYER_SLOT_TARGET].gauge = convertGaugeType(State::get(IndexOption::PLAY_GAUGE_TYPE_1P));
            gPlayContext.mods[PLAYER_SLOT_TARGET].randomLeft = convertRandomType(State::get(IndexOption::PLAY_RANDOM_TYPE_1P));
            gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask = 0;  // rival do not use assist options
        }
    }
    else // gPlayContext.isReplay
    {
        gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft = gPlayContext.replay->randomTypeLeft;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].randomRight = gPlayContext.replay->randomTypeRight;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge = gPlayContext.replay->gaugeType;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask = gPlayContext.replay->assistMask;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix = gPlayContext.replay->hispeedFix;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].laneEffect = (eModLaneEffect)gPlayContext.replay->laneEffectType;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].DPFlip = gPlayContext.replay->DPFlip;
    }
    
    // score (mybest)
    if (!gPlayContext.isBattle)
    {
        auto pScore = score;
        if (pScore == nullptr)
        {
            pScore = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
        }
        if (pScore && !pScore->replayFileName.empty())
        {
            Path replayFilePath = ReplayChart::getReplayPath(gChartContext.hash) / pScore->replayFileName;
            if (fs::is_regular_file(replayFilePath))
            {
                gPlayContext.replayMybest = std::make_shared<ReplayChart>();
                if (gPlayContext.replayMybest->loadFile(replayFilePath))
                {
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].randomLeft = gPlayContext.replayMybest->randomTypeLeft;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].randomRight = gPlayContext.replayMybest->randomTypeRight;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].gauge = gPlayContext.replayMybest->gaugeType;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].assist_mask = gPlayContext.replayMybest->assistMask;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].hispeedFix = gPlayContext.replayMybest->hispeedFix;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].laneEffect = (eModLaneEffect)gPlayContext.replayMybest->laneEffectType;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].DPFlip = gPlayContext.replayMybest->DPFlip;
                }
                else
                {
                    gPlayContext.replayMybest.reset();
                }
            }
        }
    }

    gNextScene = eScene::DECIDE;
}

void SceneSelect::_navigateUpBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

        gSelectContext.idx = (gSelectContext.entries.size() + gSelectContext.idx - 1) % gSelectContext.entries.size();

        setBarInfo();
        setEntryInfo();

        State::set(IndexTimer::LIST_MOVE, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void SceneSelect::_navigateDownBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

        gSelectContext.idx = (gSelectContext.idx + 1) % gSelectContext.entries.size();

        setBarInfo();
        setEntryInfo();

        State::set(IndexTimer::LIST_MOVE, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void SceneSelect::_navigateEnter(const Time& t)
{
    if (!gSelectContext.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

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
                State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
            }
            else
            {
                State::set(IndexSlider::SELECT_LIST, 0.0);
            }

            resetJukeboxText();

            scrollAccumulator = 0.;
            scrollAccumulatorAddUnit = 0.;

            State::set(IndexTimer::LIST_MOVE, Time().norm());
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
            auto top = std::dynamic_pointer_cast<EntryFolderTable>(e);
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
                State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
            }
            else
            {
                State::set(IndexSlider::SELECT_LIST, 0.0);
            }

            resetJukeboxText();

            scrollAccumulator = 0.;
            scrollAccumulatorAddUnit = 0.;

            State::set(IndexTimer::LIST_MOVE, Time().norm());
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
            break;
        }

        case eEntryType::COURSE_FOLDER:
        {
            SongListProperties prop{
                gSelectContext.backtrace.top().folder,
                e->md5,
                e->_name,
                {},
                {},
                0
            };
            auto top = std::dynamic_pointer_cast<EntryFolderCourse>(e);
            assert(top != nullptr);
            for (size_t i = 0; i < top->getContentsCount(); ++i)
            {
                auto pCourse = top->getEntry(i);
                prop.dbBrowseEntries.push_back({ pCourse, nullptr });
            }

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
                State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
            }
            else
            {
                State::set(IndexSlider::SELECT_LIST, 0.0);
            }

            resetJukeboxText();

            scrollAccumulator = 0.;
            scrollAccumulatorAddUnit = 0.;

            State::set(IndexTimer::LIST_MOVE, Time().norm());
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
        navigateTimestamp = t;
        postStopPreview();

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
            State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
        }
        else
        {
            State::set(IndexSlider::SELECT_LIST, 0.0);
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
        IndexSwitch p = static_cast<IndexSwitch>(int(IndexSwitch::SELECT_PANEL1) - 1 + i);
        if (State::get(p))
        {
            hasPanelOpened = true;
            State::set(p, false);
            State::set(static_cast<IndexTimer>(int(IndexTimer::PANEL1_START) - 1 + i), TIMER_NEVER);
            State::set(static_cast<IndexTimer>(int(IndexTimer::PANEL1_END) - 1 + i), t.norm());
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
            if (_skin->textEditType() == IndexText::EDIT_JUKEBOX_NAME)
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
        if (textEditType() == IndexText::EDIT_JUKEBOX_NAME)
        {
            stopTextEdit(true);
            std::string searchText = State::get(IndexText::EDIT_JUKEBOX_NAME);
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
        State::set(IndexText::EDIT_JUKEBOX_NAME, "SEARCH SONG");
    else
        State::set(IndexText::EDIT_JUKEBOX_NAME, gSelectContext.backtrace.top().name);
}

void SceneSelect::searchSong(const std::string& text)
{
    LOG_DEBUG << "Search: " << text;

    auto top = g_pSongDB->search(ROOT_FOLDER_HASH, text);
    if (top.empty())
    {
        State::set(IndexText::EDIT_JUKEBOX_NAME, "SEARCH FAILED");
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
        State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.idx / gSelectContext.entries.size());
    }
    else
    {
        State::set(IndexSlider::SELECT_LIST, 0.0);
    }

    resetJukeboxText();

    scrollAccumulator = 0.;
    scrollAccumulatorAddUnit = 0.;

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
}

void SceneSelect::updatePreview()
{
    const EntryList& e = gSelectContext.entries;
    if (e.empty()) return;

    const auto& entry = gSelectContext.entries[gSelectContext.idx].first;

    // only load previewfor charts
    if (entry->type() == eEntryType::SONG || entry->type() == eEntryType::RIVAL_SONG ||
        entry->type() == eEntryType::CHART || entry->type() == eEntryType::RIVAL_CHART)
    {
        std::unique_lock l(previewMutex, std::try_to_lock);
        if (!l.owns_lock()) return;

        switch (previewState)
        {
        case PREVIEW_NONE:
            previewState = PREVIEW_LOAD;
            std::thread([&]()
                {
                    std::unique_lock l(previewMutex);

                    previewStandalone = false;
                    previewStandaloneLength = 0;

                    // create Chart object
                    if (entry->type() == eEntryType::SONG || entry->type() == eEntryType::RIVAL_SONG)
                    {
                        auto pFile = std::reinterpret_pointer_cast<EntryFolderSong>(entry)->getCurrentChart();
                        previewChart = ChartFormatBase::createFromFile(pFile->absolutePath, gPlayContext.randomSeed);
                    }
                    else
                    {
                        auto pFile = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
                        previewChart = ChartFormatBase::createFromFile(pFile->absolutePath, gPlayContext.randomSeed);
                    }

                    // load chart object from Chart object
                    unsigned bars = 0;
                    switch (previewChart->type())
                    {
                    case eChartFormat::BMS:
                    {
                        auto bms = std::reinterpret_pointer_cast<ChartFormatBMS>(previewChart);

                        // check if #PREVIEW is valid
                        for (auto& [key, val] : bms->extraCommands)
                        {
                            if (strEqual(key, "PREVIEW", true) && !val.empty())
                            {
                                Path pWav = fs::u8path(val);
                                if (!pWav.is_absolute())
                                    pWav = bms->getDirectory() / pWav;
                                if (SoundMgr::loadNoteSample(pWav, 0) == 0)
                                    previewStandalone = true;

                                break;
                            }
                        }
                        // check if preview(*).ogg is valid
                        if (!previewStandalone)
                        {
                            for (auto& f : fs::directory_iterator(bms->getDirectory()))
                            {
                                if (strEqual(f.path().filename().u8string().substr(0, 7), "preview", true))
                                {
                                    Path pWav = f.path();
                                    if (SoundMgr::loadNoteSample(pWav, 0) == 0)
                                    {
                                        previewStandalone = true;
                                        break;
                                    }
                                }
                            }
                        }

                        if (previewStandalone)
                        {
                            previewStartTime = Time();
                            previewStandaloneLength = SoundMgr::getNoteSampleLength(0);
                            size_t idx = 0;
                            SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, 1, &idx);
                        }
                        else
                        {

                            bars = bms->lastBarIdx;
                            previewChartObj = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);
                            previewRuleset = std::make_shared<RulesetBMSAuto>(previewChart, previewChartObj,
                                eModGauge::NORMAL, bms->gamemode, RulesetBMS::JudgeDifficulty::VERYHARD, 0.2, RulesetBMS::PlaySide::RIVAL);

                            // load samples
                            gChartContext.isSampleLoaded = false;
                            SoundMgr::freeNoteSamples();
                            auto chartDir = bms->getDirectory();
                            int wavToLoad = 0;
                            for (const auto& it : bms->wavFiles)
                            {
                                if (sceneEnding || previewState != PREVIEW_LOAD) break;

                                if (it.empty()) continue;
                                ++wavToLoad;
                            }
                            if (wavToLoad != 0)
                            {
                                for (size_t i = 0; i < bms->wavFiles.size(); ++i)
                                {
                                    if (sceneEnding || previewState != PREVIEW_LOAD) break;

                                    const auto& wav = bms->wavFiles[i];
                                    if (wav.empty()) continue;
                                    Path pWav = fs::u8path(wav);
                                    if (pWav.is_absolute())
                                        SoundMgr::loadNoteSample(pWav, i);
                                    else
                                        SoundMgr::loadNoteSample((chartDir / pWav), i);
                                }
                                if (!(sceneEnding || previewState != PREVIEW_LOAD))
                                    gChartContext.isSampleLoaded = true;
                            }
                        }
                        break;
                    }
                    }

                    if (previewState == PREVIEW_LOAD)
                    {
                        if (!previewStandalone && !gChartContext.isSampleLoaded)
                        {
                            // quit
                            previewState = PREVIEW_FINISH;
                        }
                        else
                        {
                            if (!previewStandalone)
                            {
                                // start from beginning. It's difficult to seek a chart for playback due to lengthy BGM samples...
                                previewStartTime = Time() - previewChartObj->getLeadInTime();
                                previewRuleset->setStartTime(previewStartTime);
                            }

                            SoundMgr::setSysVolume(0.1, 200);

                            previewState = PREVIEW_PLAY;
                        }
                    }
                }).detach();
            break;

        case PREVIEW_PLAY:
        {
            if (previewStandalone)
            {
                if ((Time() - previewStartTime).norm() > previewStandaloneLength)
                {
                    previewState = PREVIEW_FINISH;
                    SoundMgr::setSysVolume(1.0, 400);
                }
            }
            else
            {
                auto t = Time();
                auto rt = t - previewStartTime;
                previewChartObj->update(rt);
                previewRuleset->update(t);

                // play BGM samples
                auto it = previewChartObj->noteBgmExpired.begin();
                size_t max = std::min(_bgmSampleIdxBuf.size(), previewChartObj->noteBgmExpired.size());
                size_t i = 0;
                for (; i < max && it != previewChartObj->noteBgmExpired.end(); ++i, ++it)
                {
                    // BGM
                    _bgmSampleIdxBuf[i] = (unsigned)it->dvalue;
                }
                SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_bgmSampleIdxBuf.data());

                // play KEY samples
                i = 0;
                auto it2 = previewChartObj->noteExpired.begin();
                size_t max2 = std::min(_keySampleIdxBuf.size(), max + previewChartObj->noteExpired.size());
                while (i < max2 && it2 != previewChartObj->noteExpired.end())
                {
                    if ((it2->flags & ~(Note::SCRATCH | Note::KEY_6_7)) == 0)
                    {
                        _keySampleIdxBuf[i] = (unsigned)it2->dvalue;
                        ++i;
                    }
                    ++it2;
                }
                SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_keySampleIdxBuf.data());

                if (previewRuleset->isFinished())
                {
                    previewState = PREVIEW_FINISH;
                    SoundMgr::setSysVolume(1.0, 400);
                }
            }
            break;
        }

        case PREVIEW_FINISH:
            break;

        }
    }
}

void SceneSelect::postStopPreview()
{
    SoundMgr::stopNoteSamples();
    SoundMgr::setSysVolume(1.0, 400);
    previewState = PREVIEW_NONE;
}