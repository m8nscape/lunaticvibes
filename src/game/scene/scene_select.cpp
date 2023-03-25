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

#include "game/runtime/i18n.h"

#include "game/arena/arena_data.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

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

    if (!gPlayContext.isReplay)
    {
        ConfigMgr::set('P', P_HISPEED, gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed);
        ConfigMgr::set('P', P_HISPEED_2P, gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed);
        ConfigMgr::set('P', cfg::P_GREENNUMBER, State::get(IndexNumber::GREEN_NUMBER_1P));
        ConfigMgr::set('P', cfg::P_GREENNUMBER_2P, State::get(IndexNumber::GREEN_NUMBER_2P));

        switch (State::get(IndexOption::PLAY_HSFIX_TYPE))
        {
        case Option::SPEED_FIX_MAX:      ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_MAX); break;
        case Option::SPEED_FIX_MIN:      ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_MIN); break;
        case Option::SPEED_FIX_AVG:      ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_AVG); break;
        case Option::SPEED_FIX_CONSTANT: ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_CONSTANT); break;
        case Option::SPEED_FIX_INITIAL:  ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_INITIAL); break;
        case Option::SPEED_FIX_MAIN:     ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_MAIN); break;
        default:                         ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_NORMAL); break;
        }

        switch (State::get(IndexOption::PLAY_RANDOM_TYPE_1P))
        {
        case Option::RAN_MIRROR: ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_MIRROR); break;
        case Option::RAN_RANDOM: ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_RANDOM); break;
        case Option::RAN_SRAN:   ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_SRAN); break;
        case Option::RAN_HRAN:   ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_HRAN); break;
        case Option::RAN_ALLSCR: ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_ALLSCR); break;
        case Option::RAN_RRAN:   ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_RRAN); break;
        default:                 ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_NORMAL); break;
        }
        switch (State::get(IndexOption::PLAY_RANDOM_TYPE_2P))
        {
        case Option::RAN_MIRROR: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_MIRROR); break;
        case Option::RAN_RANDOM: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_RANDOM); break;
        case Option::RAN_SRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_SRAN); break;
        case Option::RAN_HRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_HRAN); break;
        case Option::RAN_ALLSCR: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_ALLSCR); break;
        case Option::RAN_RRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_RRAN); break;
        default:                 ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_NORMAL); break;
        }


        switch (State::get(IndexOption::PLAY_GAUGE_TYPE_1P))
        {
        case Option::GAUGE_HARD:   ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_HARD); break;
        case Option::GAUGE_EASY:   ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_EASY); break;
        case Option::GAUGE_DEATH:  ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_DEATH); break;
        case Option::GAUGE_EXHARD: ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_EXHARD); break;
        case Option::GAUGE_ASSISTEASY: ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_ASSISTEASY); break;
        default:                   ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_NORMAL); break;
        }
        switch (State::get(IndexOption::PLAY_GAUGE_TYPE_2P))
        {
        case Option::GAUGE_HARD:   ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_HARD); break;
        case Option::GAUGE_EASY:   ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_EASY); break;
        case Option::GAUGE_DEATH:  ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_DEATH); break;
        case Option::GAUGE_EXHARD: ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_EXHARD); break;
        case Option::GAUGE_ASSISTEASY: ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_ASSISTEASY); break;
        default:                   ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_NORMAL); break;
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
        switch (State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_2P))
        {
        case Option::LANE_OFF:     ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_OFF); break;
        case Option::LANE_HIDDEN:  ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_HIDDEN); break;
        case Option::LANE_SUDDEN:  ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_SUDDEN); break;
        case Option::LANE_SUDHID:  ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_SUDHID); break;
        case Option::LANE_LIFT:    ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_LIFT); break;
        case Option::LANE_LIFTSUD: ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_LIFTSUD); break;
        default:                   ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_OFF); break;
        }

        switch (State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_1P))
        {
        case false: ConfigMgr::set('P', P_CHART_ASSIST_OP, P_CHART_ASSIST_OP_NONE); break;
        case true:  ConfigMgr::set('P', P_CHART_ASSIST_OP, P_CHART_ASSIST_OP_AUTOSCR); break;
        }
        switch (State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_2P))
        {
        case false: ConfigMgr::set('P', P_CHART_ASSIST_OP_2P, P_CHART_ASSIST_OP_NONE); break;
        case true:  ConfigMgr::set('P', P_CHART_ASSIST_OP_2P, P_CHART_ASSIST_OP_AUTOSCR); break;
        }

        ConfigMgr::set('P', P_FLIP, State::get(IndexSwitch::PLAY_OPTION_DP_FLIP));
    }

    switch (State::get(IndexOption::PLAY_GHOST_TYPE_1P))
    {
    case Option::GHOST_TOP:         ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_A); break;
    case Option::GHOST_SIDE:        ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_B); break;
    case Option::GHOST_SIDE_BOTTOM: ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_C); break;
    default:                        ConfigMgr::set('P',P_GHOST_TYPE, "OFF"); break;
    }
    switch (State::get(IndexOption::PLAY_GHOST_TYPE_2P))
    {
    case Option::GHOST_TOP:         ConfigMgr::set('P', P_GHOST_TYPE_2P, P_GHOST_TYPE_A); break;
    case Option::GHOST_SIDE:        ConfigMgr::set('P', P_GHOST_TYPE_2P, P_GHOST_TYPE_B); break;
    case Option::GHOST_SIDE_BOTTOM: ConfigMgr::set('P', P_GHOST_TYPE_2P, P_GHOST_TYPE_C); break;
    default:                        ConfigMgr::set('P', P_GHOST_TYPE_2P, "OFF"); break;
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

SceneSelect::SceneSelect() : SceneBase(SkinType::MUSIC_SELECT, 250)
{
    _type = SceneType::SELECT;

    _inputAvailable = INPUT_MASK_FUNC | INPUT_MASK_MOUSE;
    _inputAvailable |= INPUT_MASK_1P;
    _inputAvailable |= INPUT_MASK_2P;

    // reset globals
    ConfigMgr::setGlobals();

    gSelectContext.lastLaneEffectType1P = State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P);

    if (!gSelectContext.entries.empty())
    {
        // delay sorting chart list after playing
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
        updateEntryScore(gSelectContext.selectedEntryIndex);
        setEntryInfo();
    }
    else
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
    case Option::SORT_TITLE: gSelectContext.sortType = SongListSortType::TITLE; break;
    case Option::SORT_LEVEL: gSelectContext.sortType = SongListSortType::LEVEL; break;
    case Option::SORT_CLEAR: gSelectContext.sortType = SongListSortType::CLEAR; break;
    case Option::SORT_RATE:  gSelectContext.sortType = SongListSortType::RATE; break;
    default:                 gSelectContext.sortType = SongListSortType::DEFAULT; break;
    }

    gPlayContext.isAuto = false;
    gPlayContext.isReplay = false;
    gSelectContext.isGoingToKeyConfig = false;
    gSelectContext.isGoingToSkinSelect = false;
    gSelectContext.isGoingToAutoPlay = false;
    gSelectContext.isGoingToReplay = false;
    gSelectContext.isGoingToReboot = false;
    State::set(IndexSwitch::SYSTEM_AUTOPLAY, false);

    const auto [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);

    State::set(IndexText::_OVERLAY_TOPLEFT, "");
    State::set(IndexText::_OVERLAY_TOPLEFT2, "");

    State::set(IndexSwitch::SOUND_PITCH, true);
    lr2skin::slider::pitch(0.5);
    State::set(IndexSwitch::SOUND_PITCH, ConfigMgr::get('P', cfg::P_FREQ, false));
    lr2skin::slider::pitch((ConfigMgr::get('P', cfg::P_FREQ_VAL, 0) + 12) / 24.0);

    gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed = State::get(IndexNumber::HS_1P) / 100.0;
    gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed = State::get(IndexNumber::HS_2P) / 100.0;
    if (State::get(IndexOption::PLAY_HSFIX_TYPE) == Option::SPEED_NORMAL)
    {
        State::set(IndexSwitch::P1_LOCK_SPEED, false);
        State::set(IndexSwitch::P2_LOCK_SPEED, false);
    }
    else
    {
        State::set(IndexSwitch::P1_LOCK_SPEED, true);
        State::set(IndexSwitch::P2_LOCK_SPEED, true);
    }
    State::set(IndexNumber::GREEN_NUMBER_1P, ConfigMgr::get('P', cfg::P_GREENNUMBER, 300));
    State::set(IndexNumber::GREEN_NUMBER_2P, ConfigMgr::get('P', cfg::P_GREENNUMBER_2P, 300));

    lr2skin::button::target_type(0);

    if (!gInCustomize)
    {
        using namespace cfg;
        auto bindings = ConfigMgr::get('P', P_SELECT_KEYBINDINGS, P_SELECT_KEYBINDINGS_7K);
        if (bindings == P_SELECT_KEYBINDINGS_5K)
        {
            InputMgr::updateBindings(5);
        }
        else if (bindings == P_SELECT_KEYBINDINGS_9K)
        {
            InputMgr::updateBindings(9);
            bindings9K = true;
        }
        else
        {
            InputMgr::updateBindings(7);
        }
    }

    state = eSelectState::PREPARE;
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

    if (gArenaData.isOnline())
        State::set(IndexTimer::ARENA_SHOW_LOBBY, Time().norm());

    imguiInit();
}

