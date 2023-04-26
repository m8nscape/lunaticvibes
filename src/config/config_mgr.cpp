#include "common/pch.h"
#include "config_mgr.h"

#include "game/runtime/state.h"

namespace fs = std::filesystem;

void setNumbers()
{
    using namespace cfg;

    if (State::get(IndexNumber::HS_1P) == 0)
    {
        State::set(IndexNumber::HS_1P, ConfigMgr::get('P', cfg::P_HISPEED, 1.0) * 100);
    }
    if (State::get(IndexNumber::HS_2P) == 0)
    {
        State::set(IndexNumber::HS_2P, ConfigMgr::get('P', cfg::P_HISPEED_2P, 1.0) * 100);
    }

    State::set(IndexNumber::LANECOVER100_1P, ConfigMgr::get('P', P_LANECOVER_TOP, 0) / 10);
    State::set(IndexNumber::LANECOVER100_2P, ConfigMgr::get('P', P_LANECOVER_TOP, 0) / 10);

    State::set(IndexNumber::TIMING_ADJUST_VISUAL, ConfigMgr::get('P', P_JUDGE_OFFSET, 0));
    
    State::set(IndexNumber::DEFAULT_TARGET_RATE, ConfigMgr::get('P', P_GHOST_TARGET, 50));

    State::set(IndexNumber::VOLUME_MASTER, ConfigMgr::get('P', P_VOL_MASTER, 1.0) * 100);
    State::set(IndexNumber::VOLUME_KEY, ConfigMgr::get('P', P_VOL_KEY, 1.0) * 100);
    State::set(IndexNumber::VOLUME_BGM, ConfigMgr::get('P', P_VOL_BGM, 1.0) * 100);

    State::set(IndexNumber::FX0_P1, ConfigMgr::get('P', P_FX0_P1, 0));
    State::set(IndexNumber::FX0_P2, ConfigMgr::get('P', P_FX0_P2, 0));
    State::set(IndexNumber::FX1_P1, ConfigMgr::get('P', P_FX1_P1, 0));
    State::set(IndexNumber::FX1_P2, ConfigMgr::get('P', P_FX1_P2, 0));
    State::set(IndexNumber::FX2_P1, ConfigMgr::get('P', P_FX2_P1, 0));
    State::set(IndexNumber::FX2_P2, ConfigMgr::get('P', P_FX2_P2, 0));

    State::set(IndexNumber::EQ0, ConfigMgr::get('P', P_EQ0, 0));
    State::set(IndexNumber::EQ1, ConfigMgr::get('P', P_EQ1, 0));
    State::set(IndexNumber::EQ2, ConfigMgr::get('P', P_EQ2, 0));
    State::set(IndexNumber::EQ3, ConfigMgr::get('P', P_EQ3, 0));
    State::set(IndexNumber::EQ4, ConfigMgr::get('P', P_EQ4, 0));
    State::set(IndexNumber::EQ5, ConfigMgr::get('P', P_EQ5, 0));
    State::set(IndexNumber::EQ6, ConfigMgr::get('P', P_EQ6, 0));

    State::set(IndexNumber::PITCH, ConfigMgr::get('P', P_FREQ_VAL, 0));

    State::set(IndexNumber::NEW_ENTRY_SECONDS, ConfigMgr::get('P', P_NEW_SONG_DURATION, 0) * 60 * 60);
}

