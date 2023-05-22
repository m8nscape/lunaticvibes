#include "common/pch.h"
#include "ruleset_bms.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/chart/chart_types.h"
#include "config/config_mgr.h"
#include "game/data/data_types.h"
#include "game/replay/replay_chart.h" 

namespace lunaticvibes
{

using namespace chart;

void setJudgeTimer1P(RulesetBMS::JudgeType judge, long long t)
{
    PlayData.timers["judge_1p"] = t;
    PlayData.timers["judge_1p_pg"] = TIMER_NEVER;
    PlayData.timers["judge_1p_gr"] = TIMER_NEVER;
    PlayData.timers["judge_1p_gd"] = TIMER_NEVER;
    PlayData.timers["judge_1p_bd"] = TIMER_NEVER;
    PlayData.timers["judge_1p_pr"] = TIMER_NEVER;
    PlayData.timers["judge_1p_kp"] = TIMER_NEVER;
	switch (judge)
	{
	case RulesetBMS::JudgeType::KPOOR:   PlayData.timers["judge_1p_kp"] = t; break;
	case RulesetBMS::JudgeType::MISS:    PlayData.timers["judge_1p_pr"] = t; break;
	case RulesetBMS::JudgeType::BAD:     PlayData.timers["judge_1p_bd"] = t; break;
	case RulesetBMS::JudgeType::GOOD:    PlayData.timers["judge_1p_gd"] = t; break;
	case RulesetBMS::JudgeType::GREAT:   PlayData.timers["judge_1p_gr"] = t; break;
	case RulesetBMS::JudgeType::PERFECT: PlayData.timers["judge_1p_pg"] = t; break;
	default: break;
	}
}

void setJudgeTimer2P(RulesetBMS::JudgeType judge, long long t)
{
    PlayData.timers["judge_2p"] = t;
    PlayData.timers["judge_2p_pg"] = TIMER_NEVER;
    PlayData.timers["judge_2p_gr"] = TIMER_NEVER;
    PlayData.timers["judge_2p_gd"] = TIMER_NEVER;
    PlayData.timers["judge_2p_bd"] = TIMER_NEVER;
    PlayData.timers["judge_2p_pr"] = TIMER_NEVER;
    PlayData.timers["judge_2p_kp"] = TIMER_NEVER;
    switch (judge)
    {
    case RulesetBMS::JudgeType::KPOOR:   PlayData.timers["judge_2p_kp"] = t; break;
    case RulesetBMS::JudgeType::MISS:    PlayData.timers["judge_2p_pr"] = t; break;
    case RulesetBMS::JudgeType::BAD:     PlayData.timers["judge_2p_bd"] = t; break;
    case RulesetBMS::JudgeType::GOOD:    PlayData.timers["judge_2p_gd"] = t; break;
    case RulesetBMS::JudgeType::GREAT:   PlayData.timers["judge_2p_gr"] = t; break;
    case RulesetBMS::JudgeType::PERFECT: PlayData.timers["judge_2p_pg"] = t; break;
    default: break;
    }
}

RulesetBMS::RulesetBMS(std::shared_ptr<ChartFormatBase> format, std::shared_ptr<ChartObjectBase> chart,
    PlayModifierGaugeType gauge, GameModeKeys keys, RulesetBMS::JudgeDifficulty difficulty, double health, RulesetBMS::PlaySide side) :
    RulesetBase(format, chart), _judgeDifficulty(difficulty)
{

    static const NoteLaneTimerMap bombTimer5k[] = {
        {{
            {chart::Sc1, "bomb_1p_s"},
            {chart::N11, "bomb_1p_k1"},
            {chart::N12, "bomb_1p_k2"},
            {chart::N13, "bomb_1p_k3"},
            {chart::N14, "bomb_1p_k4"},
            {chart::N15, "bomb_1p_k5"},
            {chart::N21, "bomb_2p_k1"},
            {chart::N22, "bomb_2p_k2"},
            {chart::N23, "bomb_2p_k3"},
            {chart::N24, "bomb_2p_k4"},
            {chart::N25, "bomb_2p_k5"},
            {chart::Sc2, "bomb_2p_s"},
        }},
        {{
            {chart::Sc1, "bomb_1p_s"},
            {chart::N11, "bomb_1p_k1"},
            {chart::N12, "bomb_1p_k2"},
            {chart::N13, "bomb_1p_k3"},
            {chart::N14, "bomb_1p_k4"},
            {chart::N15, "bomb_1p_k5"},
            {chart::N21, "bomb_2p_k3"},
            {chart::N22, "bomb_2p_k4"},
            {chart::N23, "bomb_2p_k5"},
            {chart::N24, "bomb_2p_k6"},
            {chart::N25, "bomb_2p_k7"},
            {chart::Sc2, "bomb_2p_s"},
        }},
        {{
            {chart::Sc1, "bomb_1p_s"},
            {chart::N11, "bomb_1p_k3"},
            {chart::N12, "bomb_1p_k4"},
            {chart::N13, "bomb_1p_k5"},
            {chart::N14, "bomb_1p_k6"},
            {chart::N15, "bomb_1p_k7"},
            {chart::N21, "bomb_2p_k1"},
            {chart::N22, "bomb_2p_k2"},
            {chart::N23, "bomb_2p_k3"},
            {chart::N24, "bomb_2p_k4"},
            {chart::N25, "bomb_2p_k5"},
            {chart::Sc2, "bomb_2p_s"},
        }},
        {{
            {chart::Sc1, "bomb_1p_s"},
            {chart::N11, "bomb_1p_k3"},
            {chart::N12, "bomb_1p_k4"},
            {chart::N13, "bomb_1p_k5"},
            {chart::N14, "bomb_1p_k6"},
            {chart::N15, "bomb_1p_k7"},
            {chart::N21, "bomb_2p_k3"},
            {chart::N22, "bomb_2p_k4"},
            {chart::N23, "bomb_2p_k5"},
            {chart::N24, "bomb_2p_k6"},
            {chart::N25, "bomb_2p_k7"},
            {chart::Sc2, "bomb_2p_s"},
        }},
    };

    static const NoteLaneTimerMap bombTimer5kLN[] = {
        {{
            {chart::Sc1, "bomb_ln_1p_s"},
            {chart::N11, "bomb_ln_1p_k1"},
            {chart::N12, "bomb_ln_1p_k2"},
            {chart::N13, "bomb_ln_1p_k3"},
            {chart::N14, "bomb_ln_1p_k4"},
            {chart::N15, "bomb_ln_1p_k5"},
            {chart::N21, "bomb_ln_2p_k1"},
            {chart::N22, "bomb_ln_2p_k2"},
            {chart::N23, "bomb_ln_2p_k3"},
            {chart::N24, "bomb_ln_2p_k4"},
            {chart::N25, "bomb_ln_2p_k5"},
            {chart::Sc2, "bomb_ln_2p_s"},
        }},
        {{
            {chart::Sc1, "bomb_ln_1p_s"},
            {chart::N11, "bomb_ln_1p_k1"},
            {chart::N12, "bomb_ln_1p_k2"},
            {chart::N13, "bomb_ln_1p_k3"},
            {chart::N14, "bomb_ln_1p_k4"},
            {chart::N15, "bomb_ln_1p_k5"},
            {chart::N21, "bomb_ln_2p_k3"},
            {chart::N22, "bomb_ln_2p_k4"},
            {chart::N23, "bomb_ln_2p_k5"},
            {chart::N24, "bomb_ln_2p_k6"},
            {chart::N25, "bomb_ln_2p_k7"},
            {chart::Sc2, "bomb_ln_2p_s"},
        }},
        {{
            {chart::Sc1, "bomb_ln_1p_s"},
            {chart::N11, "bomb_ln_1p_k3"},
            {chart::N12, "bomb_ln_1p_k4"},
            {chart::N13, "bomb_ln_1p_k5"},
            {chart::N14, "bomb_ln_1p_k6"},
            {chart::N15, "bomb_ln_1p_k7"},
            {chart::N21, "bomb_ln_2p_k1"},
            {chart::N22, "bomb_ln_2p_k2"},
            {chart::N23, "bomb_ln_2p_k3"},
            {chart::N24, "bomb_ln_2p_k4"},
            {chart::N25, "bomb_ln_2p_k5"},
            {chart::Sc2, "bomb_ln_2p_s"},
        }},
        {{
            {chart::Sc1, "bomb_ln_1p_s"},
            {chart::N11, "bomb_ln_1p_k3"},
            {chart::N12, "bomb_ln_1p_k4"},
            {chart::N13, "bomb_ln_1p_k5"},
            {chart::N14, "bomb_ln_1p_k6"},
            {chart::N15, "bomb_ln_1p_k7"},
            {chart::N21, "bomb_ln_2p_k3"},
            {chart::N22, "bomb_ln_2p_k4"},
            {chart::N23, "bomb_ln_2p_k5"},
            {chart::N24, "bomb_ln_2p_k6"},
            {chart::N25, "bomb_ln_2p_k7"},
            {chart::Sc2, "bomb_ln_2p_s"},
        }},
    };

    static const NoteLaneTimerMap bombTimer7k = { {
        {chart::Sc1, "bomb_1p_s"},
        {chart::N11, "bomb_1p_k1"},
        {chart::N12, "bomb_1p_k2"},
        {chart::N13, "bomb_1p_k3"},
        {chart::N14, "bomb_1p_k4"},
        {chart::N15, "bomb_1p_k5"},
        {chart::N16, "bomb_1p_k6"},
        {chart::N17, "bomb_1p_k7"},
        {chart::N21, "bomb_2p_k1"},
        {chart::N22, "bomb_2p_k2"},
        {chart::N23, "bomb_2p_k3"},
        {chart::N24, "bomb_2p_k4"},
        {chart::N25, "bomb_2p_k5"},
        {chart::N26, "bomb_2p_k6"},
        {chart::N27, "bomb_2p_k7"},
        {chart::Sc2, "bomb_2p_s"},
    } };

    static const NoteLaneTimerMap bombTimer7kLN = { {
        {chart::Sc1, "bomb_ln_1p_s"},
        {chart::N11, "bomb_ln_1p_k1"},
        {chart::N12, "bomb_ln_1p_k2"},
        {chart::N13, "bomb_ln_1p_k3"},
        {chart::N14, "bomb_ln_1p_k4"},
        {chart::N15, "bomb_ln_1p_k5"},
        {chart::N16, "bomb_ln_1p_k6"},
        {chart::N17, "bomb_ln_1p_k7"},
        {chart::N21, "bomb_ln_2p_k1"},
        {chart::N22, "bomb_ln_2p_k2"},
        {chart::N23, "bomb_ln_2p_k3"},
        {chart::N24, "bomb_ln_2p_k4"},
        {chart::N25, "bomb_ln_2p_k5"},
        {chart::N26, "bomb_ln_2p_k6"},
        {chart::N27, "bomb_ln_2p_k7"},
        {chart::Sc2, "bomb_ln_2p_s"},
    } };

    static const NoteLaneTimerMap bombTimer9k = { {
        {chart::N11, "bomb_1p_k1"},
        {chart::N12, "bomb_1p_k2"},
        {chart::N13, "bomb_1p_k3"},
        {chart::N14, "bomb_1p_k4"},
        {chart::N15, "bomb_1p_k5"},
        {chart::N16, "bomb_1p_k6"},
        {chart::N17, "bomb_1p_k7"},
        {chart::N18, "bomb_1p_k8"},
        {chart::N19, "bomb_1p_k9"},
    } };

    static const NoteLaneTimerMap bombTimer9kLN = { {
        {chart::N11, "bomb_ln_1p_k1"},
        {chart::N12, "bomb_ln_1p_k2"},
        {chart::N13, "bomb_ln_1p_k3"},
        {chart::N14, "bomb_ln_1p_k4"},
        {chart::N15, "bomb_ln_1p_k5"},
        {chart::N16, "bomb_ln_1p_k6"},
        {chart::N17, "bomb_ln_1p_k7"},
        {chart::N18, "bomb_ln_1p_k8"},
        {chart::N19, "bomb_ln_1p_k9"},
    } };

    switch (keys)
    {
    case 5:
    case 10: 
    {
        size_t mapIndex = 0;
        if (PlayData.shift1PNotes5KFor7KSkin)
        {
            mapIndex = PlayData.shift2PNotes5KFor7KSkin ? 3 : 2;
        }
        else
        {
            mapIndex = PlayData.shift2PNotes5KFor7KSkin ? 1 : 0;
        }
        _bombTimerMap = &bombTimer5k[mapIndex];  _bombLNTimerMap = &bombTimer5kLN[mapIndex];
        break;
    }
    case 7:  
    case 14: _bombTimerMap = &bombTimer7k;  _bombLNTimerMap = &bombTimer7kLN; break;
    case 9:  _bombTimerMap = &bombTimer9k;  _bombLNTimerMap = &bombTimer9kLN; break;
    default: break;
    }

    switch (keys)
    {
    case 7:
    case 14: maxMoneyScore = 200000.0; break;
    case 5:
    case 10:
    case 9:  maxMoneyScore = 100000.0; break;
    default: break;
    }

    if (_chart)
    {
        noteCount = _chart->getNoteRegularCount() + _chart->getNoteLnCount();
    }

    using namespace std::string_literals;

    _basic.health = health;
    initGaugeParams(gauge);

    _side = side;
	switch (side)
	{
	case RulesetBMS::PlaySide::SINGLE:
        _k1P = true;
        _k2P = false;
        _judgeScratch = !(PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;
	case RulesetBMS::PlaySide::DOUBLE:
		_k1P = true;
		_k2P = true;
        _judgeScratch = !(PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
		break;
	case RulesetBMS::PlaySide::BATTLE_1P:
		_k1P = true;
		_k2P = false;
        _judgeScratch = !(PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
		break;
	case RulesetBMS::PlaySide::BATTLE_2P:
		_k1P = false;
		_k2P = true;
        _judgeScratch = !(PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
		break;
    default:
        _k1P = true;
        _k2P = true;
        _judgeScratch = false;
        break;
	}

    static const std::map<PlayModifierRandomType, std::string> randomTextMap =
    {
        { PlayModifierRandomType::NONE, "" },
        { PlayModifierRandomType::MIRROR, "MIRROR" },
        { PlayModifierRandomType::RANDOM, "RANDOM" },
        { PlayModifierRandomType::SRAN, "S-RANDOM" },
        { PlayModifierRandomType::HRAN, "H-RANDOM" },
        { PlayModifierRandomType::ALLSCR, "ALLSCR" },
        { PlayModifierRandomType::RRAN, "R-RANDOM" },
        { PlayModifierRandomType::DB_SYNCHRONIZE, "SYNCHRONIZE" },
        { PlayModifierRandomType::DB_SYMMETRY, "SYMMETRY" },
    };

    static const std::map<PlayModifierRandomType, std::string> randomTextShortMap =
    {
        { PlayModifierRandomType::NONE, "" },
        { PlayModifierRandomType::MIRROR, "MIR" },
        { PlayModifierRandomType::RANDOM, "RAN" },
        { PlayModifierRandomType::SRAN, "S-RAN" },
        { PlayModifierRandomType::HRAN, "H-RAN" },
        { PlayModifierRandomType::ALLSCR, "ASCR" },
        { PlayModifierRandomType::RRAN, "R-RAN" },
        { PlayModifierRandomType::DB_SYNCHRONIZE, "SYNC" },
        { PlayModifierRandomType::DB_SYMMETRY, "SYMM" },
    };

    static const char* assistText[] =
    {
        "5KEYS",
        "AUTO-SCR",
        "LEGACY",
        "NO-MINES"
    };

    std::stringstream ssMod;
    std::stringstream ssModShort;
    switch (side)
    {
    case RulesetBMS::PlaySide::SINGLE:
    case RulesetBMS::PlaySide::BATTLE_1P:
    case RulesetBMS::PlaySide::AUTO:
        ssMod << randomTextMap.at(PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft);
        ssModShort << randomTextShortMap.at(PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft);
        for (int i = 0; i < sizeof(assistText) / sizeof(assistText[0]); i++)
        {
            if (PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & (1 << i))
            {
                if (!ssMod.str().empty()) ssMod << ", ";
                if (!ssModShort.str().empty()) ssModShort << ",";
                ssMod << "" << assistText[i];
                ssModShort << "" << assistText[i];
            }
        }
        break;

    case RulesetBMS::PlaySide::RIVAL:
    case RulesetBMS::PlaySide::BATTLE_2P:
    case RulesetBMS::PlaySide::AUTO_2P:
        ssMod << randomTextMap.at(PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft);
        ssModShort << randomTextShortMap.at(PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft);
        for (int i = 0; i < sizeof(assistText) / sizeof(assistText[0]); i++)
        {
            if (PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask & (1 << i))
            {
                if (!ssMod.str().empty()) ssMod << ", ";
                if (!ssModShort.str().empty()) ssModShort << ",";
                ssMod << "" << assistText[i];
                ssModShort << "" << assistText[i];
            }
        }
        break;

    case RulesetBMS::PlaySide::DOUBLE:
    case RulesetBMS::PlaySide::AUTO_DOUBLE:
        {
        std::stringstream ss1p, ss2p, ssAssist;

        ss1p << randomTextShortMap.at(PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft);
        ss2p << randomTextShortMap.at(PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight);
        for (int i = 0; i < sizeof(assistText) / sizeof(assistText[0]); i++)
        {
            if (PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & (1 << i))
            {
                if (!ssAssist.str().empty()) ssAssist << ", ";
                ssAssist << "" << assistText[i];
            }
        }
        std::string s1p = ss1p.str();
        std::string s2p = ss2p.str();
        std::string sa = ssAssist.str();
        if (s1p.empty() && s2p.empty())
        {
            ssMod << sa;
            ssModShort << sa;
        }
        else
        {
            ssMod << (!s1p.empty() ? s1p : "-") << "/" << (!s2p.empty() ? s2p : "-");
            if (!sa.empty())
            {
                if (!ssMod.str().empty()) ssMod << ",";
                ssMod << sa;
            }
            ssModShort << ssMod.str();
        }
        }
        break;
    }
    modifierText = ssMod.str();
    if (modifierText.empty())
    {
        modifierText = "";
    }
    modifierTextShort = ssMod.str();

    saveLampMax = getMaxSaveScoreType().second;

    _lnJudge.fill(JudgeArea::NOTHING);

    if (_chart)
    {
        for (size_t k = Input::S1L; k <= Input::K2SPDDN; ++k)
        {
            NoteLaneIndex idx;
            idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
            if (idx != NoteLaneIndex::_) _noteListIterators[{NoteLaneCategory::Note, idx}] = _chart->firstNote(NoteLaneCategory::Note, idx);
            idx = _chart->getLaneFromKey(NoteLaneCategory::LN, (Input::Pad)k);
            if (idx != NoteLaneIndex::_) _noteListIterators[{NoteLaneCategory::LN, idx}] = _chart->firstNote(NoteLaneCategory::LN, idx);
            idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, (Input::Pad)k);
            if (idx != NoteLaneIndex::_) _noteListIterators[{NoteLaneCategory::Mine, idx}] = _chart->firstNote(NoteLaneCategory::Mine, idx);
            idx = _chart->getLaneFromKey(NoteLaneCategory::Invs, (Input::Pad)k);
            if (idx != NoteLaneIndex::_) _noteListIterators[{NoteLaneCategory::Invs, idx}] = _chart->firstNote(NoteLaneCategory::Invs, idx);
        }
    }
}

void RulesetBMS::initGaugeParams(PlayModifierGaugeType gauge)
{
    switch (gauge)
    {
    case PlayModifierGaugeType::HARD:               _gauge = GaugeType::HARD;    break;
    case PlayModifierGaugeType::DEATH:              _gauge = GaugeType::DEATH;   break;
    case PlayModifierGaugeType::EASY:               _gauge = GaugeType::EASY;    break;
        //case PlayModifierGaugeType::PATTACK     : _gauge = GaugeType::P_ATK;   break;
        //case PlayModifierGaugeType::GATTACK     : _gauge = GaugeType::G_ATK;   break;
    case PlayModifierGaugeType::ASSISTEASY:         _gauge = GaugeType::ASSIST;  break;
    case PlayModifierGaugeType::CLASS_NORMAL:       _gauge = GaugeType::CLASS;   break;
    case PlayModifierGaugeType::CLASS_DEATH:        _gauge = GaugeType::EXGRADE; break;
    case PlayModifierGaugeType::EXHARD:             _gauge = GaugeType::EXHARD;  break;
    case PlayModifierGaugeType::CLASS_HARD:         _gauge = GaugeType::EXGRADE; break;
    case PlayModifierGaugeType::NORMAL:
    default:                            _gauge = GaugeType::GROOVE;  break;
    }

    if (_format)
    {
        switch (_format->type())
        {
        case eChartFormat::BMS:
            _format->getExtendedProperty("TOTAL", (void*)&total);
            break;

        case eChartFormat::BMSON:
        default:
            break;
        }
    }
    if (total < 0)
    {
        switch (_gauge)
        {
        case RulesetBMS::GaugeType::HARD:
        case RulesetBMS::GaugeType::EXHARD:
        case RulesetBMS::GaugeType::DEATH:
        case RulesetBMS::GaugeType::CLASS:
        case RulesetBMS::GaugeType::EXGRADE:
            total = 300;
            break;
        case RulesetBMS::GaugeType::GROOVE:
        case RulesetBMS::GaugeType::EASY:
        case RulesetBMS::GaugeType::ASSIST:
        default:
            total = 160;
            break;
        }
    }

    switch (_gauge)
    {
    case GaugeType::HARD:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD] = -0.06;
        _healthGain[JudgeType::MISS] = -0.1;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::EXHARD:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD] = -0.12;
        _healthGain[JudgeType::MISS] = -0.2;
        _healthGain[JudgeType::KPOOR] = -0.1;
        break;

    case GaugeType::DEATH:
        //_basic.health               = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::GOOD] = 0.0;
        _healthGain[JudgeType::BAD] = -1.0;
        _healthGain[JudgeType::MISS] = -1.0;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::P_ATK:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = -0.02;
        _healthGain[JudgeType::GOOD] = -1.0;
        _healthGain[JudgeType::BAD] = -1.0;
        _healthGain[JudgeType::MISS] = -1.0;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::G_ATK:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = -0.02;
        _healthGain[JudgeType::GREAT] = -0.02;
        _healthGain[JudgeType::GOOD] = 0.0;
        _healthGain[JudgeType::BAD] = -1.0;
        _healthGain[JudgeType::MISS] = -1.0;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::GROOVE:
        //_basic.health             = 0.2;
        _minHealth = 0.02;
        _clearHealth = 0.8;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / noteCount;
        _healthGain[JudgeType::GREAT] = 0.01 * total / noteCount;
        _healthGain[JudgeType::GOOD] = 0.01 * total / noteCount / 2;
        _healthGain[JudgeType::BAD] = -0.04;
        _healthGain[JudgeType::MISS] = -0.06;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::EASY:
        //_basic.health             = 0.2;
        _minHealth = 0.02;
        _clearHealth = 0.8;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / noteCount * 1.2;
        _healthGain[JudgeType::GREAT] = 0.01 * total / noteCount * 1.2;
        _healthGain[JudgeType::GOOD] = 0.01 * total / noteCount / 2 * 1.2;
        _healthGain[JudgeType::BAD] = -0.032;
        _healthGain[JudgeType::MISS] = -0.048;
        _healthGain[JudgeType::KPOOR] = -0.016;
        break;

    case GaugeType::ASSIST:
        //_basic.health             = 0.2;
        _minHealth = 0.02;
        _clearHealth = 0.6;
        _healthGain[JudgeType::PERFECT] = 0.01 * total / noteCount * 1.2;
        _healthGain[JudgeType::GREAT] = 0.01 * total / noteCount * 1.2;
        _healthGain[JudgeType::GOOD] = 0.01 * total / noteCount / 2 * 1.2;
        _healthGain[JudgeType::BAD] = -0.032;
        _healthGain[JudgeType::MISS] = -0.048;
        _healthGain[JudgeType::KPOOR] = -0.016;
        break;

    case GaugeType::CLASS:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD] = -0.02;
        _healthGain[JudgeType::MISS] = -0.03;
        _healthGain[JudgeType::KPOOR] = -0.02;
        break;

    case GaugeType::EXGRADE:
        //_basic.health             = 1.0;
        _minHealth = 0;
        _clearHealth = 0;
        _failWhenNoHealth = true;
        _healthGain[JudgeType::PERFECT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GREAT] = 1.0 / 1001.0;
        _healthGain[JudgeType::GOOD] = 1.0 / 1001.0 / 2;
        _healthGain[JudgeType::BAD] = -0.12;
        _healthGain[JudgeType::MISS] = -0.1;
        _healthGain[JudgeType::KPOOR] = -0.1;
        break;

    default:
        break;
    }
}

RulesetBMS::JudgeRes RulesetBMS::_judge(const Note& note, Time time)
{
    // spot judge area
    JudgeArea a = JudgeArea::NOTHING;
	Time error = time - note.time;
    if (error > -judgeTime[(size_t)_judgeDifficulty].KPOOR)
    {
        if (error < -judgeTime[(size_t)_judgeDifficulty].BAD)
            a = JudgeArea::EARLY_KPOOR;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].GOOD)
            a = JudgeArea::EARLY_BAD;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].GREAT)
            a = JudgeArea::EARLY_GOOD;
        else if (error < -judgeTime[(size_t)_judgeDifficulty].PERFECT)
            a = JudgeArea::EARLY_GREAT;
        else if (error < 0)
            a = JudgeArea::EARLY_PERFECT;
        else if (error == 0)
            a = JudgeArea::EXACT_PERFECT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].PERFECT)
            a = JudgeArea::LATE_PERFECT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].GREAT)
            a = JudgeArea::LATE_GREAT;
        else if (error < judgeTime[(size_t)_judgeDifficulty].GOOD)
            a = JudgeArea::LATE_GOOD;
        else if (error < judgeTime[(size_t)_judgeDifficulty].BAD)
            a = JudgeArea::LATE_BAD;
    }

    // log
    /*
    switch (a)
    {
    case JudgeArea::EARLY_KPOOR:   LOG_DEBUG << "EARLY  KPOOR   " << error; break;
    case JudgeArea::EARLY_BAD:     LOG_DEBUG << "EARLY  BAD     " << error; break;
    case JudgeArea::EARLY_GOOD:    LOG_DEBUG << "EARLY  GOOD    " << error; break;
    case JudgeArea::EARLY_GREAT:   LOG_DEBUG << "EARLY  GREAT   " << error; break;
    case JudgeArea::EARLY_PERFECT: LOG_DEBUG << "EARLY  PERFECT " << error; break;
    case JudgeArea::LATE_PERFECT:  LOG_DEBUG << "LATE   PERFECT " << error; break;
    case JudgeArea::LATE_GREAT:    LOG_DEBUG << "LATE   GREAT   " << error; break;
    case JudgeArea::LATE_GOOD:     LOG_DEBUG << "LATE   GOOD    " << error; break;
    case JudgeArea::LATE_BAD:      LOG_DEBUG << "LATE   BAD     " << error; break;
    }
    */

    return { a, error };
}

