#include "common/pch.h"
#include "data_shared.h"
#include "data_types.h"
#include "game/chart/chart.h"
#include "config/config_mgr.h"

namespace lunaticvibes
{

void resetTimers()
{
    PlayData.timers.clear();
    ArenaData.timers.clear();
    ResultData.timers.clear();
    SelectData.timers.clear();
    SystemData.timers.clear();
    KeyConfigData.timers.clear();
}

void loadConfigs()
{
    using namespace cfg;
    using std::string;

    if (PlayData.player[PLAYER_SLOT_PLAYER].hispeed == 0)
    {
        PlayData.player[PLAYER_SLOT_PLAYER].hispeed = ConfigMgr::get('P', cfg::P_HISPEED, 1.0);
    }
    if (PlayData.player[PLAYER_SLOT_TARGET].hispeed == 0)
    {
        PlayData.player[PLAYER_SLOT_TARGET].hispeed = ConfigMgr::get('P', cfg::P_HISPEED_2P, 1.0);
    }

    PlayData.player[PLAYER_SLOT_PLAYER].lanecoverTop = ConfigMgr::get('P', P_LANECOVER_TOP, 0);
    PlayData.player[PLAYER_SLOT_PLAYER].lanecoverBottom = ConfigMgr::get('P', P_LANECOVER_BOTTOM, 0);
    PlayData.player[PLAYER_SLOT_TARGET].lanecoverTop = ConfigMgr::get('P', P_LANECOVER_TOP, 0);
    PlayData.player[PLAYER_SLOT_TARGET].lanecoverBottom = ConfigMgr::get('P', P_LANECOVER_BOTTOM, 0);

    PlayData.player[PLAYER_SLOT_PLAYER].offsetVisual = ConfigMgr::get('P', P_JUDGE_OFFSET, 0);

    PlayData.targetRate, ConfigMgr::get('P', P_GHOST_TARGET, 50);

    SystemData.playerName = ConfigMgr::Profile()->getName();

    SystemData.volumeMaster = ConfigMgr::get('P', P_VOL_MASTER, 1.0);
    SystemData.volumeKey = ConfigMgr::get('P', P_VOL_KEY, 1.0);
    SystemData.volumeBgm = ConfigMgr::get('P', P_VOL_BGM, 1.0);

    // fx type
    {
        static const std::map<string, FXType> smap =
        {
            {P_FX_TYPE_REVERB, FXType::SfxReverb},
            {P_FX_TYPE_DELAY, FXType::Echo},
            {P_FX_TYPE_LOWPASS, FXType::LowPass},
            {P_FX_TYPE_HIGHPASS, FXType::HighPass},
            {P_FX_TYPE_COMPRESSOR, FXType::Compressor},
        };

        auto&& s0 = ConfigMgr::get<string>('P', P_FX0_TYPE, "OFF");
        if (smap.find(s0) != smap.end())
            SystemData.fxType = smap.at(s0);
        else
            SystemData.fxType = FXType::Off;
    }
    SystemData.fxVal = ConfigMgr::get('P', P_FX0_P1, 0);

    SystemData.equalizerEnabled = ConfigMgr::get('P', P_EQ, true);
    SystemData.equalizerVal62_5hz = ConfigMgr::get('P', P_EQ0, 0);
    SystemData.equalizerVal160hz = ConfigMgr::get('P', P_EQ1, 0);
    SystemData.equalizerVal400hz = ConfigMgr::get('P', P_EQ2, 0);
    SystemData.equalizerVal1khz = ConfigMgr::get('P', P_EQ3, 0);
    SystemData.equalizerVal2_5khz = ConfigMgr::get('P', P_EQ4, 0);
    SystemData.equalizerVal6_25khz = ConfigMgr::get('P', P_EQ5, 0);
    SystemData.equalizerVal16khz = ConfigMgr::get('P', P_EQ6, 0);

    // freq type
    {
        static const std::map<string, FreqModifierType> smap =
        {
            {P_FREQ_TYPE_FREQ, FreqModifierType::Frequency},
            {P_FREQ_TYPE_PITCH, FreqModifierType::PitchOnly },
            {P_FREQ_TYPE_SPEED, FreqModifierType::SpeedOnly},
        };

        auto&& s = ConfigMgr::get<string>('P', P_FREQ_TYPE, P_FREQ_TYPE_FREQ);
        if (smap.find(s) != smap.end())
            SystemData.freqType = smap.at(s);
        else
            SystemData.freqType = FreqModifierType::Off;
    }
    SystemData.freqVal = ConfigMgr::get('P', P_FREQ_VAL, 0);

    SelectData.newEntrySeconds = ConfigMgr::get('P', P_NEW_SONG_DURATION, 0) * 60 * 60;

    // bga
    PlayData.panelStyle = ConfigMgr::get('P', P_PANEL_STYLE, 0);

    // speed type
    {
        static const std::map<string, PlayModifierHispeedFixType> smap =
        {
            {P_SPEED_TYPE_MIN, PlayModifierHispeedFixType::MINBPM},
            {P_SPEED_TYPE_MAX, PlayModifierHispeedFixType::MAXBPM},
            {P_SPEED_TYPE_AVG, PlayModifierHispeedFixType::AVERAGE},
            {P_SPEED_TYPE_CONSTANT, PlayModifierHispeedFixType::CONSTANT},
            {P_SPEED_TYPE_INITIAL, PlayModifierHispeedFixType::INITIAL},
            {P_SPEED_TYPE_MAIN, PlayModifierHispeedFixType::MAIN},
        };

        auto&& s = ConfigMgr::get<string>('P', P_SPEED_TYPE, P_SPEED_TYPE_NORMAL);
        if (smap.find(s) != smap.end())
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix = smap.at(s);
        }
        else
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix = PlayModifierHispeedFixType::NONE;
        }
    }

    // target type
    {
        static const std::map<string, TargetType> smap =
        {
            {P_TARGET_TYPE_0         , TargetType::Zero},
            {P_TARGET_TYPE_MYBEST    , TargetType::MyBest},
            {P_TARGET_TYPE_AAA       , TargetType::RankAAA},
            {P_TARGET_TYPE_AA        , TargetType::RankAA},
            {P_TARGET_TYPE_A         , TargetType::RankA},
            {P_TARGET_TYPE_DEFAULT   , TargetType::UseTargetRate},
        };

        auto&& s = ConfigMgr::get<string>('P', P_TARGET_TYPE, P_TARGET_TYPE_DEFAULT);
        if (smap.find(s) != smap.end())
        {
            PlayData.targetType = smap.at(s);
        }
        else
        {
            PlayData.targetType = TargetType::UseTargetRate;
        }
    }

    // chart op
    {
        static const std::map<string, PlayModifierRandomType> smap =
        {
            {P_CHART_OP_MIRROR, PlayModifierRandomType::MIRROR},
            {P_CHART_OP_RANDOM, PlayModifierRandomType::RANDOM},
            {P_CHART_OP_SRAN, PlayModifierRandomType::SRAN},
            {P_CHART_OP_HRAN, PlayModifierRandomType::HRAN},
            {P_CHART_OP_ALLSCR, PlayModifierRandomType::ALLSCR},
            {P_CHART_OP_RRAN, PlayModifierRandomType::RRAN},
            {P_CHART_OP_DB_SYNCHRONIZE, PlayModifierRandomType::DB_SYNCHRONIZE},
            {P_CHART_OP_DB_SYMMETRY, PlayModifierRandomType::DB_SYMMETRY},
        };

        auto&& s1 = ConfigMgr::get<string>('P', P_CHART_OP, P_CHART_OP_NORMAL);
        auto&& s2 = ConfigMgr::get<string>('P', P_CHART_OP_2P, P_CHART_OP_NORMAL);
        if (smap.find(s1) != smap.end())
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft = smap.at(s1);
        }
        else
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft = PlayModifierRandomType::NONE;
        }
        if (smap.find(s2) != smap.end())
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight = smap.at(s2);
            PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft = smap.at(s2);
        }
        else
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight = PlayModifierRandomType::NONE;
            PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft = PlayModifierRandomType::NONE;
        }
    }

    // gauge op
    {
        static const std::map<string, PlayModifierGaugeType> smap =
        {
            {P_GAUGE_OP_HARD, PlayModifierGaugeType::HARD},
            {P_GAUGE_OP_EASY, PlayModifierGaugeType::EASY},
            {P_GAUGE_OP_DEATH, PlayModifierGaugeType::DEATH},
            {P_GAUGE_OP_EXHARD, PlayModifierGaugeType::EXHARD},
            {P_GAUGE_OP_ASSISTEASY, PlayModifierGaugeType::ASSISTEASY},
        };

        auto&& s1 = ConfigMgr::get<string>('P', P_GAUGE_OP, P_GAUGE_OP_NORMAL);
        auto&& s2 = ConfigMgr::get<string>('P', P_GAUGE_OP_2P, P_GAUGE_OP_NORMAL);
        if (smap.find(s1) != smap.end())
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge = smap.at(s1);
        }
        else
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge = PlayModifierGaugeType::NORMAL;
        }
        if (smap.find(s2) != smap.end())
        {
            PlayData.player[PLAYER_SLOT_TARGET].mods.gauge = smap.at(s2);
        }
        else
        {
            PlayData.player[PLAYER_SLOT_TARGET].mods.gauge = PlayModifierGaugeType::NORMAL;
        }
    }

    // lane effect
    {
        static const std::map<string, PlayModifierLaneEffectType> smap =
        {
            {P_LANE_EFFECT_OP_OFF, PlayModifierLaneEffectType::OFF},
            {P_LANE_EFFECT_OP_HIDDEN, PlayModifierLaneEffectType::HIDDEN},
            {P_LANE_EFFECT_OP_SUDDEN, PlayModifierLaneEffectType::SUDDEN},
            {P_LANE_EFFECT_OP_SUDHID, PlayModifierLaneEffectType::SUDHID},
            {P_LANE_EFFECT_OP_LIFT, PlayModifierLaneEffectType::LIFT},
            {P_LANE_EFFECT_OP_LIFTSUD, PlayModifierLaneEffectType::LIFTSUD},
        };

        auto&& s1 = ConfigMgr::get<string>('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_OFF);
        auto&& s2 = ConfigMgr::get<string>('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_OFF);
        if (smap.find(s1) != smap.end())
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect = smap.at(s1);
        }
        else
        {
            PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect = PlayModifierLaneEffectType::OFF;
        }
        if (smap.find(s2) != smap.end())
        {
            PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect = smap.at(s2);
        }
        else
        {
            PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect = PlayModifierLaneEffectType::OFF;
        }
    }

    PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask = ConfigMgr::get('P', P_CHART_ASSIST_OP, 0);
    PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask = ConfigMgr::get('P', P_CHART_ASSIST_OP_2P, 0);

    PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip = ConfigMgr::get('P', P_FLIP, true);

    // ghost type
    {
        static const std::map<string, GhostScorePosition> smap =
        {
            {P_GHOST_TYPE_A, GhostScorePosition::AboveJudge},
            {P_GHOST_TYPE_B, GhostScorePosition::NearJudge},
            {P_GHOST_TYPE_C, GhostScorePosition::NearJudgeLower},
        };

        auto&& s1 = ConfigMgr::get<string>('P', P_GHOST_TYPE, "OFF");
        if (smap.find(s1) != smap.end())
        {
            PlayData.ghostType = smap.at(s1);
        }
        else
        {
            PlayData.ghostType = GhostScorePosition::Off;
        }
    }

    // sort mode
    {
        static const std::map<string, SongListSortType> smap =
        {
            {P_SORT_MODE_FOLDER, SongListSortType::DEFAULT},
            {P_SORT_MODE_TITLE, SongListSortType::TITLE},
            {P_SORT_MODE_LEVEL, SongListSortType::LEVEL},
            {P_SORT_MODE_CLEAR, SongListSortType::CLEAR},
            {P_SORT_MODE_RATE, SongListSortType::RATE},
        };

        auto&& s = ConfigMgr::get<string>('P', P_SORT_MODE, P_SORT_MODE_FOLDER);
        if (smap.find(s) != smap.end())
            SelectData.sortType = smap.at(s);
        else
            SelectData.sortType = SongListSortType::DEFAULT;
    }

    loadFilterDifficulty();
    loadFilterKeys();


    // windowed
    {
        static const std::map<string, GameWindowMode> smap =
        {
            {V_WINMODE_FULL, GameWindowMode::FULLSCREEN},
            {V_WINMODE_BORDERLESS, GameWindowMode::BORDERLESS},
            {V_WINMODE_WINDOWED, GameWindowMode::WINDOWED}
        };

        auto&& s = ConfigMgr::get<string>('V', V_WINMODE, V_WINMODE_WINDOWED);
        if (smap.find(s) != smap.end())
            SystemData.windowMode = smap.at(s);
        else
            SystemData.windowMode = GameWindowMode::WINDOWED;
    }
}