void setOptions()
{
    using namespace Option;
    using namespace cfg;
    using std::string;

    // bga
    {
        static const std::map<string, Option::e_bga_type> smap =
        {
            {P_BGA_TYPE_OFF, Option::BGA_OFF},
            {P_BGA_TYPE_ON, Option::BGA_ON},
            {P_BGA_TYPE_AUTOPLAY, Option::BGA_AUTOPLAY},
        };

        auto&& s = ConfigMgr::get<string>('P', P_BGA_TYPE, P_BGA_TYPE_ON);
        if (smap.find(s) != smap.end())
            State::set(IndexOption::PLAY_BGA_TYPE, smap.at(s));
        else
            State::set(IndexOption::PLAY_BGA_TYPE, Option::BGA_OFF);
    }
    {
        static const std::map<string, Option::e_bga_size> smap =
        {
            {P_BGA_SIZE_NORMAL, Option::BGA_NORMAL},
            {P_BGA_SIZE_EXTEND, Option::BGA_EXTEND},
        };

        auto&& s = ConfigMgr::get<string>('P', P_BGA_SIZE, P_BGA_SIZE_NORMAL);
        if (smap.find(s) != smap.end())
            State::set(IndexOption::PLAY_BGA_SIZE, smap.at(s));
        else
            State::set(IndexOption::PLAY_BGA_SIZE, Option::BGA_NORMAL);
    }


    // speed type
    {
        static const std::map<string, e_speed_type> smap =
        {
            {P_SPEED_TYPE_MIN, SPEED_FIX_MIN},
            {P_SPEED_TYPE_MAX, SPEED_FIX_MAX},
            {P_SPEED_TYPE_AVG, SPEED_FIX_AVG},
            {P_SPEED_TYPE_CONSTANT, SPEED_FIX_CONSTANT},
            {P_SPEED_TYPE_INITIAL, SPEED_FIX_INITIAL},
            {P_SPEED_TYPE_MAIN, SPEED_FIX_MAIN},
        };

        auto&& s = ConfigMgr::get<string>('P', P_SPEED_TYPE, P_SPEED_TYPE_NORMAL);
        if (smap.find(s) != smap.end())
        {
            State::set(IndexOption::PLAY_HSFIX_TYPE, smap.at(s));
        }
        else
        {
            State::set(IndexOption::PLAY_HSFIX_TYPE, SPEED_NORMAL);
        }
    }

    // target type
    {
        static const std::map<string, e_target_type> smap =
        {
            {P_TARGET_TYPE_0         , TARGET_0},
            {P_TARGET_TYPE_MYBEST    , TARGET_MYBEST},
            {P_TARGET_TYPE_AAA       , TARGET_AAA},
            {P_TARGET_TYPE_AA        , TARGET_AA},
            {P_TARGET_TYPE_A         , TARGET_A},
            {P_TARGET_TYPE_DEFAULT   , TARGET_DEFAULT},
            {P_TARGET_TYPE_IR_TOP    , TARGET_IR_TOP},
            {P_TARGET_TYPE_IR_NEXT   , TARGET_IR_NEXT},
            {P_TARGET_TYPE_IR_AVERAGE, TARGET_IR_AVERAGE},
        };

        auto&& s = ConfigMgr::get<string>('P', P_TARGET_TYPE, P_TARGET_TYPE_DEFAULT);
        if (smap.find(s) != smap.end())
        {
            State::set(IndexOption::PLAY_TARGET_TYPE, smap.at(s));
        }
        else
        {
            State::set(IndexOption::PLAY_TARGET_TYPE, TARGET_MYBEST);
        }
    }

    // chart op
    {
        static const std::map<string, e_random_type> smap =
        {
            {P_CHART_OP_MIRROR, RAN_MIRROR},
            {P_CHART_OP_RANDOM, RAN_RANDOM},
            {P_CHART_OP_SRAN, RAN_SRAN},
            {P_CHART_OP_HRAN, RAN_HRAN},
            {P_CHART_OP_ALLSCR, RAN_ALLSCR},
            {P_CHART_OP_RRAN, RAN_RRAN},
            {P_CHART_OP_DB_SYNCHRONIZE, RAN_DB_SYNCHRONIZE_RANDOM},
            {P_CHART_OP_DB_SYMMETRY, RAN_DB_SYMMETRY_RANDOM},
        };

        auto&& s1 = ConfigMgr::get<string>('P', P_CHART_OP, P_CHART_OP_NORMAL);
        auto&& s2 = ConfigMgr::get<string>('P', P_CHART_OP_2P, P_CHART_OP_NORMAL);
        if (smap.find(s1) != smap.end())
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_1P, smap.at(s1));
        }
        else
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_1P, RAN_NORMAL);
        }
        if (smap.find(s2) != smap.end())
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_2P, smap.at(s2));
        }
        else
        {
            State::set(IndexOption::PLAY_RANDOM_TYPE_2P, RAN_NORMAL);
        }
    }

    // gauge op
    {
        static const std::map<string, e_gauge_type> smap =
        {
            {P_GAUGE_OP_HARD, GAUGE_HARD},
            {P_GAUGE_OP_EASY, GAUGE_EASY},
            {P_GAUGE_OP_DEATH, GAUGE_DEATH},
            {P_GAUGE_OP_EXHARD, GAUGE_EXHARD},
            {P_GAUGE_OP_ASSISTEASY, GAUGE_ASSISTEASY},
        };

        auto&& s1 = ConfigMgr::get<string>('P', P_GAUGE_OP, P_GAUGE_OP_NORMAL);
        auto&& s2 = ConfigMgr::get<string>('P', P_GAUGE_OP_2P, P_GAUGE_OP_NORMAL);
        if (smap.find(s1) != smap.end())
        {
            State::set(IndexOption::PLAY_GAUGE_TYPE_1P, smap.at(s1));
        }
        else
        {
            State::set(IndexOption::PLAY_GAUGE_TYPE_1P, GAUGE_NORMAL);
        }
        if (smap.find(s2) != smap.end())
        {
            State::set(IndexOption::PLAY_GAUGE_TYPE_2P, smap.at(s2));
        }
        else
        {
            State::set(IndexOption::PLAY_GAUGE_TYPE_2P, GAUGE_NORMAL);
        }
    }

    // lane effect
    {
        static const std::map<string, e_lane_effect_type> smap =
        {
            {P_LANE_EFFECT_OP_OFF, LANE_OFF},
            {P_LANE_EFFECT_OP_HIDDEN, LANE_HIDDEN},
            {P_LANE_EFFECT_OP_SUDDEN, LANE_SUDDEN},
            {P_LANE_EFFECT_OP_SUDHID, LANE_SUDHID},
            {P_LANE_EFFECT_OP_LIFT, LANE_LIFT},
            {P_LANE_EFFECT_OP_LIFTSUD, LANE_LIFTSUD},
        };

        auto&& s1 = ConfigMgr::get<string>('P', P_LANE_EFFECT_OP, P_LANE_EFFECT_OP_OFF);
        auto&& s2 = ConfigMgr::get<string>('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_OFF);
        if (smap.find(s1) != smap.end())
        {
            State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, smap.at(s1));
        }
        else
        {
            State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_1P, LANE_OFF);
        }
        if (smap.find(s2) != smap.end())
        {
            State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, smap.at(s2));
        }
        else
        {
            State::set(IndexOption::PLAY_LANE_EFFECT_TYPE_2P, LANE_OFF);
        }
    }

    // ghost type
    {
        static const std::map<string, e_play_ghost_mode> smap =
        {
            {P_GHOST_TYPE_A, GHOST_TOP},
            {P_GHOST_TYPE_B, GHOST_SIDE},
            {P_GHOST_TYPE_C, GHOST_SIDE_BOTTOM},
        };

        auto&& s1 = ConfigMgr::get<string>('P', P_GHOST_TYPE, "OFF");
        auto&& s2 = ConfigMgr::get<string>('P', P_GHOST_TYPE_2P, "OFF");
        if (smap.find(s1) != smap.end())
        {
            State::set(IndexOption::PLAY_GHOST_TYPE_1P, smap.at(s1));
        }
        else
        {
            State::set(IndexOption::PLAY_GHOST_TYPE_1P, GHOST_OFF);
        }
        if (smap.find(s2) != smap.end())
        {
            State::set(IndexOption::PLAY_GHOST_TYPE_2P, smap.at(s2));
        }
        else
        {
            State::set(IndexOption::PLAY_GHOST_TYPE_2P, GHOST_OFF);
        }
    }

    // play mode
    {
        static const std::map<string, e_filter_keys> smap =
        {
            {P_FILTER_KEYS_ALL, FILTER_KEYS_ALL},
            {P_FILTER_KEYS_SINGLE,  FILTER_KEYS_SINGLE},
            {P_FILTER_KEYS_7K,  FILTER_KEYS_7},
            {P_FILTER_KEYS_5K,  FILTER_KEYS_5},
            {P_FILTER_KEYS_DOUBLE, FILTER_KEYS_DOUBLE},
            {P_FILTER_KEYS_14K, FILTER_KEYS_14},
            {P_FILTER_KEYS_10K, FILTER_KEYS_10},
            {P_FILTER_KEYS_9K,  FILTER_KEYS_9},
        };

        auto&& s = ConfigMgr::get<string>('P', P_FILTER_KEYS, P_FILTER_KEYS_ALL);
        if (smap.find(s) != smap.end())
            State::set(IndexOption::SELECT_FILTER_KEYS, smap.at(s));
        else
            State::set(IndexOption::SELECT_FILTER_KEYS, KEYS_ALL);
    }

    // sort mode
    {
        static const std::map<string, e_select_sort> smap =
        {
            {P_SORT_MODE_FOLDER, SORT_FOLDER},
            {P_SORT_MODE_TITLE, SORT_TITLE},
            {P_SORT_MODE_LEVEL, SORT_LEVEL},
            {P_SORT_MODE_CLEAR, SORT_CLEAR},
            {P_SORT_MODE_RATE, SORT_RATE},
        };

        auto&& s = ConfigMgr::get<string>('P', P_SORT_MODE, P_SORT_MODE_FOLDER);
        if (smap.find(s) != smap.end())
            State::set(IndexOption::SELECT_SORT, smap.at(s));
        else
            State::set(IndexOption::SELECT_SORT, SORT_FOLDER);
    }

    // diff filter mode
    {
        static const std::map<string, e_select_diff> smap =
        {
            {P_DIFFICULTY_FILTER_ALL, DIFF_ANY},
            {P_DIFFICULTY_FILTER_BEGINNER, DIFF_BEGINNER},
            {P_DIFFICULTY_FILTER_NORMAL, DIFF_NORMAL},
            {P_DIFFICULTY_FILTER_HYPER, DIFF_HYPER},
            {P_DIFFICULTY_FILTER_ANOTHER, DIFF_ANOTHER},
            {P_DIFFICULTY_FILTER_INSANE, DIFF_INSANE},
        };

        auto&& s = ConfigMgr::get<string>('P', P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL);
        if (smap.find(s) != smap.end())
            State::set(IndexOption::SELECT_FILTER_DIFF, smap.at(s));
        else
            State::set(IndexOption::SELECT_FILTER_DIFF, DIFF_ANY);
    }

    // freq type
    {
        static const std::map<string, e_freq_type> smap =
        {
            {P_FREQ_TYPE_FREQ, FREQ_FREQ},
            {P_FREQ_TYPE_PITCH, FREQ_PITCH},
            {P_FREQ_TYPE_SPEED, FREQ_SPEED},
        };

        auto&& s = ConfigMgr::get<string>('P', P_FREQ_TYPE, P_FREQ_TYPE_FREQ);
        if (smap.find(s) != smap.end())
            State::set(IndexOption::SOUND_PITCH_TYPE, smap.at(s));
        else
            State::set(IndexOption::SOUND_PITCH_TYPE, FREQ_FREQ);
    }

    // fx target
    {
        static const std::map<string, e_fx_target> smap =
        {
            {P_FX_TARGET_MASTER, FX_MASTER},
            {P_FX_TARGET_KEY, FX_KEY},
            {P_FX_TARGET_BGM, FX_BGM},
        };

        auto&& s0 = ConfigMgr::get<string>('P', P_FX0_TARGET, P_FX_TARGET_MASTER);
        if (smap.find(s0) != smap.end())
            State::set(IndexOption::SOUND_TARGET_FX0, smap.at(s0));
        else
            State::set(IndexOption::SOUND_TARGET_FX0, FX_MASTER);

        auto&& s1 = ConfigMgr::get<string>('P', P_FX1_TARGET, P_FX_TARGET_MASTER);
        if (smap.find(s1) != smap.end())
            State::set(IndexOption::SOUND_TARGET_FX1, smap.at(s1));
        else
            State::set(IndexOption::SOUND_TARGET_FX1, FX_MASTER);

        auto&& s2 = ConfigMgr::get<string>('P', P_FX2_TARGET, P_FX_TARGET_MASTER);
        if (smap.find(s2) != smap.end())
            State::set(IndexOption::SOUND_TARGET_FX2, smap.at(s2));
        else
            State::set(IndexOption::SOUND_TARGET_FX2, FX_MASTER);
    }

    // fx type
    {
        static const std::map<string, e_fx_type> smap =
        {
            {P_FX_TYPE_REVERB, FX_REVERB},
            {P_FX_TYPE_DELAY, FX_DELAY},
            {P_FX_TYPE_LOWPASS, FX_LOWPASS},
            {P_FX_TYPE_HIGHPASS, FX_HIGHPASS},
            {P_FX_TYPE_FLANGER, FX_FLANGER},
            {P_FX_TYPE_CHORUS, FX_CHORUS},
            {P_FX_TYPE_DIST, FX_DISTORTION},
        };

        auto&& s0 = ConfigMgr::get<string>('P', P_FX0_TYPE, "OFF");
        if (smap.find(s0) != smap.end())
            State::set(IndexOption::SOUND_FX0, smap.at(s0));
        else
            State::set(IndexOption::SOUND_FX0, FX_OFF);

        auto&& s1 = ConfigMgr::get<string>('P', P_FX1_TYPE, "OFF");
        if (smap.find(s1) != smap.end())
            State::set(IndexOption::SOUND_FX1, smap.at(s1));
        else
            State::set(IndexOption::SOUND_FX1, FX_OFF);

        auto&& s2 = ConfigMgr::get<string>('P', P_FX2_TYPE, "OFF");
        if (smap.find(s2) != smap.end())
            State::set(IndexOption::SOUND_FX2, smap.at(s2));
        else
            State::set(IndexOption::SOUND_FX2, FX_OFF);
    }

    // windowed
    {
        static const std::map<string, e_windowed> smap =
        {
            {V_WINMODE_FULL, WIN_FULLSCREEN},
            {V_WINMODE_BORDERLESS, WIN_BORDERLESS},
            {V_WINMODE_WINDOWED, WIN_WINDOWED}
        };

        auto&& s = ConfigMgr::get<string>('V', V_WINMODE, V_WINMODE_WINDOWED);
        if (smap.find(s) != smap.end())
            State::set(IndexOption::SYS_WINDOWED, smap.at(s));
        else
            State::set(IndexOption::SYS_WINDOWED, WIN_WINDOWED);
    }
}