SceneSelect::~SceneSelect()
{
    if (_virtualSceneCustomize != nullptr)
    {
        _virtualSceneCustomize->loopEnd();
        if (gNextScene == SceneType::CUSTOMIZE || gNextScene == SceneType::EXIT_TRANS || gNextScene == SceneType::EXIT)
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
    if (gNextScene != SceneType::SELECT) return;

    Time t;

    if (gAppIsExiting)
    {
        gNextScene = SceneType::EXIT_TRANS;
    }

    _updateCallback();

    if (gSelectContext.optionChangePending)
    {
        gSelectContext.optionChangePending = false;

        State::set(IndexTimer::LIST_MOVE, t.norm());
        navigateTimestamp = t;
        postStopPreview();
    }

    if (gSelectContext.cursorClick != gSelectContext.highlightBarIndex)
    {
        int idx = gSelectContext.selectedEntryIndex + gSelectContext.cursorClick - gSelectContext.highlightBarIndex;
        if (idx < 0)
            idx += gSelectContext.entries.size() * ((-idx) / gSelectContext.entries.size() + 1);
        gSelectContext.selectedEntryIndex = idx % gSelectContext.entries.size();
        gSelectContext.highlightBarIndex = gSelectContext.cursorClick;

        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
        setBarInfo();
        setEntryInfo();
        setDynamicTextures();

        State::set(IndexTimer::LIST_MOVE, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }

    if (gSelectContext.cursorEnterPending)
    {
        gSelectContext.cursorEnterPending = false;

        switch (gSelectContext.entries[gSelectContext.selectedEntryIndex].first->type())
        {
        case eEntryType::FOLDER:
        case eEntryType::CUSTOM_FOLDER:
        case eEntryType::COURSE_FOLDER:
        case eEntryType::NEW_SONG_FOLDER:
        case eEntryType::ARENA_FOLDER:
            navigateEnter(t);
            break;

        case eEntryType::SONG:
        case eEntryType::CHART:
        case eEntryType::RIVAL_SONG:
        case eEntryType::RIVAL_CHART:
        case eEntryType::COURSE:
            decide();
            break;

        case eEntryType::ARENA_COMMAND:
            arenaCommand();
            break;

        case eEntryType::ARENA_LOBBY:
            // TODO enter ARENA_LOBBY
            break;

        default:
            break;
        }
    }
    if (gSelectContext.cursorClickScroll != 0)
    {
        if (scrollAccumulator == 0.0)
        {
            if (gSelectContext.cursorClickScroll > 0)
            {
                scrollAccumulator -= gSelectContext.cursorClickScroll;
                scrollButtonTimestamp = t;
                scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (1000.0 / getRate());
            }
            else
            {
                scrollAccumulator += -gSelectContext.cursorClickScroll;
                scrollButtonTimestamp = t;
                scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (1000.0 / getRate());
            }
        }
        gSelectContext.cursorClickScroll = 0;
    }

    // update by slider
    if (gSelectContext.draggingListSlider)
    {
        gSelectContext.draggingListSlider = false;

        size_t idx_new = (size_t)std::floor(State::get(IndexSlider::SELECT_LIST) * gSelectContext.entries.size());
        if (idx_new == gSelectContext.entries.size())
            idx_new = 0;

        if (gSelectContext.selectedEntryIndex != idx_new)
        {
            navigateTimestamp = t;
            postStopPreview();

            std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
            gSelectContext.selectedEntryIndex = idx_new;
            setBarInfo();
            setEntryInfo();
            setDynamicTextures();

            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
        }
    }

    // update by arena
    if (imgui_arena_joinLobby)
    {
        imgui_arena_joinLobby = false;
        arenaJoinLobby();
    }
    if (!gSelectContext.remoteRequestedChart.empty())
    {
        std::string folderName = (boost::format(i18n::c(i18nText::ARENA_REQUEST_BY)) % gSelectContext.remoteRequestedPlayer).str();
        SongListProperties prop{
            gSelectContext.backtrace.front().folder,
            {},
            folderName,
            {},
            {},
            0,
            true
        };
        prop.dbBrowseEntries.push_back({ std::make_shared<EntryChart>(*g_pSongDB->findChartByHash(gSelectContext.remoteRequestedChart).begin()), nullptr });

        gSelectContext.backtrace.front().index = gSelectContext.selectedEntryIndex;
        gSelectContext.backtrace.front().displayEntries = gSelectContext.entries;
        gSelectContext.backtrace.push_front(prop);
        gSelectContext.entries.clear();
        loadSongList();
        sortSongList();

        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
        gSelectContext.selectedEntryIndex = 0;
        setBarInfo();
        setEntryInfo();
        setDynamicTextures();

        if (!gSelectContext.entries.empty())
        {
            State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.selectedEntryIndex / gSelectContext.entries.size());
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

        gSelectContext.remoteRequestedChart.reset();
        gSelectContext.remoteRequestedPlayer.clear();
        gSelectContext.isInArenaRequest = true;
    }
    if (gArenaData.isOnline() && gSelectContext.isArenaCancellingRequest)
    {
        gSelectContext.isArenaCancellingRequest = false;
        if (gSelectContext.isInArenaRequest)
        {
            navigateBack(t);
        }
    }

    if (gArenaData.isOnline() && gSelectContext.isArenaReady)
    {
        decide();
    }

    if (gArenaData.isOnline() && gArenaData.isExpired())
    {
        gArenaData.reset();
    }

    if (!gSelectContext.entries.empty())
    {
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
                    scrollAccumulatorAddUnit = scrollAccumulator / 100 * (1000.0 / getRate());
                    scrollModified = true;
                }
            }

            if (!scrollModified)
            {
                std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

                State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.selectedEntryIndex / gSelectContext.entries.size());
                scrollAccumulator = 0.0;
                scrollAccumulatorAddUnit = 0.0;
                gSelectContext.scrollDirection = 0;
                pSkin->reset_bar_animation();
                State::set(IndexTimer::LIST_MOVE_STOP, t.norm());
                gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
            }
        }
        else if(gSelectContext.scrollDirection != 0 || scrollAccumulatorAddUnit < -0.003 || scrollAccumulatorAddUnit > 0.003)
        {
            if (gSelectContext.scrollDirection == 0)
            {
                std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
                pSkin->start_bar_animation();
            }

            double posOld = State::get(IndexSlider::SELECT_LIST);
            double posNew = posOld + scrollAccumulatorAddUnit / gSelectContext.entries.size();

            int idxOld = (int)std::round(posOld * gSelectContext.entries.size());
            int idxNew = (int)std::round(posNew * gSelectContext.entries.size());
            if (idxOld != idxNew)
            {
                if (idxOld < idxNew)
                    navigateDownBy1(t);
                else
                    navigateUpBy1(t);
            }

            while (posNew < 0.) posNew += 1.;
            while (posNew >= 1.) posNew -= 1.;
            State::set(IndexSlider::SELECT_LIST, posNew);

            scrollAccumulator -= scrollAccumulatorAddUnit;
            if (scrollAccumulator < -0.000001 || scrollAccumulator > 0.000001)
                gSelectContext.scrollDirection = scrollAccumulator > 0. ? 1 : -1;
        }
    }

    if (!gInCustomize && gCustomizeContext.modeUpdate)
    {
        gCustomizeContext.modeUpdate = false;

        if (_virtualSceneCustomize == nullptr)
        {
            createNotification("Loading skin options...");

            pSkin->setHandleMouseEvents(false);
            _virtualSceneCustomize = std::make_shared<SceneCustomize>();
            _virtualSceneCustomize->loopStart();
            pSkin->setHandleMouseEvents(true);

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

    if (rt.norm() >= pSkin->info.timeIntro)
    {
        state = eSelectState::SELECT;
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
            if (!pSkin->isSupportGreenNumber)
            {
                std::stringstream ss;
                bool lock1 = State::get(IndexSwitch::P1_LOCK_SPEED);
                if (lock1) 
                    ss << "G(1P): FIX " << ConfigMgr::get('P', cfg::P_GREENNUMBER, 0);

                bool lock2 = State::get(IndexSwitch::P2_LOCK_SPEED);
                if (lock2) 
                    ss << (lock1 ? " | " : "") << "G(2P): FIX " << ConfigMgr::get('P', cfg::P_GREENNUMBER_2P, 0);

                std::string s = ss.str();
                if (!s.empty())
                {
                    State::set((IndexText)line++, ss.str());
                }
            }
            if (!pSkin->isSupportNewRandom && ConfigMgr::get('P', cfg::P_ENABLE_NEW_RANDOM, false))
            {
                std::stringstream ss;
                int lane1 = State::get(IndexOption::PLAY_RANDOM_TYPE_1P);
                switch (lane1)
                {
                case Option::RAN_NORMAL:                ss << "Random(1P): OFF"; break;
                case Option::RAN_MIRROR:                ss << "Random(1P): MIRROR"; break;
                case Option::RAN_RANDOM:                ss << "Random(1P): RANDOM"; break;
                case Option::RAN_SRAN:                  ss << "Random(1P): S-RANDOM"; break;
                case Option::RAN_HRAN:                  ss << "Random(1P): H-RANDOM"; break;
                case Option::RAN_ALLSCR:                ss << "Random(1P): ALL-SCRATCH"; break;
                case Option::RAN_RRAN:                  ss << "Random(1P): R-RANDOM"; break;
                case Option::RAN_DB_SYNCHRONIZE_RANDOM: ss << "Random: SYNCHRONIZE RANDOM"; break;
                case Option::RAN_DB_SYMMETRY_RANDOM:    ss << "Random: SYMMETRY RANDOM"; break;
                }

                if (State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_BATTLE || 
                    State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DOUBLE ||
                    State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DOUBLE_BATTLE)
                {
                    int lane2 = State::get(IndexOption::PLAY_RANDOM_TYPE_2P);
                    switch (lane2)
                    {
                    case Option::RAN_NORMAL:                ss << " | Random(2P): OFF"; break;
                    case Option::RAN_MIRROR:                ss << " | Random(2P): MIRROR"; break;
                    case Option::RAN_RANDOM:                ss << " | Random(2P): RANDOM"; break;
                    case Option::RAN_SRAN:                  ss << " | Random(2P): S-RANDOM"; break;
                    case Option::RAN_HRAN:                  ss << " | Random(2P): H-RANDOM"; break;
                    case Option::RAN_ALLSCR:                ss << " | Random(2P): ALL-SCRATCH"; break;
                    case Option::RAN_RRAN:                  ss << " | Random(2P): R-RANDOM"; break;
                    case Option::RAN_DB_SYNCHRONIZE_RANDOM:
                    case Option::RAN_DB_SYMMETRY_RANDOM:    break;
                    }
                }

                std::string s = ss.str();
                if (!s.empty())
                {
                    State::set((IndexText)line++, ss.str());
                }
            }
            if (!pSkin->isSupportExHardAndAssistEasy && ConfigMgr::get('P', cfg::P_ENABLE_NEW_GAUGE, false))
            {
                std::stringstream ss;
                int lane1 = State::get(IndexOption::PLAY_GAUGE_TYPE_1P);
                switch (lane1)
                {
                case Option::GAUGE_NORMAL:     ss << "Gauge(1P): NORMAL"; break;
                case Option::GAUGE_HARD:       ss << "Gauge(1P): HARD"; break;
                case Option::GAUGE_DEATH:      ss << "Gauge(1P): DEATH"; break;
                case Option::GAUGE_EASY:       ss << "Gauge(1P): EASY"; break;
                case Option::GAUGE_EXHARD:     ss << "Gauge(1P): EX-HARD"; break;
                case Option::GAUGE_ASSISTEASY: ss << "Gauge(1P): ASSIST EASY"; break;
                }

                if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_LOCAL)
                {
                    int lane2 = State::get(IndexOption::PLAY_GAUGE_TYPE_2P);
                    switch (lane2)
                    {
                    case Option::GAUGE_NORMAL:     ss << " | Gauge(2P): NORMAL"; break;
                    case Option::GAUGE_HARD:       ss << " | Gauge(2P): HARD"; break;
                    case Option::GAUGE_DEATH:      ss << " | Gauge(2P): DEATH"; break;
                    case Option::GAUGE_EASY:       ss << " | Gauge(2P): EASY"; break;
                    case Option::GAUGE_EXHARD:     ss << " | Gauge(2P): EX-HARD"; break;
                    case Option::GAUGE_ASSISTEASY: ss << " | Gauge(2P): ASSIST EASY"; break;
                    }
                }

                std::string s = ss.str();
                if (!s.empty())
                {
                    State::set((IndexText)line++, ss.str());
                }
            }
            if (!pSkin->isSupportLift && ConfigMgr::get('P', cfg::P_ENABLE_NEW_LANE_OPTION, false))
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
            if (!pSkin->isSupportHsFixInitialAndMain)
            {
                std::stringstream ss;
                int lane1 = State::get(IndexOption::PLAY_HSFIX_TYPE);
                switch (lane1)
                {
                case Option::SPEED_NORMAL:  break;
                case Option::SPEED_FIX_MIN: ss << "HiSpeed Fix: Min BPM"; break;
                case Option::SPEED_FIX_MAX: ss << "HiSpeed Fix: Max BPM"; break;
                case Option::SPEED_FIX_AVG: ss << "HiSpeed Fix: Average BPM"; break;
                case Option::SPEED_FIX_CONSTANT: ss << "HiSpeed Fix: *CONSTANT*"; break;
                case Option::SPEED_FIX_INITIAL: ss << "HiSpeed Fix: Start BPM"; break;
                case Option::SPEED_FIX_MAIN: ss << "HiSpeed Fix: Main BPM"; break;
                }

                std::string s = ss.str();
                if (!s.empty())
                {
                    State::set((IndexText)line++, ss.str());
                }
            }
        }
        while (line <= (int)IndexText::_OVERLAY_TOPLEFT4)
            State::set((IndexText)line++, "");
    }

    if (gSelectContext.isGoingToKeyConfig || gSelectContext.isGoingToSkinSelect || gSelectContext.isGoingToReboot)
    {
        if (!gInCustomize) SoundMgr::setSysVolume(0.0, 500);
        State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
        state = eSelectState::FADEOUT;
        _updateCallback = std::bind(&SceneSelect::updateFadeout, this);
    }
    else if (gSelectContext.isGoingToAutoPlay)
    {
        gSelectContext.isGoingToAutoPlay = false;
        if (!gSelectContext.entries.empty())
        {
            switch (gSelectContext.entries[gSelectContext.selectedEntryIndex].first->type())
            {
            case eEntryType::SONG:
            case eEntryType::CHART:
            case eEntryType::RIVAL_SONG:
            case eEntryType::RIVAL_CHART:
            case eEntryType::COURSE:
                gPlayContext.isAuto = true;
                State::set(IndexSwitch::SYSTEM_AUTOPLAY, true);
                decide();
                break;
            }
        }
    }
    else if (gSelectContext.isGoingToReplay)
    {
        gSelectContext.isGoingToReplay = false;
        if (!gSelectContext.entries.empty())
        {
            switch (gSelectContext.entries[gSelectContext.selectedEntryIndex].first->type())
            {
            case eEntryType::SONG:
            case eEntryType::CHART:
            case eEntryType::RIVAL_SONG:
            case eEntryType::RIVAL_CHART:
            case eEntryType::COURSE:
            {
                if (State::get(IndexSwitch::CHART_HAVE_REPLAY))
                {
                    gPlayContext.isReplay = true;
                    decide();
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

    if (ft >= pSkin->info.timeOutro)
    {
        if (gSelectContext.isGoingToKeyConfig)
        {
            SoundMgr::stopSysSamples();
            gNextScene = SceneType::KEYCONFIG;
        }
        else if (gSelectContext.isGoingToSkinSelect)
        {
            SoundMgr::stopSysSamples();
            gNextScene = SceneType::CUSTOMIZE;
            gInCustomize = true;
        }
        else if (gSelectContext.isGoingToReboot)
        {
            SoundMgr::stopSysSamples();
            gNextScene = SceneType::PRE_SELECT;
        }
        else
        {
            SoundMgr::stopSysSamples();
            gNextScene = SceneType::EXIT_TRANS;
        }
    }
}

void SceneSelect::update()
{
    SceneBase::update();

    if (_virtualSceneLoadSongs)
        _virtualSceneLoadSongs->update();
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneSelect::inputGamePress(InputMask& m, const Time& t)
{
    Time rt = t - State::get(IndexTimer::SCENE_START);

    if (rt.norm() < pSkin->info.timeIntro) return;

    if (refreshingSongList) return;

    using namespace Input;

    if (isInTextEdit())
    {
        inputGamePressTextEdit(m, t);
        return;
    }

    if (imgui_show_arenaJoinLobbyPrompt)
    {
        if (m[Input::Pad::ESC])
        {
            imgui_show_arenaJoinLobbyPrompt = false;
            pSkin->setHandleMouseEvents(true);
        }
        return;
    }
    else if (imguiShow)
    {
        if (m[Input::Pad::F9] || m[Input::Pad::ESC])
        {
            imguiShow = false;
            imgui_add_profile_popup = false;
            pSkin->setHandleMouseEvents(true);
        }
        return;
    }
    else
    {
        if (m[Input::Pad::F9] || m[Input::Pad::ESC])
        {
            imguiShow = !imguiShow;
            pSkin->setHandleMouseEvents(!imguiShow);
        }
    }

    //if (m[Input::Pad::ESC])
    //{
    //    // close panels if opened. exit if no panel is opened
    //    bool hasPanelOpened = closeAllPanels(t);
    //    if (!hasPanelOpened)
    //    {
    //        LOG_DEBUG << "[Select] ESC";
    //        gNextScene = SceneType::EXIT_TRANS;
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
            switch (state)
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
            closeAllPanels(t);
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

    if (rt.norm() < pSkin->info.timeIntro) return;

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
            switch (state)
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

    if (rt.norm() < pSkin->info.timeIntro) return;

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

            switch (state)
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
            const auto [hasPath, path] = g_pSongDB->getFolderPath(gSelectContext.backtrace.front().folder);
            if (hasPath)
            {
                LOG_INFO << "[List] Refreshing folder " << path.u8string();
                State::set(IndexText::_OVERLAY_TOPLEFT, (boost::format(i18n::c(i18nText::REFRESH_FOLDER)) % path.u8string()).str());

                g_pSongDB->resetAddSummary();
                int count = g_pSongDB->addSubFolder(path, gSelectContext.backtrace.front().parent);
                g_pSongDB->waitLoadingFinish();

                LOG_INFO << "[List] Building chart hash cache...";
                g_pSongDB->prepareCache();
                LOG_INFO << "[List] Building chart hash cache finished.";

                int added = g_pSongDB->addChartSuccess - g_pSongDB->addChartModified;
                int updated = g_pSongDB->addChartModified;
                int deleted = g_pSongDB->addChartDeleted;
                if (added || updated || deleted)
                {
                    createNotification((boost::format(i18n::c(i18nText::REFRESH_FOLDER_DETAIL)) % path.u8string() % added % updated % deleted).str());
                }
                State::set(IndexText::_OVERLAY_TOPLEFT, "");
                State::set(IndexText::_OVERLAY_TOPLEFT2, "");
            }

            // re-browse
            if (!isInVersionList)
                selectDownTimestamp = -1;

            // simplified navigateBack(t)
            {
                std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
                auto& top = gSelectContext.backtrace.front();

                gSelectContext.selectedEntryIndex = 0;
                gSelectContext.backtrace.pop_front();
                auto& parent = gSelectContext.backtrace.front();
                gSelectContext.entries = parent.displayEntries;
                gSelectContext.selectedEntryIndex = parent.index;

                if (parent.ignoreFilters)
                {
                    // change display only
                    State::set(IndexOption::SELECT_FILTER_DIFF, Option::DIFF_ANY);
                    State::set(IndexOption::SELECT_FILTER_KEYS, Option::FILTER_KEYS_ALL);
                }
                else
                {
                    // restore prev
                    State::set(IndexOption::SELECT_FILTER_DIFF, gSelectContext.filterDifficulty);
                    int keys = 0;
                    switch (gSelectContext.filterKeys)
                    {
                     case 1: keys = Option::FILTER_KEYS_SINGLE; break;
                     case 7: keys = Option::FILTER_KEYS_7; break;
                     case 5: keys = Option::FILTER_KEYS_5; break;
                     case 2: keys = Option::FILTER_KEYS_DOUBLE; break;
                     case 14: keys = Option::FILTER_KEYS_14; break;
                     case 10: keys = Option::FILTER_KEYS_10; break;
                     case 9: keys = Option::FILTER_KEYS_9; break;
                    }
                    State::set(IndexOption::SELECT_FILTER_KEYS, keys);
                }
            }

            // reset infos, play sound
            navigateEnter(Time());
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
                loadSongList();
                sortSongList();

                gSelectContext.selectedEntryIndex = 0;
                setBarInfo();
                setEntryInfo();
                setDynamicTextures();

                resetJukeboxText();
            }

            State::set(IndexTimer::LIST_MOVE, Time().norm());
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_F_OPEN);
        }

        refreshingSongList = false;
        return;
    }

    if (pSkin->version() == SkinVersion::LR2beta3)
    {
        if (input[Input::Pad::K1START] || input[Input::Pad::K2START])
        {
            // close other panels
            closeAllPanels(t);

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
                navigateBack(t);
                return;
            }
        }
        if (selectDownTimestamp == -1 && (input[Input::Pad::K1SELECT] || input[Input::Pad::K2SELECT]) && !gSelectContext.entries.empty())
        {
            switch (gSelectContext.entries[gSelectContext.selectedEntryIndex].first->type())
            {
            case eEntryType::CHART:
            case eEntryType::RIVAL_CHART:
                selectDownTimestamp = t;
                break;

            default:
                lr2skin::button::select_difficulty_filter(1);
                break;
            }
        }
    }

    // navigate
    if (!gSelectContext.entries.empty())
    {
        if (bindings9K && (input & INPUT_MASK_DECIDE_9K).any() || !bindings9K && (input & INPUT_MASK_DECIDE).any())
        {
            switch (gSelectContext.entries[gSelectContext.selectedEntryIndex].first->type())
            {
            case eEntryType::FOLDER:
            case eEntryType::CUSTOM_FOLDER:
            case eEntryType::COURSE_FOLDER:
            case eEntryType::NEW_SONG_FOLDER:
            case eEntryType::ARENA_FOLDER:
                return navigateEnter(t);

            case eEntryType::SONG:
            case eEntryType::CHART:
            case eEntryType::RIVAL_SONG:
            case eEntryType::RIVAL_CHART:
            case eEntryType::COURSE:
                return decide();

            case eEntryType::ARENA_COMMAND:
                return arenaCommand();

            default:
                break;
            }
        }
        if (bindings9K && (input & INPUT_MASK_CANCEL_9K).any() || !bindings9K && (input & INPUT_MASK_CANCEL).any())
            return navigateBack(t);

        if (bindings9K && (input & INPUT_MASK_NAV_UP_9K).any() || !bindings9K && (input & INPUT_MASK_NAV_UP).any())
        {
            isHoldingUp = true;
            scrollAccumulator -= 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (1000.0 / getRate());
        }
        if (bindings9K && (input & INPUT_MASK_NAV_DN_9K).any() || !bindings9K && (input & INPUT_MASK_NAV_DN).any())
        {
            isHoldingDown = true;
            scrollAccumulator += 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (1000.0 / getRate());
        }

        if (input[Input::MWHEELUP])
        {
            if (scrollAccumulator != 0.0)
            {
                gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
            }
            scrollAccumulator -= 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (1000.0 / getRate());
        }
        if (input[Input::MWHEELDOWN])
        {
            if (scrollAccumulator != 0.0)
            {
                gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
            }
            scrollAccumulator += 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (1000.0 / getRate());
        }
    }
    else
    {
        if ((input & INPUT_MASK_CANCEL).any())
            return navigateBack(t);
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
        scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (1000.0 / getRate());
    }
    if (isHoldingDown && (t - scrollButtonTimestamp).norm() >= gSelectContext.scrollTimeLength)
    {
        gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
        scrollButtonTimestamp = t;
        scrollAccumulator += 1.0;
        scrollAccumulatorAddUnit = scrollAccumulator / gSelectContext.scrollTimeLength * (1000.0 / getRate());
    }
    if (selectDownTimestamp != -1 && (t - selectDownTimestamp).norm() >= 233 && !isInVersionList && (input[Input::Pad::K1SELECT] || input[Input::Pad::K2SELECT]))
    {
        navigateVersionEnter(t);
    }
}