static const std::map<RulesetBMS::JudgeArea, ReplayChart::Commands::Type> judgeAreaReplayCommandType[] =
{
    {
        {RulesetBMS::JudgeArea::EXACT_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_EXACT_0},
        {RulesetBMS::JudgeArea::EARLY_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_0},
        {RulesetBMS::JudgeArea::EARLY_GREAT, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_1},
        {RulesetBMS::JudgeArea::EARLY_GOOD, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_2},
        {RulesetBMS::JudgeArea::EARLY_BAD, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_3},
        {RulesetBMS::JudgeArea::EARLY_KPOOR, ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_5},
        {RulesetBMS::JudgeArea::LATE_PERFECT, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_0},
        {RulesetBMS::JudgeArea::LATE_GREAT, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_1},
        {RulesetBMS::JudgeArea::LATE_GOOD, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_2},
        {RulesetBMS::JudgeArea::LATE_BAD, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_3},
        {RulesetBMS::JudgeArea::MISS, ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4},
    },
    {
        {RulesetBMS::JudgeArea::EXACT_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_EXACT_0},
        {RulesetBMS::JudgeArea::EARLY_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_0},
        {RulesetBMS::JudgeArea::EARLY_GREAT, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_1},
        {RulesetBMS::JudgeArea::EARLY_GOOD, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_2},
        {RulesetBMS::JudgeArea::EARLY_BAD, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_3},
        {RulesetBMS::JudgeArea::EARLY_KPOOR, ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_5},
        {RulesetBMS::JudgeArea::LATE_PERFECT, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_0},
        {RulesetBMS::JudgeArea::LATE_GREAT, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_1},
        {RulesetBMS::JudgeArea::LATE_GOOD, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_2},
        {RulesetBMS::JudgeArea::LATE_BAD, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_3},
        {RulesetBMS::JudgeArea::MISS, ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4},
    }
};

