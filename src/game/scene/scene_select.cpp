#include "common/pch.h"

#include "scene_select.h"
#include "scene_mgr.h"
#include "scene_pre_select.h"

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

#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

#include "game/data/data_types.h"

#include "db/db_song.h"
#include "db/db_score.h"

#include "game/replay/replay_chart.h"

////////////////////////////////////////////////////////////////////////////////

namespace lunaticvibes
{

#pragma region save config

void config_sys()
{
    using namespace cfg;

    switch (SystemData.windowMode)
    {
    case GameWindowMode::FULLSCREEN: ConfigMgr::set('C',V_WINMODE, V_WINMODE_FULL); break;
    case GameWindowMode::BORDERLESS: ConfigMgr::set('C',V_WINMODE, V_WINMODE_BORDERLESS); break;
    case GameWindowMode::WINDOWED:
    default:                         ConfigMgr::set('C',V_WINMODE, V_WINMODE_WINDOWED); break;
    }
}

void config_player()
{
    using namespace cfg;

    ConfigMgr::set('P', P_PANEL_STYLE, PlayData.panelStyle);

    switch (PlayData.targetType)
    {
    case TargetType::Zero: ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_0); break;
    case TargetType::MyBest: ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_MYBEST); break;
    case TargetType::RankAAA: ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_AAA); break;
    case TargetType::RankAA: ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_AA); break;
    case TargetType::RankA: ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_A); break;
    case TargetType::UseTargetRate: ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_DEFAULT); break;
    default:                        ConfigMgr::set('P', P_TARGET_TYPE, P_TARGET_TYPE_DEFAULT); break;
    }

    if (!PlayData.isReplay)
    {
        ConfigMgr::set('P', P_HISPEED, PlayData.player[PLAYER_SLOT_PLAYER].hispeed);
        ConfigMgr::set('P', P_HISPEED_2P, PlayData.player[PLAYER_SLOT_TARGET].hispeed);
        ConfigMgr::set('P', cfg::P_GREENNUMBER, PlayData.player[PLAYER_SLOT_PLAYER].greenNumber);
        ConfigMgr::set('P', cfg::P_GREENNUMBER_2P, PlayData.player[PLAYER_SLOT_TARGET].greenNumber);

        switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix)
        {
        case PlayModifierHispeedFixType::MAXBPM:      ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_MAX); break;
        case PlayModifierHispeedFixType::MINBPM:      ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_MIN); break;
        case PlayModifierHispeedFixType::AVERAGE:      ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_AVG); break;
        case PlayModifierHispeedFixType::CONSTANT: ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_CONSTANT); break;
        case PlayModifierHispeedFixType::INITIAL:  ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_INITIAL); break;
        case PlayModifierHispeedFixType::MAIN:     ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_MAIN); break;
        default:                         ConfigMgr::set('P', P_SPEED_TYPE, P_SPEED_TYPE_NORMAL); break;
        }

        switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft)
        {
        case PlayModifierRandomType::MIRROR: ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_MIRROR); break;
        case PlayModifierRandomType::RANDOM: ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_RANDOM); break;
        case PlayModifierRandomType::SRAN:   ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_SRAN); break;
        case PlayModifierRandomType::HRAN:   ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_HRAN); break;
        case PlayModifierRandomType::ALLSCR: ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_ALLSCR); break;
        case PlayModifierRandomType::RRAN:   ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_RRAN); break;
        default:                             ConfigMgr::set('P', P_CHART_OP, P_CHART_OP_NORMAL); break;
        }

        if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
        {
            switch (PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft)
            {
            case PlayModifierRandomType::MIRROR: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_MIRROR); break;
            case PlayModifierRandomType::RANDOM: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_RANDOM); break;
            case PlayModifierRandomType::SRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_SRAN); break;
            case PlayModifierRandomType::HRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_HRAN); break;
            case PlayModifierRandomType::ALLSCR: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_ALLSCR); break;
            case PlayModifierRandomType::RRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_RRAN); break;
            default:                 ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_NORMAL); break;
            }
        }
        else if (PlayData.mode == SkinType::PLAY10 || PlayData.mode == SkinType::PLAY14)
        {
            switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight)
            {
            case PlayModifierRandomType::MIRROR: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_MIRROR); break;
            case PlayModifierRandomType::RANDOM: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_RANDOM); break;
            case PlayModifierRandomType::SRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_SRAN); break;
            case PlayModifierRandomType::HRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_HRAN); break;
            case PlayModifierRandomType::ALLSCR: ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_ALLSCR); break;
            case PlayModifierRandomType::RRAN:   ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_RRAN); break;
            default:                 ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_NORMAL); break;
            }
        }
        else
        {
            ConfigMgr::set('P', P_CHART_OP_2P, P_CHART_OP_NORMAL);
        }

        switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge)
        {
        case PlayModifierGaugeType::HARD:   ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_HARD); break;
        case PlayModifierGaugeType::EASY:   ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_EASY); break;
        case PlayModifierGaugeType::DEATH:  ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_DEATH); break;
        case PlayModifierGaugeType::EXHARD: ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_EXHARD); break;
        case PlayModifierGaugeType::ASSISTEASY: ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_ASSISTEASY); break;
        default:                   ConfigMgr::set('P', P_GAUGE_OP, P_GAUGE_OP_NORMAL); break;
        }
        switch (PlayData.player[PLAYER_SLOT_TARGET].mods.gauge)
        {
        case PlayModifierGaugeType::HARD:   ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_HARD); break;
        case PlayModifierGaugeType::EASY:   ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_EASY); break;
        case PlayModifierGaugeType::DEATH:  ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_DEATH); break;
        case PlayModifierGaugeType::EXHARD: ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_EXHARD); break;
        case PlayModifierGaugeType::ASSISTEASY: ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_ASSISTEASY); break;
        default:                   ConfigMgr::set('P', P_GAUGE_OP_2P, P_GAUGE_OP_NORMAL); break;
        }

        switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect)
        {
        case PlayModifierLaneEffectType::OFF:     ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_OFF); break;
        case PlayModifierLaneEffectType::HIDDEN:  ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_HIDDEN); break;
        case PlayModifierLaneEffectType::SUDDEN:  ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_SUDDEN); break;
        case PlayModifierLaneEffectType::SUDHID:  ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_SUDHID); break;
        case PlayModifierLaneEffectType::LIFT:    ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_LIFT); break;
        case PlayModifierLaneEffectType::LIFTSUD: ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_LIFTSUD); break;
        default:                   ConfigMgr::set('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_OFF); break;
        }
        switch (PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect)
        {
        case PlayModifierLaneEffectType::OFF:     ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_OFF); break;
        case PlayModifierLaneEffectType::HIDDEN:  ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_HIDDEN); break;
        case PlayModifierLaneEffectType::SUDDEN:  ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_SUDDEN); break;
        case PlayModifierLaneEffectType::SUDHID:  ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_SUDHID); break;
        case PlayModifierLaneEffectType::LIFT:    ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_LIFT); break;
        case PlayModifierLaneEffectType::LIFTSUD: ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_LIFTSUD); break;
        default:                   ConfigMgr::set('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_OFF); break;
        }

        ConfigMgr::set('P', P_CHART_ASSIST_OP, PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask);
        ConfigMgr::set('P', P_CHART_ASSIST_OP_2P, PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask);

        ConfigMgr::set('P', P_FLIP, PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip);
    }

    // FIXME these options should be adjustable by 2P
    switch (PlayData.ghostType)
    {
    case GhostScorePosition::AboveJudge:     ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_A); break;
    case GhostScorePosition::NearJudge:      ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_B); break;
    case GhostScorePosition::NearJudgeLower: ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_C); break;
    default:                                 ConfigMgr::set('P',P_GHOST_TYPE, "OFF"); break;
    }
    ConfigMgr::set('P', P_JUDGE_OFFSET, PlayData.player[PLAYER_SLOT_PLAYER].offsetVisual);
    ConfigMgr::set('P', P_GHOST_TARGET, PlayData.targetRate);

    switch (SelectData.filterKeys)
    {
    case FilterKeysType::Single: ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_SINGLE); break;
    case FilterKeysType::_7:     ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_7K); break;
    case FilterKeysType::_5:     ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_5K); break;
    case FilterKeysType::_14:    ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_14K); break;
    case FilterKeysType::Double: ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_DOUBLE); break;
    case FilterKeysType::_10:    ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_10K); break;
    case FilterKeysType::_9:     ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_9K); break;
    default:                     ConfigMgr::set('P', P_FILTER_KEYS, P_FILTER_KEYS_ALL); break;
    }

    switch (SelectData.sortType)
    {
    case SongListSortType::TITLE: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_TITLE); break;
    case SongListSortType::LEVEL: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_LEVEL); break;
    case SongListSortType::CLEAR: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_CLEAR); break;
    case SongListSortType::RATE:  ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_RATE); break;
    default:                 ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_FOLDER); break;
    }

    switch (SelectData.filterDifficulty)
    {
    case FilterDifficultyType::B: ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_BEGINNER); break;
    case FilterDifficultyType::N: ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_NORMAL); break;
    case FilterDifficultyType::H: ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_HYPER); break;
    case FilterDifficultyType::A: ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ANOTHER); break;
    case FilterDifficultyType::I: ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_INSANE); break;
    default:                      ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL); break;
    }
}