void SceneSelect::inputGameReleaseSelect(InputMask& input, const Time& t)
{
    if (pSkin->version() == SkinVersion::LR2beta3)
    {
        if (selectDownTimestamp != -1 && (input[Input::Pad::K1SELECT] || input[Input::Pad::K2SELECT]))
        {
            if (isInVersionList)
            {
                navigateVersionBack(t);
            }
            else
            {
                // short press on song, inc version by 1
                std::unique_lock l(gSelectContext._mutex);
                switchVersion(0);
                setBarInfo();
                setEntryInfo();
                setDynamicTextures();

                gSelectContext.optionChangePending = true;
                SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
            }
            selectDownTimestamp = -1;
            return;
        }
    }

    // navigate
    if (bindings9K && (input & INPUT_MASK_NAV_UP_9K).any() || (input & INPUT_MASK_NAV_UP).any())
    {
        isHoldingUp = false;
    }
    if (bindings9K && (input & INPUT_MASK_NAV_DN_9K).any() || (input & INPUT_MASK_NAV_DN).any())
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

    if (pSkin->version() == SkinVersion::LR2beta3)
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
                if (input[Pad::K16]) lr2skin::button::autoscr(PLAYER_SLOT_PLAYER, 1);
                if (State::get(IndexOption::PLAY_HSFIX_TYPE) == Option::SPEED_NORMAL)
                {
                    if (input[Pad::K15]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, -1);
                    if (input[Pad::K17]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, 1);
                }
                else
                {
                    if (input[Pad::K15]) lr2skin::button::lock_speed_value(PLAYER_SLOT_PLAYER, -1);
                    if (input[Pad::K17]) lr2skin::button::lock_speed_value(PLAYER_SLOT_PLAYER, 1);
                }
            }

            if (State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_BATTLE)
            {
                // Battle

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
                    if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_TARGET, 1);
                    if (State::get(IndexOption::PLAY_HSFIX_TYPE) == Option::SPEED_NORMAL)
                    {
                        if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_TARGET, -1);
                        if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_TARGET, 1);
                    }
                    else
                    {
                        if (input[Pad::K25]) lr2skin::button::lock_speed_value(PLAYER_SLOT_TARGET, -1);
                        if (input[Pad::K27]) lr2skin::button::lock_speed_value(PLAYER_SLOT_TARGET, 1);
                    }
                }
            }
            else if (State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DOUBLE ||
                State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DOUBLE_BATTLE ||
                State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DP_GHOST_BATTLE)
            {
                // DP

                // 1: KEYS
                if (input[Pad::K21]) lr2skin::button::select_keys_filter(1);
                if (input[Pad::K22]) lr2skin::button::random_type(PLAYER_SLOT_TARGET, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(PLAYER_SLOT_PLAYER, 1);

                if (isHoldingK26 && input[Pad::K27] || isHoldingK27 && input[Pad::K26])
                {
                    // 6+7: LANE EFFECT
                    lr2skin::button::lane_effect(PLAYER_SLOT_PLAYER, 1);
                }
                else if (isHoldingK25 && input[Pad::K27] || isHoldingK27 && input[Pad::K25])
                {
                    // 5+7: HS FIX
                    lr2skin::button::hs_fix(1);
                }
                else
                {
                    if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_TARGET, 1);
                    if (State::get(IndexOption::PLAY_HSFIX_TYPE) == Option::SPEED_NORMAL)
                    {
                        if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, -1);
                        if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, 1);
                    }
                    else
                    {
                        if (input[Pad::K25]) lr2skin::button::lock_speed_value(PLAYER_SLOT_PLAYER, -1);
                        if (input[Pad::K27]) lr2skin::button::lock_speed_value(PLAYER_SLOT_PLAYER, 1);
                    }
                }
            }
            else
            {
                // SP, using 2P input

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
                    if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_PLAYER, 1);
                    if (State::get(IndexOption::PLAY_HSFIX_TYPE) == Option::SPEED_NORMAL)
                    {
                        if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, -1);
                        if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_PLAYER, 1);
                    }
                    else
                    {
                        if (input[Pad::K25]) lr2skin::button::lock_speed_value(PLAYER_SLOT_PLAYER, -1);
                        if (input[Pad::K27]) lr2skin::button::lock_speed_value(PLAYER_SLOT_PLAYER, 1);
                    }
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