static std::unordered_map<const char*, size_t> cache;

long long getTimerValue(std::string_view key)
{
    if (key.find('.') == key.npos)
        return TIMER_NEVER;

    int sep = key.find('.');
    if (sep + 1 == key.length())
        return TIMER_NEVER;

    std::string_view category(key.data(), sep);
    std::string_view type(key.data() + sep + 1);

    long long val = 0;
    if (category == "system")
    {
        val = SystemData.timers[type];
    }
    else if (category == "play")
    {
        if (type == "beat")
        {
            auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
            if (c)
            {
                val = int(1000 * (c->getCurrentMetre() * 4.0)) % 1000;
            }
        }
        else
        {
            val = PlayData.timers[type];
        }
    }
    else if (category == "select")
    {
        val = SelectData.timers[type];
    }
    else if (category == "result")
    {
        val = ResultData.timers[type];
    }
    else if (category == "arena")
    {
        val = ArenaData.timers[type];
    }
    else if (category == "keyconfig")
    {
        val = KeyConfigData.timers[type];
    }

    return val == 0 ? TIMER_NEVER : val;
}

namespace cfg
{
void loadFilterDifficulty()
{
    using std::string;
    static const std::map<string, FilterDifficultyType> smap =
    {
        {P_DIFFICULTY_FILTER_ALL, FilterDifficultyType::All},
        {P_DIFFICULTY_FILTER_BEGINNER, FilterDifficultyType::B},
        {P_DIFFICULTY_FILTER_NORMAL, FilterDifficultyType::N},
        {P_DIFFICULTY_FILTER_HYPER, FilterDifficultyType::H},
        {P_DIFFICULTY_FILTER_ANOTHER, FilterDifficultyType::A},
        {P_DIFFICULTY_FILTER_INSANE, FilterDifficultyType::I},
    };

    auto&& s = ConfigMgr::get<string>('P', P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL);
    if (smap.find(s) != smap.end())
        SelectData.filterDifficulty = smap.at(s);
    else
        SelectData.filterDifficulty = FilterDifficultyType::All;

}


void loadFilterKeys()
{
    using std::string;
    static const std::map<string, FilterKeysType> smap =
    {
        {P_FILTER_KEYS_ALL, FilterKeysType::All},
        {P_FILTER_KEYS_SINGLE,  FilterKeysType::Single},
        {P_FILTER_KEYS_7K,  FilterKeysType::_7},
        {P_FILTER_KEYS_5K,  FilterKeysType::_5},
        {P_FILTER_KEYS_DOUBLE, FilterKeysType::Double},
        {P_FILTER_KEYS_14K, FilterKeysType::_14},
        {P_FILTER_KEYS_10K, FilterKeysType::_10},
        {P_FILTER_KEYS_9K,  FilterKeysType::_9},
    };

    auto&& s = ConfigMgr::get<string>('P', P_FILTER_KEYS, P_FILTER_KEYS_ALL);
    if (smap.find(s) != smap.end())
        SelectData.filterKeys = smap.at(s);
    else
        SelectData.filterKeys = FilterKeysType::All;
}
}