void config_vol()
{
    using namespace cfg;

    ConfigMgr::set('P',P_VOL_MASTER, SystemData.volumeMaster);
    ConfigMgr::set('P',P_VOL_KEY, SystemData.volumeKey);
    ConfigMgr::set('P',P_VOL_BGM, SystemData.volumeBgm);
}

void config_eq()
{
    using namespace cfg;

    ConfigMgr::set('P', P_EQ,  SystemData.equalizerEnabled);
    ConfigMgr::set('P', P_EQ0, SystemData.equalizerVal62_5hz);
    ConfigMgr::set('P', P_EQ1, SystemData.equalizerVal160hz);
    ConfigMgr::set('P', P_EQ2, SystemData.equalizerVal400hz);
    ConfigMgr::set('P', P_EQ3, SystemData.equalizerVal1khz);
    ConfigMgr::set('P', P_EQ4, SystemData.equalizerVal2_5khz);
    ConfigMgr::set('P', P_EQ5, SystemData.equalizerVal6_25khz);
    ConfigMgr::set('P', P_EQ6, SystemData.equalizerVal16khz);
}

void config_freq()
{
    using namespace cfg;

    switch (SystemData.freqType)
    {
    case FreqModifierType::Off: ConfigMgr::set('P', P_FREQ_TYPE, "OFF"); break;
    case FreqModifierType::Frequency: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_FREQ); break;
    case FreqModifierType::PitchOnly: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_PITCH); break;
    case FreqModifierType::SpeedOnly: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_SPEED); break;
    default: break;
    }
    ConfigMgr::set('P',P_FREQ_VAL, SystemData.freqVal);
}

void config_fx()
{
    using namespace cfg;

    switch (SystemData.fxType)
    {
    case FXType::SfxReverb:     ConfigMgr::set('P', P_FX0_TYPE, P_FX_TYPE_REVERB); break;
    case FXType::Echo:          ConfigMgr::set('P', P_FX0_TYPE, P_FX_TYPE_DELAY); break;
    case FXType::LowPass:       ConfigMgr::set('P', P_FX0_TYPE, P_FX_TYPE_LOWPASS); break;
    case FXType::HighPass:      ConfigMgr::set('P', P_FX0_TYPE, P_FX_TYPE_HIGHPASS); break;
    case FXType::Compressor:    ConfigMgr::set('P', P_FX0_TYPE, P_FX_TYPE_COMPRESSOR); break;
    default:                    ConfigMgr::set('P', P_FX0_TYPE, "OFF"); break;
    }
    ConfigMgr::set('P', P_FX0_P1, SystemData.fxVal);
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////

SceneSelect::SceneSelect() : SceneBase(SkinType::MUSIC_SELECT, 250)
{
    _type = SceneType::SELECT;

    _inputAvailable = INPUT_MASK_FUNC | INPUT_MASK_MOUSE;
    _inputAvailable |= INPUT_MASK_1P;
    _inputAvailable |= INPUT_MASK_2P;

    // reset globals
    loadConfigs();

    SelectData.lastLaneEffectType1P = PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect;

    if (!SelectData.entries.empty())
    {
        // delay sorting chart list after playing
        std::unique_lock<std::shared_mutex> u(SelectData._mutex);
        SelectData.updateEntryScore(SelectData.selectedEntryIndex);
        SelectData.setEntryInfo();
    }
    else
    {
        std::unique_lock<std::shared_mutex> u(SelectData._mutex);
        SelectData.loadSongList();
        SelectData.sortSongList();
        SelectData.setBarInfo();
        SelectData.setEntryInfo();

        resetJukeboxText();
    }

    PlayData.isAuto = false;
    PlayData.isReplay = false;
    SelectData.isGoingToKeyConfig = false;
    SelectData.isGoingToSkinSelect = false;
    SelectData.isGoingToAutoPlay = false;
    SelectData.isGoingToReplay = false;
    SelectData.isGoingToReboot = false;
    PlayData.isAuto = false;

    SystemData.overlayTopLeftText[0] = "";
    SystemData.overlayTopLeftText[1] = "";

    lr2skin::button::target_type(0);

    // if (!LR2CustomizeData.isInCustomize)
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

    SoundMgr::stopNoteSamples();
    SoundMgr::stopSysSamples();
    SoundMgr::setSysVolume(1.0);
    SoundMgr::setNoteVolume(1.0);
    SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);

    // do not play preview right after scene is loaded
    previewState = PREVIEW_FINISH;

    if (ArenaData.isOnline())
        ArenaData.timers["show_lobby"] = Time().norm();

    imguiInit();
}

