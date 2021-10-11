#include <algorithm>

#include "scene_select.h"
#include "scene_context.h"
#include "common/chartformat/chartformat_types.h"
#include "common/entry/entry_song.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "config/config_mgr.h"

#include "game/skin/skin_lr2_button_callbacks.h"

////////////////////////////////////////////////////////////////////////////////

void setBarInfo()
{
    const auto& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;
    const size_t count = int(eText::_SELECT_BAR_TITLE_FULL_MAX) - int(eText::_SELECT_BAR_TITLE_FULL_0) + 1;
    for (size_t list_idx = (e.size() + idx - cursor) % e.size(), i = 0; i < count; list_idx = (list_idx + 1) % e.size(), ++i)
    {
        // set title
        gTexts.set(eText(int(eText::_SELECT_BAR_TITLE_FULL_0) + i), e[list_idx]->_name);

        // set level
        if (e[list_idx]->type() == eEntryType::SONG)
        {
            auto ps = std::reinterpret_pointer_cast<Song>(e[list_idx]);

            switch (ps->_file->type())
            {
            case eChartFormat::BMS:
            {
                const auto bms = std::reinterpret_pointer_cast<const BMS_prop>(ps->_file);
                gNumbers.set(eNumber(int(eNumber::_SELECT_BAR_LEVEL_0) + i), bms->playLevel);
                break;
            }

            default:
                gNumbers.set(eNumber(int(eNumber::_SELECT_BAR_LEVEL_0) + i), 0);
                break;
            }
        }
    }

}