std::pair<bool, LampType> getMaxSaveScoreType()
{
    // if (LR2CustomizeData.isInCustomize) return { false, LampType::NOPLAY };

    if (SystemData.freqType != FreqModifierType::Off && SystemData.freqVal < 0)
        return { false, LampType::NOPLAY };

    auto battleType = PlayData.battleType;
    if (battleType == PlayModifierBattleType::LocalBattle || battleType == PlayModifierBattleType::DoubleBattle)
        return { false, LampType::NOPLAY };

    const auto& player = PlayData.player[PLAYER_SLOT_PLAYER];
    if (player.mods.hispeedFix == PlayModifierHispeedFixType::CONSTANT)
        return { false, LampType::NOPLAY };

    bool isPlaymodeDP = PlayData.mode == SkinType::PLAY10 || PlayData.mode == SkinType::PLAY14;

    if (player.mods.randomLeft == PlayModifierRandomType::HRAN)
        return { false, LampType::ASSIST };
    else if (player.mods.randomLeft == PlayModifierRandomType::ALLSCR)
        return { false, LampType::NOPLAY };

    if (isPlaymodeDP)
    {
        if (player.mods.randomRight == PlayModifierRandomType::HRAN)
            return { false, LampType::ASSIST };
        else if (player.mods.randomRight == PlayModifierRandomType::ALLSCR)
            return { false, LampType::NOPLAY };
    }

    if (player.mods.assist_mask != 0)
        return { true, LampType::ASSIST };

    auto gaugeType = player.mods.gauge;
    auto lampType = LampType::FULLCOMBO; // FIXME change to PERFECT / MAX
    return { true, lampType };
}

void createNotification(StringContentView text)
{
    SystemData.overlayTextManager.newNotification(StringContent(text));
}


}