void setSliders()
{
    using namespace cfg;

    State::set(IndexSlider::VOLUME_MASTER, ConfigMgr::get('P', P_VOL_MASTER, 1.0));
    State::set(IndexSlider::VOLUME_KEY, ConfigMgr::get('P', P_VOL_KEY, 1.0));
    State::set(IndexSlider::VOLUME_BGM, ConfigMgr::get('P', P_VOL_BGM, 1.0));

    State::set(IndexSlider::EQ0, ((ConfigMgr::get('P', P_EQ0, 0) + 12) / 24.0));
    State::set(IndexSlider::EQ1, ((ConfigMgr::get('P', P_EQ1, 0) + 12) / 24.0));
    State::set(IndexSlider::EQ2, ((ConfigMgr::get('P', P_EQ2, 0) + 12) / 24.0));
    State::set(IndexSlider::EQ3, ((ConfigMgr::get('P', P_EQ3, 0) + 12) / 24.0));
    State::set(IndexSlider::EQ4, ((ConfigMgr::get('P', P_EQ4, 0) + 12) / 24.0));
    State::set(IndexSlider::EQ5, ((ConfigMgr::get('P', P_EQ5, 0) + 12) / 24.0));
    State::set(IndexSlider::EQ6, ((ConfigMgr::get('P', P_EQ6, 0) + 12) / 24.0));

    State::set(IndexSlider::PITCH, ((ConfigMgr::get('P', P_FREQ_VAL, 0) + 12) / 24.0));

    State::set(IndexSlider::FX0_P1, ConfigMgr::get('P', P_FX0_P1, 0) / 100.0);
    State::set(IndexSlider::FX0_P2, ConfigMgr::get('P', P_FX0_P2, 0) / 100.0);
    State::set(IndexSlider::FX1_P1, ConfigMgr::get('P', P_FX1_P1, 0) / 100.0);
    State::set(IndexSlider::FX1_P2, ConfigMgr::get('P', P_FX1_P2, 0) / 100.0);
    State::set(IndexSlider::FX2_P1, ConfigMgr::get('P', P_FX2_P1, 0) / 100.0);
    State::set(IndexSlider::FX2_P2, ConfigMgr::get('P', P_FX2_P2, 0) / 100.0);
}