SceneSelect::~SceneSelect()
{
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
    if (SystemData.gNextScene != SceneType::SELECT) return;

    Time t;

    if (SystemData.isAppExiting)
    {
        SystemData.gNextScene = SceneType::EXIT_TRANS;
    }

    _updateCallback();

    if (SelectData.optionChangePending)
    {
        SelectData.optionChangePending = false;

        SelectData.timers["list_entry_change"] = t.norm();
        navigateTimestamp = t;
        postStopPreview();
    }

    if (SelectData.cursorClick != SelectData.highlightBarIndex)
    {
        int idx = SelectData.selectedEntryIndex + SelectData.cursorClick - SelectData.highlightBarIndex;
        if (idx < 0)
            idx += SelectData.entries.size() * ((-idx) / SelectData.entries.size() + 1);
        SelectData.selectedEntryIndex = idx % SelectData.entries.size();
        SelectData.highlightBarIndex = SelectData.cursorClick;

        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(SelectData._mutex);
        SelectData.setBarInfo();
        SelectData.setEntryInfo();
        SelectData.setDynamicTextures();

        SelectData.timers["list_entry_change"] = t.norm();
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }

    if (SelectData.cursorEnterPending)
    {
        SelectData.cursorEnterPending = false;

        switch (SelectData.entries[SelectData.selectedEntryIndex].first->type())
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
    if (SelectData.cursorClickScroll != 0)
    {
        if (scrollAccumulator == 0.0)
        {
            if (SelectData.cursorClickScroll > 0)
            {
                scrollAccumulator -= SelectData.cursorClickScroll;
                scrollButtonTimestamp = t;
                scrollAccumulatorAddUnit = scrollAccumulator / SelectData.scrollTimeLength * (1000.0 / getRate());
            }
            else
            {
                scrollAccumulator += -SelectData.cursorClickScroll;
                scrollButtonTimestamp = t;
                scrollAccumulatorAddUnit = scrollAccumulator / SelectData.scrollTimeLength * (1000.0 / getRate());
            }
            SelectData.timers["list_move"] = TIMER_NEVER;
        }
        SelectData.cursorClickScroll = 0;
    }

    // update by slider
    if (SelectData.draggingListSlider)
    {
        SelectData.draggingListSlider = false;

        size_t idx_new = (size_t)std::floor(SelectData.selectedEntryIndexRolling * SelectData.entries.size());
        if (idx_new == SelectData.entries.size())
            idx_new = 0;

        if (SelectData.selectedEntryIndex != idx_new)
        {
            navigateTimestamp = t;
            postStopPreview();

            std::unique_lock<std::shared_mutex> u(SelectData._mutex);
            SelectData.selectedEntryIndex = idx_new;
            SelectData.setBarInfo();
            SelectData.setEntryInfo();
            SelectData.setDynamicTextures();

            SelectData.timers["list_entry_change"] = t.norm();
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
        }
    }

    // update by arena
    if (imgui_arena_joinLobby)
    {
        imgui_arena_joinLobby = false;
        arenaJoinLobby();
    }
    if (!ArenaData.remoteRequestedChart.empty())
    {
        std::string folderName = (boost::format(i18n::c(i18nText::ARENA_REQUEST_BY)) % ArenaData.remoteRequestedPlayer).str();
        SongListProperties prop{
            SelectData.backtrace.front().folder,
            {},
            folderName,
            {},
            {},
            0,
            true
        };
        prop.dbBrowseEntries.push_back({ std::make_shared<EntryChart>(*g_pSongDB->findChartByHash(ArenaData.remoteRequestedChart).begin()), nullptr });

        SelectData.backtrace.front().index = SelectData.selectedEntryIndex;
        SelectData.backtrace.front().displayEntries = SelectData.entries;
        SelectData.backtrace.push_front(prop);
        SelectData.entries.clear();
        SelectData.loadSongList();
        SelectData.sortSongList();

        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(SelectData._mutex);
        SelectData.selectedEntryIndex = 0;
        SelectData.setBarInfo();
        SelectData.setEntryInfo();
        SelectData.setDynamicTextures();

        if (!SelectData.entries.empty())
        {
            SelectData.selectedEntryIndexRolling = (double)SelectData.selectedEntryIndex / SelectData.entries.size();
        }
        else
        {
            SelectData.selectedEntryIndexRolling = 0.0;
        }

        resetJukeboxText();

        scrollAccumulator = 0.;
        scrollAccumulatorAddUnit = 0.;

        SelectData.timers["list_move"] = TIMER_NEVER;
        SelectData.timers["list_entry_change"] = t.norm();
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);

        ArenaData.remoteRequestedChart.reset();
        ArenaData.remoteRequestedPlayer.clear();
        ArenaData.isInArenaRequest = true;
    }
    if (ArenaData.isOnline() && ArenaData.isArenaCancellingRequest)
    {
        ArenaData.isArenaCancellingRequest = false;
        if (ArenaData.isInArenaRequest)
        {
            navigateBack(t);
        }
    }

    if (ArenaData.isOnline() && ArenaData.isArenaReady)
    {
        decide();
    }

    if (ArenaData.isOnline() && ArenaData.isExpired())
    {
        ArenaData.reset();
    }

    if (!SelectData.entries.empty())
    {
        if (!(isHoldingUp || isHoldingDown) && 
            (scrollAccumulator > 0 && scrollAccumulator - scrollAccumulatorAddUnit < 0 ||
                scrollAccumulator < 0 && scrollAccumulator - scrollAccumulatorAddUnit > 0 ||
                -0.000001 < scrollAccumulator && scrollAccumulator < 0.000001 ||
                scrollAccumulator * scrollAccumulatorAddUnit < 0))
        {
            bool scrollModified = false;

            if (SelectData.scrollDirection != 0)
            {
                double posOld = SelectData.selectedEntryIndexRolling;
                double idxOld = posOld * SelectData.entries.size();

                int idxNew = (int)idxOld;
                if (SelectData.scrollDirection > 0)
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
                std::unique_lock<std::shared_mutex> u(SelectData._mutex);

                SelectData.selectedEntryIndexRolling = (double)SelectData.selectedEntryIndex / SelectData.entries.size();
                scrollAccumulator = 0.0;
                scrollAccumulatorAddUnit = 0.0;
                SelectData.scrollDirection = 0;
                pSkin->reset_bar_animation();
                SelectData.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
            }
        }
        else if(SelectData.scrollDirection != 0 || scrollAccumulatorAddUnit < -0.003 || scrollAccumulatorAddUnit > 0.003)
        {
            if (SelectData.scrollDirection == 0)
            {
                std::unique_lock<std::shared_mutex> u(SelectData._mutex);
                pSkin->start_bar_animation();
                SelectData.timers["list_move"] = t.norm();
            }

            double posOld = SelectData.selectedEntryIndexRolling;
            double posNew = posOld + scrollAccumulatorAddUnit / SelectData.entries.size();

            int idxOld = (int)std::round(posOld * SelectData.entries.size());
            int idxNew = (int)std::round(posNew * SelectData.entries.size());
            if (idxOld != idxNew)
            {
                if (idxOld < idxNew)
                    navigateDownBy1(t);
                else
                    navigateUpBy1(t);
            }

            while (posNew < 0.) posNew += 1.;
            while (posNew >= 1.) posNew -= 1.;
            SelectData.selectedEntryIndexRolling = posNew;

            scrollAccumulator -= scrollAccumulatorAddUnit;
            if (scrollAccumulator < -0.000001 || scrollAccumulator > 0.000001)
                SelectData.scrollDirection = scrollAccumulator > 0. ? 1 : -1;
        }
    }

    // load preview
    if ((t - navigateTimestamp).norm() > 500)
    {
        updatePreview();
    }
}

void SceneSelect::updatePrepare()
{
    Time t;
    Time rt = t - SystemData.timers["scene_start"];

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

        SelectData.timers["list_entry_change"] = t.norm();

        // restore panel stat
        for (int i = 0; i < 9; ++i)
        {
            static const std::string panelTimerMap[] =
            {
                "panel1_start",
                "panel2_start",
                "panel3_start",
                "panel4_start",
                "panel5_start",
                "panel6_start",
                "panel7_start",
                "panel8_start",
                "panel9_start",
            };
            if (SelectData.panel[i])
            {
                SelectData.timers[panelTimerMap[i]] = t.norm();
                SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
            }
        }

        LOG_DEBUG << "[Select] State changed to SELECT";
    }
}