void RulesetBMS::_judgePress(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
    if (cat == NoteLaneCategory::LN && 
        (note.flags & Note::LN_TAIL) &&
        (idx == NoteLaneIndex::Sc1 || idx == NoteLaneIndex::Sc2) && 
        _lnJudge[idx] != JudgeArea::NOTHING)
    {
        // Handle scratch direction change as miss
        _judgeRelease(cat, idx, note, judge, t, slot);
        if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
            PlayData.timers[_bombLNTimerMap->at(idx)] = TIMER_NEVER;
    }

    bool pushReplayCommand = false;
    switch (cat)
    {
    case NoteLaneCategory::Note:
        switch (judge.area)
        {
        case JudgeArea::EARLY_PERFECT:
        case JudgeArea::EXACT_PERFECT:
        case JudgeArea::LATE_PERFECT:
        case JudgeArea::EARLY_GREAT:
        case JudgeArea::LATE_GREAT:
        case JudgeArea::EARLY_GOOD:
        case JudgeArea::LATE_GOOD:
            updateJudge(t, idx, judge.area, slot);
            pushReplayCommand = true;
            note.hit = true;
            note.expired = true;
            notesExpired++;
            break;

        case JudgeArea::EARLY_BAD:
        case JudgeArea::LATE_BAD:
            updateJudge(t, idx, judge.area, slot);
            pushReplayCommand = true;
            note.expired = true;
            notesExpired++;
            break;

        case JudgeArea::EARLY_KPOOR:
            updateJudge(t, idx, judge.area, slot);
            pushReplayCommand = true;
            break;
        }
        break;

    case NoteLaneCategory::Invs:
        break;

    case NoteLaneCategory::LN:
        if (!(note.flags & Note::LN_TAIL))
        {
            switch (judge.area)
            {
            case JudgeArea::EARLY_PERFECT:
            case JudgeArea::EXACT_PERFECT:
            case JudgeArea::LATE_PERFECT:
            case JudgeArea::EARLY_GREAT:
            case JudgeArea::LATE_GREAT:
            case JudgeArea::EARLY_GOOD:
            case JudgeArea::LATE_GOOD:
                _lnJudge[idx] = judge.area;
                note.hit = true;
                note.expired = true;
                if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                    PlayData.timers[_bombLNTimerMap->at(idx)] = t.norm();
                break;

            case JudgeArea::EARLY_BAD:
            case JudgeArea::LATE_BAD:
                updateJudge(t, idx, judge.area, slot);
                note.expired = true;
                notesExpired++;
                pushReplayCommand = true;
                break;

            case JudgeArea::EARLY_KPOOR:
                updateJudge(t, idx, judge.area, slot);
                pushReplayCommand = true;
                break;
            }
            break;
        }
        break;
    }

    if (note.expired || judge.area == JudgeArea::EARLY_KPOOR || judge.area == JudgeArea::MINE_KPOOR)
    {
        _lastNoteJudge[slot] = judge;
    }

    unsigned max = getNoteCount() * 2;
    _basic.total_acc = 100.0 * exScore / max;
    _basic.acc = notesExpired ? (100.0 * exScore / notesExpired / 2) : 0;

    // push replay command
    if (pushReplayCommand && doJudge && PlayData.playStarted && PlayData.replayNew)
    {
        if (judgeAreaReplayCommandType[slot].find(judge.area) != judgeAreaReplayCommandType[slot].end())
        {
            long long ms = t.norm() - _startTime.norm();
            ReplayChart::Commands cmd;
            cmd.ms = ms;
            cmd.type = judgeAreaReplayCommandType[slot].at(judge.area);
            PlayData.replayNew->commands.push_back(cmd);
        }
    }
}
void RulesetBMS::_judgeHold(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
    switch (cat)
    {
    case NoteLaneCategory::Mine:
    {
        if (judge.area == JudgeArea::EXACT_PERFECT ||
            judge.area == JudgeArea::EARLY_PERFECT && judge.time < -2 ||
            judge.area == JudgeArea::LATE_PERFECT && judge.time < 2)
        {
            note.hit = true;
            note.expired = true;
            _updateHp(-0.01 * note.dvalue / 2);

            // kpoor + 1
            for (auto& i : JudgeAreaIndexAccMap.at(JudgeArea::MINE_KPOOR))
            {
                ++_basic.judge[i];
            }
            if (showJudge)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    setJudgeTimer1P(JudgeType::KPOOR, t.norm());
                    SoundMgr::playSysSample(SoundChannelType::KEY_LEFT, eSoundSample::SOUND_LANDMINE);
                }
                else if (slot == PLAYER_SLOT_TARGET)
                {
                    setJudgeTimer2P(JudgeType::KPOOR, t.norm());
                    SoundMgr::playSysSample(SoundChannelType::KEY_RIGHT, eSoundSample::SOUND_LANDMINE);
                }
            }

            _lastNoteJudge = { JudgeArea::MINE_KPOOR, t.norm() };

            unsigned max = getNoteCount() * 2;
            _basic.total_acc = 100.0 * exScore / max;
            _basic.acc = notesExpired ? (100.0 * exScore / notesExpired / 2) : 0;

            // push replay command
            if (doJudge && PlayData.playStarted && PlayData.replayNew)
            {
                long long ms = t.norm() - _startTime.norm();
                ReplayChart::Commands cmd;
                cmd.ms = ms;
                cmd.type = slot == PLAYER_SLOT_PLAYER ? ReplayChart::Commands::Type::JUDGE_LEFT_LANDMINE : ReplayChart::Commands::Type::JUDGE_RIGHT_LANDMINE;
                PlayData.replayNew->commands.push_back(cmd);
            }
        }
        break;
    }
    case NoteLaneCategory::LN:
        if ((note.flags & Note::LN_TAIL) &&
            _lnJudge[idx] != RulesetBMS::JudgeArea::NOTHING &&
            _lnJudge[idx] != RulesetBMS::JudgeArea::EARLY_BAD &&
            _lnJudge[idx] != RulesetBMS::JudgeArea::LATE_BAD)
        {
            if (judge.area == JudgeArea::EXACT_PERFECT ||
                judge.area == JudgeArea::EARLY_PERFECT && judge.time < -2 ||
                judge.area == JudgeArea::LATE_PERFECT && judge.time < 2)
            {
                updateJudge(t, idx, _lnJudge[idx], slot);
                note.hit = true;
                note.expired = true;
                notesExpired++;

                if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                    PlayData.timers[_bombLNTimerMap->at(idx)] = TIMER_NEVER;

                _lastNoteJudge[slot].area = _lnJudge[idx];
                _lastNoteJudge[slot].time = 0;

                unsigned max = getNoteCount() * 2;
                _basic.total_acc = 100.0 * exScore / max;
                _basic.acc = notesExpired ? (100.0 * exScore / notesExpired / 2) : 0;

                // push replay command
                if (doJudge && PlayData.playStarted && PlayData.replayNew)
                {
                    if (judgeAreaReplayCommandType[slot].find(_lnJudge[idx]) != judgeAreaReplayCommandType[slot].end())
                    {
                        long long ms = t.norm() - _startTime.norm();
                        ReplayChart::Commands cmd;
                        cmd.ms = ms;
                        cmd.type = judgeAreaReplayCommandType[slot].at(_lnJudge[idx]);
                        PlayData.replayNew->commands.push_back(cmd);
                    }
                }

                _lnJudge[idx] = RulesetBMS::JudgeArea::NOTHING;
            }
        }
        break;

    default:
        break;
    }
}
void RulesetBMS::_judgeRelease(NoteLaneCategory cat, NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot)
{
    bool pushReplayCommand = false;
    switch (cat)
    {
    case NoteLaneCategory::LN:
        if ((note.flags & Note::LN_TAIL) &&
            _lnJudge[idx] != RulesetBMS::JudgeArea::NOTHING)
        {
            bool hit = true;
            JudgeArea lnJudge = judge.area;
            switch (judge.area)
            {
            case JudgeArea::NOTHING:
            case JudgeArea::EARLY_KPOOR:
            case JudgeArea::EARLY_BAD:
                lnJudge = JudgeArea::EARLY_BAD;
                hit = false;
                break;

            default:
                switch (_lnJudge[idx])
                {
                case JudgeArea::EARLY_PERFECT:
                case JudgeArea::LATE_PERFECT:
                    if (judge.area == JudgeArea::EARLY_PERFECT)
                        lnJudge = JudgeArea::EARLY_PERFECT;
                    break;

                case JudgeArea::EARLY_GREAT:
                case JudgeArea::LATE_GREAT:
                    if (judge.area == JudgeArea::EARLY_PERFECT || judge.area == JudgeArea::EARLY_GREAT)
                        lnJudge = JudgeArea::EARLY_GREAT;
                    break;

                case JudgeArea::EARLY_GOOD:
                case JudgeArea::LATE_GOOD:
                    if (judge.area == JudgeArea::EARLY_PERFECT || judge.area == JudgeArea::EARLY_GREAT || judge.area == JudgeArea::EARLY_GOOD)
                        lnJudge = JudgeArea::EARLY_GOOD;
                    break;

                default:
                    lnJudge = JudgeArea::EARLY_BAD;
                    break;
                }
                break;
            }

            updateJudge(t, idx, lnJudge, slot);
            note.hit = hit;
            note.expired = true;
            notesExpired++;
            _lnJudge[idx] = RulesetBMS::JudgeArea::NOTHING;
            _lastNoteJudge[slot] = judge;
            pushReplayCommand = true;

            if (showJudge && _bombLNTimerMap != nullptr && _bombLNTimerMap->find(idx) != _bombLNTimerMap->end())
                PlayData.timers[_bombLNTimerMap->at(idx)] = TIMER_NEVER;

            break;
        }
        break;

    default:
        break;
    }

    unsigned max = getNoteCount() * 2;
    _basic.total_acc = 100.0 * exScore / max;
    _basic.acc = notesExpired ? (100.0 * exScore / notesExpired / 2) : 0;

    // push replay command
    if (pushReplayCommand && doJudge && PlayData.playStarted && PlayData.replayNew)
    {
        if (judgeAreaReplayCommandType[slot].find(judge.area) != judgeAreaReplayCommandType[slot].end())
        {
            long long ms = t.norm() - _startTime.norm();
            ReplayChart::Commands cmd;
            cmd.ms = ms;
            cmd.type = judgeAreaReplayCommandType[slot].at(judge.area);
            PlayData.replayNew->commands.push_back(cmd);
        }
    }
}