void SceneSelect::decide()
{
    std::shared_lock<std::shared_mutex> u(gSelectContext._mutex);

    if (gArenaData.isOnline())
    {
        if (!gSelectContext.isArenaReady)
        {
            if (State::get(IndexOption::SELECT_ENTRY_TYPE) == Option::ENTRY_SONG)
            {
                auto& [entry, score] = gSelectContext.entries[gSelectContext.selectedEntryIndex];
                if (gArenaData.isClient())
                    g_pArenaClient->requestChart(entry->md5);
                else
                    g_pArenaHost->requestChart(entry->md5, "host");
            }
            else
            {
                createNotification(i18n::s(i18nText::ARENA_REQUEST_FAILED_CHART_ONLY));
            }
            return;
        }
        else
        {
            // do nothing. The list is guaranteed to contain only the correct chart
        }
    }

    if (State::get(IndexOption::SELECT_ENTRY_TYPE) == Option::ENTRY_COURSE && State::get(IndexSwitch::COURSE_NOT_PLAYABLE))
        return;

    int bga = State::get(IndexOption::PLAY_BGA_TYPE);
    State::set(IndexSwitch::_LOAD_BGA,
        bga == Option::BGA_ON ||
        bga == Option::BGA_AUTOPLAY && (gPlayContext.isAuto || gPlayContext.isReplay));

    auto& [entry, score] = gSelectContext.entries[gSelectContext.selectedEntryIndex];
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

        if (gPlayContext.courseStage + 1 == gPlayContext.courseCharts.size())
            State::set(IndexOption::PLAY_COURSE_STAGE, Option::STAGE_FINAL);
        else
        {
            State::set(IndexOption::PLAY_COURSE_STAGE, Option::STAGE_1 + gPlayContext.courseStage);
        }
    }
    else
    {
        State::set(IndexOption::PLAY_COURSE_STAGE, Option::STAGE_NOT_COURSE);
    }

    if (gArenaData.isOnline())
    {
        gPlayContext.canRetry = false;
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
            gChartContext.chart = pFile;
        }
        else
        {
            auto pFile = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
            gChartContext.chart = pFile;
        }

        auto& chart = *gChartContext.chart;
        //gChartContext.path = chart._filePath;
        gChartContext.path = chart.absolutePath;

        // only reload resources if selected chart is different
        if (gChartContext.hash != gChartContext.sampleLoadedHash)
        {
            gChartContext.isBgaLoaded = false;
            gChartContext.isSampleLoaded = false;
            gChartContext.sampleLoadedHash.reset();
        }
        gChartContext.hash = chart.fileHash;

        //gChartContext.chart = std::make_shared<ChartFormatBase>(chart);
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
        if (gChartContext.chart->type() == eChartFormat::BMS)
        {
            auto pBMS = std::reinterpret_pointer_cast<ChartFormatBMSMeta>(gChartContext.chart);
            switch (pBMS->gamemode)
            {
            case 5:  gPlayContext.mode = SkinType::PLAY5;  break;
            case 7:  gPlayContext.mode = SkinType::PLAY7;  break;
            case 9:  gPlayContext.mode = SkinType::PLAY9;  break;
            case 10: gPlayContext.mode = SkinType::PLAY10; break;
            case 14: gPlayContext.mode = SkinType::PLAY14; break;
            default: gPlayContext.mode = SkinType::PLAY7;  break;
            }
            if (gPlayContext.isBattle)
            {
                if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_LOCAL || 
                    State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_GHOST)
                {
                    switch (pBMS->gamemode)
                    {
                    case 5:  gPlayContext.mode = SkinType::PLAY5_2;  break;
                    case 7:  gPlayContext.mode = SkinType::PLAY7_2;  break;
                    case 9:  gPlayContext.mode = SkinType::PLAY9;  gPlayContext.isBattle = false; break;
                    case 10: gPlayContext.mode = SkinType::PLAY10; gPlayContext.isBattle = false; break;
                    case 14: gPlayContext.mode = SkinType::PLAY14; gPlayContext.isBattle = false; break;
                    default: assert(false); break;
                    }
                }
            }
            else if (State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_DB)
            {
                switch (pBMS->gamemode)
                {
                case 5:  gPlayContext.mode = SkinType::PLAY10;  break;
                case 7:  gPlayContext.mode = SkinType::PLAY14;  break;
                default: assert(false); break;
                }
                gChartContext.isDoubleBattle = true;
            }
        }

        switch (gPlayContext.mode)
        {
        case SkinType::PLAY5:
        case SkinType::PLAY7:
        case SkinType::PLAY9:
            assert(!gPlayContext.isBattle);
            assert(!gChartContext.isDoubleBattle);
            break;

        case SkinType::PLAY5_2:
        case SkinType::PLAY7_2:
        case SkinType::PLAY9_2:
            assert(gPlayContext.isBattle);
            assert(!gChartContext.isDoubleBattle);
            break;

        case SkinType::PLAY10:
        case SkinType::PLAY14:
            assert(!gPlayContext.isBattle);
            break;
        }

        break;
    }
    case eEntryType::COURSE:
    {
        // reset mods
        static const std::set<PlayModifierGaugeType> courseGaugeModsAllowed = { PlayModifierGaugeType::NORMAL , PlayModifierGaugeType::HARD };
        if (courseGaugeModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge) == courseGaugeModsAllowed.end())
        {
            State::set(IndexOption::PLAY_GAUGE_TYPE_1P, 0);
            gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge = PlayModifierGaugeType::NORMAL;
        }
        if (courseGaugeModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_TARGET].gauge) == courseGaugeModsAllowed.end())
        {
            State::set(IndexOption::PLAY_GAUGE_TYPE_2P, 0);
            gPlayContext.mods[PLAYER_SLOT_TARGET].gauge = PlayModifierGaugeType::NORMAL;
        }
        static const std::set<PlayModifierRandomType> courseChartModsAllowed = { PlayModifierRandomType::NONE , PlayModifierRandomType::MIRROR };
        if (courseChartModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft) == courseChartModsAllowed.end())
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_1P, 0);
            gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft = PlayModifierRandomType::NONE;
        }
        if (courseChartModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_TARGET].randomLeft) == courseChartModsAllowed.end())
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_2P, 0);
            gPlayContext.mods[PLAYER_SLOT_TARGET].randomLeft = PlayModifierRandomType::NONE;
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
        gPlayContext.courseStageReplayPathNew.clear();

        auto pChart = *g_pSongDB->findChartByHash(*pCourse->charts.begin()).begin();
        gChartContext.chart = pChart;

        auto& chart = *gChartContext.chart;
        //gChartContext.path = chart._filePath;
        gChartContext.path = chart.absolutePath;

        // only reload resources if selected chart is different
        if (gChartContext.hash != gChartContext.sampleLoadedHash)
        {
            gChartContext.isBgaLoaded = false;
            gChartContext.isSampleLoaded = false;
            gChartContext.sampleLoadedHash.reset();
        }
        gChartContext.hash = chart.fileHash;

        //gChartContext.chart = std::make_shared<ChartFormatBase>(chart);
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
        auto convertGaugeType = [](int nType) -> PlayModifierGaugeType
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
                    case 6: return PlayModifierGaugeType::GRADE_HARD;
                    case 2: return PlayModifierGaugeType::GRADE_DEATH;
                    case 0:
                    default: return PlayModifierGaugeType::GRADE_NORMAL;
                    };
                }
            }

            switch (nType)
            {
            case 1: return PlayModifierGaugeType::HARD;
            case 2: return PlayModifierGaugeType::DEATH;
            case 3: return PlayModifierGaugeType::EASY;
                // case 4: return PlayModifierGaugeType::PATTACK;
                // case 5: return PlayModifierGaugeType::GATTACK;
            case 4: return PlayModifierGaugeType::HARD;
            case 5: return PlayModifierGaugeType::HARD;
            case 6: return PlayModifierGaugeType::EXHARD;
            case 7: return PlayModifierGaugeType::ASSISTEASY;
            case 0:
            default: return PlayModifierGaugeType::NORMAL;
            };
        };
        gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge = convertGaugeType(State::get(IndexOption::PLAY_GAUGE_TYPE_1P));

        // random
        auto convertRandomType = [](int nType) -> PlayModifierRandomType
        {
            switch (nType)
            {
            case 1: return PlayModifierRandomType::MIRROR;
            case 2: return PlayModifierRandomType::RANDOM;
            case 3: return PlayModifierRandomType::SRAN;
            case 4: return PlayModifierRandomType::HRAN;
            case 5: return PlayModifierRandomType::ALLSCR;
            case 6: return PlayModifierRandomType::RRAN;
            case 7: return PlayModifierRandomType::DB_SYNCHRONIZE;
            case 8: return PlayModifierRandomType::DB_SYMMETRY;
            case 0:
            default: return PlayModifierRandomType::NONE;
            };
        };
        gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft = convertRandomType(State::get(IndexOption::PLAY_RANDOM_TYPE_1P));

        if (gPlayContext.mode == SkinType::PLAY10 || gPlayContext.mode == SkinType::PLAY14)
        {
            gPlayContext.mods[PLAYER_SLOT_PLAYER].randomRight = convertRandomType(State::get(IndexOption::PLAY_RANDOM_TYPE_2P));
            gPlayContext.mods[PLAYER_SLOT_TARGET].randomRight = convertRandomType(State::get(IndexOption::PLAY_RANDOM_TYPE_2P));
            gPlayContext.mods[PLAYER_SLOT_PLAYER].DPFlip = State::get(IndexSwitch::PLAY_OPTION_DP_FLIP);
            gPlayContext.mods[PLAYER_SLOT_TARGET].DPFlip = State::get(IndexSwitch::PLAY_OPTION_DP_FLIP);
        }

        // assist
        gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask |= State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_1P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;

        // lane
        gPlayContext.mods[PLAYER_SLOT_PLAYER].laneEffect = (PlayModifierLaneEffectType)State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P);
        gPlayContext.mods[PLAYER_SLOT_TARGET].laneEffect = (PlayModifierLaneEffectType)State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_2P);

        // HS fix
        auto convertHSType = [](int nType) -> PlayModifierHispeedFixType
        {
            switch (nType)
            {
            case 1: return PlayModifierHispeedFixType::MAXBPM;
            case 2: return PlayModifierHispeedFixType::MINBPM;
            case 3: return PlayModifierHispeedFixType::AVERAGE;
            case 4: return PlayModifierHispeedFixType::CONSTANT;
            case 5: return PlayModifierHispeedFixType::INITIAL;
            case 6: return PlayModifierHispeedFixType::MAIN;
            case 0:
            default: return PlayModifierHispeedFixType::NONE;
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
                gPlayContext.mods[PLAYER_SLOT_TARGET].laneEffect = (PlayModifierLaneEffectType)gPlayContext.replay->laneEffectType;

                switch (gPlayContext.replay->randomTypeRight)
                {
                case PlayModifierRandomType::MIRROR:         State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_MIRROR); break;
                case PlayModifierRandomType::RANDOM:         State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_RANDOM); break;
                case PlayModifierRandomType::SRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_SRAN); break;
                case PlayModifierRandomType::HRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_HRAN); break;
                case PlayModifierRandomType::ALLSCR:         State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_ALLSCR); break;
                case PlayModifierRandomType::RRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_RRAN); break;
                case PlayModifierRandomType::DB_SYNCHRONIZE: State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_DB_SYNCHRONIZE_RANDOM); break;
                case PlayModifierRandomType::DB_SYMMETRY:    State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_DB_SYMMETRY_RANDOM); break;
                default:                         State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_NORMAL); break;
                }

                switch (gPlayContext.replay->gaugeType)
                {
                case PlayModifierGaugeType::HARD:
                case PlayModifierGaugeType::GRADE_NORMAL: State::set(IndexOption::PLAY_GAUGE_TYPE_2P, Option::GAUGE_HARD); break;
                case PlayModifierGaugeType::EASY:         State::set(IndexOption::PLAY_GAUGE_TYPE_2P, Option::GAUGE_EASY); break;
                case PlayModifierGaugeType::ASSISTEASY:   State::set(IndexOption::PLAY_GAUGE_TYPE_2P, Option::GAUGE_ASSISTEASY); break;
                case PlayModifierGaugeType::EXHARD:
                case PlayModifierGaugeType::GRADE_HARD:   State::set(IndexOption::PLAY_GAUGE_TYPE_2P, Option::GAUGE_EXHARD); break;
                default:                      State::set(IndexOption::PLAY_GAUGE_TYPE_2P, Option::GAUGE_NORMAL); break;
                }

                switch ((PlayModifierLaneEffectType)gPlayContext.replay->laneEffectType)
                {
                case PlayModifierLaneEffectType::SUDDEN:  State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, Option::LANE_SUDDEN); break;
                case PlayModifierLaneEffectType::HIDDEN:  State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, Option::LANE_HIDDEN); break;
                case PlayModifierLaneEffectType::SUDHID:  State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, Option::LANE_SUDHID); break;
                case PlayModifierLaneEffectType::LIFT:    State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, Option::LANE_LIFT); break;
                case PlayModifierLaneEffectType::LIFTSUD: State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, Option::LANE_LIFTSUD); break;
                default:                      State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, Option::LANE_OFF); break;
                }

                State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_2P, !!(gPlayContext.replay->assistMask & PLAY_MOD_ASSIST_AUTOSCR));
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

            State::set(IndexOption::PLAY_GAUGE_TYPE_2P, State::get(IndexOption::PLAY_GAUGE_TYPE_1P));
            State::set(IndexOption::PLAY_RANDOM_TYPE_2P, State::get(IndexOption::PLAY_RANDOM_TYPE_1P));
            State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_2P, false);
        }
    }
    else // gPlayContext.isReplay
    {
        gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft = gPlayContext.replay->randomTypeLeft;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].randomRight = gPlayContext.replay->randomTypeRight;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge = gPlayContext.replay->gaugeType;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask = gPlayContext.replay->assistMask;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix = gPlayContext.replay->hispeedFix;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].laneEffect = (PlayModifierLaneEffectType)gPlayContext.replay->laneEffectType;
        gPlayContext.mods[PLAYER_SLOT_PLAYER].DPFlip = gPlayContext.replay->DPFlip;

        switch (gPlayContext.replay->randomTypeLeft)
        {
        case PlayModifierRandomType::MIRROR:         State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_MIRROR); break;
        case PlayModifierRandomType::RANDOM:         State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_RANDOM); break;
        case PlayModifierRandomType::SRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_SRAN); break;
        case PlayModifierRandomType::HRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_HRAN); break;
        case PlayModifierRandomType::ALLSCR:         State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_ALLSCR); break;
        case PlayModifierRandomType::RRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_RRAN); break;
        case PlayModifierRandomType::DB_SYNCHRONIZE: State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_DB_SYNCHRONIZE_RANDOM); break;
        case PlayModifierRandomType::DB_SYMMETRY:    State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_DB_SYMMETRY_RANDOM); break;
        default:                         State::set(IndexOption::PLAY_RANDOM_TYPE_1P, Option::RAN_NORMAL); break;
        }

        switch (gPlayContext.replay->randomTypeRight)
        {
        case PlayModifierRandomType::MIRROR:         State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_MIRROR); break;
        case PlayModifierRandomType::RANDOM:         State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_RANDOM); break;
        case PlayModifierRandomType::SRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_SRAN); break;
        case PlayModifierRandomType::HRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_HRAN); break;
        case PlayModifierRandomType::ALLSCR:         State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_ALLSCR); break;
        case PlayModifierRandomType::RRAN:           State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_RRAN); break;
        case PlayModifierRandomType::DB_SYNCHRONIZE: State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_DB_SYNCHRONIZE_RANDOM); break;
        case PlayModifierRandomType::DB_SYMMETRY:    State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_DB_SYMMETRY_RANDOM); break;
        default:                         State::set(IndexOption::PLAY_RANDOM_TYPE_2P, Option::RAN_NORMAL); break;
        }

        switch (gPlayContext.replay->gaugeType)
        {
        case PlayModifierGaugeType::HARD:
        case PlayModifierGaugeType::GRADE_NORMAL: State::set(IndexOption::PLAY_GAUGE_TYPE_1P, Option::GAUGE_HARD); break;
        case PlayModifierGaugeType::EASY:         State::set(IndexOption::PLAY_GAUGE_TYPE_1P, Option::GAUGE_EASY); break;
        case PlayModifierGaugeType::ASSISTEASY:   State::set(IndexOption::PLAY_GAUGE_TYPE_1P, Option::GAUGE_ASSISTEASY); break;
        case PlayModifierGaugeType::EXHARD:
        case PlayModifierGaugeType::GRADE_HARD:   State::set(IndexOption::PLAY_GAUGE_TYPE_1P, Option::GAUGE_EXHARD); break;
        default:                      State::set(IndexOption::PLAY_GAUGE_TYPE_1P, Option::GAUGE_NORMAL); break;
        }

        switch ((PlayModifierLaneEffectType)gPlayContext.replay->laneEffectType)
        {
        case PlayModifierLaneEffectType::SUDDEN:  State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, Option::LANE_SUDDEN); break;
        case PlayModifierLaneEffectType::HIDDEN:  State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, Option::LANE_HIDDEN); break;
        case PlayModifierLaneEffectType::SUDHID:  State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, Option::LANE_SUDHID); break;
        case PlayModifierLaneEffectType::LIFT:    State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, Option::LANE_LIFT); break;
        case PlayModifierLaneEffectType::LIFTSUD: State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, Option::LANE_LIFTSUD); break;
        default:                      State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, Option::LANE_OFF); break;
        }

        switch (gPlayContext.replay->hispeedFix)
        {
        case PlayModifierHispeedFixType::MAXBPM:    State::set(IndexOption::PLAY_HSFIX_TYPE, Option::SPEED_FIX_MAX); break;
        case PlayModifierHispeedFixType::MINBPM:    State::set(IndexOption::PLAY_HSFIX_TYPE, Option::SPEED_FIX_MIN); break;
        case PlayModifierHispeedFixType::AVERAGE:   State::set(IndexOption::PLAY_HSFIX_TYPE, Option::SPEED_FIX_AVG); break;
        case PlayModifierHispeedFixType::CONSTANT:  State::set(IndexOption::PLAY_HSFIX_TYPE, Option::SPEED_FIX_CONSTANT); break;
        case PlayModifierHispeedFixType::INITIAL:     State::set(IndexOption::PLAY_HSFIX_TYPE, Option::SPEED_FIX_INITIAL); break;
        case PlayModifierHispeedFixType::MAIN:      State::set(IndexOption::PLAY_HSFIX_TYPE, Option::SPEED_FIX_MAIN); break;
        default:                State::set(IndexOption::PLAY_HSFIX_TYPE, Option::SPEED_NORMAL); break;
        }

        State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_1P, !!(gPlayContext.replay->assistMask& PLAY_MOD_ASSIST_AUTOSCR));
        State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_2P, false);
        State::set(IndexSwitch::PLAY_OPTION_DP_FLIP, gPlayContext.replay->DPFlip);
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
            Path replayFilePath;
            if ((entry->type() == eEntryType::CHART || entry->type() == eEntryType::RIVAL_CHART))
            {
                replayFilePath = ReplayChart::getReplayPath(gChartContext.hash) / pScore->replayFileName;
            }
            else if (entry->type() == eEntryType::COURSE && !gPlayContext.courseCharts.empty())
            {
                auto pScoreStage1 = g_pScoreDB->getChartScoreBMS(gPlayContext.courseCharts[0]);
                if (pScoreStage1 && !pScoreStage1->replayFileName.empty())
                {
                    replayFilePath = ReplayChart::getReplayPath(gPlayContext.courseCharts[0]) / pScoreStage1->replayFileName;
                }
            }
            if (!replayFilePath.empty() && fs::is_regular_file(replayFilePath))
            {
                gPlayContext.replayMybest = std::make_shared<ReplayChart>();
                if (gPlayContext.replayMybest->loadFile(replayFilePath))
                {
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].randomLeft = gPlayContext.replayMybest->randomTypeLeft;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].randomRight = gPlayContext.replayMybest->randomTypeRight;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].gauge = gPlayContext.replayMybest->gaugeType;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].assist_mask = gPlayContext.replayMybest->assistMask;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].hispeedFix = gPlayContext.replayMybest->hispeedFix;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].laneEffect = (PlayModifierLaneEffectType)gPlayContext.replayMybest->laneEffectType;
                    gPlayContext.mods[PLAYER_SLOT_MYBEST].DPFlip = gPlayContext.replayMybest->DPFlip;
                }
                else
                {
                    gPlayContext.replayMybest.reset();
                }
            }
        }
    }

    gNextScene = SceneType::DECIDE;
}