void setSwitches()
{
    using namespace cfg;
    using std::string;

    // lane effect
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
        auto&& s2 = ConfigMgr::get<string>('P', P_LANE_EFFECT_OP_2P, P_LANE_EFFECT_OP_OFF);
        if (smap.find(s1) != smap.end())
        {
            State::set(IndexSwitch::P1_LANECOVER_ENABLED, smap.at(s1));
        }
        else
        {
            State::set(IndexSwitch::P1_LANECOVER_ENABLED, false);
        }
        if (smap.find(s2) != smap.end())
        {
            State::set(IndexSwitch::P2_LANECOVER_ENABLED, smap.at(s2));
        }
        else
        {
            State::set(IndexSwitch::P2_LANECOVER_ENABLED, false);
        }
    }


    State::set(IndexSwitch::PLAY_OPTION_DP_FLIP, ConfigMgr::get('P', P_FLIP, true));
    State::set(IndexSwitch::SYSTEM_SCOREGRAPH, ConfigMgr::get('P', P_SCORE_GRAPH, true));
    State::set(IndexSwitch::SOUND_VOLUME, true);
    State::set(IndexSwitch::SOUND_EQ, ConfigMgr::get('P', P_EQ, true));
    State::set(IndexSwitch::SOUND_PITCH, ConfigMgr::get('P', P_FREQ, true));
    State::set(IndexSwitch::SOUND_FX0, ConfigMgr::get('P', P_FX0, true));
    State::set(IndexSwitch::SOUND_FX1, ConfigMgr::get('P', P_FX1, true));
    State::set(IndexSwitch::SOUND_FX2, ConfigMgr::get('P', P_FX2, true));

    State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_1P, ConfigMgr::get<string>('P', P_CHART_ASSIST_OP, P_CHART_ASSIST_OP_NONE) == P_CHART_ASSIST_OP_AUTOSCR);
    State::set(IndexSwitch::PLAY_OPTION_AUTOSCR_2P, ConfigMgr::get<string>('P', P_CHART_ASSIST_OP_2P, P_CHART_ASSIST_OP_NONE) == P_CHART_ASSIST_OP_AUTOSCR);
}