void RulesetBMS::_updateHp(double diff)
{
    // TOTAL補正, totalnotes補正
    // ref: https://web.archive.org/web/20150226213104/http://2nd.geocities.jp/yoshi_65c816/bms/LR2.html
    switch (_gauge)
    {
    case RulesetBMS::GaugeType::HARD:
    case RulesetBMS::GaugeType::EXHARD:
        if (diff < 0)
        {
            double pTotal = 1.0;
            if (total >= 240);
            else if (total >= 230) pTotal = 10.0 / 9;
            else if (total >= 210) pTotal = 1.25;
            else if (total >= 200) pTotal = 1.5;
            else if (total >= 180) pTotal = 5.0 / 3;
            else if (total >= 160) pTotal = 2.0;
            else if (total >= 150) pTotal = 2.5;
            else if (total >= 130) pTotal = 10.0 / 3;
            else if (total >= 120) pTotal = 5.0;
            else                   pTotal = 10.0;

            double pNotes = 1.0;
            int notes = getNoteCount();
            if (notes >= 1000);
            else if (notes >= 500) pNotes = (notes - 500) * 0.002;
            else if (notes >= 250) pNotes = 1.0 + (notes - 250) * 0.004;
            else if (notes >= 125) pNotes = 2.0 + (notes - 125) * 0.008;
            else if (notes >= 62)  pNotes = 3.0 + (notes - 62) * (1.0 / 62);
            else if (notes >= 31)  pNotes = 4.0 + (notes - 31) * (1.0 / 31);
            else if (notes >= 16)  pNotes = 5.0 + (notes - 16) * 0.0625;
            else if (notes >= 8)   pNotes = 6.0 + (notes - 8) * 0.125;
            else if (notes >= 4)   pNotes = 7.0 + (notes - 4) * 0.25;
            else if (notes >= 2)   pNotes = 8.0 + (notes - 2) * 0.50;
            else if (notes == 1)   pNotes = 9.0;
            else                   pNotes = 10.0;

            diff *= 1.0 * std::max(pTotal, pNotes);
        }
    }

    double tmp = _basic.health;

    // 30% buff
    switch (_gauge)
    {
    case RulesetBMS::GaugeType::HARD:
    case RulesetBMS::GaugeType::CLASS:
        if (tmp < 0.32 && diff < 0.0)
            tmp += diff * 0.6;
        else
            tmp += diff;
        break;
    default:
        tmp += diff;
        break;
    }

    _basic.health = std::max(_minHealth, std::min(1.0, tmp));

    if (failWhenNoHealth() && _basic.health <= _minHealth)
    {
        fail();
    }
}
void RulesetBMS::_updateHp(JudgeArea judge)
{
    _updateHp(_healthGain.at(JudgeAreaTypeMap.at(judge)));
}