void SceneSelect::updateSelect()
{
    Time t;
    Time rt = t - SystemData.timers["scene_start"];

    if (!refreshingSongList)
    {
        int line = 0;
        if (SelectData.panel[0])
        {
            bool isDP = false;
            bool isLocalBattle = false;
            if (PlayData.mode == SkinType::PLAY10 || PlayData.mode == SkinType::PLAY14)
            {
                isDP = true;
            }
            else if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
            {
                isLocalBattle = true;
            }

            if (!pSkin->isSupportGreenNumber)
            {
                std::stringstream ss;
                bool lock1 = PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix != PlayModifierHispeedFixType::NONE;
                if (lock1) 
                    ss << "G(1P): FIX " << ConfigMgr::get('P', cfg::P_GREENNUMBER, 0);

                bool lock2 = PlayData.player[PLAYER_SLOT_TARGET].mods.hispeedFix != PlayModifierHispeedFixType::NONE;
                if (lock2) 
                    ss << (lock1 ? " | " : "") << "G(2P): FIX " << ConfigMgr::get('P', cfg::P_GREENNUMBER_2P, 0);

                std::string s = ss.str();
                if (!s.empty())
                {
                    SystemData.overlayTopLeftText[line++] = s;
                }
            }
            if (!pSkin->isSupportNewRandom && ConfigMgr::get('P', cfg::P_ENABLE_NEW_RANDOM, false))
            {
                std::stringstream ss;
                switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft)
                {
                case PlayModifierRandomType::NONE:           ss << "Random(1P): OFF"; break;
                case PlayModifierRandomType::MIRROR:         ss << "Random(1P): MIRROR"; break;
                case PlayModifierRandomType::RANDOM:         ss << "Random(1P): RANDOM"; break;
                case PlayModifierRandomType::SRAN:           ss << "Random(1P): S-RANDOM"; break;
                case PlayModifierRandomType::HRAN:           ss << "Random(1P): H-RANDOM"; break;
                case PlayModifierRandomType::ALLSCR:         ss << "Random(1P): ALL-SCRATCH"; break;
                case PlayModifierRandomType::RRAN:           ss << "Random(1P): R-RANDOM"; break;
                case PlayModifierRandomType::DB_SYNCHRONIZE: ss << "Random: SYNCHRONIZE RANDOM"; break;
                case PlayModifierRandomType::DB_SYMMETRY:    ss << "Random: SYMMETRY RANDOM"; break;
                }

                if (isDP || isLocalBattle)
                {
                    switch (isLocalBattle ? PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft : PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight)
                    {
                    case PlayModifierRandomType::NONE:                ss << " | Random(2P): OFF"; break;
                    case PlayModifierRandomType::MIRROR:              ss << " | Random(2P): MIRROR"; break;
                    case PlayModifierRandomType::RANDOM:              ss << " | Random(2P): RANDOM"; break;
                    case PlayModifierRandomType::SRAN:                ss << " | Random(2P): S-RANDOM"; break;
                    case PlayModifierRandomType::HRAN:                ss << " | Random(2P): H-RANDOM"; break;
                    case PlayModifierRandomType::ALLSCR:              ss << " | Random(2P): ALL-SCRATCH"; break;
                    case PlayModifierRandomType::RRAN:                ss << " | Random(2P): R-RANDOM"; break;
                    case PlayModifierRandomType::DB_SYNCHRONIZE:
                    case PlayModifierRandomType::DB_SYMMETRY:    break;
                    }
                }

                std::string s = ss.str();
                if (!s.empty())
                {
                    SystemData.overlayTopLeftText[line++] = s;
                }
            }
            if (!pSkin->isSupportExHardAndAssistEasy && ConfigMgr::get('P', cfg::P_ENABLE_NEW_GAUGE, false))
            {
                std::stringstream ss;
                switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge)
                {
                case PlayModifierGaugeType::NORMAL:     ss << "Gauge(1P): NORMAL"; break;
                case PlayModifierGaugeType::HARD:       ss << "Gauge(1P): HARD"; break;
                case PlayModifierGaugeType::DEATH:      ss << "Gauge(1P): DEATH"; break;
                case PlayModifierGaugeType::EASY:       ss << "Gauge(1P): EASY"; break;
                case PlayModifierGaugeType::EXHARD:     ss << "Gauge(1P): EX-HARD"; break;
                case PlayModifierGaugeType::ASSISTEASY: ss << "Gauge(1P): ASSIST EASY"; break;
                }

                if (isLocalBattle)
                {
                    switch (PlayData.player[PLAYER_SLOT_TARGET].mods.gauge)
                    {
                    case PlayModifierGaugeType::NORMAL:     ss << " | Gauge(2P): NORMAL"; break;
                    case PlayModifierGaugeType::HARD:       ss << " | Gauge(2P): HARD"; break;
                    case PlayModifierGaugeType::DEATH:      ss << " | Gauge(2P): DEATH"; break;
                    case PlayModifierGaugeType::EASY:       ss << " | Gauge(2P): EASY"; break;
                    case PlayModifierGaugeType::EXHARD:     ss << " | Gauge(2P): EX-HARD"; break;
                    case PlayModifierGaugeType::ASSISTEASY: ss << " | Gauge(2P): ASSIST EASY"; break;
                    }
                }

                std::string s = ss.str();
                if (!s.empty())
                {
                    SystemData.overlayTopLeftText[line++] = s;
                }
            }
            if (!pSkin->isSupportLift && ConfigMgr::get('P', cfg::P_ENABLE_NEW_LANE_OPTION, false))
            {
                std::stringstream ss;
                switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect)
                {
                case PlayModifierLaneEffectType::OFF:      ss << "Lane(1P): OFF"; break;
                case PlayModifierLaneEffectType::HIDDEN:   ss << "Lane(1P): HIDDEN+"; break;
                case PlayModifierLaneEffectType::SUDDEN:   ss << "Lane(1P): SUDDEN+"; break;
                case PlayModifierLaneEffectType::SUDHID:   ss << "Lane(1P): SUD+ & HID+"; break;
                case PlayModifierLaneEffectType::LIFT:     ss << "Lane(1P): LIFT"; break;
                case PlayModifierLaneEffectType::LIFTSUD:  ss << "Lane(1P): LIFT & SUD+"; break;
                }

                if (isLocalBattle)
                {
                    switch (PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect)
                    {
                    case PlayModifierLaneEffectType::OFF:      ss << " | Lane(2P): OFF"; break;
                    case PlayModifierLaneEffectType::HIDDEN:   ss << " | Lane(2P): HIDDEN+"; break;
                    case PlayModifierLaneEffectType::SUDDEN:   ss << " | Lane(2P): SUDDEN+"; break;
                    case PlayModifierLaneEffectType::SUDHID:   ss << " | Lane(2P): SUD+ & HID+"; break;
                    case PlayModifierLaneEffectType::LIFT:     ss << " | Lane(2P): LIFT"; break;
                    case PlayModifierLaneEffectType::LIFTSUD:  ss << " | Lane(2P): LIFT & SUD+"; break;
                    }
                }

                std::string s = ss.str();
                if (!s.empty())
                {
                    SystemData.overlayTopLeftText[line++] = s;
                }
            }
            if (!pSkin->isSupportHsFixInitialAndMain)
            {
                std::stringstream ss;
                switch (PlayData.player[PLAYER_SLOT_TARGET].mods.hispeedFix)
                {
                case PlayModifierHispeedFixType::NONE:  break;
                case PlayModifierHispeedFixType::MINBPM: ss << "HiSpeed Fix: Min BPM"; break;
                case PlayModifierHispeedFixType::MAXBPM: ss << "HiSpeed Fix: Max BPM"; break;
                case PlayModifierHispeedFixType::AVERAGE: ss << "HiSpeed Fix: Average BPM"; break;
                case PlayModifierHispeedFixType::CONSTANT: ss << "HiSpeed Fix: *CONSTANT*"; break;
                case PlayModifierHispeedFixType::INITIAL: ss << "HiSpeed Fix: Start BPM"; break;
                case PlayModifierHispeedFixType::MAIN: ss << "HiSpeed Fix: Main BPM"; break;
                }

                std::string s = ss.str();
                if (!s.empty())
                {
                    SystemData.overlayTopLeftText[line++] = s;
                }
            }
        }
        while (line <= sizeof(SystemData.overlayTopLeftText) / sizeof(SystemData.overlayTopLeftText[0]))
            SystemData.overlayTopLeftText[line++] = "";
    }

    if (SelectData.isGoingToKeyConfig || SelectData.isGoingToSkinSelect || SelectData.isGoingToReboot)
    {
        SoundMgr::setSysVolume(0.0, 500);
        SystemData.timers["fadeout_start"] = t.norm();
        state = eSelectState::FADEOUT;
        _updateCallback = std::bind(&SceneSelect::updateFadeout, this);
    }
    else if (SelectData.isGoingToAutoPlay)
    {
        SelectData.isGoingToAutoPlay = false;
        if (!SelectData.entries.empty())
        {
            switch (SelectData.entries[SelectData.selectedEntryIndex].first->type())
            {
            case eEntryType::SONG:
            case eEntryType::CHART:
            case eEntryType::RIVAL_SONG:
            case eEntryType::RIVAL_CHART:
            case eEntryType::COURSE:
                PlayData.isAuto = true;
                decide();
                break;
            }
        }
    }
    else if (SelectData.isGoingToReplay)
    {
        SelectData.isGoingToReplay = false;
        if (!SelectData.entries.empty())
        {
            switch (SelectData.entries[SelectData.selectedEntryIndex].first->type())
            {
            case eEntryType::SONG:
            case eEntryType::CHART:
            case eEntryType::RIVAL_SONG:
            case eEntryType::RIVAL_CHART:
            case eEntryType::COURSE:
            {
                if (PlayData.replay != nullptr)
                {
                    PlayData.isReplay = true;
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
    Time rt = t - SystemData.timers["scene_start"];
}

void SceneSelect::updatePanel(unsigned idx)
{
    Time t;
    Time rt = t - SystemData.timers["scene_start"];
}

void SceneSelect::updateFadeout()
{
    Time t;
    Time rt = t - SystemData.timers["scene_start"];
    Time ft = t - SystemData.timers["fadeout_start"];

    if (ft >= pSkin->info.timeOutro)
    {
        if (SelectData.isGoingToKeyConfig)
        {
            SoundMgr::stopSysSamples();
            SystemData.gNextScene = SceneType::KEYCONFIG;
        }
        else if (SelectData.isGoingToSkinSelect)
        {
            //SoundMgr::stopSysSamples();
            //SystemData.gNextScene = SceneType::CUSTOMIZE;
            //LR2CustomizeData.isInCustomize = true;
        }
        else if (SelectData.isGoingToReboot)
        {
            SoundMgr::stopSysSamples();
            SystemData.gNextScene = SceneType::PRE_SELECT;
        }
        else
        {
            SoundMgr::stopSysSamples();
            SystemData.gNextScene = SceneType::EXIT_TRANS;
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
    Time rt = t - SystemData.timers["scene_start"];

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
    //        SystemData.gNextScene = SceneType::EXIT_TRANS;
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
        if (SelectData.panel[0])
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
                PlayData.timers[InputGamePressMap.at(Input::Pad(k))] = t.norm();
                PlayData.timers[InputGameReleaseMap.at(Input::Pad(k))] = TIMER_NEVER;
            }
        }
        for (size_t k = Pad::K21; k <= Pad::K29; ++k)
        {
            if (input[k])
            {
                if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
                {
                    PlayData.timers[InputGamePressMap.at(Input::Pad(k))] = t.norm();
                    PlayData.timers[InputGameReleaseMap.at(Input::Pad(k))] = TIMER_NEVER;
                }
                else
                {
                    PlayData.timers[InputGamePressMap.at(Input::Pad(k - Pad::K21 + Pad::K11))] = t.norm();
                    PlayData.timers[InputGameReleaseMap.at(Input::Pad(k - Pad::K21 + Pad::K11))] = TIMER_NEVER;
                }
            }
        }
    }
}

// CALLBACK
void SceneSelect::inputGameHold(InputMask& m, const Time& t)
{
    Time rt = t - SystemData.timers["scene_start"];

    if (rt.norm() < pSkin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // sub callbacks
        if (SelectData.panel[0])
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
    Time rt = t - SystemData.timers["scene_start"];

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
        if (SelectData.panel[0])
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
                PlayData.timers[InputGamePressMap.at(Input::Pad(k))] = TIMER_NEVER;
                PlayData.timers[InputGameReleaseMap.at(Input::Pad(k))] = t.norm();
            }
        }
        for (size_t k = Pad::K21; k <= Pad::K29; ++k)
        {
            if (input[k])
            {
                if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
                {
                    PlayData.timers[InputGamePressMap.at(Input::Pad(k))] = TIMER_NEVER;
                    PlayData.timers[InputGameReleaseMap.at(Input::Pad(k))] = t.norm();
                }
                else
                {
                    PlayData.timers[InputGamePressMap.at(Input::Pad(k - Pad::K21 + Pad::K11))] = TIMER_NEVER;
                    PlayData.timers[InputGameReleaseMap.at(Input::Pad(k - Pad::K21 + Pad::K11))] = t.norm();
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

        if (SelectData.backtrace.size() >= 2)
        {
            // only update current folder
            const auto [hasPath, path] = g_pSongDB->getFolderPath(SelectData.backtrace.front().folder);
            if (hasPath)
            {
                LOG_INFO << "[List] Refreshing folder " << path.u8string();
                SystemData.overlayTopLeftText[0] = (boost::format(i18n::c(i18nText::REFRESH_FOLDER)) % path.u8string()).str();

                g_pSongDB->resetAddSummary();
                int count = g_pSongDB->addSubFolder(path, SelectData.backtrace.front().parent);
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

                SystemData.overlayTopLeftText[0] = "";
                SystemData.overlayTopLeftText[1] = "";
            }

            // re-browse
            if (!isInVersionList)
                selectDownTimestamp = -1;

            // simplified navigateBack(t)
            {
                std::unique_lock<std::shared_mutex> u(SelectData._mutex);
                auto& top = SelectData.backtrace.front();

                SelectData.selectedEntryIndex = 0;
                SelectData.backtrace.pop_front();
                auto& parent = SelectData.backtrace.front();
                SelectData.entries = parent.displayEntries;
                SelectData.selectedEntryIndex = parent.index;

                if (parent.ignoreFilters)
                {
                    // change display only
                    SelectData.filterDifficulty = FilterDifficultyType::All;
                    SelectData.filterKeys = FilterKeysType::All;
                }
                else
                {
                    // restore prev
                    cfg::loadFilterDifficulty();
                    cfg::loadFilterKeys();
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
                std::unique_lock<std::shared_mutex> u(SelectData._mutex);
                SelectData.loadSongList();
                SelectData.sortSongList();

                SelectData.selectedEntryIndex = 0;
                SelectData.setBarInfo();
                SelectData.setEntryInfo();
                SelectData.setDynamicTextures();

                resetJukeboxText();
            }

            SelectData.timers["list_entry_change"] = t.norm();
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
            SelectData.panel[0] = true;
            SelectData.timers["panel1_start"] = t.norm();
            SelectData.timers["panel1_end"] = TIMER_NEVER;
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
            return;
        }
        if (input[Input::M2])
        {
            bool hasPanelOpened = false;
            for (int i = 0; i < 9; ++i)
            {
                if (SelectData.panel[i])
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
        if (selectDownTimestamp == -1 && (input[Input::Pad::K1SELECT] || input[Input::Pad::K2SELECT]) && !SelectData.entries.empty())
        {
            switch (SelectData.entries[SelectData.selectedEntryIndex].first->type())
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
    if (!SelectData.entries.empty())
    {
        if (bindings9K && (input & INPUT_MASK_DECIDE_9K).any() || !bindings9K && (input & INPUT_MASK_DECIDE).any())
        {
            switch (SelectData.entries[SelectData.selectedEntryIndex].first->type())
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
            scrollAccumulatorAddUnit = scrollAccumulator / SelectData.scrollTimeLength * (1000.0 / getRate());
        }
        if (bindings9K && (input & INPUT_MASK_NAV_DN_9K).any() || !bindings9K && (input & INPUT_MASK_NAV_DN).any())
        {
            isHoldingDown = true;
            scrollAccumulator += 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / SelectData.scrollTimeLength * (1000.0 / getRate());
        }

        if (input[Input::MWHEELUP])
        {
            if (scrollAccumulator != 0.0)
            {
                SelectData.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
            }
            scrollAccumulator -= 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / SelectData.scrollTimeLength * (1000.0 / getRate());
        }
        if (input[Input::MWHEELDOWN])
        {
            if (scrollAccumulator != 0.0)
            {
                SelectData.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
            }
            scrollAccumulator += 1.0;
            scrollButtonTimestamp = t;
            scrollAccumulatorAddUnit = scrollAccumulator / SelectData.scrollTimeLength * (1000.0 / getRate());
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
    if (isHoldingUp && (t - scrollButtonTimestamp).norm() >= SelectData.scrollTimeLength)
    {
        SelectData.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
        scrollButtonTimestamp = t;
        scrollAccumulator -= 1.0;
        scrollAccumulatorAddUnit = scrollAccumulator / SelectData.scrollTimeLength * (1000.0 / getRate());
    }
    if (isHoldingDown && (t - scrollButtonTimestamp).norm() >= SelectData.scrollTimeLength)
    {
        SelectData.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
        scrollButtonTimestamp = t;
        scrollAccumulator += 1.0;
        scrollAccumulatorAddUnit = scrollAccumulator / SelectData.scrollTimeLength * (1000.0 / getRate());
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
                std::unique_lock l(SelectData._mutex);
                SelectData.switchVersion(0);
                SelectData.setBarInfo();
                SelectData.setEntryInfo();
                SelectData.setDynamicTextures();

                SelectData.optionChangePending = true;
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
        if (SelectData.panel[0])
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
                if (PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix == PlayModifierHispeedFixType::NONE)
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

            if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
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
                    // FIXME load 2p hsfix type
                    if (PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix == PlayModifierHispeedFixType::NONE)
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
            else if (PlayData.mode == SkinType::PLAY10 || PlayData.mode == SkinType::PLAY14)
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
                    if (PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix == PlayModifierHispeedFixType::NONE)
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
                    if (PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix == PlayModifierHispeedFixType::NONE)
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
    if (SelectData.panel[0] && (input[Input::Pad::K1START] || input[Input::Pad::K2START]))
    {
        // close panel 1
        SelectData.panel[0] = false;
        SelectData.timers["panel1_start"] = TIMER_NEVER;
        SelectData.timers["panel1_end"] = t.norm();
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
        return;
    }
}


void SceneSelect::decide()
{
    std::shared_lock<std::shared_mutex> u(SelectData._mutex);

    auto [entry, score] = SelectData.getCurrentEntry();
    if (entry == nullptr)
        return;

    if (ArenaData.isOnline())
    {
        if (!ArenaData.isArenaReady)
        {
            if (entry->type() == CHART)
            {
                if (ArenaData.isClient())
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

    if (entry->type() == COURSE && !SelectData.coursePlayable)
        return;

    PlayData.clearContextPlay();

    PlayData.canRetry = 
        !PlayData.isAuto && 
        !PlayData.isReplay && 
        entry->type() != eEntryType::COURSE && 
        !ArenaData.isOnline();


    if (PlayData.battleType == PlayModifierBattleType::GhostBattle && PlayData.replay != nullptr)
        PlayData.battleType = PlayModifierBattleType::Off;

    if (entry->type() == eEntryType::COURSE && PlayData.battleType != PlayModifierBattleType::LocalBattle)
    {
        PlayData.battleType = PlayModifierBattleType::Off;
        PlayData.courseStage = 0;
    }
    else
    {
        PlayData.courseStage = -1;
    }

    // chart
    PlayData.playStarted = false;
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
            SelectData.selectedChart.chart = pFile;
        }
        else
        {
            auto pFile = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
            SelectData.selectedChart.chart = pFile;
        }

        auto& chart = *SelectData.selectedChart.chart;
        //SelectData.selectedChart.path = chart._filePath;
        SelectData.selectedChart.path = chart.absolutePath;

        // only reload resources if selected chart is different
        SelectData.selectedChart.hash = chart.fileHash;
        if (SelectData.selectedChart.hash != SelectData.selectedChart.sampleLoadedHash)
        {
            SelectData.selectedChart.isSampleLoaded = false;
            SelectData.selectedChart.sampleLoadedHash.reset();
        }
        if (SelectData.selectedChart.hash != SelectData.selectedChart.bgaLoadedHash)
        {
            SelectData.selectedChart.isBgaLoaded = false;
            SelectData.selectedChart.bgaLoadedHash.reset();
        }

        //SelectData.selectedChart.chart = std::make_shared<ChartFormatBase>(chart);
        SelectData.selectedChart.title = chart.title;
        SelectData.selectedChart.title2 = chart.title2;
        SelectData.selectedChart.artist = chart.artist;
        SelectData.selectedChart.artist2 = chart.artist2;
        SelectData.selectedChart.genre = chart.genre;
        SelectData.selectedChart.version = chart.version;
        SelectData.selectedChart.level = chart.levelEstimated;
        SelectData.selectedChart.minBPM = chart.minBPM;
        SelectData.selectedChart.maxBPM = chart.maxBPM;
        SelectData.selectedChart.startBPM = chart.startBPM;


        // set gamemode
        SelectData.selectedChart.isDoubleBattle = false;
        if (SelectData.selectedChart.chart->type() == eChartFormat::BMS)
        {
            auto pBMS = std::reinterpret_pointer_cast<ChartFormatBMSMeta>(SelectData.selectedChart.chart);
            switch (pBMS->gamemode)
            {
            case 5:  PlayData.mode = SkinType::PLAY5;  break;
            case 7:  PlayData.mode = SkinType::PLAY7;  break;
            case 9:  PlayData.mode = SkinType::PLAY9;  break;
            case 10: PlayData.mode = SkinType::PLAY10; break;
            case 14: PlayData.mode = SkinType::PLAY14; break;
            default: PlayData.mode = SkinType::PLAY7;  break;
            }
            if (PlayData.battleType == PlayModifierBattleType::LocalBattle || 
                PlayData.battleType == PlayModifierBattleType::GhostBattle)
            {
                switch (pBMS->gamemode)
                {
                case 5:  PlayData.mode = SkinType::PLAY5_2;  break;
                case 7:  PlayData.mode = SkinType::PLAY7_2;  break;
                case 9:  PlayData.mode = SkinType::PLAY9;  PlayData.battleType = PlayModifierBattleType::Off; break;
                case 10: PlayData.mode = SkinType::PLAY10; PlayData.battleType = PlayModifierBattleType::Off; break;
                case 14: PlayData.mode = SkinType::PLAY14; PlayData.battleType = PlayModifierBattleType::Off; break;
                default: assert(false); break;
                }
            }
            else if (PlayData.battleType == PlayModifierBattleType::DoubleBattle)
            {
                switch (pBMS->gamemode)
                {
                case 5:  PlayData.mode = SkinType::PLAY10;  break;
                case 7:  PlayData.mode = SkinType::PLAY14;  break;
                default: assert(false); break;
                }
                SelectData.selectedChart.isDoubleBattle = true;
            }
        }

        break;
    }
    case eEntryType::COURSE:
    {
        if (std::dynamic_pointer_cast<EntryCourse>(entry)->courseType == EntryCourse::CourseType::CLASS)
        {
            // reset mods
            static const std::set<PlayModifierGaugeType> courseGaugeModsAllowed = { PlayModifierGaugeType::NORMAL , PlayModifierGaugeType::HARD };
            if (courseGaugeModsAllowed.find(PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge) == courseGaugeModsAllowed.end())
            {
                PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge = PlayModifierGaugeType::NORMAL;
            }
            if (courseGaugeModsAllowed.find(PlayData.player[PLAYER_SLOT_TARGET].mods.gauge) == courseGaugeModsAllowed.end())
            {
                PlayData.player[PLAYER_SLOT_TARGET].mods.gauge = PlayModifierGaugeType::NORMAL;
            }
            static const std::set<PlayModifierRandomType> courseChartModsAllowed = { PlayModifierRandomType::NONE , PlayModifierRandomType::MIRROR };
            if (courseChartModsAllowed.find(PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft) == courseChartModsAllowed.end())
            {
                PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft = PlayModifierRandomType::NONE;
            }
            if (courseChartModsAllowed.find(PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft) == courseChartModsAllowed.end())
            {
                PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft = PlayModifierRandomType::NONE;
            }
            PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask = 0;
            PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask = 0;
        }

        // set metadata
        auto pCourse = std::dynamic_pointer_cast<EntryCourse>(entry);
        PlayData.courseHash = pCourse->md5;
        PlayData.courseStageData.clear();
        PlayData.courseStageData.resize(pCourse->charts.size());
        for (size_t i = 0; i < pCourse->charts.size(); i++)
        {
            PlayData.courseStageData[i].hash = pCourse->charts[i];
        }

        auto pChart = *g_pSongDB->findChartByHash(*pCourse->charts.begin()).begin();
        SelectData.selectedChart.chart = pChart;

        auto& chart = *SelectData.selectedChart.chart;
        //SelectData.selectedChart.path = chart._filePath;
        SelectData.selectedChart.path = chart.absolutePath;

        // only reload resources if selected chart is different
        SelectData.selectedChart.hash = chart.fileHash;
        if (SelectData.selectedChart.hash != SelectData.selectedChart.sampleLoadedHash)
        {
            SelectData.selectedChart.isSampleLoaded = false;
            SelectData.selectedChart.sampleLoadedHash.reset();
        }
        if (SelectData.selectedChart.hash != SelectData.selectedChart.bgaLoadedHash)
        {
            SelectData.selectedChart.isBgaLoaded = false;
            SelectData.selectedChart.bgaLoadedHash.reset();
        }

        //SelectData.selectedChart.chart = std::make_shared<ChartFormatBase>(chart);
        SelectData.selectedChart.title = chart.title;
        SelectData.selectedChart.title2 = chart.title2;
        SelectData.selectedChart.artist = chart.artist;
        SelectData.selectedChart.artist2 = chart.artist2;
        SelectData.selectedChart.genre = chart.genre;
        SelectData.selectedChart.version = chart.version;
        SelectData.selectedChart.level = chart.levelEstimated;
        SelectData.selectedChart.minBPM = chart.minBPM;
        SelectData.selectedChart.maxBPM = chart.maxBPM;
        SelectData.selectedChart.startBPM = chart.startBPM;

        break;
    }
    default:
        break;
    }

    if (!PlayData.isReplay)
    {
        switch (PlayData.battleType)
        {
        case PlayModifierBattleType::Off:
            // copy 1P setting for target
            PlayData.player[PLAYER_SLOT_TARGET].mods.gauge = PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge;
            PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft = PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft;
            PlayData.player[PLAYER_SLOT_TARGET].mods.randomRight = PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight;
            PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask = 0;  // rival do not use assist options
            break;

        case PlayModifierBattleType::GhostBattle:
            // replay notes are loaded in 2P area, we should check randomRight instead of randomLeft
            // FIXME ^ ?
            PlayData.player[PLAYER_SLOT_TARGET].mods.gauge = PlayData.replay->gaugeType;
            PlayData.player[PLAYER_SLOT_TARGET].mods.randomRight = PlayData.replay->randomTypeLeft;
            PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask = PlayData.replay->assistMask;
            PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect = (PlayModifierLaneEffectType)PlayData.replay->laneEffectType;
            break;
        }
    }
    else // PlayData.isReplay
    {
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft = PlayData.replay->randomTypeLeft;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight = PlayData.replay->randomTypeRight;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge = PlayData.replay->gaugeType;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask = PlayData.replay->assistMask;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix = PlayData.replay->hispeedFix;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect = (PlayModifierLaneEffectType)PlayData.replay->laneEffectType;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip = PlayData.replay->DPFlip;
    }
    
    // score (mybest)
    if (PlayData.battleType != PlayModifierBattleType::LocalBattle && PlayData.battleType != PlayModifierBattleType::DoubleBattle)
    {
        auto pScore = score;
        if (pScore == nullptr)
        {
            pScore = g_pScoreDB->getChartScoreBMS(SelectData.selectedChart.hash);
        }
        if (pScore && !pScore->replayFileName.empty())
        {
            Path replayFilePath;
            if ((entry->type() == eEntryType::CHART || entry->type() == eEntryType::RIVAL_CHART))
            {
                replayFilePath = ReplayChart::getReplayPath(SelectData.selectedChart.hash) / pScore->replayFileName;
            }
            else if (entry->type() == eEntryType::COURSE && !PlayData.courseStageData.empty())
            {
                auto pScoreStage1 = g_pScoreDB->getChartScoreBMS(PlayData.courseStageData[0].hash);
                if (pScoreStage1 && !pScoreStage1->replayFileName.empty())
                {
                    replayFilePath = ReplayChart::getReplayPath(PlayData.courseStageData[0].hash) / pScoreStage1->replayFileName;
                }
            }
            if (!replayFilePath.empty() && fs::is_regular_file(replayFilePath))
            {
                PlayData.replayMybest = std::make_shared<ReplayChart>();
                if (PlayData.replayMybest->loadFile(replayFilePath))
                {
                    PlayData.player[PLAYER_SLOT_MYBEST].mods.randomLeft = PlayData.replayMybest->randomTypeLeft;
                    PlayData.player[PLAYER_SLOT_MYBEST].mods.randomRight = PlayData.replayMybest->randomTypeRight;
                    PlayData.player[PLAYER_SLOT_MYBEST].mods.gauge = PlayData.replayMybest->gaugeType;
                    PlayData.player[PLAYER_SLOT_MYBEST].mods.assist_mask = PlayData.replayMybest->assistMask;
                    PlayData.player[PLAYER_SLOT_MYBEST].mods.hispeedFix = PlayData.replayMybest->hispeedFix;
                    PlayData.player[PLAYER_SLOT_MYBEST].mods.laneEffect = (PlayModifierLaneEffectType)PlayData.replayMybest->laneEffectType;
                    PlayData.player[PLAYER_SLOT_MYBEST].mods.DPFlip = PlayData.replayMybest->DPFlip;
                }
                else
                {
                    PlayData.replayMybest.reset();
                }
            }
        }
    }

    SystemData.gNextScene = SceneType::DECIDE;
}

void SceneSelect::navigateUpBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!SelectData.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(SelectData._mutex);
        SelectData.selectedEntryIndex = (SelectData.entries.size() + SelectData.selectedEntryIndex - 1) % SelectData.entries.size();
        SelectData.setBarInfo();
        SelectData.setEntryInfo();
        SelectData.setDynamicTextures();

        SelectData.timers["list_entry_change"] = t.norm();
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void SceneSelect::navigateDownBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!SelectData.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(SelectData._mutex);
        SelectData.selectedEntryIndex = (SelectData.selectedEntryIndex + 1) % SelectData.entries.size();
        SelectData.setBarInfo();
        SelectData.setEntryInfo();
        SelectData.setDynamicTextures();

        SelectData.timers["list_entry_change"] = t.norm();
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void SceneSelect::navigateEnter(const Time& t)
{
    if (!SelectData.entries.empty())
    {
        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(SelectData._mutex);

        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() == eEntryType::FOLDER)
        {
            const auto& [e, s] = SelectData.entries[SelectData.selectedEntryIndex];

            SongListProperties prop{
                SelectData.backtrace.front().folder,
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

            SelectData.backtrace.front().index = SelectData.selectedEntryIndex;
            SelectData.backtrace.front().displayEntries = SelectData.entries;
            SelectData.backtrace.push_front(prop);
            SelectData.entries.clear();

            cfg::loadFilterDifficulty();
            cfg::loadFilterKeys();
            SelectData.loadSongList();
            if (SelectData.entries.empty())
            {
                SelectData.filterDifficulty = FilterDifficultyType::All;
                SelectData.loadSongList();
            }
            if (SelectData.entries.empty())
            {
                SelectData.filterKeys = FilterKeysType::All;
                SelectData.loadSongList();
            }
        }
        else if (auto top = std::dynamic_pointer_cast<EntryFolderBase>(SelectData.entries[SelectData.selectedEntryIndex].first); top)
        {
            auto folderType = SelectData.entries[SelectData.selectedEntryIndex].first->type();
            const auto& [e, s] = SelectData.entries[SelectData.selectedEntryIndex];

            SongListProperties prop{
                SelectData.backtrace.front().folder,
                e->md5,
                e->_name,
                {},
                {},
                0,
                folderType == eEntryType::CUSTOM_FOLDER || folderType == eEntryType::RIVAL
            };
            for (size_t i = 0; i < top->getContentsCount(); ++i)
                prop.dbBrowseEntries.push_back({ top->getEntry(i), nullptr });

            SelectData.backtrace.front().index = SelectData.selectedEntryIndex;
            SelectData.backtrace.front().displayEntries = SelectData.entries;
            SelectData.backtrace.push_front(prop);
            SelectData.entries.clear();

            cfg::loadFilterDifficulty();
            cfg::loadFilterKeys();
            SelectData.loadSongList();
        }

        SelectData.sortSongList();
        SelectData.selectedEntryIndex = 0;

        SelectData.setBarInfo();
        SelectData.setEntryInfo();
        SelectData.setDynamicTextures();

        if (!SelectData.entries.empty())
        {
            SelectData.selectedEntryIndexRolling = (double)SelectData.selectedEntryIndex / SelectData.entries.size();
        }
        else
        {
            SelectData.selectedEntryIndexRolling = 0.0;
        }

        resetJukeboxText();

        scrollAccumulator = 0.;
        scrollAccumulatorAddUnit = 0.;

        SelectData.timers["list_move"] = TIMER_NEVER;
        SelectData.timers["list_entry_change"] = t.norm();
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
    }
}
void SceneSelect::navigateBack(const Time& t, bool sound)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (SelectData.backtrace.size() >= 2)
    {
        navigateTimestamp = t;
        postStopPreview();

        std::unique_lock<std::shared_mutex> u(SelectData._mutex);

        if (ArenaData.isOnline())
        {
            if (ArenaData.isClient())
                g_pArenaClient->requestChart(HashMD5());
            else
                g_pArenaHost->requestChart(HashMD5(), "host");

            ArenaData.isInArenaRequest = false;
        }

        auto& top = SelectData.backtrace.front();

        SelectData.selectedEntryIndex = 0;
        SelectData.backtrace.pop_front();
        auto& parent = SelectData.backtrace.front();
        SelectData.entries = parent.displayEntries;
        SelectData.selectedEntryIndex = parent.index;

        if (parent.ignoreFilters)
        {
            // change display only
            SelectData.filterDifficulty = FilterDifficultyType::All;
            SelectData.filterKeys = FilterKeysType::All;
        }
        else
        {
            // restore prev
            cfg::loadFilterDifficulty();
            cfg::loadFilterKeys();
        }

        SelectData.setBarInfo();
        SelectData.setEntryInfo();
        SelectData.setDynamicTextures();

        if (!SelectData.entries.empty())
        {
            SelectData.selectedEntryIndexRolling = (double)SelectData.selectedEntryIndex / SelectData.entries.size();
        }
        else
        {
            SelectData.selectedEntryIndexRolling = 0.0;
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
    for (int i = 0; i < 9; ++i)
    {
        if (SelectData.panel[i])
        {
            hasPanelOpened = true;
            SelectData.panel[i] = false;
            SelectData.timers["panel1_start"] = TIMER_NEVER;
            SelectData.timers["panel1_end"] = t.norm();
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
            startTextEdit(true);
            return true;
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
        stopTextEdit(true);
        searchSong(SelectData.jukeboxName);
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
    if (SelectData.backtrace.front().name.empty())
        SelectData.jukeboxName = i18n::s(i18nText::SEARCH_SONG);
    else
        SelectData.jukeboxName = SelectData.backtrace.front().name;
}

void SceneSelect::searchSong(const std::string& text)
{
    LOG_DEBUG << "Search: " << text;

    auto top = g_pSongDB->search(ROOT_FOLDER_HASH, text);
    if (!top || top->empty())
    {
        SelectData.jukeboxName = i18n::s(i18nText::SEARCH_FAILED);
        return;
    }

    std::unique_lock<std::shared_mutex> u(SelectData._mutex);

    std::string name = (boost::format(i18n::c(i18nText::SEARCH_RESULT)) % text % top->getContentsCount()).str();
    SongListProperties prop{
        {},
        {},
        name,
        {},
        {},
        0
    };
    for (size_t i = 0; i < top->getContentsCount(); ++i)
        prop.dbBrowseEntries.push_back({ top->getEntry(i), nullptr });

    SelectData.backtrace.front().index = SelectData.selectedEntryIndex;
    SelectData.backtrace.front().displayEntries = SelectData.entries;
    SelectData.backtrace.push_front(prop);
    SelectData.entries.clear();
    SelectData.loadSongList();
    SelectData.sortSongList();

    navigateTimestamp = Time();
    postStopPreview();

    SelectData.selectedEntryIndex = 0;
    SelectData.setBarInfo();
    SelectData.setEntryInfo();
    SelectData.setDynamicTextures();

    if (!SelectData.entries.empty())
    {
        SelectData.selectedEntryIndexRolling = (double)SelectData.selectedEntryIndex / SelectData.entries.size();
    }
    else
    {
        SelectData.selectedEntryIndexRolling = 0.0;
    }

    resetJukeboxText();

    scrollAccumulator = 0.;
    scrollAccumulatorAddUnit = 0.;

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
}

void SceneSelect::updatePreview()
{
    const EntryList& e = SelectData.entries;
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
            std::shared_lock<std::shared_mutex> s(SelectData._mutex);
            if (!SelectData.entries.empty())
            {
                const auto& entry = SelectData.entries[SelectData.selectedEntryIndex].first;
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
                    previewChartTmp = ChartFormatBase::createFromFile(previewChartPath, PlayData.randomSeed);
                }
                else
                {
                    previewChartTmp = ChartFormatBase::createFromFile(previewChartPath, PlayData.randomSeed);
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

                SelectData.selectedChart.isSampleLoaded = false;
                SelectData.selectedChart.sampleLoadedHash.reset();

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

                        SelectData.selectedChart.isSampleLoaded = true;
                        SelectData.selectedChart.sampleLoadedHash = bms->fileHash;

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
        else if (SelectData.selectedChart.isSampleLoaded)
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
    if (auto p = std::dynamic_pointer_cast<EntryArenaCommand>(SelectData.entries[SelectData.selectedEntryIndex].first); p)
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
    if (ArenaData.isOnline())
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
        ArenaData.timers["show_lobby"] = t.norm();

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
    if (!ArenaData.isOnline())
    {
        createNotification(i18n::s(i18nText::ARENA_LEAVE_FAILED_NO_LOBBY));
        return;
    }

    if (ArenaData.isClient())
    {
        g_pArenaClient->leaveLobby();
    }
    if (ArenaData.isServer())
    {
        g_pArenaHost->disbandLobby();
    }

    Time t;
    navigateBack(t, false);
    ArenaData.timers["show_lobby"] = TIMER_NEVER;

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_F_OPEN);
}

void SceneSelect::arenaJoinLobbyPrompt()
{
    if (ArenaData.isOnline())
    {
        createNotification(i18n::s(i18nText::ARENA_JOIN_FAILED_IN_LOBBY));
        return;
    }

    imgui_show_arenaJoinLobbyPrompt = true;
    pSkin->setHandleMouseEvents(false);
}

void SceneSelect::arenaJoinLobby()
{
    assert(!ArenaData.isOnline());

    std::string address = imgui_arena_address_buf;
    createNotification((boost::format(i18n::c(i18nText::ARENA_JOINING)) % address).str());

    g_pArenaClient = std::make_shared<ArenaClient>();
    if (g_pArenaClient->joinLobby(address))
    {
        g_pArenaClient->loopStart();
        createNotification(i18n::s(i18nText::ARENA_JOIN_SUCCESS));

        Time t;
        navigateBack(t, false);
        ArenaData.timers["show_lobby"] = t.norm();

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

}
