

#include "skin_lr2_button_callbacks.h"

#include "game/runtime/state.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/scene/scene_context.h"
#include "game/arena/arena_data.h"

#include "config/config_mgr.h"

#include <boost/algorithm/string.hpp>

namespace lr2skin::button
{

#pragma region helpers

std::tuple<IndexOption, IndexSwitch, IndexNumber, IndexNumber, IndexSlider, IndexSlider, IndexOption, SampleChannel> disp_fx(int idx)
{
    IndexOption op;
    IndexSwitch sw;
    IndexNumber num_p1, num_p2;
    IndexSlider sli_p1, sli_p2;
    IndexOption target;
    switch (idx)
    {
    case 0: 
        op = IndexOption::SOUND_FX0; 
        sw = IndexSwitch::SOUND_FX0;
        num_p1 = IndexNumber::FX0_P1;
        num_p2 = IndexNumber::FX0_P2;
        sli_p1 = IndexSlider::FX0_P1;
        sli_p2 = IndexSlider::FX0_P2; 
        target = IndexOption::SOUND_TARGET_FX0;
        break;
    case 1:
        op = IndexOption::SOUND_FX1; 
        sw = IndexSwitch::SOUND_FX1;
        num_p1 = IndexNumber::FX1_P1;
        num_p2 = IndexNumber::FX1_P2;
        sli_p1 = IndexSlider::FX1_P1; 
        sli_p2 = IndexSlider::FX1_P2; 
        target = IndexOption::SOUND_TARGET_FX1;
        break;
    case 2: 
        op = IndexOption::SOUND_FX2;
        sw = IndexSwitch::SOUND_FX2;
        num_p1 = IndexNumber::FX2_P1;
        num_p2 = IndexNumber::FX2_P2;
        sli_p1 = IndexSlider::FX2_P1; 
        sli_p2 = IndexSlider::FX2_P2; 
        target = IndexOption::SOUND_TARGET_FX2;
        break;
    default: return {};
    }

    SampleChannel ch = SampleChannel::MASTER;
    switch (State::get(target))
    {
    case 1: ch = SampleChannel::KEY; break;
    case 2: ch = SampleChannel::BGM; break;
    case 0:
    default: break;
    }

    return { op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch };
}

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

void update_pitch()
{
    int p = static_cast<int>(std::round((State::get(IndexSlider::PITCH) - 0.5) * 2 * 12));
    static const double tick = std::pow(2, 1.0 / 12);
    double f = std::pow(tick, p);
    switch (State::get(IndexOption::SOUND_PITCH_TYPE))
    {
    case 0: // FREQUENCY
        SoundMgr::setFreqFactor(f);
        gSelectContext.pitchSpeed = f;
        break;
    case 1: // PITCH
        SoundMgr::setFreqFactor(1.0);
        SoundMgr::setPitch(f);
        gSelectContext.pitchSpeed = 1.0;
        break;
    case 2: // SPEED (freq up, pitch down)
        SoundMgr::setFreqFactor(1.0);
        SoundMgr::setSpeed(f);
        gSelectContext.pitchSpeed = f;
        break;
    default:
        break;
    }
}

void number_change(IndexNumber type, int plus)
{
    State::set(type, State::get(type) + plus);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

void number_change_clamp(IndexNumber type, int min, int max, int plus)
{
    int val = std::clamp(State::get(type) + plus, min, max);
    State::set(type, val);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

#pragma endregion

#pragma region button type callbacks

// 1 - 9
void panel_switch(int idx, int plus)
{
    if (idx < 1 || idx > 9) return;
    IndexSwitch panel = static_cast<IndexSwitch>(int(IndexSwitch::SELECT_PANEL1) - 1 + idx);
    Time t{};

    // close other panels
    for (int i = 1; i <= 9; ++i)
    {
        if (i == idx) continue;
        IndexSwitch p = static_cast<IndexSwitch>(int(IndexSwitch::SELECT_PANEL1) - 1 + i);
        if (State::get(p))
        {
            State::set(p, false);
            State::set(static_cast<IndexTimer>(int(IndexTimer::PANEL1_START) - 1 + i), TIMER_NEVER);
            State::set(static_cast<IndexTimer>(int(IndexTimer::PANEL1_END) - 1 + i), t.norm());
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
        }
    }

    if (State::get(panel))
    {
        // close panel
        State::set(panel, false);
        State::set(static_cast<IndexTimer>(int(IndexTimer::PANEL1_START) - 1 + idx), TIMER_NEVER);
        State::set(static_cast<IndexTimer>(int(IndexTimer::PANEL1_END) - 1 + idx), t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
    }
    else
    {
        // open panel
        State::set(panel, true);
        State::set(static_cast<IndexTimer>(int(IndexTimer::PANEL1_START) - 1 + idx), t.norm());
        State::set(static_cast<IndexTimer>(int(IndexTimer::PANEL1_END) - 1 + idx), TIMER_NEVER);
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
    }
}

// 10
void select_difficulty_filter(int plus, int iterateCount)
{
    if (!gSelectContext.backtrace.front().ignoreFilters && iterateCount < 6)
    {
        int val = (State::get(IndexOption::SELECT_FILTER_DIFF) + 6 + plus) % 6;
        if (val == Option::DIFF_ANY && ConfigMgr::get("P", cfg::P_DISABLE_DIFFICULTY_ALL, false))
        {
            val++;
            iterateCount++;
        }

        State::set(IndexOption::SELECT_FILTER_DIFF, val);
        gSelectContext.filterDifficulty = State::get(IndexOption::SELECT_FILTER_DIFF);

        {
            std::unique_lock l(gSelectContext._mutex);
            loadSongList();
            sortSongList();
            setBarInfo();
            setEntryInfo();
        }
        if (gSelectContext.entries.empty())
        {
            return select_difficulty_filter(plus, iterateCount + 1);
        }
    }

    gSelectContext.optionChangePending = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 11
void select_keys_filter(int plus, int iterateCount)
{
    if (!gSelectContext.backtrace.front().ignoreFilters && iterateCount < 8)
    {
        int val = (State::get(IndexOption::SELECT_FILTER_KEYS) + 8 + plus) % 8;
        if (val == Option::FILTER_KEYS_ALL && ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_ALL, false))
        {
            val++;
            iterateCount++;
        }
        if (val == Option::FILTER_KEYS_SINGLE && ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_SINGLE, false))
        {
            val++;
            iterateCount++;
        }
        if (val == Option::FILTER_KEYS_DOUBLE && ConfigMgr::get("P", cfg::P_DISABLE_PLAYMODE_DOUBLE, false))
        {
            val++;
            iterateCount++;
        }

        State::set(IndexOption::SELECT_FILTER_KEYS, val);

        switch (val)
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

        {
            std::unique_lock l(gSelectContext._mutex);
            loadSongList();
            sortSongList();
            setBarInfo();
            setEntryInfo();

        }
        if (!gSelectContext.entries.empty())
        {
            State::set(IndexOption::PLAY_BATTLE_TYPE, Option::BATTLE_OFF);
            State::set(IndexText::BATTLE, "OFF");
            State::set(IndexSwitch::PLAY_OPTION_DP_FLIP, false);
            State::set(IndexText::FLIP, "OFF");

            setPlayModeInfo();

            const auto [score, lamp] = getSaveScoreType();
            State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
            State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
            State::set(IndexText::FILTER_KEYS, Option::s_filter_keys[val]);
        }
        else
        {
            return select_keys_filter(plus, iterateCount + 1);
        }
    }

    auto ran = State::get(IndexOption::PLAY_RANDOM_TYPE_1P);
    if (ran == Option::RAN_DB_SYNCHRONIZE_RANDOM || ran == Option::RAN_DB_SYMMETRY_RANDOM)
    {
        State::set(IndexOption::PLAY_RANDOM_TYPE_1P, 0);
        State::set(IndexOption::PLAY_RANDOM_TYPE_2P, 0);
    }

    gSelectContext.optionChangePending = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 12
void select_sort_type(int plus)
{
    State::set(IndexOption::SELECT_SORT, (State::get(IndexOption::SELECT_SORT) + 5 + plus) % 5);

    switch (State::get(IndexOption::SELECT_SORT))
    {
    case Option::SORT_TITLE: gSelectContext.sortType = SongListSortType::TITLE; break;
    case Option::SORT_LEVEL: gSelectContext.sortType = SongListSortType::LEVEL; break;
    case Option::SORT_CLEAR: gSelectContext.sortType = SongListSortType::CLEAR; break;
    case Option::SORT_RATE:  gSelectContext.sortType = SongListSortType::RATE; break;
    default:                 gSelectContext.sortType = SongListSortType::DEFAULT; break;
    }

    {
        std::unique_lock l(gSelectContext._mutex);
        loadSongList();
        sortSongList();
        setBarInfo();
        setEntryInfo();
    }

    gSelectContext.optionChangePending = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 13
void enter_key_config()
{
    gSelectContext.isGoingToKeyConfig = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 14
void enter_skin_config()
{
    gSelectContext.isGoingToSkinSelect = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 16
void autoplay()
{
    gSelectContext.isGoingToAutoPlay = true;
}

// 19
void replay()
{
    gSelectContext.isGoingToReplay = true;
}

// 20, 21, 22
void fx_type(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    // OFF/REVERB/DELAY/LOWPASS/HIGHPASS/FLANGER/CHORUS/DISTORTION/PITCH
    // pitch is unused, remaining 8 options
    int val = (State::get(op) + 8 + plus) % 8;
    State::set(op, val);

    if (val == Option::FX_LOWPASS)
    {
        State::set(sli_p1, 1.0);
        State::set(sli_p2, 0.0);
        State::set(num_p1, 100);
        State::set(num_p2, 0);
    }
    else
    {
        State::set(sli_p1, 0.0);
        State::set(sli_p2, 0.0);
        State::set(num_p1, 0);
        State::set(num_p2, 0);
    }

    if (State::get(sw))
    {
        float p1 = float(State::get(sli_p1));
        float p2 = float(State::get(sli_p2));
        update_fx(val, idx, ch, p1, p2);
    }
    else
    {
        update_fx(0, idx, ch, 0.f, 0.f);
    }
}

// 23, 24, 25
void fx_switch(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    if (State::get(sw))
    {
        // button clicked, close fx
        State::set(sw, false);
        update_fx(0, idx, ch, 0.f, 0.f);
    }
    else
    {
        // button clicked, open fx
        State::set(sw, true);
        float p1 = float(State::get(sli_p1));
        float p2 = float(State::get(sli_p2));
        update_fx(State::get(op), idx, ch, p1, p2);
    }
}

// 26, 27, 28
void fx_target(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    // MASTER/KEY/BGM
    int val = (State::get(target) + 3 + plus) % 3;
    State::set(target, val);

    if (State::get(sw))
    {
        // target is modified, thus we should get ch once again
        switch (State::get(target))
        {
        case 1: ch = SampleChannel::KEY; break;
        case 2: ch = SampleChannel::BGM; break;
        case 0: ch = SampleChannel::MASTER; break;
        default: break;
        }

        float p1 = float(State::get(sli_p1));
        float p2 = float(State::get(sli_p2));
        State::set(num_p1, static_cast<int>(p1 * 100));
        State::set(num_p2, static_cast<int>(p2 * 100));
        update_fx(State::get(op), idx, ch, p1, p2);
    }
}

// 29
void eq_switch(int plus)
{
    if (State::get(IndexSwitch::SOUND_EQ))
    {
        // close
        State::set(IndexSwitch::SOUND_EQ, false);

        for (int idx = 0; idx < 7; ++idx)
        {
            SoundMgr::setEQ((EQFreq)idx, 0);
        }
    }
    else
    {
        // open
        State::set(IndexSwitch::SOUND_EQ, true);

        for (int idx = 0; idx < 7; ++idx)
        {
            int val = State::get(IndexNumber(idx + (int)IndexNumber::EQ0));
            SoundMgr::setEQ((EQFreq)idx, val);
        }
    }
}

// 31
void vol_switch(int plus)
{
    if (State::get(IndexSwitch::SOUND_VOLUME))
    {
        // close
        State::set(IndexSwitch::SOUND_VOLUME, false);

        SoundMgr::setVolume(SampleChannel::MASTER, 0.8);
        SoundMgr::setVolume(SampleChannel::KEY, 1.0);
        SoundMgr::setVolume(SampleChannel::BGM, 1.0);
    }
    else
    {
        // open
        State::set(IndexSwitch::SOUND_VOLUME, true);

        SoundMgr::setVolume(SampleChannel::MASTER, State::get(IndexSlider::VOLUME_MASTER));
        SoundMgr::setVolume(SampleChannel::KEY, State::get(IndexSlider::VOLUME_KEY));
        SoundMgr::setVolume(SampleChannel::BGM, State::get(IndexSlider::VOLUME_BGM));
    }
}

// 32
void pitch_switch(int plus)
{
    if (gArenaData.isOnline() || State::get(IndexSwitch::SOUND_PITCH))
    {
        // close
        State::set(IndexSwitch::SOUND_PITCH, false);
        SoundMgr::setFreqFactor(1.0);
        gSelectContext.pitchSpeed = 1.0;
    }
    else
    {
        // open
        State::set(IndexSwitch::SOUND_PITCH, true);
        update_pitch();
    }

    const auto [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
}

// 33
void pitch_type(int plus)
{
    // FREQENCY/PITCH/SPEED
    int val = (State::get(IndexOption::SOUND_PITCH_TYPE) + 3 + plus) % 3;
    State::set(IndexOption::SOUND_PITCH_TYPE, val);

    if (State::get(IndexSwitch::SOUND_PITCH))
    {
        int p = static_cast<int>(std::round((State::get(IndexSlider::PITCH) - 0.5) * 2 * 12));
        State::set(IndexNumber::PITCH, p);
        update_pitch();
    }

    const auto [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
}

// 40, 41
void gauge_type(int player, int plus)
{
    int slot = 0;
    IndexOption op;
    IndexText tx;
    switch (player)
    {
    case 0: slot = PLAYER_SLOT_PLAYER; op = IndexOption::PLAY_GAUGE_TYPE_1P; tx = IndexText::GAUGE_1P; break;
    case 1: slot = PLAYER_SLOT_TARGET; op = IndexOption::PLAY_GAUGE_TYPE_2P; tx = IndexText::GAUGE_2P; break;
    default: return;
    }

    // PlayModifierGaugeType
    int types = ConfigMgr::get('P', cfg::P_ENABLE_NEW_GAUGE, false) ? 8 : 4;
    int val = (State::get(op) + types + plus) % types;
    if (val == Option::GAUGE_PATTACK || val == Option::GAUGE_GATTACK)
    {
        val = (plus >= 0) ? Option::GAUGE_EXHARD : Option::GAUGE_EASY;
    }
    State::set(op, val);
    State::set(tx, Option::s_gauge_type[val]);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

    const auto [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
}

// 42, 43
void random_type(int player, int plus)
{
    int slot = 0;
    IndexOption op;
    IndexText tx;
    switch (player)
    {
    case 0: slot = PLAYER_SLOT_PLAYER; op = IndexOption::PLAY_RANDOM_TYPE_1P; tx = IndexText::RANDOM_1P; break;
    case 1: slot = PLAYER_SLOT_TARGET; op = IndexOption::PLAY_RANDOM_TYPE_2P; tx = IndexText::RANDOM_2P; break;
    default: return;
    }

    // PlayModifierRandomType
    int types = ConfigMgr::get('P', cfg::P_ENABLE_NEW_RANDOM, false) ? 9 : 6;
    int oldVal = State::get(op);
    int val = (State::get(op) + types + plus) % types;
    if (val == Option::RAN_DB_SYNCHRONIZE_RANDOM || val == Option::RAN_DB_SYMMETRY_RANDOM)
    {
        if (State::get(IndexOption::PLAY_BATTLE_TYPE) != Option::BATTLE_DB)
        {
            val = (plus >= 0) ? Option::RAN_NORMAL : Option::RAN_RRAN;
        }
        else
        {
            if (slot == PLAYER_SLOT_PLAYER)
            {
                State::set(IndexOption::PLAY_RANDOM_TYPE_2P, val);
                State::set(IndexText::RANDOM_2P, Option::s_random_type[val]);
            }
            else
            {
                State::set(IndexOption::PLAY_RANDOM_TYPE_1P, val);
                State::set(IndexText::RANDOM_1P, Option::s_random_type[val]);
            }
        }
    }
    else if ((oldVal == Option::RAN_DB_SYNCHRONIZE_RANDOM || oldVal == Option::RAN_DB_SYMMETRY_RANDOM) &&
        (val != Option::RAN_DB_SYNCHRONIZE_RANDOM && val != Option::RAN_DB_SYMMETRY_RANDOM))
    {
        if (slot == PLAYER_SLOT_PLAYER)
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_2P, val);
            State::set(IndexText::RANDOM_2P, Option::s_random_type[val]);
        }
        else
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_1P, val);
            State::set(IndexText::RANDOM_1P, Option::s_random_type[val]);
        }
    }
    State::set(op, val);
    State::set(tx, Option::s_random_type[val]);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

    const auto [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
}

// 44, 45
void autoscr(int player, int plus)
{
    int slot = 0;
    IndexSwitch sw;
    IndexText tx;
    switch (player)
    {
    case 0: slot = PLAYER_SLOT_PLAYER; sw = IndexSwitch::PLAY_OPTION_AUTOSCR_1P; tx = IndexText::ASSIST_1P; break;
    case 1: slot = PLAYER_SLOT_TARGET; sw = IndexSwitch::PLAY_OPTION_AUTOSCR_2P; tx = IndexText::ASSIST_2P; break;
    default: return;
    }

    // PlayModifierRandomType
    bool val = State::get(sw);
    if (plus % 2) val = !val;
    State::set(sw, val);
    State::set(tx, Option::s_assist_type[(int)val]);

    if (plus != 0)
    {
        if (State::get(IndexOption::PLAY_MODE) != Option::PLAY_MODE_BATTLE)
        {
            if (slot == PLAYER_SLOT_PLAYER)
            {
                State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_2P, val);
                autoscr(PLAYER_SLOT_TARGET, 0);
            }
            else
            {
                State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_1P, val);
                autoscr(PLAYER_SLOT_PLAYER, 0);
            }
        }

        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

        const auto [score, lamp] = getSaveScoreType();
        State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
        State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
    }
}

// 46
void shutter(int plus)
{
    bool val = State::get(IndexSwitch::P1_LANECOVER_ENABLED);
    int type = Option::LANE_OFF;
    switch (gSelectContext.lastLaneEffectType1P)
    {
    case Option::LANE_OFF:      
    case Option::LANE_SUDDEN:   val = !val; type = val ? Option::LANE_SUDDEN : Option::LANE_OFF; break;
    case Option::LANE_HIDDEN:   val = !val; type = val ? Option::LANE_HIDDEN : Option::LANE_OFF; break;
    case Option::LANE_SUDHID:   val = !val; type = val ? Option::LANE_SUDHID : Option::LANE_OFF; break;
    case Option::LANE_LIFT:     
    case Option::LANE_LIFTSUD:  val = !val; type = val ? Option::LANE_LIFTSUD : Option::LANE_LIFT; break;
    }
    State::set(IndexText::SHUTTER, val ? "ON" : "OFF");
    State::set(IndexText::EFFECT_1P, Option::s_lane_effect_type[type]);
    State::set(IndexSwitch::P1_LANECOVER_ENABLED, val);
    State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, type);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 50, 51
void lane_effect(int player, int plus)
{
    int slot = 0;
    IndexOption op;
    IndexText tx;
    switch (player)
    {
    case 0: slot = PLAYER_SLOT_PLAYER; op = IndexOption::PLAY_LANE_EFFECT_TYPE_1P; tx = IndexText::EFFECT_1P; break;
    case 1: slot = PLAYER_SLOT_TARGET; op = IndexOption::PLAY_LANE_EFFECT_TYPE_2P; tx = IndexText::EFFECT_2P; break;
    default: return;
    }

    // 
    int types = ConfigMgr::get('P', cfg::P_ENABLE_NEW_LANE_OPTION, false) ? 6 : 4;
    int val = (State::get(op) + types + plus) % types;
    State::set(op, val);
    State::set(tx, Option::s_lane_effect_type[val]);

    bool enabled = false;
    switch (val)
    {
    case Option::LANE_OFF:
    case Option::LANE_LIFT:     enabled = false; break;
    case Option::LANE_HIDDEN:
    case Option::LANE_SUDDEN:
    case Option::LANE_SUDHID:
    case Option::LANE_LIFTSUD:  enabled = true; break;
    }
    if (slot == PLAYER_SLOT_PLAYER)
    {
        gSelectContext.lastLaneEffectType1P = val;
        State::set(IndexSwitch::P1_LANECOVER_ENABLED, enabled);
    }
    else
    {
        State::set(IndexSwitch::P2_LANECOVER_ENABLED, enabled);
    }

    if (plus != 0)
    {
        if (State::get(IndexOption::PLAY_MODE) != Option::PLAY_MODE_BATTLE)
        {
            if (slot == PLAYER_SLOT_PLAYER)
            {
                State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, val);
                lane_effect(PLAYER_SLOT_TARGET, 0);
            }
            else
            {
                State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, val);
                lane_effect(PLAYER_SLOT_PLAYER, 0);
            }
        }

        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
    }
}

// 54
void flip(int plus)
{
    bool val = false;
    auto gamemode = State::get(IndexOption::PLAY_MODE);
    if (gamemode == Option::PLAY_MODE_DOUBLE || gamemode == Option::PLAY_MODE_DP_GHOST_BATTLE)
    {
        val = !State::get(IndexSwitch::PLAY_OPTION_DP_FLIP);
    }
    State::set(IndexSwitch::PLAY_OPTION_DP_FLIP, val);
    State::set(IndexText::FLIP, val ? "DP FLIP" : "OFF");

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 55
void hs_fix(int plus)
{
    // PlayModifierHispeedFixType
    //OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT/INITIAL/MAIN
    int val = (State::get(IndexOption::PLAY_HSFIX_TYPE) + 7 + plus) % 7;

    if (val == Option::SPEED_NORMAL)
    {
        State::set(IndexSwitch::P1_LOCK_SPEED, false);
        State::set(IndexSwitch::P2_LOCK_SPEED, false);
    }
    else
    {
        State::set(IndexSwitch::P1_LOCK_SPEED, true);
        State::set(IndexSwitch::P2_LOCK_SPEED, true);
    }
    
    State::set(IndexOption::PLAY_HSFIX_TYPE, val);
    State::set(IndexText::SCROLL_TYPE, Option::s_speed_type[val]);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

    const auto [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
}

// 56
void battle(int plus)
{
    // we only planned to support 3 battle modes: SP Battle (SP->2P) / DB (SP->DP) / Ghost Battle (SP->2P)
    switch (gSelectContext.filterKeys)
    {
    case 0:
    case 1:
    case 5:
    case 7:
    case 9:
    {
        static const std::vector<Option::e_battle_type> modesSP =
        {
            Option::BATTLE_OFF,
            Option::BATTLE_LOCAL,
            Option::BATTLE_GHOST
        };
        auto it = std::find(modesSP.begin(), modesSP.end(), State::get(IndexOption::PLAY_BATTLE_TYPE));
        if (it != modesSP.end())
        {
            int idx = std::distance(modesSP.begin(), it);
            idx = (idx + modesSP.size() + plus) % modesSP.size();
            State::set(IndexOption::PLAY_BATTLE_TYPE, modesSP[idx]);

            setPlayModeInfo();
        }
        break;
    }
    case 2:
    case 10:
    case 14:
    {
        static const std::vector<Option::e_battle_type> modesDP =
        {
            Option::BATTLE_OFF,
            Option::BATTLE_DB,
            Option::BATTLE_GHOST
        };
        auto it = std::find(modesDP.begin(), modesDP.end(), State::get(IndexOption::PLAY_BATTLE_TYPE));
        if (it != modesDP.end())
        {
            int idx = std::distance(modesDP.begin(), it);
            idx = (idx + modesDP.size() + plus) % modesDP.size();
            State::set(IndexOption::PLAY_BATTLE_TYPE, modesDP[idx]);

            if (*it == Option::BATTLE_DB || modesDP[idx] == Option::BATTLE_DB)
            {
                std::unique_lock l(gSelectContext._mutex);
                loadSongList();
                sortSongList();
                setBarInfo();
                setEntryInfo();
            }
            else
            {
                State::set(IndexOption::PLAY_MODE, Option::PLAY_MODE_DOUBLE);
                setPlayModeInfo();
            }
        }
        break;
    }
    }
    State::set(IndexText::BATTLE, Option::s_battle_type[State::get(IndexOption::PLAY_BATTLE_TYPE)]);

    if (State::get(IndexOption::PLAY_MODE) != Option::PLAY_MODE_BATTLE)
    {
        State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P));
        lane_effect(PLAYER_SLOT_TARGET, 0);
        State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_2P, State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_1P));
        autoscr(PLAYER_SLOT_TARGET, 0);
    }

    State::set(IndexSwitch::PLAY_OPTION_DP_FLIP, false);
    State::set(IndexText::FLIP, "OFF");
    
    auto ran = State::get(IndexOption::PLAY_RANDOM_TYPE_1P);
    if (ran == Option::RAN_DB_SYNCHRONIZE_RANDOM || ran == Option::RAN_DB_SYMMETRY_RANDOM)
    {
        State::set(IndexOption::PLAY_RANDOM_TYPE_1P, 0);
        State::set(IndexOption::PLAY_RANDOM_TYPE_2P, 0);
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

    const auto [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
}

// 57, 58
void hs(int player, int plus)
{
    // TODO margin, min/max
    plus = plus > 0 ? 25 : -25;
    switch (player)
    {
    case 0: 
        number_change_clamp(IndexNumber::HS_1P, 50, 1000, plus);
        gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed = State::get(IndexNumber::HS_1P) / 100.0;
        State::set(IndexSlider::HISPEED_1P, gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed / 10.0);
        break;
    case 1: 
        number_change_clamp(IndexNumber::HS_2P, 50, 1000, plus);
        gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed = State::get(IndexNumber::HS_2P) / 100.0;
        State::set(IndexSlider::HISPEED_2P, gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed / 10.0);
        break;
    default: break;
    }
}
// 47, 48
void lock_speed_value(int player, int plus)
{
    // TODO margin, min/max
    plus = plus > 0 ? 1 : -1;
    switch (player)
    {
    case 0:
        number_change_clamp(IndexNumber::GREEN_NUMBER_1P, 0, 2000, plus);
        break;
    case 1:
        number_change_clamp(IndexNumber::GREEN_NUMBER_2P, 0, 2000, plus);
        break;
    default: break;
    }
}

// 70
void score_graph(int plus)
{
    if (State::get(IndexSwitch::SYSTEM_SCOREGRAPH))
    {
        // close
        State::set(IndexSwitch::SYSTEM_SCOREGRAPH, false);
        State::set(IndexText::SCORE_GRAPH, "OFF");
    }
    else
    {
        // open
        State::set(IndexSwitch::SYSTEM_SCOREGRAPH, true);
        State::set(IndexText::SCORE_GRAPH, "ON");
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 71
void ghost_type(int plus)
{
    int val = (State::get(IndexOption::PLAY_GHOST_TYPE_1P) + 4 + plus) % 4;

    State::set(IndexOption::PLAY_GHOST_TYPE_1P, val);
    State::set(IndexOption::PLAY_GHOST_TYPE_2P, val);
    State::set(IndexText::GHOST, Option::s_play_ghost_mode[val]);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 72
void bga(int plus)
{
    int val = (State::get(IndexOption::PLAY_BGA_TYPE) + 3 + plus) % 3;

    State::set(IndexOption::PLAY_BGA_TYPE, val);
    State::set(IndexText::BGA, Option::s_bga_type[val]);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

    // Play scene will check this config
    ConfigMgr::set('P', cfg::P_BGA_TYPE, val);

}

// 73
void bga_size(int plus)
{
    int val = (State::get(IndexOption::PLAY_BGA_SIZE) + 2 + plus) % 2;

    State::set(IndexOption::PLAY_BGA_SIZE, val);
    State::set(IndexText::BGA_SIZE, Option::s_bga_size[val]);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

    // Play scene will check this config
    ConfigMgr::set('P', cfg::P_BGA_SIZE, val);
}

// 75
void judge_auto_adjust(int plus)
{

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

}

// 76
void default_target_rate(int plus)
{
    number_change_clamp(IndexNumber::DEFAULT_TARGET_RATE, 0, 100, plus);

    if (State::get(IndexOption::PLAY_TARGET_TYPE) == Option::TARGET_DEFAULT)
        State::set(IndexText::TARGET_NAME, (boost::format("%d%%") % State::get(IndexNumber::DEFAULT_TARGET_RATE)).str());
}

// 77
void target_type(int plus)
{
    int val = (State::get(IndexOption::PLAY_TARGET_TYPE) + 6 + plus) % 6;

    State::set(IndexOption::PLAY_TARGET_TYPE, val);
    if (val == Option::TARGET_DEFAULT)
    {
        State::set(IndexText::TARGET_NAME, (boost::format("RATE %d%%") % State::get(IndexNumber::DEFAULT_TARGET_RATE)).str());
    }
    else
    {
        State::set(IndexText::TARGET_NAME, Option::s_target_type[val]);
    }

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
    int val = (State::get(IndexOption::SYS_VSYNC) + 2 + plus) % 2;
#else
    int val = (State::get(IndexOption::SYS_VSYNC) + 3 + plus) % 3;
#endif

    State::set(IndexOption::SYS_VSYNC, val);
    State::set(IndexText::VSYNC, Option::s_vsync_mode[val]);

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
    if (diff == gSelectContext.filterDifficulty)
    {
        std::unique_lock l(gSelectContext._mutex);
        switchVersion(diff);
        setBarInfo();
        setEntryInfo();
    }
    else
    {
        State::set(IndexOption::SELECT_FILTER_DIFF, diff);
        gSelectContext.filterDifficulty = State::get(IndexOption::SELECT_FILTER_DIFF);
        {
            std::unique_lock l(gSelectContext._mutex);
            loadSongList();
            sortSongList();
            setBarInfo();
            setEntryInfo();
        }
    }

    gSelectContext.optionChangePending = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 100 - 139
void key_config_pad(Input::Pad pad, bool force)
{
    auto& sel = gKeyconfigContext.selecting;
    auto old = sel.first;

    if (!gKeyconfigContext.skinHasAbsAxis)
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
        auto setSwitch = [](Input::Pad pad, bool sw)
        {
            switch (pad)
            {
            case Input::Pad::K11:      State::set(IndexSwitch::K11_CONFIG, sw); break;
            case Input::Pad::K12:      State::set(IndexSwitch::K12_CONFIG, sw); break;
            case Input::Pad::K13:      State::set(IndexSwitch::K13_CONFIG, sw); break;
            case Input::Pad::K14:      State::set(IndexSwitch::K14_CONFIG, sw); break;
            case Input::Pad::K15:      State::set(IndexSwitch::K15_CONFIG, sw); break;
            case Input::Pad::K16:      State::set(IndexSwitch::K16_CONFIG, sw); break;
            case Input::Pad::K17:      State::set(IndexSwitch::K17_CONFIG, sw); break;
            case Input::Pad::K18:      State::set(IndexSwitch::K18_CONFIG, sw); break;
            case Input::Pad::K19:      State::set(IndexSwitch::K19_CONFIG, sw); break;
            case Input::Pad::S1L:      State::set(IndexSwitch::S1L_CONFIG, sw); break;
            case Input::Pad::S1R:      State::set(IndexSwitch::S1R_CONFIG, sw); break;
            case Input::Pad::K1START:  State::set(IndexSwitch::K1START_CONFIG, sw); break;
            case Input::Pad::K1SELECT: State::set(IndexSwitch::K1SELECT_CONFIG, sw); break;
            case Input::Pad::K1SPDUP:  State::set(IndexSwitch::K1SPDUP_CONFIG, sw); break;
            case Input::Pad::K1SPDDN:  State::set(IndexSwitch::K1SPDDN_CONFIG, sw); break;
            case Input::Pad::K21:      State::set(IndexSwitch::K21_CONFIG, sw); break;
            case Input::Pad::K22:      State::set(IndexSwitch::K22_CONFIG, sw); break;
            case Input::Pad::K23:      State::set(IndexSwitch::K23_CONFIG, sw); break;
            case Input::Pad::K24:      State::set(IndexSwitch::K24_CONFIG, sw); break;
            case Input::Pad::K25:      State::set(IndexSwitch::K25_CONFIG, sw); break;
            case Input::Pad::K26:      State::set(IndexSwitch::K26_CONFIG, sw); break;
            case Input::Pad::K27:      State::set(IndexSwitch::K27_CONFIG, sw); break;
            case Input::Pad::K28:      State::set(IndexSwitch::K28_CONFIG, sw); break;
            case Input::Pad::K29:      State::set(IndexSwitch::K29_CONFIG, sw); break;
            case Input::Pad::S2L:      State::set(IndexSwitch::S2L_CONFIG, sw); break;
            case Input::Pad::S2R:      State::set(IndexSwitch::S2R_CONFIG, sw); break;
            case Input::Pad::K2START:  State::set(IndexSwitch::K2START_CONFIG, sw); break;
            case Input::Pad::K2SELECT: State::set(IndexSwitch::K2SELECT_CONFIG, sw); break;
            case Input::Pad::K2SPDUP:  State::set(IndexSwitch::K2SPDUP_CONFIG, sw); break;
            case Input::Pad::K2SPDDN:  State::set(IndexSwitch::K2SPDDN_CONFIG, sw); break;
            case Input::Pad::S1A:      
                if (!gKeyconfigContext.skinHasAbsAxis)
                {
                    State::set(IndexSwitch::S1L_CONFIG, sw);
                    State::set(IndexSwitch::S1R_CONFIG, sw);
                }
                State::set(IndexSwitch::S1A_CONFIG, sw); 
                break;
            case Input::Pad::S2A:
                if (!gKeyconfigContext.skinHasAbsAxis)
                {
                    State::set(IndexSwitch::S2L_CONFIG, sw);
                    State::set(IndexSwitch::S2R_CONFIG, sw);
                }
                State::set(IndexSwitch::S2A_CONFIG, sw);
                break;
            default: break;
            }
        };
        auto setOption = [](Input::Pad pad)
        {
            int idx = 0;
            switch (gKeyconfigContext.keys)
            {
            case 5:
                switch (pad)
                {
                case Input::Pad::K11:      idx = 1; break;
                case Input::Pad::K12:      idx = 2; break;
                case Input::Pad::K13:      idx = 3; break;
                case Input::Pad::K14:      idx = 4; break;
                case Input::Pad::K15:      idx = 5; break;
                case Input::Pad::S1L:      idx = 6; break;
                case Input::Pad::S1R:      idx = 7; break;
                case Input::Pad::K1START:  idx = 8; break;
                case Input::Pad::K1SELECT: idx = 9; break;
                case Input::Pad::K21:      idx = 10; break;
                case Input::Pad::K22:      idx = 11; break;
                case Input::Pad::K23:      idx = 12; break;
                case Input::Pad::K24:      idx = 13; break;
                case Input::Pad::K25:      idx = 14; break;
                case Input::Pad::S2L:      idx = 15; break;
                case Input::Pad::S2R:      idx = 16; break;
                case Input::Pad::K2START:  idx = 17; break;
                case Input::Pad::K2SELECT: idx = 18; break;
                case Input::Pad::S1A:      idx = 19; break;
                case Input::Pad::S2A:      idx = 20; break;
                default: break;
                }
                State::set(IndexOption::KEY_CONFIG_KEY5, idx);
                break;
            case 7:
                switch (pad)
                {
                case Input::Pad::K11:      idx = 1; break;
                case Input::Pad::K12:      idx = 2; break;
                case Input::Pad::K13:      idx = 3; break;
                case Input::Pad::K14:      idx = 4; break;
                case Input::Pad::K15:      idx = 5; break;
                case Input::Pad::K16:      idx = 6; break;
                case Input::Pad::K17:      idx = 7; break;
                case Input::Pad::S1L:      idx = 8; break;
                case Input::Pad::S1R:      idx = 9; break;
                case Input::Pad::K1START:  idx = 10; break;
                case Input::Pad::K1SELECT: idx = 11; break;
                case Input::Pad::K21:      idx = 12; break;
                case Input::Pad::K22:      idx = 13; break;
                case Input::Pad::K23:      idx = 14; break;
                case Input::Pad::K24:      idx = 15; break;
                case Input::Pad::K25:      idx = 16; break;
                case Input::Pad::K26:      idx = 17; break;
                case Input::Pad::K27:      idx = 18; break;
                case Input::Pad::S2L:      idx = 19; break;
                case Input::Pad::S2R:      idx = 20; break;
                case Input::Pad::K2START:  idx = 21; break;
                case Input::Pad::K2SELECT: idx = 22; break;
                case Input::Pad::S1A:      idx = 23; break;
                case Input::Pad::S2A:      idx = 24; break;
                default: break;
                }
                State::set(IndexOption::KEY_CONFIG_KEY7, idx);
                break;
            case 9:
                switch (pad)
                {
                case Input::Pad::K11:      idx = 1; break;
                case Input::Pad::K12:      idx = 2; break;
                case Input::Pad::K13:      idx = 3; break;
                case Input::Pad::K14:      idx = 4; break;
                case Input::Pad::K15:      idx = 5; break;
                case Input::Pad::K16:      idx = 6; break;
                case Input::Pad::K17:      idx = 7; break;
                case Input::Pad::K18:      idx = 8; break;
                case Input::Pad::K19:      idx = 9; break;
                case Input::Pad::K1START:  idx = 10; break;
                case Input::Pad::K1SELECT: idx = 11; break;
                default: break;
                }
                State::set(IndexOption::KEY_CONFIG_KEY9, idx);
                break;
            default:
                break;
            }
        };
        setSwitch(old, false);
        setSwitch(pad, true);
        setOption(pad);

        sel.first = pad;

        auto bindings = ConfigMgr::Input(gKeyconfigContext.keys)->getBindings(pad);
        State::set(IndexText::KEYCONFIG_SLOT1, bindings.toString());
        for (size_t i = 1; i < 10; ++i)
        {
            State::set(IndexText(unsigned(IndexText::KEYCONFIG_SLOT1) + i), "-");
        }
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 143
void key_config_mode_rotate()
{
    auto& keys = gKeyconfigContext.keys;
    switch (keys)
    {
    case 7: keys = 9; break;
    case 9: keys = 5; break;
    case 5: keys = 7; break;
    default: keys = 7; break;
    }
    switch (keys)
    {
    case 7: State::set(IndexOption::KEY_CONFIG_MODE, Option::KEYCFG_7); break;
    case 9: State::set(IndexOption::KEY_CONFIG_MODE, Option::KEYCFG_9); break;
    case 5: State::set(IndexOption::KEY_CONFIG_MODE, Option::KEYCFG_5); break;
    default: return;
    }

    key_config_pad(Input::Pad::K11, true);

    gKeyconfigContext.modeChanged = true;
}

// 150 - 159
void key_config_slot(int slot)
{
    auto& sel = gKeyconfigContext.selecting;
    auto old = sel.second;
    if (old != slot)
    {
        auto setSwitch = [](int slot, bool sw)
        {
            switch (slot)
            {
            case 0:      State::set(IndexSwitch::KEY_CONFIG_SLOT0, sw); break;
            case 1:      State::set(IndexSwitch::KEY_CONFIG_SLOT1, sw); break;
            case 2:      State::set(IndexSwitch::KEY_CONFIG_SLOT2, sw); break;
            case 3:      State::set(IndexSwitch::KEY_CONFIG_SLOT3, sw); break;
            case 4:      State::set(IndexSwitch::KEY_CONFIG_SLOT4, sw); break;
            case 5:      State::set(IndexSwitch::KEY_CONFIG_SLOT5, sw); break;
            case 6:      State::set(IndexSwitch::KEY_CONFIG_SLOT6, sw); break;
            case 7:      State::set(IndexSwitch::KEY_CONFIG_SLOT7, sw); break;
            case 8:      State::set(IndexSwitch::KEY_CONFIG_SLOT8, sw); break;
            case 9:      State::set(IndexSwitch::KEY_CONFIG_SLOT9, sw); break;
            default: break;
            }
        };
        setSwitch(old, false);
        setSwitch(slot, true);
        sel.second = slot;
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}


void skinselect_mode(int mode)
{
    State::set(IndexSwitch::SKINSELECT_7KEYS, false);
    State::set(IndexSwitch::SKINSELECT_5KEYS, false);
    State::set(IndexSwitch::SKINSELECT_14KEYS, false);
    State::set(IndexSwitch::SKINSELECT_10KEYS, false);
    State::set(IndexSwitch::SKINSELECT_9KEYS, false);
    State::set(IndexSwitch::SKINSELECT_SELECT, false);
    State::set(IndexSwitch::SKINSELECT_DECIDE, false);
    State::set(IndexSwitch::SKINSELECT_RESULT, false);
    State::set(IndexSwitch::SKINSELECT_KEYCONFIG, false);
    State::set(IndexSwitch::SKINSELECT_SKINSELECT, false);
    State::set(IndexSwitch::SKINSELECT_SOUNDSET, false);
    State::set(IndexSwitch::SKINSELECT_THEME, false);
    State::set(IndexSwitch::SKINSELECT_7KEYS_BATTLE, false);
    State::set(IndexSwitch::SKINSELECT_5KEYS_BATTLE, false);
    State::set(IndexSwitch::SKINSELECT_9KEYS_BATTLE, false);
    State::set(IndexSwitch::SKINSELECT_COURSE_RESULT, false);

    switch (mode)
    {
    case 0:  gCustomizeContext.mode = SkinType::PLAY7;         State::set(IndexSwitch::SKINSELECT_7KEYS, true);         break;
    case 1:  gCustomizeContext.mode = SkinType::PLAY5;         State::set(IndexSwitch::SKINSELECT_5KEYS, true);         break;
    case 2:  gCustomizeContext.mode = SkinType::PLAY14;        State::set(IndexSwitch::SKINSELECT_14KEYS, true);        break;
    case 3:  gCustomizeContext.mode = SkinType::PLAY10;        State::set(IndexSwitch::SKINSELECT_10KEYS, true);        break;
    case 4:  gCustomizeContext.mode = SkinType::PLAY9;         State::set(IndexSwitch::SKINSELECT_9KEYS, true);         break;
    case 5:  gCustomizeContext.mode = SkinType::MUSIC_SELECT;  State::set(IndexSwitch::SKINSELECT_SELECT, true);        break;
    case 6:  gCustomizeContext.mode = SkinType::DECIDE;        State::set(IndexSwitch::SKINSELECT_DECIDE, true);        break;
    case 7:  gCustomizeContext.mode = SkinType::RESULT;        State::set(IndexSwitch::SKINSELECT_RESULT, true);        break;
    case 8:  gCustomizeContext.mode = SkinType::KEY_CONFIG;    State::set(IndexSwitch::SKINSELECT_KEYCONFIG, true);     break;
    case 9:  gCustomizeContext.mode = SkinType::THEME_SELECT;  State::set(IndexSwitch::SKINSELECT_SKINSELECT, true);    break;
    case 10: gCustomizeContext.mode = SkinType::SOUNDSET;      State::set(IndexSwitch::SKINSELECT_SOUNDSET, true);      break;
    case 11:                                                State::set(IndexSwitch::SKINSELECT_THEME, true);         break;
    case 12: gCustomizeContext.mode = SkinType::PLAY7_2;       State::set(IndexSwitch::SKINSELECT_7KEYS_BATTLE, true);  break;
    case 13: gCustomizeContext.mode = SkinType::PLAY5_2;       State::set(IndexSwitch::SKINSELECT_5KEYS_BATTLE, true);  break;
    case 14: gCustomizeContext.mode = SkinType::PLAY9_2;       State::set(IndexSwitch::SKINSELECT_9KEYS_BATTLE, true);  break;
    case 15: gCustomizeContext.mode = SkinType::COURSE_RESULT; State::set(IndexSwitch::SKINSELECT_COURSE_RESULT, true); break;
    default: break;
    }

    gCustomizeContext.modeUpdate = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

void skinselect_skin(int plus)
{
    gCustomizeContext.skinDir = plus;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

void skinselect_option(int index, int plus)
{
    gCustomizeContext.optionIdx = index;
    gCustomizeContext.optionDir = plus;
    gCustomizeContext.optionUpdate = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
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
        return std::bind(panel_switch, type, _1);

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
        return std::bind(number_change_clamp, IndexNumber::TIMING_ADJUST_VISUAL, -99, 99, _1);

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