void RulesetBMS::updateJudge(const Time& t, NoteLaneIndex ch, RulesetBMS::JudgeArea judge, int slot, bool force)
{
    if (isFailed()) return;
    
    if (doJudge || force)
    {
        for (auto& i : JudgeAreaIndexAccMap.at(judge))
        {
            ++_basic.judge[i];
        }

        switch (judge)
        {
        case JudgeArea::EARLY_PERFECT:
        case JudgeArea::EXACT_PERFECT:
        case JudgeArea::LATE_PERFECT:
            //moneyScore += 150000.0 / getNoteCount() +
            //    std::min(int(_basic.combo) - 1, 10) * 50000.0 / (10 * getNoteCount() - 55);
            moneyScore += 1.0 * maxMoneyScore / getNoteCount();
            exScore += 2;
            ++_basic.combo;
            break;

        case JudgeArea::EARLY_GREAT:
        case JudgeArea::LATE_GREAT:
            //moneyScore += 100000.0 / getNoteCount() +
            //    std::min(int(_basic.combo) - 1, 10) * 50000.0 / (10 * getNoteCount() - 55);
            moneyScore += 0.5 * maxMoneyScore / getNoteCount();
            exScore += 1;
            ++_basic.combo;
            break;

        case JudgeArea::EARLY_GOOD:
        case JudgeArea::LATE_GOOD:
            //moneyScore += 20000.0 / getNoteCount() +
            //    std::min(int(_basic.combo) - 1, 10) * 50000.0 / (10 * getNoteCount() - 55);
            moneyScore += 0.25 * maxMoneyScore / getNoteCount();
            ++_basic.combo;
            break;

        case JudgeArea::EARLY_BAD:
        case JudgeArea::LATE_BAD:
        case JudgeArea::MISS:
            _basic.combo = 0;
            _basic.comboEx = 0;
            break;

        default:
            break;
        }

        _updateHp(judge);
        if (_basic.combo > _basic.maxCombo)
            _basic.maxCombo = _basic.combo;
        if (_basic.combo + _basic.comboEx > _basic.maxComboDisplay)
            _basic.maxComboDisplay = _basic.combo + _basic.comboEx;
    }

    JudgeType judgeType = JudgeAreaTypeMap.at(judge);
    if (showJudge)
    {
        bool setBombTimer = false;
        switch (judgeType)
        {
        case JudgeType::PERFECT:
        case JudgeType::GREAT:
            setBombTimer = true;
            break;
        default:
            break;
        }
        if (_bombTimerMap == nullptr || _bombTimerMap->find(ch) == _bombTimerMap->end())
            setBombTimer = false;

        if (setBombTimer) 
            PlayData.timers[_bombTimerMap->at(ch)] = t.norm();

        if (slot == PLAYER_SLOT_PLAYER)
        {
            setJudgeTimer1P(judgeType, t.norm());
        }
        else if (slot == PLAYER_SLOT_TARGET)
        {
            setJudgeTimer2P(judgeType, t.norm());
        }
    }
}

