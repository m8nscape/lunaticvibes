#include "config_mgr.h"

#include <filesystem>
#include <fstream>
#include <plog/Log.h>

#include "game/data/data.h"

namespace fs = std::filesystem;

std::shared_ptr<ConfigGeneral>     ConfigMgr::G;
std::shared_ptr<ConfigProfile>     ConfigMgr::P;
std::shared_ptr<ConfigInput>       ConfigMgr::I5;
std::shared_ptr<ConfigInput>       ConfigMgr::I7;
std::shared_ptr<ConfigInput>       ConfigMgr::I9;
std::shared_ptr<ConfigSkin>        ConfigMgr::S;

void setNumbers()
{
    const auto& p = ConfigMgr::P;
    using e = eNumber;
    auto& g = gNumbers;
    using namespace cfg;

    g.set(e::HS_1P, p->get(P_HISPEED, 1.0) * 100);
    g.set(e::HS_2P, p->get(P_HISPEED, 1.0) * 100);

    g.set(e::LANECOVER_1P, p->get(P_LANECOVER, 0));
    g.set(e::LANECOVER_2P, p->get(P_LANECOVER, 0));

    g.set(e::TIMING_ADJUST_VISUAL, p->get(P_JUDGE_OFFSET, 0));
    
    g.set(e::DEFAULT_TARGET_RATE, p->get(P_GHOST_TARGET, 50));

    g.set(e::VOLUME_MASTER, p->get(P_VOL_MASTER, 1.0) * 100);
    g.set(e::VOLUME_KEY, p->get(P_VOL_KEY, 1.0) * 100);
    g.set(e::VOLUME_BGM, p->get(P_VOL_BGM, 1.0) * 100);

    g.set(e::FX0_P1, p->get(P_FX0_P1, 0));
    g.set(e::FX0_P2, p->get(P_FX0_P2, 0));
    g.set(e::FX1_P1, p->get(P_FX1_P1, 0));
    g.set(e::FX1_P2, p->get(P_FX1_P2, 0));
    g.set(e::FX2_P1, p->get(P_FX2_P1, 0));
    g.set(e::FX2_P2, p->get(P_FX2_P2, 0));

    g.set(e::PITCH, p->get(P_FREQ_VAL, 0));
}