void SceneSelect::navigateUpBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
        gSelectContext.selectedEntryIndex = (gSelectContext.entries.size() + gSelectContext.selectedEntryIndex - 1) % gSelectContext.entries.size();
        setBarInfo();
        setEntryInfo();
        setDynamicTextures();

        State::set(IndexTimer::LIST_MOVE, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void SceneSelect::navigateDownBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
        gSelectContext.selectedEntryIndex = (gSelectContext.selectedEntryIndex + 1) % gSelectContext.entries.size();
        setBarInfo();
        setEntryInfo();
        setDynamicTextures();

        State::set(IndexTimer::LIST_MOVE, t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void SceneSelect::navigateEnter(const Time& t)
{
    if (!gSelectContext.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        if (gSelectContext.entries[gSelectContext.selectedEntryIndex].first->type() == eEntryType::FOLDER)
        {
            {
                const auto& [e, s] = gSelectContext.entries[gSelectContext.selectedEntryIndex];

                SongListProperties prop{
                    gSelectContext.backtrace.front().folder,
                    e->md5,
                    e->_name,
                    {},
                    {},
                    0,
                    false
                };
                auto top = g_pSongDB->browse(e->md5, false);
                if (top && !top->empty())
                {
                    for (size_t i = 0; i < top->getContentsCount(); ++i)
                        prop.dbBrowseEntries.push_back({ top->getEntry(i), nullptr });
                }

                gSelectContext.backtrace.front().index = gSelectContext.selectedEntryIndex;
                gSelectContext.backtrace.front().displayEntries = gSelectContext.entries;
                gSelectContext.backtrace.push_front(prop);
                gSelectContext.entries.clear();
            }

            loadSongList();
            if (gSelectContext.entries.empty())
            {
                State::set(IndexOption::SELECT_FILTER_DIFF, Option::DIFF_ANY);
                gSelectContext.filterDifficulty = State::get(IndexOption::SELECT_FILTER_DIFF);
                loadSongList();
            }
            if (gSelectContext.entries.empty())
            {
                State::set(IndexOption::SELECT_FILTER_KEYS, Option::FILTER_KEYS_ALL);
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
                loadSongList();
            }
        }
        else if (auto top = std::dynamic_pointer_cast<EntryFolderBase>(gSelectContext.entries[gSelectContext.selectedEntryIndex].first); top)
        {
            {
                auto folderType = gSelectContext.entries[gSelectContext.selectedEntryIndex].first->type();
                const auto& [e, s] = gSelectContext.entries[gSelectContext.selectedEntryIndex];

                SongListProperties prop{
                    gSelectContext.backtrace.front().folder,
                    e->md5,
                    e->_name,
                    {},
                    {},
                    0,
                    folderType == eEntryType::CUSTOM_FOLDER || folderType == eEntryType::RIVAL
                };
                for (size_t i = 0; i < top->getContentsCount(); ++i)
                    prop.dbBrowseEntries.push_back({ top->getEntry(i), nullptr });

                gSelectContext.backtrace.front().index = gSelectContext.selectedEntryIndex;
                gSelectContext.backtrace.front().displayEntries = gSelectContext.entries;
                gSelectContext.backtrace.push_front(prop);
                gSelectContext.entries.clear();
            }

            loadSongList();
        }

        sortSongList();
        gSelectContext.selectedEntryIndex = 0;

        setBarInfo();
        setEntryInfo();
        setDynamicTextures();

        if (!gSelectContext.entries.empty())
        {
            State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.selectedEntryIndex / gSelectContext.entries.size());
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
    }
}
void SceneSelect::navigateBack(const Time& t, bool sound)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (gSelectContext.backtrace.size() >= 2)
    {
        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        if (gArenaData.isOnline())
        {
            if (gArenaData.isClient())
                g_pArenaClient->requestChart(HashMD5());
            else
                g_pArenaHost->requestChart(HashMD5(), "host");

            gSelectContext.isInArenaRequest = false;
        }

        auto& top = gSelectContext.backtrace.front();

        gSelectContext.selectedEntryIndex = 0;
        gSelectContext.backtrace.pop_front();
        auto& parent = gSelectContext.backtrace.front();
        gSelectContext.entries = parent.displayEntries;
        gSelectContext.selectedEntryIndex = parent.index;

        if (parent.ignoreFilters)
        {
            // change display only
            State::set(IndexOption::SELECT_FILTER_DIFF, Option::DIFF_ANY);
            State::set(IndexOption::SELECT_FILTER_KEYS, Option::FILTER_KEYS_ALL);
        }
        else
        {
            // restore prev
            State::set(IndexOption::SELECT_FILTER_DIFF, gSelectContext.filterDifficulty);
            int keys = 0;
            switch (gSelectContext.filterKeys)
            {
            case 1: keys = Option::FILTER_KEYS_SINGLE; break;
            case 7: keys = Option::FILTER_KEYS_7; break;
            case 5: keys = Option::FILTER_KEYS_5; break;
            case 2: keys = Option::FILTER_KEYS_DOUBLE; break;
            case 14: keys = Option::FILTER_KEYS_14; break;
            case 10: keys = Option::FILTER_KEYS_10; break;
            case 9: keys = Option::FILTER_KEYS_9; break;
            }
            State::set(IndexOption::SELECT_FILTER_KEYS, keys);
        }

        setBarInfo();
        setEntryInfo();
        setDynamicTextures();

        if (!gSelectContext.entries.empty())
        {
            State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.selectedEntryIndex / gSelectContext.entries.size());
        }
        else
        {
            State::set(IndexSlider::SELECT_LIST, 0.0);
        }

        resetJukeboxText();

        scrollAccumulator = 0.;
        scrollAccumulatorAddUnit = 0.;

        if (sound)
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_CLOSE);
    }
}

