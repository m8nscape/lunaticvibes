#include "common/pch.h"
#include "data_shared.h"
#include "data_types.h"
#include "game/chart/chart.h"
#include "config/config_mgr.h"

namespace lunaticvibes::data
{

void loadConfigs()
{
    //void setNumbers()
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
    }

    //void setOptions()
    {
        using namespace cfg;
        using std::string;

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

        // play mode
        {
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

        // diff filter mode
        {
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

    //void setSwitches()
    {
        using namespace cfg;
        using std::string;

    }

    //void setText()
    {
        using namespace cfg;
        using std::string;

        // player displayName
        SystemData.playerName = ConfigMgr::Profile()->getName();

        // speed type
        {
            static const std::map<string, string> smap =
            {
                {P_SPEED_TYPE_NORMAL,   Option::s_speed_type[Option::SPEED_NORMAL]},
                {P_SPEED_TYPE_MIN,      Option::s_speed_type[Option::SPEED_FIX_MIN]},
                {P_SPEED_TYPE_MAX,      Option::s_speed_type[Option::SPEED_FIX_MAX]},
                {P_SPEED_TYPE_AVG,      Option::s_speed_type[Option::SPEED_FIX_AVG]},
                {P_SPEED_TYPE_CONSTANT, Option::s_speed_type[Option::SPEED_FIX_CONSTANT]},
                {P_SPEED_TYPE_INITIAL,  Option::s_speed_type[Option::SPEED_FIX_INITIAL]},
                {P_SPEED_TYPE_MAIN,     Option::s_speed_type[Option::SPEED_FIX_MAIN]},
            };

            auto&& s = ConfigMgr::get<string>('P', P_SPEED_TYPE, P_SPEED_TYPE_NORMAL);
            if (smap.find(s) != smap.end())
                State::set(IndexText::SCROLL_TYPE, smap.at(s));
            else
                State::set(IndexText::SCROLL_TYPE, Option::s_speed_type[Option::SPEED_NORMAL]);
        }

        // lanecovers
        {
            static const std::map<string, bool> smap =
            {
                {P_LANE_EFFECT_OP_OFF, false},
                {P_LANE_EFFECT_OP_HIDDEN, true},
                {P_LANE_EFFECT_OP_SUDDEN, true},
                {P_LANE_EFFECT_OP_SUDHID, true},
                {P_LANE_EFFECT_OP_LIFT, false},
                {P_LANE_EFFECT_OP_LIFTSUD, true},
            };

            auto&& s1 = ConfigMgr::get<string>('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_OFF);
            if (smap.find(s1) != smap.end())
            {
                State::set(IndexText::SHUTTER, smap.at(s1) ? "ON" : "OFF");
            }
            else
            {
                State::set(IndexText::SHUTTER, "OFF");
            }
        }

        // chart op
        {
            static const std::map<string, string> smap =
            {
                {P_CHART_OP_NORMAL, Option::s_random_type[Option::RAN_NORMAL]},
                {P_CHART_OP_MIRROR, Option::s_random_type[Option::RAN_MIRROR]},
                {P_CHART_OP_RANDOM, Option::s_random_type[Option::RAN_RANDOM]},
                {P_CHART_OP_SRAN,   Option::s_random_type[Option::RAN_SRAN]},
                {P_CHART_OP_HRAN,   Option::s_random_type[Option::RAN_HRAN]},
                {P_CHART_OP_ALLSCR, Option::s_random_type[Option::RAN_ALLSCR]},
            };

            auto&& s1 = ConfigMgr::get<string>('P', P_CHART_OP, P_CHART_OP_NORMAL);
            auto&& s2 = ConfigMgr::get<string>('P', P_CHART_OP_2P, P_CHART_OP_NORMAL);
            if (smap.find(s1) != smap.end())
            {
                State::set(IndexText::RANDOM_1P, smap.at(s1));
            }
            else
            {
                State::set(IndexText::RANDOM_1P, Option::s_random_type[Option::RAN_NORMAL]);
            }
            if (smap.find(s2) != smap.end())
            {
                State::set(IndexText::RANDOM_2P, smap.at(s2));
            }
            else
            {
                State::set(IndexText::RANDOM_2P, Option::s_random_type[Option::RAN_NORMAL]);
            }
        }

        // gauge op
        {
            static const std::map<string, string> smap =
            {
                {P_GAUGE_OP_NORMAL, Option::s_gauge_type[Option::GAUGE_NORMAL]},
                {P_GAUGE_OP_HARD,   Option::s_gauge_type[Option::GAUGE_HARD]},
                {P_GAUGE_OP_EASY,   Option::s_gauge_type[Option::GAUGE_EASY]},
                {P_GAUGE_OP_DEATH,  Option::s_gauge_type[Option::GAUGE_DEATH]},
                {P_GAUGE_OP_EXHARD, Option::s_gauge_type[Option::GAUGE_EXHARD]},
                {P_GAUGE_OP_ASSISTEASY, Option::s_gauge_type[Option::GAUGE_ASSISTEASY]},
            };

            auto&& s1 = ConfigMgr::get<string>('P', P_GAUGE_OP, P_GAUGE_OP_NORMAL);
            auto&& s2 = ConfigMgr::get<string>('P', P_GAUGE_OP_2P, P_GAUGE_OP_NORMAL);
            if (smap.find(s1) != smap.end())
            {
                State::set(IndexText::GAUGE_1P, smap.at(s1));
            }
            else
            {
                State::set(IndexText::GAUGE_1P, Option::s_gauge_type[Option::GAUGE_NORMAL]);
            }
            if (smap.find(s2) != smap.end())
            {
                State::set(IndexText::GAUGE_2P, smap.at(s2));
            }
            else
            {
                State::set(IndexText::GAUGE_2P, Option::s_gauge_type[Option::GAUGE_NORMAL]);
            }
        }

        // assist
        {
            static const std::map<string, string> smap =
            {
                {P_CHART_ASSIST_OP_NONE,    Option::s_assist_type[Option::ASSIST_NONE]},
                {P_CHART_ASSIST_OP_AUTOSCR, Option::s_assist_type[Option::ASSIST_AUTOSCR]},
            };

            auto&& s1 = ConfigMgr::get<string>('P', P_CHART_ASSIST_OP, P_CHART_ASSIST_OP_NONE);
            auto&& s2 = ConfigMgr::get<string>('P', P_CHART_ASSIST_OP_2P, P_CHART_ASSIST_OP_NONE);
            if (smap.find(s1) != smap.end())
            {
                State::set(IndexText::ASSIST_1P, smap.at(s1));
            }
            else
            {
                State::set(IndexText::ASSIST_1P, Option::s_assist_type[Option::ASSIST_NONE]);
            }
            if (smap.find(s2) != smap.end())
            {
                State::set(IndexText::ASSIST_2P, smap.at(s2));
            }
            else
            {
                State::set(IndexText::ASSIST_2P, Option::s_assist_type[Option::ASSIST_NONE]);
            }
        }

        // lane effect
        {
            static const std::map<string, string> smap =
            {
                {P_LANE_EFFECT_OP_OFF,     Option::s_lane_effect_type[Option::LANE_OFF]},
                {P_LANE_EFFECT_OP_HIDDEN,  Option::s_lane_effect_type[Option::LANE_HIDDEN]},
                {P_LANE_EFFECT_OP_SUDDEN,  Option::s_lane_effect_type[Option::LANE_SUDDEN]},
                {P_LANE_EFFECT_OP_SUDHID,  Option::s_lane_effect_type[Option::LANE_SUDHID]},
                {P_LANE_EFFECT_OP_LIFT,    Option::s_lane_effect_type[Option::LANE_LIFT]},
                {P_LANE_EFFECT_OP_LIFTSUD, Option::s_lane_effect_type[Option::LANE_LIFTSUD]},
            };

            auto&& s1 = ConfigMgr::get<string>('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_OFF);
            auto&& s2 = ConfigMgr::get<string>('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_OFF);
            if (smap.find(s1) != smap.end())
            {
                State::set(IndexText::EFFECT_1P, smap.at(s1));
            }
            else
            {
                State::set(IndexText::EFFECT_1P, Option::s_lane_effect_type[Option::LANE_OFF]);
            }
            if (smap.find(s2) != smap.end())
            {
                State::set(IndexText::EFFECT_2P, smap.at(s2));
            }
            else
            {
                State::set(IndexText::EFFECT_2P, Option::s_lane_effect_type[Option::LANE_OFF]);
            }
        }

        // ghost type
        {
            static const std::map<string, string> smap =
            {
                {P_GHOST_TYPE_OFF, Option::s_play_ghost_mode[Option::GHOST_OFF]},
                {P_GHOST_TYPE_A,   Option::s_play_ghost_mode[Option::GHOST_TOP]},
                {P_GHOST_TYPE_B,   Option::s_play_ghost_mode[Option::GHOST_SIDE]},
                {P_GHOST_TYPE_C,   Option::s_play_ghost_mode[Option::GHOST_SIDE_BOTTOM]},
            };

            auto&& s = ConfigMgr::get('P', P_GHOST_TYPE, P_GHOST_TYPE_OFF);
            if (smap.find(s) != smap.end())
                State::set(IndexText::GHOST, smap.at(s));
            else
                State::set(IndexText::GHOST, Option::s_play_ghost_mode[Option::GHOST_OFF]);
        }

        // target
        // target type
        {
            static const std::map<string, string> smap =
            {
                {P_TARGET_TYPE_0         , Option::s_target_type[Option::TARGET_0]},
                {P_TARGET_TYPE_MYBEST    , Option::s_target_type[Option::TARGET_MYBEST]},
                {P_TARGET_TYPE_AAA       , Option::s_target_type[Option::TARGET_AAA]},
                {P_TARGET_TYPE_AA        , Option::s_target_type[Option::TARGET_AA]},
                {P_TARGET_TYPE_A         , Option::s_target_type[Option::TARGET_A]},
                {P_TARGET_TYPE_DEFAULT   , Option::s_target_type[Option::TARGET_DEFAULT]},
                {P_TARGET_TYPE_IR_TOP    , Option::s_target_type[Option::TARGET_IR_TOP]},
                {P_TARGET_TYPE_IR_NEXT   , Option::s_target_type[Option::TARGET_IR_NEXT]},
                {P_TARGET_TYPE_IR_AVERAGE, Option::s_target_type[Option::TARGET_IR_AVERAGE]},
            };

            auto&& s = ConfigMgr::get<string>('P', P_TARGET_TYPE, P_TARGET_TYPE_DEFAULT);
            if (smap.find(s) != smap.end())
            {
                State::set(IndexText::TARGET_NAME, smap.at(s));
            }
            else
            {
                State::set(IndexText::TARGET_NAME, Option::s_target_type[Option::TARGET_DEFAULT]);
            }
        }

        // play mode
        {
            static const std::map<string, string> smap =
            {
                {P_FILTER_KEYS_ALL,    Option::s_filter_keys[Option::FILTER_KEYS_ALL]},
                {P_FILTER_KEYS_SINGLE, Option::s_filter_keys[Option::FILTER_KEYS_SINGLE]},
                {P_FILTER_KEYS_7K,     Option::s_filter_keys[Option::FILTER_KEYS_7]},
                {P_FILTER_KEYS_5K,     Option::s_filter_keys[Option::FILTER_KEYS_5]},
                {P_FILTER_KEYS_DOUBLE, Option::s_filter_keys[Option::FILTER_KEYS_DOUBLE]},
                {P_FILTER_KEYS_14K,    Option::s_filter_keys[Option::FILTER_KEYS_14]},
                {P_FILTER_KEYS_10K,    Option::s_filter_keys[Option::FILTER_KEYS_10]},
                {P_FILTER_KEYS_9K,     Option::s_filter_keys[Option::FILTER_KEYS_9]},
            };

            auto&& s = ConfigMgr::get<string>('P', P_FILTER_KEYS, P_FILTER_KEYS_ALL);
            if (smap.find(s) != smap.end())
                State::set(IndexText::FILTER_KEYS, smap.at(s));
            else
                State::set(IndexText::FILTER_KEYS, Option::s_filter_keys[Option::FILTER_KEYS_ALL]);
        }

        // sort mode
        {
            static const std::map<string, string> smap =
            {
                {P_SORT_MODE_FOLDER, Option::s_select_sort[Option::SORT_FOLDER]},
                {P_SORT_MODE_TITLE,  Option::s_select_sort[Option::SORT_TITLE]},
                {P_SORT_MODE_LEVEL,  Option::s_select_sort[Option::SORT_LEVEL]},
                {P_SORT_MODE_CLEAR,  Option::s_select_sort[Option::SORT_CLEAR]},
                {P_SORT_MODE_RATE,   Option::s_select_sort[Option::SORT_RATE]},
            };

            auto&& s = ConfigMgr::get<string>('P', P_SORT_MODE, P_SORT_MODE_FOLDER);
            if (smap.find(s) != smap.end())
                State::set(IndexText::SORT_MODE, smap.at(s));
            else
                State::set(IndexText::SORT_MODE, Option::s_select_sort[Option::SORT_FOLDER]);
        }

        // diff filter mode
        {
            static const std::map<string, string> smap =
            {
                {P_DIFFICULTY_FILTER_ALL,      Option::s_select_diff[Option::DIFF_ANY]},
                {P_DIFFICULTY_FILTER_BEGINNER, Option::s_select_diff[Option::DIFF_BEGINNER]},
                {P_DIFFICULTY_FILTER_NORMAL,   Option::s_select_diff[Option::DIFF_NORMAL]},
                {P_DIFFICULTY_FILTER_HYPER,    Option::s_select_diff[Option::DIFF_HYPER]},
                {P_DIFFICULTY_FILTER_ANOTHER,  Option::s_select_diff[Option::DIFF_ANOTHER]},
                {P_DIFFICULTY_FILTER_INSANE,   Option::s_select_diff[Option::DIFF_INSANE]},
            };

            auto&& s = ConfigMgr::get<string>('P', P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL);
            if (smap.find(s) != smap.end())
                State::set(IndexText::FILTER_DIFFICULTY, smap.at(s));
            else
                State::set(IndexText::FILTER_DIFFICULTY, Option::s_select_diff[Option::DIFF_ANY]);
        }

        // battle
        State::set(IndexText::BATTLE, Option::s_battle_type[State::get(IndexOption::PLAY_BATTLE_TYPE)]);

        // flip
        State::set(IndexText::FLIP, ConfigMgr::get('P', P_FLIP, true) ? "DP FLIP" : "OFF");

        // graph
        State::set(IndexText::SCORE_GRAPH, ConfigMgr::get('P', P_SCORE_GRAPH, true) ? "ON" : "OFF");

        // windowed
        {
            static const std::map<string, string> smap =
            {
                {V_WINMODE_FULL,       Option::s_windowed[Option::WIN_FULLSCREEN]},
                {V_WINMODE_BORDERLESS, Option::s_windowed[Option::WIN_BORDERLESS]},
                {V_WINMODE_WINDOWED,   Option::s_windowed[Option::WIN_WINDOWED]},
            };

            auto&& s = ConfigMgr::get<string>('V', V_WINMODE, V_WINMODE_WINDOWED);
            if (smap.find(s) != smap.end())
                State::set(IndexText::WINDOWMODE, smap.at(s));
            else
                State::set(IndexText::WINDOWMODE, Option::s_windowed[Option::WIN_WINDOWED]);
        }

        // vsync
        {
            static const std::map<int, string> smap =
            {
                {0, Option::s_vsync_mode[Option::VSYNC_OFF]},
                {1, Option::s_vsync_mode[Option::VSYNC_ON]},
                {2, Option::s_vsync_mode[Option::VSYNC_ADAPTIVE]}
            };

            auto&& s = ConfigMgr::get<int>('V', V_VSYNC, 0);
            if (smap.find(s) != smap.end())
                State::set(IndexText::VSYNC, smap.at(s));
            else
                State::set(IndexText::VSYNC, Option::s_vsync_mode[Option::VSYNC_OFF]);
        }

        // fixed tokens
        State::set(IndexText::COLOR_DEPTH, "WHAT DO YOU WANT FROM THIS");
        State::set(IndexText::JUDGE_AUTO, "NOT SUPPORTED");
        State::set(IndexText::REPLAY_SAVE_TYPE, "BEST EXSCORE");

        State::set(IndexText::TRIAL1, "NOT SUPPORTED");
        State::set(IndexText::TRIAL2, "THANK YOU FOR PLAYING!");
    }

}

long long getTimerValue(const std::string& key)
{
    if (key.find('.') == key.npos)
        return TIMER_NEVER;

    int sep = key.find('.');
    if (sep + 1 == key.length())
        return TIMER_NEVER;

    std::string_view category(key.c_str(), sep);
    std::string type(key.c_str() + sep + 1);

    int val = 0;
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

}