void RulesetBMS::judgeNotePress(Input::Pad k, const Time& t, const Time& rt, int slot)
{
    NoteLaneIndex idx1 = _chart->getLaneFromKey(NoteLaneCategory::Note, k);
    HitableNote* pNote1 = nullptr;
    if (idx1 != _ && !_chart->isLastNote(NoteLaneCategory::Note, idx1))
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx1);
        while (!_chart->isLastNote(NoteLaneCategory::Note, idx1, itNote) && itNote->expired)
            ++itNote;
        if (!_chart->isLastNote(NoteLaneCategory::Note, idx1, itNote))
            pNote1 = &*itNote;
    }
    NoteLaneIndex idx2 = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    HitableNote* pNote2 = nullptr;
    if (idx2 != _ && !_chart->isLastNote(NoteLaneCategory::LN, idx2))
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx2);
        while (!_chart->isLastNote(NoteLaneCategory::LN, idx2, itNote) && itNote->expired)
            ++itNote;
        if (!_chart->isLastNote(NoteLaneCategory::LN, idx2, itNote))
            pNote2 = &*itNote;
    }

    JudgeRes j;
    if (pNote1 && (pNote2 == nullptr || pNote1->time < pNote2->time) && !pNote1->expired)
    {
        j = _judge(*pNote1, rt);
        _judgePress(NoteLaneCategory::Note, idx1, *pNote1, j, t, slot);
    }
    else if (pNote2 && !pNote2->expired)
    {
        j = _judge(*pNote2, rt);
        _judgePress(NoteLaneCategory::LN, idx2, *pNote2, j, t, slot);
    }

    // break-out BAD chain 
    if (j.area == JudgeArea::LATE_BAD)
    {
         judgeNotePress(k, t, rt, slot);
    }
}
void RulesetBMS::judgeNoteHold(Input::Pad k, const Time& t, const Time& rt, int slot)
{
    NoteLaneIndex idx; 

    idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, k);
    if (idx != _ && !_chart->isLastNote(NoteLaneCategory::Mine, idx))
    {
        auto& note = *_chart->incomingNote(NoteLaneCategory::Mine, idx);
        auto j = _judge(note, rt);
        _judgeHold(NoteLaneCategory::Mine, idx, note, j, t, slot);
    }

    idx = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    if (idx != _ && !_chart->isLastNote(NoteLaneCategory::LN, idx))
    {
        auto& note = *_chart->incomingNote(NoteLaneCategory::LN, idx);
        auto j = _judge(note, rt);
        _judgeHold(NoteLaneCategory::LN, idx, note, j, t, slot);
    }
}
void RulesetBMS::judgeNoteRelease(Input::Pad k, const Time& t, const Time& rt, int slot)
{
    NoteLaneIndex idx = _chart->getLaneFromKey(NoteLaneCategory::LN, k);
    if (idx != _)
    {
        auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx);
        while (!_chart->isLastNote(NoteLaneCategory::LN, idx, itNote))
        {
            if (!itNote->expired)
            {
;                auto j = _judge(*itNote, rt);
                _judgeRelease(NoteLaneCategory::LN, idx, *itNote, j, t, slot);
                break;
            }
            ++itNote;
        }
    }
}