void SceneSelect::navigateVersionEnter(const Time& t)
{
    isInVersionList = true;

    // TODO
    // play some sound
    // play some animation
    // push current list into buffer
    // create version list
    // show list

    // For now we just switch difficulty filter
    lr2skin::button::select_difficulty_filter(1);
}

void SceneSelect::navigateVersionBack(const Time& t)
{
    // TODO
    // play some sound
    // play some animation
    // behavior like navigateBack

    isInVersionList = false;
}

bool SceneSelect::closeAllPanels(const Time& t)
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
    if (pSkin)
    {
        if (pSkin->textEditSpriteClicked())
        {
            if (pSkin->textEditType() == IndexText::EDIT_JUKEBOX_NAME)
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
    SceneBase::stopTextEdit(modify);
    if (!modify)
        resetJukeboxText();
}

void SceneSelect::resetJukeboxText()
{
    if (gSelectContext.backtrace.front().name.empty())
        State::set(IndexText::EDIT_JUKEBOX_NAME, i18n::s(i18nText::SEARCH_SONG));
    else
        State::set(IndexText::EDIT_JUKEBOX_NAME, gSelectContext.backtrace.front().name);
}

void SceneSelect::searchSong(const std::string& text)
{
    LOG_DEBUG << "Search: " << text;

    auto top = g_pSongDB->search(ROOT_FOLDER_HASH, text);
    if (!top || top->empty())
    {
        State::set(IndexText::EDIT_JUKEBOX_NAME, i18n::s(i18nText::SEARCH_FAILED));
        return;
    }

    std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

    std::string name = (boost::format(i18n::c(i18nText::SEARCH_RESULT)) % text % top->getContentsCount()).str();
    SongListProperties prop{
        "",
        "",
        name,
        {},
        {},
        0
    };
    for (size_t i = 0; i < top->getContentsCount(); ++i)
        prop.dbBrowseEntries.push_back({ top->getEntry(i), nullptr });

    gSelectContext.backtrace.front().index = gSelectContext.selectedEntryIndex;
    gSelectContext.backtrace.front().displayEntries = gSelectContext.entries;
    gSelectContext.backtrace.push_front(prop);
    gSelectContext.entries.clear();
    loadSongList();
    sortSongList();

    navigateTimestamp = Time();
    postStopPreview();

    gSelectContext.selectedEntryIndex = 0;
    setBarInfo();
    setEntryInfo();
    setDynamicTextures();

    if (!gSelectContext.entries.empty())
    {
        State::set(IndexSlider::SELECT_LIST, (double)gSelectContext.selectedEntryIndex / gSelectContext.entries.size());
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

    bool previewDedicated = ConfigMgr::get('P', cfg::P_PREVIEW_DEDICATED, false);
    bool previewDirect = ConfigMgr::get('P', cfg::P_PREVIEW_DIRECT, false);
    if (!previewDedicated && !previewDirect)
    {
        std::unique_lock l(previewMutex);
        previewState = PREVIEW_FINISH;
        return;
    }

    switch (previewState)
    {
    case PREVIEW_NONE:
    {
        {
            std::unique_lock l(previewMutex);
            previewState = PREVIEW_LOAD;
        }

        Path previewChartPath;
        eEntryType type = eEntryType::UNKNOWN;
        {
            std::shared_lock<std::shared_mutex> s(gSelectContext._mutex);
            if (!gSelectContext.entries.empty())
            {
                const auto& entry = gSelectContext.entries[gSelectContext.selectedEntryIndex].first;
                type = entry->type();

                if (type == eEntryType::SONG || type == eEntryType::RIVAL_SONG)
                {
                    auto pFile = std::reinterpret_pointer_cast<EntryFolderSong>(entry)->getCurrentChart();
                    if (pFile) previewChartPath = pFile->absolutePath;
                }
                else if (type == eEntryType::CHART || type == eEntryType::RIVAL_CHART)
                {
                    auto pFile = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
                    if (pFile) previewChartPath = pFile->absolutePath;
                }
            }
        }
        if (previewChartPath.empty())
        {
            std::unique_lock l(previewMutex);
            previewState = PREVIEW_FINISH;
            return;
        }

        // create Chart object
        {
            std::unique_lock l(previewMutex);
            previewState = PREVIEW_CHART;
            previewChart.reset();
        }
        std::thread([&, previewChartPath, type]()
            {
                std::shared_ptr<ChartFormatBase> previewChartTmp;
                if (type == eEntryType::SONG || type == eEntryType::RIVAL_SONG)
                {
                    previewChartTmp = ChartFormatBase::createFromFile(previewChartPath, gPlayContext.randomSeed);
                }
                else
                {
                    previewChartTmp = ChartFormatBase::createFromFile(previewChartPath, gPlayContext.randomSeed);
                }

                {
                    std::unique_lock l(previewMutex);
                    if (previewState == PREVIEW_CHART)
                    {
                        previewChart = previewChartTmp;
                    }
                }
            }).detach();

            break;
    }

    case PREVIEW_CHART:
    {
        std::shared_ptr<ChartFormatBase> previewChartTmp;
        {
            std::shared_lock l(previewMutex);
            if (previewChart == nullptr)
                break;

            previewChartTmp = previewChart;
        }

        previewStandalone = false;
        previewStandaloneLength = 0;

        // load chart object from Chart object
        switch (previewChartTmp->type())
        {
        case eChartFormat::BMS:
        {
            auto bms = std::reinterpret_pointer_cast<ChartFormatBMS>(previewChartTmp);

            if (previewDedicated)
            {
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
            }

            if (previewStandalone)
            {
                LOG_DEBUG << "[Select] Preview dedicated";

                std::unique_lock l(previewMutex);
                previewState = PREVIEW_LOADED;
            }
            else if (previewDirect)
            {
                LOG_DEBUG << "[Select] Preview direct";

                {
                    std::unique_lock l(previewMutex);
                    previewState = PREVIEW_LOAD;
                }

                gChartContext.isSampleLoaded = false;
                gChartContext.sampleLoadedHash.reset();

                std::thread([&, bms] {
                    unsigned bars = bms->lastBarIdx;
                    auto previewChartObjTmp = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);
                    auto previewRulesetTmp = std::make_shared<RulesetBMSAuto>(bms, previewChartObjTmp,
                        PlayModifierGaugeType::NORMAL, bms->gamemode, RulesetBMS::JudgeDifficulty::VERYHARD, 0.2, RulesetBMS::PlaySide::RIVAL);

                    // load samples
                    SoundMgr::freeNoteSamples();
                    auto chartDir = bms->getDirectory();

                    int wavTotal = 0;
                    for (const auto& it : bms->wavFiles)
                    {
                        if (!it.empty()) ++wavTotal;
                    }
                    if (wavTotal != 0)
                    {
                        boost::asio::thread_pool pool(std::max(1u, std::thread::hardware_concurrency() - 2));
                        for (size_t i = 0; i < bms->wavFiles.size(); ++i)
                        {
                            const auto& wav = bms->wavFiles[i];
                            if (wav.empty()) continue;

                            boost::asio::post(pool, std::bind([&](size_t i)
                                {
                                    Path pWav = fs::u8path(wav);
                                    if (pWav.is_absolute())
                                        SoundMgr::loadNoteSample(pWav, i);
                                    else
                                        SoundMgr::loadNoteSample((chartDir / pWav), i);

                                    std::shared_lock l(previewMutex);
                                    if (sceneEnding || previewState != PREVIEW_LOAD)
                                    {
                                        LOG_DEBUG << "[Select] Preview loading interrupted";
                                        return;
                                    }
                                }, i));
                        }
                        pool.wait();

                        gChartContext.isSampleLoaded = true;
                        gChartContext.sampleLoadedHash = bms->fileHash;

                        {
                            std::unique_lock l(previewMutex);
                            if (bms == previewChart)
                            {
                                LOG_DEBUG << "[Select] Preview loading finished";
                                previewChartObj = previewChartObjTmp;
                                previewRuleset = previewRulesetTmp;
                                previewState = PREVIEW_LOADED;
                            }
                            else
                            {
                                LOG_DEBUG << "[Select] Preview chart has changed, stop";
                                previewState = PREVIEW_FINISH;
                            }
                        }
                    }
                    }).detach();
            }
            else
            {
                std::unique_lock l(previewMutex);
                previewState = PREVIEW_FINISH;
            }
            break;
        }
        }
        
        break;
    }

    case PREVIEW_LOAD:
    {
        // wait for the loading thread
        break;
    }

    case PREVIEW_LOADED:
    {
        if (previewStandalone)
        {
            std::unique_lock l(previewMutex);

            LOG_DEBUG << "[Select] Preview start";

            previewStartTime = 0;
            previewStandaloneLength = SoundMgr::getNoteSampleLength(0);

            SoundMgr::setSysVolume(0.1, 200);
            previewState = PREVIEW_PLAY;
        }
        else if (gChartContext.isSampleLoaded)
        {
            std::unique_lock l(previewMutex);

            LOG_DEBUG << "[Select] Preview start";

            // start from beginning. It's difficult to seek a chart for playback due to lengthy BGM samples...
            previewStartTime = Time() - previewChartObj->getLeadInTime();
            previewEndTime = 0;
            previewRuleset->setStartTime(previewStartTime);

            SoundMgr::setSysVolume(0.1, 200);
            previewState = PREVIEW_PLAY;
        }
        else
        {
            std::unique_lock l(previewMutex);
            previewState = PREVIEW_FINISH;
        }

        break;
    }

    case PREVIEW_PLAY:
    {
        std::unique_lock l(previewMutex);

        if (previewStandalone)
        {
            if (previewStartTime == 0)
            {
                previewStartTime = Time();

                size_t idx = 0;
                SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, 1, &idx);
            }
            else if ((Time() - previewStartTime).norm() > previewStandaloneLength)
            {
                LOG_DEBUG << "[Select] Preview finished";

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
                if (previewEndTime == 0)
                {
                    previewEndTime = t;
                }
                else if ((t - previewEndTime).norm() > 1000)
                {
                    LOG_DEBUG << "[Select] Preview finished";

                    previewState = PREVIEW_FINISH;
                    SoundMgr::setSysVolume(1.0, 400);
                }
            }
        }
        break;
    }

    case PREVIEW_FINISH:
        break;
    }
}

