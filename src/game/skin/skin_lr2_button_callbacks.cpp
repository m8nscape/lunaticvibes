#include "common/pch.h"


#include "skin_lr2_button_callbacks.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/data/data_types.h"
#include "game/graphics/graphics.h"

#include "config/config_mgr.h"

namespace lunaticvibes
{

namespace lr2skin::button
{

#pragma region helpers

void update_fx(int type, int index, SampleChannel ch, float p1, float p2)
{
    switch (type)
    {
    case 0: // OFF
        SoundMgr::setDSP(DSPType::OFF, index, ch, 0.f, 0.f);
        break;
    case 1: // REVERB
        SoundMgr::setDSP(DSPType::REVERB, index, ch, p1, p2);
        break;
    case 2: // DELAY
        SoundMgr::setDSP(DSPType::DELAY, index, ch, p1, p2);
        break;
    case 3: // LOWPASS
        SoundMgr::setDSP(DSPType::LOWPASS, index, ch, p1, p2);
        break;
    case 4: // HIGHPASS
        SoundMgr::setDSP(DSPType::HIGHPASS, index, ch, p1, p2);
        break;
    case 5: // FLANGER
        SoundMgr::setDSP(DSPType::FLANGER, index, ch, p1, p2);
        break;
    case 6: // CHORUS
        SoundMgr::setDSP(DSPType::CHORUS, index, ch, p1, p2);
        break;
    case 7: // DISTORTION
        SoundMgr::setDSP(DSPType::DISTORTION, index, ch, p1, p2);
        break;
    default:
        break;
    }
}

template <typename T>
void number_change(T &val, T plus)
{
    val += plus;

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

template <typename T>
void number_change_clamp(T& val, T min, T max, T plus)
{
    val = std::clamp(val + plus, min, max);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

#pragma endregion

#pragma region button type callbacks

// 1 - 9
void panel_switch(int idx, int plus)
{
    if (idx < 0 || idx >= 9) return;
    Time t{};

    static const std::unordered_map<int, std::pair<std::string, std::string>> panelTimerMap =
    {
        { 0, { "panel1_start", "panel1_end" } },
        { 1, { "panel2_start", "panel2_end" } },
        { 2, { "panel3_start", "panel3_end" } },
        { 3, { "panel4_start", "panel4_end" } },
        { 4, { "panel5_start", "panel5_end" } },
        { 5, { "panel6_start", "panel6_end" } },
        { 6, { "panel7_start", "panel7_end" } },
        { 7, { "panel8_start", "panel8_end" } },
        { 8, { "panel9_start", "panel9_end" } },
    };

    // close other panels
    for (int i = 0; i < 9; ++i)
    {
        if (i == idx) continue;
        if (SelectData.panel[i])
        {
            SelectData.panel[i] = false;
            SelectData.timers[panelTimerMap.at(i).first] = TIMER_NEVER;
            SelectData.timers[panelTimerMap.at(i).second] = t.norm();
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
        }
    }

    if (SelectData.panel[idx])
    {
        // close panel
        SelectData.panel[idx] = false;
        SelectData.timers[panelTimerMap.at(idx).first] = TIMER_NEVER;
        SelectData.timers[panelTimerMap.at(idx).second] = t.norm();
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
    }
    else
    {
        // open panel
        SelectData.panel[idx] = true;
        SelectData.timers[panelTimerMap.at(idx).first] = t.norm();
        SelectData.timers[panelTimerMap.at(idx).second] = TIMER_NEVER;
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
    }
}

// 10
void select_difficulty_filter(int plus, int iterateCount)
{
    auto p = SelectData.songList.getCurrentList();
    if (p && !p->ignoreFilters && iterateCount < 6)
    {
        FilterDifficultyType val = FilterDifficultyType(((int)SelectData.filterDifficulty + 6 + plus) % 6);
        if (val == FilterDifficultyType::All && ConfigMgr::get("P", cfg::P_DISABLE_DIFFICULTY_ALL, false))
        {
            val = FilterDifficultyType::B;
            iterateCount++;
        }

        SelectData.filterDifficulty = val;

        SelectData.updateSongList(true);
        if (p->displayEntries.empty())
        {
            return select_difficulty_filter(plus, iterateCount + 1);
        }
    }

    SelectData.optionChangePending = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 11
void select_keys_filter(int plus, int iterateCount)
{
    auto p = SelectData.songList.getCurrentList();
    if (p && !p->ignoreFilters && iterateCount < 8)
    {
        FilterKeysType val = FilterKeysType(((int)SelectData.filterKeys + 8 + plus) % 8);
        if (val == FilterKeysType::All && ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_ALL, false))
        {
            val = FilterKeysType::_5;
            iterateCount++;
        }
        if (val == FilterKeysType::Single && ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_SINGLE, false))
        {
            val = FilterKeysType::Double;
            iterateCount++;
        }
        if (val == FilterKeysType::Double && ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_DOUBLE, false))
        {
            val = FilterKeysType::All;
            iterateCount++;
        }

        SelectData.filterKeys = val;

        if (!p->displayEntries.empty())
        {
            PlayData.battleType = PlayModifierBattleType::Off;
            PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip = false;
            SelectData.updateSongList(true);
        }
        else
        {
            SelectData.updateSongList(true);
            return select_keys_filter(plus, iterateCount + 1);
        }
    }

    auto ran = PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft;
    if (ran == PlayModifierRandomType::DB_SYNCHRONIZE || ran == PlayModifierRandomType::DB_SYMMETRY)
    {
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft = PlayModifierRandomType::NONE;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight = PlayModifierRandomType::NONE;
    }

    SelectData.optionChangePending = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 12
void select_sort_type(int plus)
{
    SongListSortType val = SongListSortType(((int)SelectData.sortType + 5 + plus) % 5);

    SelectData.sortType = val;

    SelectData.updateSongList(true);
    SelectData.optionChangePending = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 13
void enter_key_config()
{
    SelectData.isGoingToKeyConfig = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 14
void enter_skin_config()
{
    // SelectData.isGoingToSkinSelect = true;
    // SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 16
void autoplay()
{
    SelectData.isGoingToAutoPlay = true;
}

// 19
void replay()
{
    SelectData.isGoingToReplay = true;
}

// 20, 21, 22
void fx_type(int idx, int plus)
{
    if (idx != 0) return;

    if (plus > 0)
    {
        switch (SystemData.fxType)
        {
        case FXType::Off: SystemData.fxType = FXType::Echo; break;
        case FXType::Compressor: SystemData.fxType = FXType::Echo; break;
        case FXType::Echo: SystemData.fxType = FXType::SfxReverb; break;
        case FXType::SfxReverb: SystemData.fxType = FXType::LowPass; break;
        case FXType::LowPass: SystemData.fxType = FXType::HighPass; break;
        case FXType::HighPass: SystemData.fxType = FXType::Off; break;
        default: SystemData.fxType = FXType::Off; break;
        }
    }
    else if (plus < 0)
    {
        switch (SystemData.fxType)
        {
        case FXType::Off: SystemData.fxType = FXType::HighPass; break;
        case FXType::Compressor: SystemData.fxType = FXType::Off; break;
        case FXType::Echo: SystemData.fxType = FXType::Off; break;
        case FXType::SfxReverb: SystemData.fxType = FXType::Echo; break;
        case FXType::LowPass: SystemData.fxType = FXType::SfxReverb; break;
        case FXType::HighPass: SystemData.fxType = FXType::LowPass; break;
        default: SystemData.fxType = FXType::Off; break;
        }
    }


    if (SystemData.fxType == FXType::Off)
        SoundMgr::setDSP(DSPType::OFF, idx, SampleChannel::MASTER, 0.f, 0.f);
    else
        SoundMgr::setDSP(SystemData.fxType, idx, SampleChannel::MASTER, SystemData.fxVal, SystemData.fxVal);
}

// 23, 24, 25
void fx_switch(int idx, int plus)
{
    if (idx != 0) return;

    if (SystemData.fxType == FXType::Off)
    {
        SystemData.fxType = FXType::Echo;
        SoundMgr::setDSP(SystemData.fxType, idx, SampleChannel::MASTER, SystemData.fxVal, SystemData.fxVal);
    }
    else
    {
        SystemData.fxType = FXType::Off;
        SoundMgr::setDSP(SystemData.fxType, idx, SampleChannel::MASTER, 0.f, 0.f);
    }
}

// 26, 27, 28
void fx_target(int idx, int plus)
{
}

// 29
void eq_switch(int plus)
{
    if (SystemData.equalizerEnabled)
    {
        // close
        SystemData.equalizerEnabled = false;

        for (int idx = 0; idx < 7; ++idx)
        {
            SoundMgr::setEQ((EQFreq)idx, 0);
        }
    }
    else
    {
        // open
        SystemData.equalizerEnabled = true;

        SoundMgr::setEQ(EQFreq::_62_5, SystemData.equalizerVal62_5hz);
        SoundMgr::setEQ(EQFreq::_160, SystemData.equalizerVal160hz);
        SoundMgr::setEQ(EQFreq::_400, SystemData.equalizerVal400hz);
        SoundMgr::setEQ(EQFreq::_1000, SystemData.equalizerVal1khz);
        SoundMgr::setEQ(EQFreq::_2500, SystemData.equalizerVal2_5khz);
        SoundMgr::setEQ(EQFreq::_6250, SystemData.equalizerVal6_25khz);
        SoundMgr::setEQ(EQFreq::_16k, SystemData.equalizerVal16khz);
    }
}

// 31
void vol_switch(int plus)
{
}

// 32
void pitch_switch(int plus)
{
    if (ArenaData.isOnline() || SystemData.freqType != FreqModifierType::Off)
    {
        // close
        setFreqModifier(FreqModifierType::Off, 0);
    }
    else
    {
        // open
        setFreqModifier(FreqModifierType::Frequency, SystemData.freqVal);
    }
}

// 33
void pitch_type(int plus)
{
    switch (SystemData.freqType)
    {
    case FreqModifierType::Off:       setFreqModifier(FreqModifierType::Frequency, SystemData.freqVal); break;
    case FreqModifierType::Frequency: setFreqModifier(FreqModifierType::PitchOnly, SystemData.freqVal); break;
    case FreqModifierType::PitchOnly: setFreqModifier(FreqModifierType::SpeedOnly, SystemData.freqVal); break;
    case FreqModifierType::SpeedOnly: setFreqModifier(FreqModifierType::Off, SystemData.freqVal); break;
    }
}

// 40, 41
void gauge_type(int player, int plus)
{
    // PlayModifierGaugeType
    int types = ConfigMgr::get('P', cfg::P_ENABLE_NEW_GAUGE, false) ? 8 : 4;
    PlayModifierGaugeType val = PlayModifierGaugeType(((int)PlayData.player[player].mods.gauge + types + plus) % types);
    if (val == PlayModifierGaugeType::PATTACK || val == PlayModifierGaugeType::GATTACK)
    {
        val = (plus >= 0) ? PlayModifierGaugeType::EXHARD : PlayModifierGaugeType::EASY;
    }
    PlayData.player[player].mods.gauge = val;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 42, 43
void random_type(int slot, int plus)
{
    // PlayModifierRandomType
    int types = ConfigMgr::get('P', cfg::P_ENABLE_NEW_RANDOM, false) ? 9 : 6;
    int isDP = PlayData.mode == SkinType::PLAY10 || PlayData.mode == SkinType::PLAY14;
    PlayModifierRandomType& target = isDP ? PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight : PlayData.player[slot].mods.randomLeft;
    PlayModifierRandomType oldVal = target;
    PlayModifierRandomType newVal = PlayModifierRandomType((int(target) + types + plus) % types);
    if (newVal == PlayModifierRandomType::DB_SYNCHRONIZE || newVal == PlayModifierRandomType::DB_SYMMETRY)
    {
        if (PlayData.battleType != PlayModifierBattleType::DoubleBattle)
        {
            newVal = (plus >= 0) ? PlayModifierRandomType::NONE : PlayModifierRandomType::RRAN;
            target = newVal;
        }
        else
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft = newVal;
            PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight = newVal;
        }
    }
    else if ((oldVal == PlayModifierRandomType::DB_SYNCHRONIZE || oldVal == PlayModifierRandomType::DB_SYMMETRY) &&
        (newVal != PlayModifierRandomType::DB_SYNCHRONIZE && newVal != PlayModifierRandomType::DB_SYMMETRY))
    {
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft = newVal;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight = newVal;
    }
    else
    {
        target = newVal;
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 44, 45
void autoscr(int slot, int plus)
{
    // PlayModifierRandomType
    bool val = PlayData.player[slot].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR;
    if (plus % 2) val = !val;

    if (plus != 0)
    {
        int isDP = PlayData.mode == SkinType::PLAY10 || PlayData.mode == SkinType::PLAY14;
        if (isDP)
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask =
                (PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & ~PLAY_MOD_ASSIST_AUTOSCR) | (val ? PLAY_MOD_ASSIST_AUTOSCR : 0);
            PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask =
                (PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask & ~PLAY_MOD_ASSIST_AUTOSCR) | (val ? PLAY_MOD_ASSIST_AUTOSCR : 0);
        }
        else
        {
            PlayData.player[slot].mods.assist_mask =
                (PlayData.player[slot].mods.assist_mask & ~PLAY_MOD_ASSIST_AUTOSCR) | (val ? PLAY_MOD_ASSIST_AUTOSCR : 0);
        }

        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
    }
}

// 46
void shutter(int plus)
{
    bool val = !(PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect == PlayModifierLaneEffectType::OFF ||
        PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect == PlayModifierLaneEffectType::LIFT);
    auto type = PlayModifierLaneEffectType::OFF;
    switch (SelectData.lastLaneEffectType1P)
    {
    case PlayModifierLaneEffectType::OFF:      
    case PlayModifierLaneEffectType::SUDDEN:   val = !val; type = val ? PlayModifierLaneEffectType::SUDDEN : PlayModifierLaneEffectType::OFF; break;
    case PlayModifierLaneEffectType::HIDDEN:   val = !val; type = val ? PlayModifierLaneEffectType::HIDDEN : PlayModifierLaneEffectType::OFF; break;
    case PlayModifierLaneEffectType::SUDHID:   val = !val; type = val ? PlayModifierLaneEffectType::SUDHID : PlayModifierLaneEffectType::OFF; break;
    case PlayModifierLaneEffectType::LIFT:     
    case PlayModifierLaneEffectType::LIFTSUD:  val = !val; type = val ? PlayModifierLaneEffectType::LIFTSUD : PlayModifierLaneEffectType::LIFT; break;
    }
    if (type != PlayModifierLaneEffectType::OFF)
        SelectData.lastLaneEffectType1P = PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect;
    PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect = type;

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 50, 51
void lane_effect(int slot, int plus)
{
    auto& op = (slot == PLAYER_SLOT_TARGET && PlayData.battleType == PlayModifierBattleType::LocalBattle) ?
        PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect : PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect;
    int types = ConfigMgr::get('P', cfg::P_ENABLE_NEW_LANE_OPTION, false) ? 6 : 4;
    int val = (int(op) + types + plus) % types;

    if (plus != 0)
    {
        op = PlayModifierLaneEffectType(val);
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
    }
}

// 54
void flip(int plus)
{
    if (PlayData.mode == SkinType::PLAY10 || PlayData.mode == SkinType::PLAY14)
    {
        PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip = !PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip;
    }
    else
    {
        PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip = false;
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 55
void hs_fix(int plus)
{
    // PlayModifierHispeedFixType
    //OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT/INITIAL/MAIN
    int val = (int(PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix) + 7 + plus) % 7;
    PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix = PlayModifierHispeedFixType(val);
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 56
void battle(int plus)
{
    // we only planned to support 3 battle modes: SP Battle (SP->2P) / DB (SP->DP) / Ghost Battle (SP->2P)
    switch (SelectData.filterKeys)
    {
    case FilterKeysType::All:
    case FilterKeysType::Single:
    case FilterKeysType::_5:
    case FilterKeysType::_7:
    case FilterKeysType::_9:
    {
        auto val = (int(PlayData.battleType) + 4 + plus) % 4;
        if (PlayModifierBattleType(val) == PlayModifierBattleType::DoubleBattle)
            val = (plus > 0) ? int(PlayModifierBattleType::GhostBattle) : int(PlayModifierBattleType::LocalBattle);
        PlayData.battleType = PlayModifierBattleType(val);
        SelectData.updateSongList(false);
        break;
    }
    case FilterKeysType::Double:
    case FilterKeysType::_10:
    case FilterKeysType::_14:
    {
        auto val = (int(PlayData.battleType) + 4 + plus) % 4;
        if (PlayModifierBattleType(val) == PlayModifierBattleType::LocalBattle)
            val = (plus > 0) ? int(PlayModifierBattleType::DoubleBattle) : int(PlayModifierBattleType::Off);
        PlayData.battleType = PlayModifierBattleType(val);
        SelectData.updateSongList(false);
        break;
    }
    }

    PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip = false;
    
    if (PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft == PlayModifierRandomType::DB_SYNCHRONIZE || 
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft == PlayModifierRandomType::DB_SYMMETRY)
    {
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft = PlayModifierRandomType::NONE;
        PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight = PlayModifierRandomType::NONE;
    }

    if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
    {
        PlayData.player[PLAYER_SLOT_TARGET].mods = PlayData.player[PLAYER_SLOT_PLAYER].mods;
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 57, 58
void hs(int player, int plus)
{
    // TODO margin, min/max
    double dplus = plus > 0 ? 0.25: -0.25;
    switch (player)
    {
    case 0: 
        number_change_clamp(PlayData.player[PLAYER_SLOT_PLAYER].hispeed, 0.5, 10.0, dplus);
        break;
    case 1: 
        number_change_clamp(PlayData.player[PLAYER_SLOT_TARGET].hispeed, 0.5, 10.0, dplus);
        break;
    default: break;
    }
}
// 47, 48
void lock_speed_value(int player, int plus)
{
    // TODO margin, min/max
    double dplus = plus > 0 ? 1.0 : -1.0;
    switch (player)
    {
    case 0:
        number_change_clamp(PlayData.player[PLAYER_SLOT_PLAYER].greenNumber, 0., 2000., dplus);
        break;
    case 1:
        number_change_clamp(PlayData.player[PLAYER_SLOT_TARGET].greenNumber, 0., 2000., dplus);
        break;
    default: break;
    }
}

// 70
void score_graph(int plus)
{
    if (PlayData.panelStyle & PANEL_STYLE_GRAPH_MASK)
    {
        PlayData.panelStyle &= ~PANEL_STYLE_GRAPH_MASK;
    }
    else
    {
        PlayData.panelStyle |= PANEL_STYLE_GRAPH_FAR;
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 71
void ghost_type(int plus)
{
    int val = int((int(PlayData.ghostType) + 4 + plus) % 4);
    PlayData.ghostType = GhostScorePosition(val);
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 72
void bga(int plus)
{
    if (PlayData.panelStyle & PANEL_STYLE_BGA_MASK)
    {
        PlayData.panelStyle &= ~PANEL_STYLE_BGA_MASK;
    }
    else
    {
        PlayData.panelStyle |= PANEL_STYLE_BGA_SIDE;
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 73
void bga_size(int plus)
{
    switch (PlayData.panelStyle & PANEL_STYLE_BGA_MASK)
    {
    case PANEL_STYLE_BGA_OFF:
        PlayData.panelStyle = PlayData.panelStyle & ~PANEL_STYLE_BGA_MASK | PANEL_STYLE_BGA_SIDE;
        break;
    case PANEL_STYLE_BGA_SIDE:
        PlayData.panelStyle = PlayData.panelStyle & ~PANEL_STYLE_BGA_MASK | PANEL_STYLE_BGA_SIDE_EXPAND;
        break;
    case PANEL_STYLE_BGA_SIDE_EXPAND:
    case PANEL_STYLE_BGA_FULLSCREEN:
        PlayData.panelStyle = PlayData.panelStyle & ~PANEL_STYLE_BGA_MASK | PANEL_STYLE_BGA_SIDE;
        break;
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 75
void judge_auto_adjust(int plus)
{

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

}

// 76
void default_target_rate(int plus)
{
    number_change_clamp(PlayData.targetRate, 0, 100, plus);
}

// 77
void target_type(int plus)
{
    int val = int((int(PlayData.targetType) + 6 + plus) % 6);
    PlayData.targetType = TargetType(val);

    if (plus)
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 80
void window_mode(int plus)
{
    // This button is disabled.
}

// 82
void vsync(int plus)
{
#if _WIN32
    int val = (int(SystemData.vsyncMode) + 2 + plus) % 2;
#else
    int val = (int(SystemData.vsyncMode) + 3 + plus) % 3;
#endif


    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

    graphics_change_vsync(val);
}

// 83
void save_replay_type(int plus)
{
    // TODO 
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 90
void favorite_ignore(int plus)
{
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 91 - 96
void difficulty(int diff, int plus)
{
    if (diff == int(SelectData.filterDifficulty))
    {
        SelectData.switchVersion(diff);
        SelectData.updateSongList(false);
    }
    else
    {
        SelectData.filterDifficulty = FilterDifficultyType(diff);
        SelectData.updateSongList(true);
    }

    SelectData.optionChangePending = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 100 - 139
void key_config_pad(Input::Pad pad, bool force)
{
    auto old = KeyConfigData.selecting.first;

    if (!KeyConfigData.skinHasAbsAxis)
    {
        if (old == pad)
        {
            switch (pad)
            {
            case Input::Pad::S1L:
            case Input::Pad::S1R: pad = Input::Pad::S1A; break;
            case Input::Pad::S2L:
            case Input::Pad::S2R: pad = Input::Pad::S2A; break;
            }
        }
    }

    if (old != pad || force)
    {
        KeyConfigData.selecting.first = pad;
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 143
void key_config_mode_rotate()
{
    auto& keys = KeyConfigData.currentMode;
    switch (keys)
    {
    case 7: keys = 9; break;
    case 9: keys = 5; break;
    case 5: keys = 7; break;
    default: keys = 7; break;
    }

    key_config_pad(Input::Pad::K11, true);

    KeyConfigData.modeChanged = true;
}

// 150 - 159
void key_config_slot(int slot)
{
    KeyConfigData.selecting.second = 0;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}


void skinselect_mode(int mode)
{
}

void skinselect_skin(int plus)
{
}

void skinselect_option(int index, int plus)
{
}

void help(int index)
{
    // TODO #SRC_README, #HELPFILE
}

#pragma endregion

std::function<void(int)> getButtonCallback(int type)
{
    using namespace lr2skin::button;
    using namespace std::placeholders;
    switch (type)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        return std::bind(panel_switch, type - 1, _1);

    case 10:
        return std::bind(select_difficulty_filter, _1, 0);
    case 11:
        return std::bind(select_keys_filter, _1, 0);
    case 12:
        return std::bind(select_sort_type, _1);

    case 13:
        return std::bind(enter_key_config);

    case 14:
        return std::bind(enter_skin_config);

    case 16:
        return std::bind(autoplay);

    case 19:
        return std::bind(replay);

    case 20:
    case 21:
    case 22:
        return std::bind(fx_type, type - 20, _1);

    case 23:
    case 24:
    case 25:
        return std::bind(fx_switch, type - 23, _1);

    case 26:
    case 27:
    case 28:
        return std::bind(fx_target, type - 26, _1);

    case 29:
        return std::bind(eq_switch, _1);

    case 31:
        return std::bind(vol_switch, _1);

    case 32:
        return std::bind(pitch_switch, _1);

    case 33:
        return std::bind(pitch_type, _1);

    case 40:
        return std::bind(gauge_type, 0, _1);
    case 41:
        return std::bind(gauge_type, 1, _1);

    case 42:
        return std::bind(random_type, 0, _1);
    case 43:
        return std::bind(random_type, 1, _1);

    case 44:
        return std::bind(autoscr, 0, _1);
    case 45:
        return std::bind(autoscr, 1, _1);

    case 46:
        return std::bind(shutter, _1);

    case 47:
        return std::bind(lock_speed_value, 0, _1);
    case 48:
        return std::bind(lock_speed_value, 1, _1);

    case 50:
        return std::bind(lane_effect, 0, _1);
    case 51:
        return std::bind(lane_effect, 1, _1);

    case 54:
        return std::bind(flip, _1);

    case 55: 
        return std::bind(hs_fix, _1);

    case 56:
        return std::bind(battle, _1);

    case 57:
        return std::bind(hs, 0, _1);
    case 58:
        return std::bind(hs, 1, _1);

    case 70:
        return std::bind(score_graph, _1);

    case 71:
        return std::bind(ghost_type, _1);

    case 72:
        return std::bind(bga, _1);

    case 73:
        return std::bind(bga_size, _1);

    case 74:
        return std::bind(number_change_clamp<int>, PlayData.player[PLAYER_SLOT_PLAYER].offsetVisual, -99, 99, _1);

    case 75:
        return std::bind(judge_auto_adjust, _1);

    case 76:
        return std::bind(default_target_rate, _1);

    case 77:
        return std::bind(target_type, _1);

    case 80:
        return std::bind(window_mode, _1);

    case 82:
        return std::bind(vsync, _1);

    case 83:
        return std::bind(save_replay_type, _1);

    case 90:
        return std::bind(favorite_ignore, _1);

    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
        return std::bind(difficulty, type - 91, _1);

    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
        return std::bind(key_config_pad, Input::Pad(unsigned(Input::Pad::K11) + type - 101), false);

    case 110:
        return std::bind(key_config_pad, Input::Pad::S1L, false);
    case 111:
        return std::bind(key_config_pad, Input::Pad::S1R, false);
    case 112:
        return std::bind(key_config_pad, Input::Pad::K1START, false);
    case 113:
        return std::bind(key_config_pad, Input::Pad::K1SELECT, false);
    case 114:
    case 115:
        break;
    case 116:
        return std::bind(key_config_pad, Input::Pad::S1A, false);

    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
    case 127:
    case 128:
    case 129:
        return std::bind(key_config_pad, Input::Pad(unsigned(Input::Pad::K21) + type - 121), false);

    case 130:
        return std::bind(key_config_pad, Input::Pad::S2L, false);
    case 131:
        return std::bind(key_config_pad, Input::Pad::S2R, false);
    case 132:
        return std::bind(key_config_pad, Input::Pad::K2START, false);
    case 133:
        return std::bind(key_config_pad, Input::Pad::K2SELECT, false);
    case 134:
    case 135:
        break;
    case 136:
        return std::bind(key_config_pad, Input::Pad::S2A, false);

    case 143:
        return std::bind(key_config_mode_rotate);

    case 150:
    case 151:
    case 152:
    case 153:
    case 154:
    case 155:
    case 156:
    case 157:
    case 158:
    case 159:
        return std::bind(key_config_slot, type - 150);

    case 170:
    case 171:
    case 172:
    case 173:
    case 174:
    case 175:
    case 176:
    case 177:
    case 178:
    case 179:
    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
        return std::bind(skinselect_mode, type - 170);

    case 190:
        return std::bind(skinselect_skin, _1);

    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
    case 208:
    case 209:
        return std::bind(help, type - 200);

    case 210:
        // IR page
        break;

    case 220:
    case 221:
    case 222:
    case 223:
    case 224:
    case 225:
    case 226:
    case 227:
    case 228:
    case 229:
        return std::bind(skinselect_option, type - 220, _1);

    }
    return [](bool) {};
}


}

}