void RulesetBMS::updatePress(InputMask& pg, const Time& t)
{
	Time rt = t - _startTime.norm();
    if (rt.norm() < 0) return;
    if (PlayData.isAuto) return;
    auto updatePressRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!pg[k]) continue;
            judgeNotePress((Input::Pad)k, t, rt, slot);
        }
    };
    if (_k1P) updatePressRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P) updatePressRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P)
        {
            if (pg[Input::S1L]) playerScratchDirection[PLAYER_SLOT_PLAYER] = AxisDir::AXIS_UP;
            if (pg[Input::S1R]) playerScratchDirection[PLAYER_SLOT_PLAYER] = AxisDir::AXIS_DOWN;
            updatePressRange(Input::S1L, Input::S1R, PLAYER_SLOT_PLAYER);
        }
        if (_k2P)
        {
            if (pg[Input::S2L]) playerScratchDirection[PLAYER_SLOT_TARGET] = AxisDir::AXIS_UP;
            if (pg[Input::S2R]) playerScratchDirection[PLAYER_SLOT_TARGET] = AxisDir::AXIS_DOWN;
            updatePressRange(Input::S2L, Input::S2R, PLAYER_SLOT_TARGET);
        }
    }
}
void RulesetBMS::updateHold(InputMask& hg, const Time& t)
{
	Time rt = t - _startTime.norm();
    if (rt < 0) return;
    if (PlayData.isAuto) return;

    auto updateHoldRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!hg[k]) continue;
            judgeNoteHold((Input::Pad)k, t, rt, slot);
        }
    };
    if (_k1P) updateHoldRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P) updateHoldRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P) updateHoldRange(Input::S1L, Input::S1R, PLAYER_SLOT_PLAYER);
        if (_k2P) updateHoldRange(Input::S2L, Input::S2R, PLAYER_SLOT_TARGET);
    }
}
void RulesetBMS::updateRelease(InputMask& rg, const Time& t)
{
	Time rt = t - _startTime.norm();
    if (rt < 0) return;
    if (PlayData.isAuto) return;

    auto updateReleaseRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            if (!rg[k]) continue;
            judgeNoteRelease((Input::Pad)k, t, rt, slot);
        }
    };
    if (_k1P) updateReleaseRange(Input::K11, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P) updateReleaseRange(Input::K21, Input::K29, PLAYER_SLOT_TARGET);
    if (_judgeScratch)
    {
        if (_k1P)
        {
            if (playerScratchDirection[PLAYER_SLOT_PLAYER] == AxisDir::AXIS_UP && rg[Input::S1L])
                updateReleaseRange(Input::S1L, Input::S1L, PLAYER_SLOT_PLAYER);
            if (playerScratchDirection[PLAYER_SLOT_PLAYER] == AxisDir::AXIS_DOWN && rg[Input::S1R])
                updateReleaseRange(Input::S1R, Input::S1R, PLAYER_SLOT_PLAYER);
        }
        if (_k2P)
        {
            if (playerScratchDirection[PLAYER_SLOT_TARGET] == AxisDir::AXIS_UP && rg[Input::S2L])
                updateReleaseRange(Input::S2L, Input::S2L, PLAYER_SLOT_TARGET);
            if (playerScratchDirection[PLAYER_SLOT_TARGET] == AxisDir::AXIS_DOWN && rg[Input::S2R])
                updateReleaseRange(Input::S2R, Input::S2R, PLAYER_SLOT_TARGET);
        }
    }
}
void RulesetBMS::updateAxis(double s1, double s2, const Time& t)
{
    Time rt = t - _startTime.norm();
    if (rt.norm() < 0) return;

    using namespace Input;

    if (!PlayData.isAuto && (!PlayData.isReplay || !PlayData.playStarted))
    {
        playerScratchAccumulator[PLAYER_SLOT_PLAYER] += s1;
        playerScratchAccumulator[PLAYER_SLOT_TARGET] += s2;
    }
}