void SceneSelect::postStopPreview()
{
    std::unique_lock l(previewMutex);

    if (previewState != PREVIEW_NONE && previewState != PREVIEW_FINISH)
    {
        LOG_DEBUG << "[Select] Preview stop";
    }

    SoundMgr::stopNoteSamples();
    SoundMgr::setSysVolume(1.0, 400);
    previewState = PREVIEW_NONE;
}


void SceneSelect::arenaCommand()
{
    if (auto p = std::dynamic_pointer_cast<EntryArenaCommand>(gSelectContext.entries[gSelectContext.selectedEntryIndex].first); p)
    {
        switch (p->getCmdType())
        {
        case EntryArenaCommand::Type::HOST_LOBBY:  arenaHostLobby(); break;
        case EntryArenaCommand::Type::LEAVE_LOBBY: arenaLeaveLobby(); break;
        case EntryArenaCommand::Type::JOIN_LOBBY:  arenaJoinLobbyPrompt(); break;
        }
    }
}

void SceneSelect::arenaHostLobby()
{
    if (gArenaData.isOnline())
    {
        createNotification(i18n::s(i18nText::ARENA_HOST_FAILED_IN_LOBBY));
        return;
    }

    g_pArenaHost = std::make_shared<ArenaHost>();

    if (g_pArenaHost->createLobby())
    {
        g_pArenaHost->loopStart();
        createNotification(i18n::s(i18nText::ARENA_HOST_SUCCESS));

        Time t;
        navigateBack(t, false);
        State::set(IndexTimer::ARENA_SHOW_LOBBY, t.norm());

        // Reset freq option for arena. Not supported yet
        lr2skin::button::pitch_switch(0);

        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
    }
    else
    {
        g_pArenaHost.reset();
        createNotification(i18n::s(i18nText::ARENA_HOST_FAILED));
    }
}