void setText()
{
    using namespace cfg;
    using std::string;

    // player displayName
    State::set(IndexText::PLAYER_NAME, ConfigMgr::Profile()->getName());

    // bga
    {
        {
            static const std::map<string, string> smap =
            {
                {P_BGA_TYPE_OFF,      Option::s_bga_type[Option::BGA_OFF]},
                {P_BGA_TYPE_ON,       Option::s_bga_type[Option::BGA_ON]},
                {P_BGA_TYPE_AUTOPLAY, Option::s_bga_type[Option::BGA_AUTOPLAY]},
            };

            auto&& s = ConfigMgr::get<string>('P', P_BGA_TYPE, P_BGA_TYPE_ON);
            if (smap.find(s) != smap.end())
                State::set(IndexText::BGA, smap.at(s));
            else
                State::set(IndexText::BGA, Option::s_bga_type[Option::BGA_OFF]);
        }
        {
            static const std::map<string, string> smap =
            {
                {P_BGA_SIZE_NORMAL, Option::s_bga_size[Option::BGA_NORMAL]},
                {P_BGA_SIZE_EXTEND, Option::s_bga_size[Option::BGA_EXTEND]},
            };

            auto&& s = ConfigMgr::get<string>('P', P_BGA_SIZE, P_BGA_SIZE_NORMAL);
            if (smap.find(s) != smap.end())
                State::set(IndexText::BGA_SIZE, smap.at(s));
            else
                State::set(IndexText::BGA_SIZE, Option::s_bga_size[Option::BGA_NORMAL]);
        }
    }

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

int ConfigMgr::_selectProfile(const std::string& name)
{
    Path folder = Path(GAMEDATA_PATH) / "profile" / name;
    if (!fs::exists(folder))
        fs::create_directories(folder);

    if (!fs::is_directory(folder))
    {
        LOG_WARNING << "[Config] Bad profile: " << name;
        return 2;
    }

    auto createFile = [](Path&& p) {
        if (fs::exists(p))
        {
            if (!fs::is_regular_file(p))
            {
                LOG_WARNING << "[Config] File is not regular file: " << p.u8string();
                return true;
            }
        }
        else
        {
            std::ofstream(p).close();
        }
        return false;
    };

    if (createFile(folder / CONFIG_FILE_INPUT_5) ||
        createFile(folder / CONFIG_FILE_INPUT_7) ||
        createFile(folder / CONFIG_FILE_INPUT_9) ||
        createFile(folder / CONFIG_FILE_SKIN) ||
        createFile(folder / CONFIG_FILE_PROFILE))
    {
        LOG_WARNING << "[Config] Bad profile: " << name;
        return 3;
    }

    {
        std::unique_lock l(getInst()._mutex);

        try
        {
            P = std::make_shared<ConfigProfile>(name);
            I5 = std::make_shared<ConfigInput>(name, 5);
            I7 = std::make_shared<ConfigInput>(name, 7);
            I9 = std::make_shared<ConfigInput>(name, 9);
            S = std::make_shared<ConfigSkin>(name);
        }
        catch (YAML::BadFile&)
        {
            LOG_WARNING << "[Config] Bad profile: " << name;
            return 1;
        }
    }

    load();

    profileName = name;
    G->set(cfg::E_PROFILE, profileName);

    return 0;
}

int ConfigMgr::_createProfile(const std::string& newProfile, const std::string& oldProfile)
{
    Path newFolder = Path(GAMEDATA_PATH) / "profile" / newProfile;
    if (fs::exists(newFolder))
    {
        LOG_WARNING << "[Config] Profile name duplicate: " << newProfile;
        return 2;
    }

    fs::create_directories(newFolder);

    Path oldFolder = Path(GAMEDATA_PATH) / "profile" / oldProfile;
    if (!oldProfile.empty() && fs::exists(oldFolder))
    {
        for (auto& f : fs::directory_iterator(oldFolder))
        {
            if (strEqual(f.path().extension().u8string(), ".yml", true) ||
                strEqual(f.path().filename().u8string(), "customize", true))
            {
                fs::copy(f, newFolder / f.path().lexically_relative(oldFolder));
            }
        }
    }

    ConfigProfile p(newProfile);
    p.setDefaults();
    p.set(cfg::P_PLAYERNAME, newProfile);
    p.save();

    return 0;
}

void ConfigMgr::_setGlobals()
{
    std::shared_lock l(getInst()._mutex);

    setNumbers();
    setOptions();
    setSliders();
    setSwitches();
    setText();
}