void RulesetBMS::update(const Time& t)
{
    if (!_hasStartTime)
        setStartTime(t);

	auto rt = t - _startTime.norm();

    for (auto& [c, n]: _noteListIterators)
    {
        auto [cat, idx] = c;
        while (!_chart->isLastNote(cat, idx, n) && rt >= n->time)
        {
            switch (cat)
            {
            case NoteLaneCategory::Note:
                notesReached++;
                break;

            case NoteLaneCategory::LN:
                if (n->flags & Note::LN_TAIL)
                    notesReached++;
                break;
            }

            n++;
        }
    }

    auto updateRange = [&](Input::Pad begin, Input::Pad end, int slot)
    {
        for (size_t k = begin; k <= end; ++k)
        {
            bool scratch = false;
            switch (k)
            {
            case Input::S1L:
            case Input::S1R:
            case Input::S2L:
            case Input::S2R:
                scratch = true;
                break;
            }

            NoteLaneIndex idx;

            idx = _chart->getLaneFromKey(NoteLaneCategory::Note, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Note, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Note, idx, itNote) && !itNote->expired)
                {
                    Time hitTime = (!scratch || _judgeScratch) ? judgeTime[(size_t)_judgeDifficulty].BAD : 0;
                    if (rt - itNote->time >= hitTime)
                    {
                        itNote->expired = true;

                        if (doJudge && (!scratch || _judgeScratch))
                        {
                            updateJudge(t, idx, JudgeArea::MISS, slot);
                            _lastNoteJudge[slot].area = JudgeArea::MISS;
                            _lastNoteJudge[slot].time = hitTime;

                            // push replay command
                            if (PlayData.playStarted && PlayData.replayNew)
                            {
                                long long ms = t.norm() - _startTime.norm();
                                ReplayChart::Commands cmd;
                                cmd.ms = ms;
                                cmd.type = slot == PLAYER_SLOT_PLAYER ? ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4 : ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4;
                                PlayData.replayNew->commands.push_back(cmd);
                            }
                        }

                        notesExpired++;
                        //LOG_DEBUG << "LATE   POOR    "; break;
                    }
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::LN, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::LN, idx);
                while (!_chart->isLastNote(NoteLaneCategory::LN, idx, itNote) && !itNote->expired)
                {
                    if (!(itNote->flags & Note::LN_TAIL))
                    {
                        if (rt >= itNote->time)
                        {
                            Time hitTime = itNote->time + judgeTime[(size_t)_judgeDifficulty].BAD;
                            auto itTail = itNote;
                            itTail++;
                            if (!_chart->isLastNote(NoteLaneCategory::LN, idx, itTail) && (itTail->flags & Note::LN_TAIL) && hitTime > itTail->time)
                            {
                                hitTime = itTail->time;
                            }
                            if (rt >= hitTime)
                            {
                                itNote->expired = true;

                                if (!scratch || _judgeScratch)
                                {
                                    updateJudge(t, idx, JudgeArea::MISS, slot);
                                    _lastNoteJudge[slot].area = JudgeArea::MISS;
                                    _lastNoteJudge[slot].time = hitTime;

                                    // push replay command
                                    if (doJudge && PlayData.playStarted && PlayData.replayNew)
                                    {
                                        long long ms = t.norm() - _startTime.norm();
                                        ReplayChart::Commands cmd;
                                        cmd.ms = ms;
                                        cmd.type = slot == PLAYER_SLOT_PLAYER ? ReplayChart::Commands::Type::JUDGE_LEFT_LATE_3 : ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_3;
                                        PlayData.replayNew->commands.push_back(cmd);
                                    }
                                }

                                //LOG_DEBUG << "LATE   POOR    "; break;
                            }
                        }
                    }
                    else
                    {
                        auto itHead = itNote;
                        itHead--;
                        if (rt >= itNote->time)
                        {
                            if (!scratch || _judgeScratch)
                            {
                                if (!itHead->hit)
                                {
                                    itNote->expired = true;
                                    notesExpired++;
                                }
                            }
                        }
                    }
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Invs, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                const Time& hitTime = -judgeTime[(size_t)_judgeDifficulty].BAD;
                auto itNote = _chart->incomingNote(NoteLaneCategory::Invs, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Invs, idx, itNote) && !itNote->expired && rt - itNote->time >= hitTime)
                {
                    itNote->expired = true;
                    itNote++;
                }
            }

            idx = _chart->getLaneFromKey(NoteLaneCategory::Mine, (Input::Pad)k);
            if (idx != NoteLaneIndex::_)
            {
                auto itNote = _chart->incomingNote(NoteLaneCategory::Mine, idx);
                while (!_chart->isLastNote(NoteLaneCategory::Mine, idx, itNote) && !itNote->expired && rt >= itNote->time)
                {
                    itNote->expired = true;
                    itNote++;
                }
            }
        }
    };
    if (_k1P) updateRange(Input::S1L, Input::K19, PLAYER_SLOT_PLAYER);
    if (_k2P) updateRange(Input::S2L, Input::K29, PLAYER_SLOT_TARGET);

    if (_judgeScratch)
    {
        auto updateScratch = [&](const Time& t, Input::Pad up, Input::Pad dn, double& val, int slot)
        {
            double scratchThreshold = 0.001;
            double scratchRewind = 0.0001;
            if (val > scratchThreshold)
            {
                // scratch down
                val -= scratchThreshold;

                switch (playerScratchDirection[slot])
                {
                case AxisDir::AXIS_DOWN:
                    judgeNoteHold(dn, t, rt, slot);
                    break;
                case AxisDir::AXIS_UP:
                    judgeNoteRelease(up, t, rt, slot);
                    judgeNotePress(dn, t, rt, slot);
                    break;
                case AxisDir::AXIS_NONE:
                    judgeNoteRelease(up, t, rt, slot);
                    judgeNotePress(dn, t, rt, slot);
                    break;
                }

                playerScratchLastUpdate[slot] = t;
                playerScratchDirection[slot] = AxisDir::AXIS_DOWN;
            }
            else if (val < -scratchThreshold)
            {
                // scratch up
                val += scratchThreshold;

                switch (playerScratchDirection[slot])
                {
                case AxisDir::AXIS_UP:
                    judgeNoteHold(up, t, rt, slot);
                    break;
                case AxisDir::AXIS_DOWN:
                    judgeNoteRelease(dn, t, rt, slot);
                    judgeNotePress(up, t, rt, slot);
                    break;
                case AxisDir::AXIS_NONE:
                    judgeNoteRelease(dn, t, rt, slot);
                    judgeNotePress(up, t, rt, slot);
                    break;
                }

                playerScratchLastUpdate[slot] = t;
                playerScratchDirection[slot] = AxisDir::AXIS_UP;
            }

            if (val > scratchRewind)
                val -= scratchRewind;
            else if (val < -scratchRewind)
                val += scratchRewind;
            else
                val = 0.;

            if ((t - playerScratchLastUpdate[slot]).norm() > 133)
            {
                // release
                switch (playerScratchDirection[slot])
                {
                case AxisDir::AXIS_UP:
                    judgeNoteRelease(up, t, rt, slot);
                    break;
                case AxisDir::AXIS_DOWN:
                    judgeNoteRelease(dn, t, rt, slot);
                    break;
                }

                playerScratchDirection[slot] = AxisDir::AXIS_NONE;
                playerScratchLastUpdate[slot] = TIMER_NEVER;
            }
        };
        updateScratch(t, Input::S1L, Input::S1R, playerScratchAccumulator[PLAYER_SLOT_PLAYER], PLAYER_SLOT_PLAYER);
        updateScratch(t, Input::S2L, Input::S2R, playerScratchAccumulator[PLAYER_SLOT_TARGET], PLAYER_SLOT_TARGET);
    }

    _isCleared = isCleared();

    if (isFinished()) 
        _isFailed |= _basic.health < getClearHealth();
}

LampType RulesetBMS::getClearType() const
{
    LampType lamp = LampType::FAILED;
    if (!isNoScore() && isFinished())
    {
        if (_basic.judge[JUDGE_CB] == 0)
        {
            if (_basic.acc >= 100.0)
                lamp = LampType::MAX;
            else if (_basic.judge[JUDGE_GOOD] == 0)
                lamp = LampType::PERFECT;
            else if (isCleared())
                lamp = LampType::FULLCOMBO;
        }
        else if (isFailed())
        {
            lamp = LampType::FAILED;
        }
        else
        {
            switch (_gauge)
            {
            case GaugeType::HARD:       lamp = LampType::HARD; break;
            case GaugeType::EXHARD:     lamp = LampType::EXHARD; break;
            case GaugeType::DEATH:      lamp = LampType::FULLCOMBO; break;
            case GaugeType::GROOVE:     lamp = LampType::NORMAL; break;
            case GaugeType::EASY:       lamp = LampType::EASY; break;
            case GaugeType::ASSIST:     lamp = LampType::ASSIST; break;
            case GaugeType::CLASS:      lamp = LampType::NOPLAY; break;
            case GaugeType::EXGRADE:    lamp = LampType::NOPLAY; break;
            default:
                break;
            }
        }
    }
    return lamp;
}

double RulesetBMS::getScore() const
{
    return moneyScore;
}

double RulesetBMS::getMaxMoneyScore() const
{
    return maxMoneyScore;
}

unsigned RulesetBMS::getExScore() const
{
    return exScore;
}

unsigned RulesetBMS::getJudgeCount(JudgeType idx) const
{
    switch (idx)
    {
        case JudgeType::PERFECT: return _basic.judge[JUDGE_PERFECT];
        case JudgeType::GREAT: return _basic.judge[JUDGE_GREAT];
        case JudgeType::GOOD: return _basic.judge[JUDGE_GOOD];
        case JudgeType::BAD: return _basic.judge[JUDGE_BAD];
        case JudgeType::KPOOR: return _basic.judge[JUDGE_KPOOR];
        case JudgeType::MISS: return _basic.judge[JUDGE_MISS];
    }
    return 0;
}

unsigned RulesetBMS::getJudgeCountEx(JudgeIndex idx) const
{
    return _basic.judge[idx];
}

std::string RulesetBMS::getModifierText() const
{
    return modifierText;
}
std::string RulesetBMS::getModifierTextShort() const
{
    return modifierTextShort;
}

RulesetBMS::JudgeRes RulesetBMS::getLastJudge(int side) const
{
    return _lastNoteJudge[side];
}


unsigned RulesetBMS::getNoteCount() const
{
    return noteCount;
}

unsigned RulesetBMS::getMaxCombo() const
{
    if (_judgeScratch)
    {
        return getNoteCount();
    }
    else
    {
        unsigned count = getNoteCount();
        auto pChart = std::dynamic_pointer_cast<ChartObjectBMS>(_chart);
        if (pChart != nullptr)
        {
            count -= pChart->getScratchCount();
        }
        return count;
    }
}


void RulesetBMS::fail()
{
    _isFailed = true;

    _basic.health = _minHealth; 
    _basic.combo = 0;

    int notesRemain = getNoteCount() - notesExpired;
    _basic.judge[JUDGE_BP] += notesRemain;
    _basic.judge[JUDGE_CB] += notesRemain;
    notesExpired = notesReached = getNoteCount();

    //_basic.acc = _basic.total_acc;
}

}