void setEntryInfo()
{
    const auto& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;

    // chart parameters
    if (e[idx]->type() == eEntryType::SONG || e[idx]->type() == eEntryType::RIVAL_SONG)
    {
        auto ps = std::reinterpret_pointer_cast<Song>(e[idx]);
        auto pf = std::reinterpret_pointer_cast<vChartFormat>(ps->_file);

        gSwitches.set(eSwitch::CHART_HAVE_README, 
            !(pf->text1.empty() && pf->text2.empty() && pf->text3.empty()));
        gSwitches.set(eSwitch::CHART_HAVE_BANNER, !pf->banner.empty());
        gSwitches.set(eSwitch::CHART_HAVE_STAGEFILE, !pf->stagefile.empty());

        gTexts.set(eText::PLAY_TITLE, pf->title);
        gTexts.set(eText::PLAY_SUBTITLE, pf->title2);
        gTexts.set(eText::PLAY_ARTIST, pf->artist);
        gTexts.set(eText::PLAY_SUBARTIST, pf->artist2);
        gTexts.set(eText::PLAY_GENRE, pf->genre);
        gTexts.set(eText::PLAY_DIFFICULTY, pf->version);
        // _level

        // _totalLength_sec
        gNumbers.set(eNumber::INFO_TOTALNOTE, pf->totalNotes);

        gNumbers.set(eNumber::PLAY_BPM, static_cast<int>(std::round(pf->startBPM)));
        gNumbers.set(eNumber::INFO_BPM_MIN, static_cast<int>(std::round(pf->minBPM)));
        gNumbers.set(eNumber::INFO_BPM_MAX, static_cast<int>(std::round(pf->maxBPM)));

        switch (ps->_file->type())
        {
        case eChartFormat::BMS:
        {
            const auto bms = std::reinterpret_pointer_cast<const BMS_prop>(pf);

            // gamemode
            switch (bms->player)
            {
            case 5:
            case 7:
            case 9:
            case 24:
                gOptions.set(eOption::CHART_PLAY_MODE, Option::PLAY_SINGLE);
                break;

            case 10:
            case 14:
            case 48:
                gOptions.set(eOption::CHART_PLAY_MODE, Option::PLAY_DOUBLE);
                break;

            default:
                break;
            }

            gNumbers.set(eNumber::PLAY_BPM, static_cast<int>(std::round(bms->bpm)));

            // gamemode
            unsigned op_keys = Option::KEYS_NOT_PLAYABLE;
            switch (bms->gamemode)
            {
            case 7:  op_keys = Option::KEYS_7; break;
            case 5:  op_keys = Option::KEYS_5; break;
            case 14: op_keys = Option::KEYS_14; break;
            case 10: op_keys = Option::KEYS_10; break;
            case 9:  op_keys = Option::KEYS_9; break;
            case 24: op_keys = Option::KEYS_24; break;
            case 48: op_keys = Option::KEYS_48; break;
            default: break;
            }
            gOptions.set(eOption::CHART_PLAY_KEYS, op_keys);

            // judge
            unsigned op_judgerank = Option::JUDGE_NORMAL;
            switch (bms->rank)
            {
            case 0: op_judgerank = Option::JUDGE_VHARD; break;
            case 1: op_judgerank = Option::JUDGE_HARD; break;
            case 2: op_judgerank = Option::JUDGE_NORMAL; break;
            case 3: op_judgerank = Option::JUDGE_EASY; break;
            default: break;
            }
            gOptions.set(eOption::CHART_JUDGE_TYPE, op_judgerank);

            // difficulty
            unsigned op_difficulty = Option::DIFF_0;
            gNumbers.set(eNumber::MUSIC_BEGINNER_LEVEL, 0);
            gNumbers.set(eNumber::MUSIC_NORMAL_LEVEL, 0);
            gNumbers.set(eNumber::MUSIC_HYPER_LEVEL, 0);
            gNumbers.set(eNumber::MUSIC_ANOTHER_LEVEL, 0);
            gNumbers.set(eNumber::MUSIC_INSANE_LEVEL, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_BEGINNER, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_NORMAL, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_HYPER, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_ANOTHER, 0);
            gBargraphs.set(eBargraph::LEVEL_BAR_INSANE, 0);
            switch (bms->difficulty)
            {
            case 0:
                op_difficulty = Option::DIFF_0; 
                break;
            case 1: 
                op_difficulty = Option::DIFF_1; 
                gNumbers.set(eNumber::MUSIC_BEGINNER_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_BEGINNER, bms->playLevel / 12.0); 
                break;
            case 2: 
                op_difficulty = Option::DIFF_2; 
                gNumbers.set(eNumber::MUSIC_NORMAL_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_NORMAL, bms->playLevel / 12.0);
                break;
            case 3: 
                op_difficulty = Option::DIFF_3; 
                gNumbers.set(eNumber::MUSIC_HYPER_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_HYPER, bms->playLevel / 12.0); 
                break;
            case 4: 
                op_difficulty = Option::DIFF_4; 
                gNumbers.set(eNumber::MUSIC_ANOTHER_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_ANOTHER, bms->playLevel / 12.0); 
                break;
            case 5: 
                op_difficulty = Option::DIFF_5; 
                gNumbers.set(eNumber::MUSIC_INSANE_LEVEL, bms->playLevel); 
                gBargraphs.set(eBargraph::LEVEL_BAR_INSANE, bms->playLevel / 12.0); 
                break;
            }
            gOptions.set(eOption::CHART_DIFFICULTY, op_difficulty);

            // TODO TOTAL

            gSwitches.set(eSwitch::CHART_HAVE_BGA, bms->haveBGA);
            gSwitches.set(eSwitch::CHART_HAVE_BPMCHANGE, bms->haveBPMChange);
            gSwitches.set(eSwitch::CHART_HAVE_LN, bms->haveLN);
            gSwitches.set(eSwitch::CHART_HAVE_RANDOM, bms->haveRandom);

            //gSwitches.set(eSwitch::CHART_HAVE_BACKBMP, ?);

            //gSwitches.set(eSwitch::CHART_HAVE_SPEEDCHANGE, ?);

            break;
        }

        default:
            break;
        }
    }
    else
    {
        gTexts.set(eText::PLAY_TITLE, e[idx]->_name);
        gTexts.set(eText::PLAY_SUBTITLE, e[idx]->_name2);
        gTexts.set(eText::PLAY_ARTIST, "");
        gTexts.set(eText::PLAY_SUBARTIST, "");
        gTexts.set(eText::PLAY_GENRE, "");
        gTexts.set(eText::PLAY_DIFFICULTY, "");
     }

    // score.db
    switch (e[idx]->type())
    {
    case eEntryType::SONG:
    case eEntryType::RIVAL_SONG:
        gOptions.set(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_SONG);
        gOptions.set(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        gOptions.set(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        break;

    case eEntryType::COURSE:
        gOptions.set(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_COURSE);
        gOptions.set(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        gOptions.set(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        break;

    case eEntryType::NEW_COURSE:
        gOptions.set(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_NEW_COURSE);
        gOptions.set(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOT_APPLICIABLE);
        gOptions.set(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        gOptions.set(eOption::CHART_PLAY_KEYS, Option::KEYS_NOT_PLAYABLE);
        break;

    case eEntryType::FOLDER:
    case eEntryType::CUSTOM_FOLDER:
    case eEntryType::RIVAL:
    default:
        gOptions.set(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_FOLDER);
        gOptions.set(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOT_APPLICIABLE);
        gOptions.set(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        gOptions.set(eOption::CHART_PLAY_KEYS, Option::KEYS_NOT_PLAYABLE);
        break;
    }
}

void setDynamicTextures()
{
    std::shared_lock<std::shared_mutex> u(gSelectContext._mutex);

    const auto& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;

    // chart parameters
    if (e[idx]->type() == eEntryType::SONG || e[idx]->type() == eEntryType::RIVAL_SONG)
    {
        auto ps = std::reinterpret_pointer_cast<Song>(e[idx]);
        auto pf = std::reinterpret_pointer_cast<vChartFormat>(ps->_file);

        // _BG
        if (!pf->stagefile.empty())
            gChartContext.texStagefile.setPath(pf->getDirectory() / pf->stagefile);

        // backbmp
        if (!pf->backbmp.empty())
            gChartContext.texBackbmp.setPath(pf->getDirectory() / pf->backbmp);

        // _banner
        if (!pf->banner.empty())
            gChartContext.texBanner.setPath(pf->getDirectory() / pf->banner);
    }

}

void config_sys()
{
    auto& p = ConfigMgr::P;
    auto& c = ConfigMgr::G;
    using namespace cfg;

    switch (gOptions.get(eOption::SYS_WINDOWED))
    {
    case Option::WIN_FULLSCREEN: c->set(V_WINMODE, V_WINMODE_FULL); break;
    case Option::WIN_BORDERLESS: c->set(V_WINMODE, V_WINMODE_BORDERLESS); break;
    case Option::WIN_WINDOWED: 
    default: c->set(V_WINMODE, V_WINMODE_WINDOWED); break;
    }

    
}

void config_player()
{
    auto& p = ConfigMgr::P;
    using namespace cfg;

    p->set(P_HISPEED, gNumbers.get(eNumber::HS_1P) / 100.0);
    switch (gOptions.get(eOption::PLAY_HSFIX_TYPE_1P))
    {
    case Option::SPEED_FIX_MAX: p->set(P_SPEED_TYPE, P_SPEED_TYPE_MAX); break;
    case Option::SPEED_FIX_MIN: p->set(P_SPEED_TYPE, P_SPEED_TYPE_MIN); break;
    case Option::SPEED_FIX_AVG: p->set(P_SPEED_TYPE, P_SPEED_TYPE_AVG); break;
    case Option::SPEED_FIX_CONSTANT: p->set(P_SPEED_TYPE, P_SPEED_TYPE_CONSTANT); break;
    default: p->set(P_SPEED_TYPE, P_SPEED_TYPE_NORMAL); break;
    }

    p->set(P_LOAD_BGA, gOptions.get(eOption::PLAY_BGA_TYPE) != Option::BGA_OFF ? ON : OFF);
    p->set(P_LANECOVER, gNumbers.get(eNumber::LANECOVER_1P));


    switch (gOptions.get(eOption::PLAY_RANDOM_TYPE_1P))
    {
    case Option::RAN_MIRROR: p->set(P_CHART_OP, P_CHART_OP_MIRROR); break;
    case Option::RAN_RANDOM: p->set(P_CHART_OP, P_CHART_OP_RANDOM); break;
    case Option::RAN_SRAN: p->set(P_CHART_OP, P_CHART_OP_SRAN); break;
    case Option::RAN_HRAN: p->set(P_CHART_OP, P_CHART_OP_HRAN); break;
    case Option::RAN_ALLSCR: p->set(P_CHART_OP, P_CHART_OP_ALLSCR); break;
    default: p->set(P_CHART_OP, P_CHART_OP_NORMAL); break;
    }

    switch (gOptions.get(eOption::PLAY_GAUGE_TYPE_1P))
    {
    case Option::GAUGE_HARD: p->set(P_GAUGE_OP, P_GAUGE_OP_HARD); break;
    case Option::GAUGE_EASY: p->set(P_GAUGE_OP, P_GAUGE_OP_EASY); break;
    case Option::GAUGE_DEATH: p->set(P_GAUGE_OP, P_GAUGE_OP_DEATH); break;
    case Option::GAUGE_EXHARD: p->set(P_GAUGE_OP, P_GAUGE_OP_EXHARD); break;
    case Option::GAUGE_ASSIST: p->set(P_GAUGE_OP, P_GAUGE_OP_ASSIST); break;
    default: p->set(P_GAUGE_OP, P_GAUGE_OP_NORMAL); break;
    }

    switch (gOptions.get(eOption::PLAY_GHOST_TYPE_1P))
    {
    case Option::GHOST_TOP: p->set(P_GHOST_TYPE, P_GHOST_TYPE_A); break;
    case Option::GHOST_SIDE: p->set(P_GHOST_TYPE, P_GHOST_TYPE_B); break;
    case Option::GHOST_SIDE_BOTTOM: p->set(P_GHOST_TYPE, P_GHOST_TYPE_C); break;
    default: p->set(P_GHOST_TYPE, OFF); break;
    }

    p->set(P_JUDGE_OFFSET, gNumbers.get(eNumber::TIMING_ADJUST_VISUAL));
    p->set(P_GHOST_TARGET, gNumbers.get(eNumber::DEFAULT_TARGET_RATE));

    switch (gOptions.get(eOption::SELECT_FILTER_KEYS))
    {
    case Option::KEYS_7: p->set(P_PLAY_MODE, P_PLAY_MODE_7K); break;
    case Option::KEYS_5: p->set(P_PLAY_MODE, P_PLAY_MODE_5K); break;
    case Option::KEYS_14: p->set(P_PLAY_MODE, P_PLAY_MODE_14K); break;
    case Option::KEYS_10: p->set(P_PLAY_MODE, P_PLAY_MODE_10K); break;
    case Option::KEYS_9: p->set(P_PLAY_MODE, P_PLAY_MODE_9K); break;
    default: p->set(P_PLAY_MODE, P_PLAY_MODE_ALL); break;
    }

    switch (gOptions.get(eOption::SELECT_SORT))
    {
    case Option::SORT_TITLE: p->set(P_SORT_MODE, P_SORT_MODE_TITLE); break;
    case Option::SORT_LEVEL: p->set(P_SORT_MODE, P_SORT_MODE_LEVEL); break;
    case Option::SORT_CLEAR: p->set(P_SORT_MODE, P_SORT_MODE_CLEAR); break;
    case Option::SORT_RATE: p->set(P_SORT_MODE, P_SORT_MODE_RATE); break;
    default: p->set(P_SORT_MODE, P_SORT_MODE_FOLDER); break;
    }

    switch (gOptions.get(eOption::SELECT_FILTER_DIFF))
    {
    case Option::DIFF_BEGINNER: p->set(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_BEGINNER); break;
    case Option::DIFF_NORMAL: p->set(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_NORMAL); break;
    case Option::DIFF_HYPER: p->set(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_HYPER); break;
    case Option::DIFF_ANOTHER: p->set(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ANOTHER); break;
    case Option::DIFF_INSANE: p->set(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_INSANE); break;
    default: p->set(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL); break;
    }

    p->set(P_BATTLE, gOptions.get(eOption::PLAY_BATTLE_TYPE) != 0 ? ON : OFF);
    p->set(P_FLIP, gSwitches.get(eSwitch::PLAY_OPTION_DP_FLIP) ? ON : OFF);
    p->set(P_SCORE_GRAPH, gSwitches.get(eSwitch::SYSTEM_SCOREGRAPH) ? ON : OFF);
}

void config_vol()
{
    auto& p = ConfigMgr::P;
    using namespace cfg;

    p->set(P_VOL_MASTER, gSliders.get(eSlider::VOLUME_MASTER));
    p->set(P_VOL_KEY, gSliders.get(eSlider::VOLUME_KEY));
    p->set(P_VOL_BGM, gSliders.get(eSlider::VOLUME_BGM));
}

void config_eq()
{
    auto& p = ConfigMgr::P;
    using namespace cfg;

    p->set(P_EQ, gSwitches.get(eSwitch::SOUND_EQ) ? ON : OFF);
    p->set(P_EQ0, (gSliders.get(eSlider::EQ0) + 0.5) / 2);
    p->set(P_EQ1, (gSliders.get(eSlider::EQ1) + 0.5) / 2);
    p->set(P_EQ2, (gSliders.get(eSlider::EQ2) + 0.5) / 2);
    p->set(P_EQ3, (gSliders.get(eSlider::EQ3) + 0.5) / 2);
    p->set(P_EQ4, (gSliders.get(eSlider::EQ4) + 0.5) / 2);
    p->set(P_EQ5, (gSliders.get(eSlider::EQ5) + 0.5) / 2);
    p->set(P_EQ6, (gSliders.get(eSlider::EQ6) + 0.5) / 2);
}

void config_freq()
{
    auto& p = ConfigMgr::P;
    using namespace cfg;

    p->set(P_FREQ, gSwitches.get(eSwitch::SOUND_PITCH) ? ON : OFF);
    switch (gOptions.get(eOption::SOUND_PITCH_TYPE))
    {
    case Option::FREQ_FREQ: p->set(P_FREQ_TYPE, P_FREQ_TYPE_FREQ); break;
    case Option::FREQ_PITCH: p->set(P_FREQ_TYPE, P_FREQ_TYPE_PITCH); break;
    case Option::FREQ_SPEED: p->set(P_FREQ_TYPE, P_FREQ_TYPE_SPEED); break;
    default: break;
    }
    p->set(P_FREQ_VAL, gNumbers.get(eNumber::PITCH));
}

void config_fx()
{
    auto& p = ConfigMgr::P;
    using namespace cfg;

    p->set(P_FX0, gSwitches.get(eSwitch::SOUND_FX0) ? ON : OFF);
    switch (gOptions.get(eOption::SOUND_TARGET_FX0))
    {
    case Option::FX_MASTER: p->set(P_FX0_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY: p->set(P_FX0_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM: p->set(P_FX0_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX0))
    {
    case Option::FX_OFF: p->set(P_FX0_TYPE, OFF); break;
    case Option::FX_REVERB: p->set(P_FX0_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY: p->set(P_FX0_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS: p->set(P_FX0_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS: p->set(P_FX0_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER: p->set(P_FX0_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS: p->set(P_FX0_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: p->set(P_FX0_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    p->set(P_FX0_P1, gNumbers.get(eNumber::FX0_P1));
    p->set(P_FX0_P2, gNumbers.get(eNumber::FX0_P2));

    p->set(P_FX1, gSwitches.get(eSwitch::SOUND_FX1) ? ON : OFF);
    switch (gOptions.get(eOption::SOUND_TARGET_FX1))
    {
    case Option::FX_MASTER: p->set(P_FX1_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY: p->set(P_FX1_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM: p->set(P_FX1_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX1))
    {
    case Option::FX_OFF: p->set(P_FX1_TYPE, OFF); break;
    case Option::FX_REVERB: p->set(P_FX1_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY: p->set(P_FX1_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS: p->set(P_FX1_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS: p->set(P_FX1_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER: p->set(P_FX1_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS: p->set(P_FX1_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: p->set(P_FX1_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    p->set(P_FX1_P1, gNumbers.get(eNumber::FX1_P1));
    p->set(P_FX1_P2, gNumbers.get(eNumber::FX1_P2));

    p->set(P_FX2, gSwitches.get(eSwitch::SOUND_FX2) ? ON : OFF);
    switch (gOptions.get(eOption::SOUND_TARGET_FX2))
    {
    case Option::FX_MASTER: p->set(P_FX2_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY: p->set(P_FX2_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM: p->set(P_FX2_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX2))
    {
    case Option::FX_OFF: p->set(P_FX2_TYPE, OFF); break;
    case Option::FX_REVERB: p->set(P_FX2_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY: p->set(P_FX2_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS: p->set(P_FX2_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS: p->set(P_FX2_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER: p->set(P_FX2_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS: p->set(P_FX2_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: p->set(P_FX2_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    p->set(P_FX2_P1, gNumbers.get(eNumber::FX2_P1));
    p->set(P_FX2_P2, gNumbers.get(eNumber::FX2_P2));
}


////////////////////////////////////////////////////////////////////////////////

SceneSelect::SceneSelect() : vScene(eMode::MUSIC_SELECT, 1000)
{
    _inputAvailable = INPUT_MASK_FUNC;

    //if (context_play.chartObj[PLAYER_SLOT_1P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }

    //if (context_play.chartObj[PLAYER_SLOT_2P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
        loadSongList();
        setBarInfo();
        setEntryInfo();
        _skin->reset_bar_animation();
    }

    _state = eSelectState::PREPARE;

    loopStart();

    SoundMgr::stopSamples();
    SoundMgr::playSample(static_cast<size_t>(eSoundSample::BGM_SELECT));
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
}

void SceneSelect::_updateAsync()
{
    switch (_state)
    {
    case eSelectState::PREPARE:
        updatePrepare();
        break;
    case eSelectState::SELECT:
        updateSelect();
        break;
    case eSelectState::SEARCH:
        updateSearch();
        break;
    case eSelectState::PANEL1:
    case eSelectState::PANEL2:
    case eSelectState::PANEL3:
    case eSelectState::PANEL4:
    case eSelectState::PANEL5:
    case eSelectState::PANEL6:
    case eSelectState::PANEL7:
    case eSelectState::PANEL8:
    case eSelectState::PANEL9:
        updatePanel(unsigned(_state) - unsigned(eSelectState::PANEL1) + 1);
        break;
    case eSelectState::FADEOUT:
        updateFadeout();
        break;
    }
}

void SceneSelect::updatePrepare()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeIntro)
    {
        _state = eSelectState::SELECT;

        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneSelect::inputGamePress, this, _1, _2));
        _input.register_h("SCENE_HOLD", std::bind(&SceneSelect::inputGameHold, this, _1, _2));
        _input.register_r("SCENE_RELEASE", std::bind(&SceneSelect::inputGameRelease, this, _1, _2));
        _input.loopStart();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());

        LOG_DEBUG << "[Select] State changed to SELECT";
    }
}

void SceneSelect::updateSelect()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    if (t.norm() - scrollTimestamp >= gSelectContext.scrollTime)
    {
        if (!isHoldingUp && !isHoldingDown)
            scrollTimestamp = -1;
    }
}

void SceneSelect::updateSearch()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updatePanel(unsigned idx)
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updateFadeout()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (ft >= _skin->info.timeOutro)
    {
        loopEnd();
        _input.loopEnd();
        gNextScene = eScene::EXIT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneSelect::inputGamePress(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        if (input[ESC])
        {
            LOG_DEBUG << "[Select] ESC";
            gNextScene = eScene::EXIT;
            return;
        }
        if (!gSwitches.get(eSwitch::SELECT_PANEL1) && (input[K1START] || input[K2START]))
        {
            // close other panels
            for (int i = 2; i <= 9; ++i)
            {
                eSwitch p = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + i);
                if (gSwitches.get(p))
                {
                    gSwitches.set(p, false);
                    gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + i), TIMER_NEVER);
                    gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + i), t.norm());
                    //SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CLOSE));
                }
            }

            // open panel 1
            gSwitches.set(eSwitch::SELECT_PANEL1, true);
            gTimers.set(eTimer::PANEL1_START, t.norm());
            gTimers.set(eTimer::PANEL1_END, TIMER_NEVER);
            SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_OPEN));
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
        if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
        {
            for (size_t k = Pad::K21; k <= Pad::K29; ++k)
            {
                if (input[k])
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_DOWN) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_UP) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }
        else
        {
            for (size_t k = Pad::K21; k <= Pad::K29; ++k)
            {
                if (input[k])
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }

        // TODO check skin type
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            // 1: KEYS
            if (input[Pad::K12]) lr2skin::button::random_type(0, 1);
            if (input[Pad::K13]) lr2skin::button::battle(1);
            if (input[Pad::K14]) lr2skin::button::gauge_type(0, 1);
            if (input[Pad::K15]) lr2skin::button::hs(0, -1);
            if (input[Pad::K16]) lr2skin::button::autoscr(0, 1);
            if (input[Pad::K17]) lr2skin::button::hs(0, 1);

            if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
            {
                // 1: KEYS
                if (input[Pad::K22]) lr2skin::button::random_type(1, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(1, 1);
                if (input[Pad::K25]) lr2skin::button::hs(1, -1);
                if (input[Pad::K26]) lr2skin::button::autoscr(1, 1);
                if (input[Pad::K27]) lr2skin::button::hs(1, 1);
            }
            else
            {
                // 1: KEYS
                if (input[Pad::K22]) lr2skin::button::random_type(0, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(0, 1);
                if (input[Pad::K25]) lr2skin::button::hs(0, -1);
                if (input[Pad::K26]) lr2skin::button::autoscr(0, 1);
                if (input[Pad::K27]) lr2skin::button::hs(0, 1);
            }
        }

        // navigate
        switch (_state)
        {
        case eSelectState::SELECT:
            if (!gSwitches.get(eSwitch::SELECT_PANEL1))
            {
                switch (gSelectContext.entries[gSelectContext.idx]->type())
                {
                case eEntryType::FOLDER:
                case eEntryType::CUSTOM_FOLDER:
                    if ((input & INPUT_MASK_DECIDE).any())
                        _navigateEnter(t);
                    break;

                case eEntryType::SONG:
                case eEntryType::COURSE:
                    if ((input & INPUT_MASK_DECIDE).any())
                        _decide();
                    break;

                default:
                    break;
                }
                if ((input & INPUT_MASK_CANCEL).any())
                    _navigateBack(t);
            }
            if ((input & INPUT_MASK_NAV_UP).any())
            {
                isHoldingUp = true;
                if (scrollTimestamp == -1)
                {
                    scrollTimestamp = t.norm();
                    _navigateUpBy1(t);
                }
            }
            if ((input & INPUT_MASK_NAV_DN).any())
            {
                isHoldingDown = true;
                if (scrollTimestamp == -1)
                {
                    scrollTimestamp = t.norm();
                    _navigateDownBy1(t);
                }
            }

            break;

        case eSelectState::FADEOUT:
            break;

        default:
            break;
        }
    }
}

// CALLBACK
void SceneSelect::inputGameHold(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // navigate
        switch (_state)
        {
        case eSelectState::SELECT:
            if (!gSwitches.get(eSwitch::SELECT_PANEL1))
            {
                if (isHoldingUp && t.norm() - scrollTimestamp >= gSelectContext.scrollTime)
                {
                    gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
                    scrollTimestamp = t.norm();
                    _navigateUpBy1(t);
                }
                if (isHoldingDown && t.norm() - scrollTimestamp >= gSelectContext.scrollTime)
                {
                    gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);;
                    scrollTimestamp = t.norm();
                    _navigateDownBy1(t);
                }
            }
            break;

        default:
            break;
        }
    }
}

// CALLBACK
void SceneSelect::inputGameRelease(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        if (gSwitches.get(eSwitch::SELECT_PANEL1) && (input[K1START] || input[K2START]))
        {
            // close panel 1
            gSwitches.set(eSwitch::SELECT_PANEL1, false);
            gTimers.set(eTimer::PANEL1_START, TIMER_NEVER);
            gTimers.set(eTimer::PANEL1_END, t.norm());
            SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CLOSE));
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
        if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
        {
            for (size_t k = Pad::K21; k <= Pad::K29; ++k)
            {
                if (input[k])
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_UP) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_DOWN) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }
        else
        {
            for (size_t k = Pad::K21; k <= Pad::K29; ++k)
            {
                if (input[k])
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }

        // navigate
        switch (_state)
        {
        case eSelectState::SELECT:
            if (!gSwitches.get(eSwitch::SELECT_PANEL1))
            {
                if ((input & INPUT_MASK_NAV_UP).any())
                {
                    isHoldingUp = false;
                    gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
                }
                if ((input & INPUT_MASK_NAV_DN).any())
                {
                    isHoldingDown = false;
                    gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
                }
            }
            break;

        default:
            break;
        }
    }
}

void SceneSelect::_decide()
{
    std::shared_lock<std::shared_mutex> u(gSelectContext._mutex);

    auto entry = gSelectContext.entries[gSelectContext.idx];
    //auto& chart = entry.charts[entry.chart_idx];
    auto& c = gChartContext;
    auto& p = gPlayContext;

    clearContextPlay();

    // gauge
    switch (gOptions.get(eOption::PLAY_GAUGE_TYPE_1P))
    {
    case 0: gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::NORMAL; break;
    case 1: gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::HARD; break;
    case 2: gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::DEATH; break;
    case 3: gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::EASY; break;
    case 4: gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::PATTACK; break;
    case 5: gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::GATTACK; break;
    case 6: gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::ASSISTEASY;break;
    case 7: gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::EXHARD; break;
    default: break;
    }
    switch (gOptions.get(eOption::PLAY_GAUGE_TYPE_2P))
    {
    case 0: gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::NORMAL; break;
    case 1: gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::HARD; break;
    case 2: gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::DEATH; break;
    case 3: gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::EASY; break;
    case 4: gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::PATTACK; break;
    case 5: gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::GATTACK; break;
    case 6: gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::ASSISTEASY;break;
    case 7: gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::EXHARD; break;
    default: break;
    }

    // random
    switch (gOptions.get(eOption::PLAY_RANDOM_TYPE_1P))
    {
    case 0: gPlayContext.mods[PLAYER_SLOT_1P].chart = eModChart::NONE; break;
    case 1: gPlayContext.mods[PLAYER_SLOT_1P].chart = eModChart::MIRROR; break;
    case 2: gPlayContext.mods[PLAYER_SLOT_1P].chart = eModChart::RANDOM; break;
    case 3: gPlayContext.mods[PLAYER_SLOT_1P].chart = eModChart::SRAN; break;
    case 4: gPlayContext.mods[PLAYER_SLOT_1P].chart = eModChart::HRAN; break;
    case 5: gPlayContext.mods[PLAYER_SLOT_1P].chart = eModChart::ALLSCR; break;
    default: break;
    }
    switch (gOptions.get(eOption::PLAY_RANDOM_TYPE_2P))
    {
    case 0: gPlayContext.mods[PLAYER_SLOT_2P].chart = eModChart::NONE; break;
    case 1: gPlayContext.mods[PLAYER_SLOT_2P].chart = eModChart::MIRROR; break;
    case 2: gPlayContext.mods[PLAYER_SLOT_2P].chart = eModChart::RANDOM; break;
    case 3: gPlayContext.mods[PLAYER_SLOT_2P].chart = eModChart::SRAN; break;
    case 4: gPlayContext.mods[PLAYER_SLOT_2P].chart = eModChart::HRAN; break;
    case 5: gPlayContext.mods[PLAYER_SLOT_2P].chart = eModChart::ALLSCR; break;
    default: break;
    }

    // assist
    gPlayContext.mods[PLAYER_SLOT_1P].assist_mask |= gSwitches.get(eSwitch::PLAY_OPTION_AUTOSCR_1P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;
    gPlayContext.mods[PLAYER_SLOT_2P].assist_mask |= gSwitches.get(eSwitch::PLAY_OPTION_AUTOSCR_2P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;

    // HS fix
    switch (gOptions.get(eOption::PLAY_HSFIX_TYPE_1P))
    {
    case 0: gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::NONE; break;
    case 1: gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::MAXBPM; break;
    case 2: gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::MINBPM; break;
    case 3: gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::AVERAGE; break;
    case 4: gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::CONSTANT; break;
    default: break;
    }
    switch (gOptions.get(eOption::PLAY_HSFIX_TYPE_2P))
    {
    case 0: gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::NONE; break;
    case 1: gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::MAXBPM; break;
    case 2: gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::MINBPM; break;
    case 3: gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::AVERAGE; break;
    case 4: gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::CONSTANT; break;
    default: break;
    }

    // chart
    switch (entry->type())
    {
    case eEntryType::SONG:
    {
        c.chartObj = std::reinterpret_pointer_cast<Song>(entry)->_file;

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

        // FIXME get play mode
        gPlayContext.mode = eMode::PLAY7;

        break;
    }
    default:
        break;
    }

    loopEnd();
    _input.loopEnd();
    gNextScene = eScene::DECIDE;
}

void SceneSelect::loadSongList()
{
    // TODO load song list
    gSelectContext.entries.clear();
    for (auto& e : gSelectContext.backtrace.top().list)
    {
        // TODO replace name/name2 by tag.db

        // apply filter
        bool skip = false;
        switch (e->type())
        {
        case eEntryType::SONG:
        {
            auto f = std::reinterpret_pointer_cast<FolderSong>(e);
            for (size_t idx = 0; idx < f->getContentsCount() && !skip; ++idx)
            {
                auto p = std::reinterpret_pointer_cast<BMS_prop>(f->getChart(idx));
                switch (p->type())
                {
                case eChartFormat::BMS:
                    // difficulty filter
                    if (gSelectContext.difficulty != 0 &&
                        std::reinterpret_pointer_cast<BMS_prop>(p)->difficulty != gSelectContext.difficulty)
                    {
                        continue;
                    }
                    // gamemode filter
                    if (gSelectContext.gamemode != 0 &&
                        std::reinterpret_pointer_cast<BMS_prop>(p)->gamemode != gSelectContext.gamemode)
                    {
                        continue;
                    }
                    break;

                default:
                    break;
                }

                // currently add all charts into list
                gSelectContext.entries.push_back(std::make_shared<Song>(p));
            }
            break;
        }
        default:
            gSelectContext.entries.push_back(e);
            break;
        }
    }

    // TODO sort song list
    switch (gSelectContext.sort)
    {
    case SongListSort::DEFAULT:
    {
        auto& l = gSelectContext.entries;
        std::sort(l.begin(), l.end(), [](const std::shared_ptr<vEntry>& lhs, const std::shared_ptr<vEntry>& rhs)
        {
            if (lhs->type() != rhs->type())
                return lhs->type() > rhs->type();
            else
            {
                if (lhs->type() == eEntryType::SONG)
                {
                    const auto& l = std::reinterpret_pointer_cast<const Song>(lhs)->_file;
                    const auto& r = std::reinterpret_pointer_cast<const Song>(rhs)->_file;
                    if (l->levelEstimated != r->levelEstimated) return l->levelEstimated > r->levelEstimated;
                    if (l->title != r->title) return l->title > r->title;
                    if (l->title2 != r->title2) return l->title2 > r->title2;
                    if (l->version != r->version) return l->version > r->version;
                    return l->fileHash > r->fileHash;
                }
                else
                {
                    if (lhs->_name != rhs->_name) return lhs->_name > rhs->_name;
                    if (lhs->_name2 != rhs->_name2) return lhs->_name2 > rhs->_name2;
                    return false;
                }
            }
        });
        break;
    }
    case SongListSort::TITLE:
    {
        auto& l = gSelectContext.entries;
        std::sort(l.begin(), l.end(), [](const std::shared_ptr<vEntry>& lhs, const std::shared_ptr<vEntry>& rhs)
        {
            if (lhs->type() != rhs->type())
                return lhs->type() > rhs->type();
            else
            {
                if (lhs->type() == eEntryType::SONG)
                {
                    const auto& l = std::reinterpret_pointer_cast<const Song>(lhs)->_file;
                    const auto& r = std::reinterpret_pointer_cast<const Song>(rhs)->_file;
                    if (l->title != r->title) return l->title > r->title;
                    if (l->title2 != r->title2) return l->title2 > r->title2;
                    if (l->version != r->version) return l->version > r->version;
                    return l->fileHash > r->fileHash;
                }
                else
                {
                    if (lhs->_name != rhs->_name) return lhs->_name > rhs->_name;
                    if (lhs->_name2 != rhs->_name2) return lhs->_name2 > rhs->_name2;
                    return false;
                }
            }
        });
        break;
    }
    default:
        break;
    }

}

void SceneSelect::_navigateUpBy1(Time t)
{
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        gSelectContext.idx = (gSelectContext.entries.size() + gSelectContext.idx - 1) % gSelectContext.entries.size();
        _skin->start_bar_animation(-1);

        setBarInfo();
        setEntryInfo();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_SCRATCH));
    }
    setDynamicTextures();
}

void SceneSelect::_navigateDownBy1(Time t)
{
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        gSelectContext.idx = (gSelectContext.idx + 1) % gSelectContext.entries.size();
        _skin->start_bar_animation(+1);

        setBarInfo();
        setEntryInfo();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_SCRATCH));
    }
    setDynamicTextures();
}

void SceneSelect::_navigateEnter(Time t)
{
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        const auto& e = gSelectContext.entries[gSelectContext.idx];
        switch (e->type())
        {
        case eEntryType::FOLDER:
        case eEntryType::CUSTOM_FOLDER:
        {
            SongListProperties prop{
                gSelectContext.backtrace.top().folder,
                e->md5,
                e->_name,
                {},
                gSelectContext.idx
            };
            auto top = g_pSongDB->browse(e->md5, false);
            for (size_t i = 0; i < top.getContentsCount(); ++i)
                prop.list.push_back(top.getEntry(i));

            gSelectContext.backtrace.push(prop);
            gSelectContext.entries.clear();
            gSelectContext.idx = 0;
            loadSongList();

            setBarInfo();
            setEntryInfo();

            SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_F_OPEN));
            break;
        }
        default:
            break;
        }
    }

    setDynamicTextures();
}
void SceneSelect::_navigateBack(Time t)
{
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        // TODO
        auto top = gSelectContext.backtrace.top();
        if (!top.parent.empty())
        {
            gSelectContext.idx = 0;
            gSelectContext.backtrace.pop();
            top = gSelectContext.backtrace.top();
            gSelectContext.entries = top.list;
            gSelectContext.idx = top.index;

            setBarInfo();
            setEntryInfo();

            SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_F_CLOSE));
        }
    }
    setDynamicTextures();
}