void setOptions()
{
    const auto& p = ConfigMgr::P;
    const auto& c = ConfigMgr::G;
    using e = eOption;
    auto& g = gOptions;
    using namespace Option;
    using namespace cfg;
    using std::string;

    // bga
    g.set(e::PLAY_BGA_TYPE, p->get<string>(P_LOAD_BGA, cfg::ON) == cfg::ON ? BGA_NORMAL : BGA_OFF);

    // speed type
    {
        static const std::map<string, e_speed_type> smap =
        {
            {P_SPEED_TYPE_MIN, SPEED_FIX_MIN},
            {P_SPEED_TYPE_MAX, SPEED_FIX_MAX},
            {P_SPEED_TYPE_AVG, SPEED_FIX_AVG},
            {P_SPEED_TYPE_CONSTANT, SPEED_FIX_CONSTANT},
        };

        auto&& s = p->get<string>(P_SPEED_TYPE, P_SPEED_TYPE_NORMAL);
        if (smap.find(s) != smap.end())
        {
            g.set(e::PLAY_HSFIX_TYPE_1P, smap.at(s));
            g.set(e::PLAY_HSFIX_TYPE_2P, smap.at(s));
        }
        else
        {
            g.set(e::PLAY_HSFIX_TYPE_1P, SPEED_NORMAL);
            g.set(e::PLAY_HSFIX_TYPE_2P, SPEED_NORMAL);
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
        };

        auto&& s = p->get<string>(P_CHART_OP, P_CHART_OP_NORMAL);
        if (smap.find(s) != smap.end())
        {
            g.set(e::PLAY_RANDOM_TYPE_1P, smap.at(s));
            g.set(e::PLAY_RANDOM_TYPE_2P, smap.at(s));
        }
        else
        {
            g.set(e::PLAY_RANDOM_TYPE_1P, RAN_NORMAL);
            g.set(e::PLAY_RANDOM_TYPE_2P, RAN_NORMAL);
        }
    }

    // gauge op
    {
        static const std::map<string, e_gauge_type> smap =
        {
            {P_GAUGE_OP_HARD, GAUGE_HARD},
            {P_GAUGE_OP_EASY, GAUGE_EASY},
            {P_GAUGE_OP_DEATH, GAUGE_DEATH},
        };

        auto&& s = p->get<string>(P_GAUGE_OP, P_GAUGE_OP);
        if (smap.find(s) != smap.end())
        {
            g.set(e::PLAY_GAUGE_TYPE_1P, smap.at(s));
            g.set(e::PLAY_GAUGE_TYPE_2P, smap.at(s));
        }
        else
        {
            g.set(e::PLAY_GAUGE_TYPE_1P, GAUGE_NORMAL);
            g.set(e::PLAY_GAUGE_TYPE_2P, GAUGE_NORMAL);
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

        auto&& s = p->get<string>(P_GHOST_TYPE, cfg::OFF);
        if (smap.find(s) != smap.end())
        {
            g.set(e::PLAY_GHOST_TYPE_1P, smap.at(s));
            g.set(e::PLAY_GHOST_TYPE_2P, smap.at(s));
        }
        else
        {
            g.set(e::PLAY_GHOST_TYPE_1P, GHOST_OFF);
            g.set(e::PLAY_GHOST_TYPE_2P, GHOST_OFF);
        }
    }

    // play mode
    {
        static const std::map<string, e_play_keys> smap =
        {
            {P_PLAY_MODE_ALL, KEYS_ALL},
            {P_PLAY_MODE_7K, KEYS_7},
            {P_PLAY_MODE_5K, KEYS_5},
            {P_PLAY_MODE_14K, KEYS_14},
            {P_PLAY_MODE_10K, KEYS_10},
            {P_PLAY_MODE_9K, KEYS_9},
        };

        auto&& s = p->get<string>(P_PLAY_MODE, P_PLAY_MODE_ALL);
        if (smap.find(s) != smap.end())
            g.set(e::SELECT_FILTER_KEYS, smap.at(s));
        else
            g.set(e::SELECT_FILTER_KEYS, KEYS_ALL);
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

        auto&& s = p->get<string>(P_SORT_MODE, P_SORT_MODE_FOLDER);
        if (smap.find(s) != smap.end())
            g.set(e::SELECT_SORT, smap.at(s));
        else
            g.set(e::SELECT_SORT, SORT_FOLDER);
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

        auto&& s = p->get<string>(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL);
        if (smap.find(s) != smap.end())
            g.set(e::SELECT_FILTER_DIFF, smap.at(s));
        else
            g.set(e::SELECT_FILTER_DIFF, DIFF_ANY);
    }

    // freq type
    {
        static const std::map<string, e_freq_type> smap =
        {
            {P_FREQ_TYPE_FREQ, FREQ_FREQ},
            {P_FREQ_TYPE_PITCH, FREQ_PITCH},
            {P_FREQ_TYPE_SPEED, FREQ_SPEED},
        };

        auto&& s = p->get<string>(P_FREQ_TYPE, P_FREQ_TYPE_FREQ);
        if (smap.find(s) != smap.end())
            g.set(e::SOUND_PITCH_TYPE, smap.at(s));
        else
            g.set(e::SOUND_PITCH_TYPE, FREQ_FREQ);
    }

    // fx target
    {
        static const std::map<string, e_fx_target> smap =
        {
            {P_FX_TARGET_MASTER, FX_MASTER},
            {P_FX_TARGET_KEY, FX_KEY},
            {P_FX_TARGET_BGM, FX_BGM},
        };

        auto&& s0 = p->get<string>(P_FX0_TARGET, P_FX_TARGET_MASTER);
        if (smap.find(s0) != smap.end())
            g.set(e::SOUND_TARGET_FX0, smap.at(s0));
        else
            g.set(e::SOUND_TARGET_FX0, FX_MASTER);

        auto&& s1 = p->get<string>(P_FX1_TARGET, P_FX_TARGET_MASTER);
        if (smap.find(s1) != smap.end())
            g.set(e::SOUND_TARGET_FX1, smap.at(s1));
        else
            g.set(e::SOUND_TARGET_FX1, FX_MASTER);

        auto&& s2 = p->get<string>(P_FX2_TARGET, P_FX_TARGET_MASTER);
        if (smap.find(s2) != smap.end())
            g.set(e::SOUND_TARGET_FX2, smap.at(s2));
        else
            g.set(e::SOUND_TARGET_FX2, FX_MASTER);
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

        auto&& s0 = p->get<string>(P_FX0_TYPE, cfg::OFF);
        if (smap.find(s0) != smap.end())
            g.set(e::SOUND_FX0, smap.at(s0));
        else
            g.set(e::SOUND_FX0, FX_OFF);

        auto&& s1 = p->get<string>(P_FX1_TYPE, cfg::OFF);
        if (smap.find(s1) != smap.end())
            g.set(e::SOUND_FX1, smap.at(s1));
        else
            g.set(e::SOUND_FX1, FX_OFF);

        auto&& s2 = p->get<string>(P_FX2_TYPE, cfg::OFF);
        if (smap.find(s2) != smap.end())
            g.set(e::SOUND_FX2, smap.at(s2));
        else
            g.set(e::SOUND_FX2, FX_OFF);
    }

    // battle
    g.set(e::PLAY_BATTLE_TYPE, 0);

    // windowed
    {
        static const std::map<string, e_windowed> smap =
        {
            {V_WINMODE_FULL, WIN_FULLSCREEN},
            {V_WINMODE_BORDERLESS, WIN_BORDERLESS},
            {V_WINMODE_WINDOWED, WIN_WINDOWED}
        };

        auto&& s = c->get<string>(V_WINMODE, V_WINMODE_WINDOWED);
        if (smap.find(s) != smap.end())
            g.set(e::SYS_WINDOWED, smap.at(s));
        else
            g.set(e::SYS_WINDOWED, WIN_WINDOWED);
    }
}

void setSliders()
{
    const auto& p = ConfigMgr::P;
    using e = eSlider;
    auto& g = gSliders;
    using namespace cfg;

    g.set(e::VOLUME_MASTER, p->get(P_VOL_MASTER, 1.0));
    g.set(e::VOLUME_KEY, p->get(P_VOL_KEY, 1.0));
    g.set(e::VOLUME_BGM, p->get(P_VOL_BGM, 1.0));

    g.set(e::EQ0, (p->get(P_EQ0, 0.0) + 1.0) / 2);
    g.set(e::EQ1, (p->get(P_EQ1, 0.0) + 1.0) / 2);
    g.set(e::EQ2, (p->get(P_EQ2, 0.0) + 1.0) / 2);
    g.set(e::EQ3, (p->get(P_EQ3, 0.0) + 1.0) / 2);
    g.set(e::EQ4, (p->get(P_EQ4, 0.0) + 1.0) / 2);
    g.set(e::EQ5, (p->get(P_EQ5, 0.0) + 1.0) / 2);
    g.set(e::EQ6, (p->get(P_EQ6, 0.0) + 1.0) / 2);

    g.set(e::PITCH, (p->get(P_FREQ_VAL, 0.0) + 1.0) / 2);

    g.set(e::FX0_P1, p->get(P_FX0_P1, 0.0));
    g.set(e::FX0_P2, p->get(P_FX0_P2, 0.0));
    g.set(e::FX1_P1, p->get(P_FX1_P1, 0.0));
    g.set(e::FX1_P2, p->get(P_FX1_P2, 0.0));
    g.set(e::FX2_P1, p->get(P_FX2_P1, 0.0));
    g.set(e::FX2_P2, p->get(P_FX2_P2, 0.0));

}

void setSwitches()
{
    const auto& p = ConfigMgr::P;
    const auto& c = ConfigMgr::G;
    using e = eSwitch;
    auto& g = gSwitches;
    using namespace cfg;
    using std::string;

    g.set(e::SYSTEM_BGA, p->get<string>(P_LOAD_BGA, ON) == ON);
    g.set(e::PLAY_OPTION_DP_FLIP, p->get<string>(P_FLIP, ON) == ON);
    g.set(e::SYSTEM_SCOREGRAPH, p->get<string>(P_SCORE_GRAPH, ON) == ON);
    g.set(e::SOUND_VOLUME, true);
    g.set(e::SOUND_EQ, p->get<string>(P_EQ, ON) == ON);
    g.set(e::SOUND_PITCH, p->get<string>(P_FREQ, ON) == ON);
    g.set(e::SOUND_FX0, p->get<string>(P_FX0, ON) == ON);
    g.set(e::SOUND_FX1, p->get<string>(P_FX1, ON) == ON);
    g.set(e::SOUND_FX2, p->get<string>(P_FX2, ON) == ON);
}

void setText()
{
    const auto& p = ConfigMgr::P;
    const auto& c = ConfigMgr::G;
    using e = eText;
    auto& g = gTexts;
    using namespace cfg;
    using std::string;

    // player name
    g.set(e::PLAYER_NAME, p->getName());

    // bga
    g.set(e::BGA, p->get<string>(P_LOAD_BGA, ON) == ON ? "ON" : "OFF");

    // speed type
    {
        static const std::map<string, string> smap =
        {
            {P_SPEED_TYPE_MIN, "MIN"},
            {P_SPEED_TYPE_MAX, "MAX"},
            {P_SPEED_TYPE_AVG, "AVERAGE"},
            {P_SPEED_TYPE_CONSTANT, "CONSTANT"},
        };

        auto&& s = p->get<string>(P_SPEED_TYPE, P_SPEED_TYPE_NORMAL);
        if (smap.find(s) != smap.end())
            g.set(e::SCROLL_TYPE, smap.at(s));
        else
            g.set(e::SCROLL_TYPE, "NORMAL");
    }

    // lanecover
    g.set(e::SHUTTER, p->get(P_LANECOVER, 0) > 0 ? "ON" : "OFF");

    // chart op
    {
        static const std::map<string, string> smap =
        {
            {P_CHART_OP_MIRROR, "MIRROR"},
            {P_CHART_OP_RANDOM, "RANDOM"},
            {P_CHART_OP_SRAN, "S-RANDOM"},
            {P_CHART_OP_HRAN, "H-RANDOM"},
            {P_CHART_OP_ALLSCR, "ALL-SCR"},
        };

        auto&& s = p->get<string>(P_CHART_OP, P_CHART_OP_NORMAL);
        if (smap.find(s) != smap.end())
        {
            g.set(e::RANDOM_1P, smap.at(s));
            g.set(e::RANDOM_2P, smap.at(s));
        }
        else
        {
            g.set(e::RANDOM_1P, "NORMAL");
            g.set(e::RANDOM_2P, "NORMAL");
        }
    }

    // gauge op
    {
        static const std::map<string, string> smap =
        {
            {P_GAUGE_OP_HARD, "HARD"},
            {P_GAUGE_OP_EASY, "EASY"},
            {P_GAUGE_OP_DEATH, "DEATH"},
        };

        auto&& s = p->get<string>(P_GAUGE_OP, P_GAUGE_OP);
        if (smap.find(s) != smap.end())
        {
            g.set(e::GAUGE_1P, smap.at(s));
            g.set(e::GAUGE_2P, smap.at(s));
        }
        else
        {
            g.set(e::GAUGE_1P, "NORMAL");
            g.set(e::GAUGE_2P, "NORMAL");
        }
    }

    // ghost type
    {
        static const std::map<string, string> smap =
        {
            {P_GHOST_TYPE_A, "TYPE A"},
            {P_GHOST_TYPE_B, "TYPE B"},
            {P_GHOST_TYPE_C, "TYPE C"},
        };

        auto&& s = p->get<string>(P_GHOST_TYPE, OFF);
        if (smap.find(s) != smap.end())
            g.set(e::GHOST, smap.at(s));
        else
            g.set(e::GHOST, "OFF");
    }

    // target
    {
        static const std::map<string, string> smap =
        {
            {P_TARGET_MYBEST, "MY BEST"},
            {P_TARGET_RANK_A, "RANK A"},
            {P_TARGET_RANK_AA, "RANK AA"},
            {P_TARGET_RANK_AAA, "RANK AAA"},
        };

        auto&& s = p->get<string>(P_TARGET, OFF);
        if (smap.find(s) != smap.end())
            g.set(e::TARGET_NAME, smap.at(s));
        else
            g.set(e::TARGET_NAME, "OFF");
    }

    // play mode
    {
        static const std::map<string, string> smap =
        {
            {P_PLAY_MODE_ALL, "ALL"},
            {P_PLAY_MODE_7K, "7KEYS"},
            {P_PLAY_MODE_5K, "5KEYS"},
            {P_PLAY_MODE_14K, "14KEYS"},
            {P_PLAY_MODE_10K, "10KEYS"},
            {P_PLAY_MODE_9K, "9KEYS"},
        };

        auto&& s = p->get<string>(P_PLAY_MODE, P_PLAY_MODE_ALL);
        if (smap.find(s) != smap.end())
            g.set(e::PLAY_MODE, smap.at(s));
        else
            g.set(e::PLAY_MODE, "ALL");
    }

    // sort mode
    {
        static const std::map<string, string> smap =
        {
            {P_SORT_MODE_FOLDER, "FOLDER"},
            {P_SORT_MODE_TITLE, "TITLE"},
            {P_SORT_MODE_LEVEL, "LEVEL"},
            {P_SORT_MODE_CLEAR, "CLEAR"},
            {P_SORT_MODE_RATE, "RATE"},
        };

        auto&& s = p->get<string>(P_SORT_MODE, P_SORT_MODE_FOLDER);
        if (smap.find(s) != smap.end())
            g.set(e::SORT_MODE, smap.at(s));
        else
            g.set(e::SORT_MODE, "FOLDER");
    }

    // diff filter mode
    {
        static const std::map<string, string> smap =
        {
            {P_DIFFICULTY_FILTER_ALL, "ALL"},
            {P_DIFFICULTY_FILTER_BEGINNER, "BEGINNER"},
            {P_DIFFICULTY_FILTER_NORMAL, "NORMAL"},
            {P_DIFFICULTY_FILTER_HYPER, "HYPER"},
            {P_DIFFICULTY_FILTER_ANOTHER, "ANOTHER"},
            {P_DIFFICULTY_FILTER_INSANE, "INSANE"},
        };

        auto&& s = p->get<string>(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL);
        if (smap.find(s) != smap.end())
            g.set(e::DIFFICULTY, smap.at(s));
        else
            g.set(e::DIFFICULTY, "ALL");
    }

    // battle
    g.set(e::BATTLE, p->get<string>(P_BATTLE, ON) == ON ? "ON" : "OFF");

    // flip
    g.set(e::FLIP, p->get<string>(P_FLIP, ON) == ON ? "ON" : "OFF");

    // graph
    g.set(e::SCORE_GRAPH, p->get<string>(P_SCORE_GRAPH, ON) == ON ? "ON" : "OFF");

    // windowed
    {
        static const std::map<string, string> smap =
        {
            {V_WINMODE_FULL, "FULLSCREEN"},
            {V_WINMODE_BORDERLESS, "BORDERLESS"},
            {V_WINMODE_WINDOWED, "WINDOWED"}
        };

        auto&& s = c->get<string>(V_WINMODE, V_WINMODE_WINDOWED);
        if (smap.find(s) != smap.end())
            g.set(e::WINDOWMODE, smap.at(s));
        else
            g.set(e::WINDOWMODE, "WINDOWED");
    }

    // vsync
    g.set(e::VSYNC, c->get<string>(V_VSYNC, ON) == ON ? "ON" : "OFF");

    // fixed tokens
    g.set(e::BGA_SIZE, "NORMAL");
    g.set(e::COLOR_DEPTH, "WHAT DO YOU WANT FROM THIS");
    g.set(e::JUDGE_AUTO, "NOT SUPPORTED");
    g.set(e::REPLAY_SAVE_TYPE, "NOT SUPPORTED");
    g.set(e::TRIAL1, "NOT SUPPORTED");
    g.set(e::TRIAL2, "NOT SUPPORTED");
}

int ConfigMgr::selectProfile(const std::string& name)
{
    Path folder = Path(".") / "profile" / name;
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
                LOG_WARNING << "[Config] File is not regular file: " << p.generic_string();
                return true;
            }
        }
        else
        {
            std::ofstream(p).close();
        }
        return false;
    };

    if (createFile(folder / CONFIG_FILE_GENERAL) ||
        createFile(folder / CONFIG_FILE_INPUT_5) ||
        createFile(folder / CONFIG_FILE_INPUT_7) ||
        createFile(folder / CONFIG_FILE_INPUT_9) ||
        createFile(folder / CONFIG_FILE_SKIN) ||
        createFile(folder / CONFIG_FILE_PROFILE))
    {
        LOG_WARNING << "[Config] Bad profile: " << name;
        return 3;
    }

    try
    {
        G = std::make_shared<ConfigGeneral>(name);
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

    load();

    return 0;
}

void ConfigMgr::setGlobals()
{
    setNumbers();
    setOptions();
    setSliders();
    setSwitches();
    setText();
}