void SceneSelect::arenaLeaveLobby()
{
    if (!gArenaData.isOnline())
    {
        createNotification(i18n::s(i18nText::ARENA_LEAVE_FAILED_NO_LOBBY));
        return;
    }

    if (gArenaData.isClient())
    {
        g_pArenaClient->leaveLobby();
    }
    if (gArenaData.isServer())
    {
        g_pArenaHost->disbandLobby();
    }

    Time t;
    navigateBack(t, false);
    State::set(IndexTimer::ARENA_SHOW_LOBBY, TIMER_NEVER);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
}

void SceneSelect::arenaJoinLobbyPrompt()
{
    if (gArenaData.isOnline())
    {
        createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_IN_LOBBY));
        return;
    }

    imgui_show_arenaJoinLobbyPrompt = true;
    pSkin->setHandleMouseEvents(false);
}

void SceneSelect::arenaJoinLobby()
{
    assert(!gArenaData.isOnline());

    std::string address = imgui_arena_address_buf;
    createNotification((boost::format(i18n::c(i18nText::ARENA_JOINING)) % address).str());

    g_pArenaClient = std::make_shared<ArenaClient>();
    if (g_pArenaClient->joinLobby(address))
    {
        g_pArenaClient->loopStart();
        createNotification(i18n::s(i18nText::ARENA_JOIN_SUCCESS));

        Time t;
        navigateBack(t, false);
        State::set(IndexTimer::ARENA_SHOW_LOBBY, t.norm());

        // Reset freq option for arena. Not supported yet
        lr2skin::button::pitch_switch(0);

        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
    }
    else
    {
        g_pArenaClient.reset();
        createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED));
    }
    pSkin->setHandleMouseEvents(true);
}