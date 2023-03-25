#include <cassert>
#include <future>
#include <set>
#include <random>
#include "scene_play.h"
#include "scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_auto.h"
#include "game/ruleset/ruleset_bms_replay.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/chart/chart_bms.h"
#include "game/graphics/sprite_video.h"
#include "config/config_mgr.h"
#include "common/log.h"
#include "common/sysutil.h"
#include "game/sound/sound_sample.h"
#include "game/skin/skin_lr2_button_callbacks.h"
#include "game/skin/skin_lr2_slider_callbacks.h"
#include "game/arena/arena_data.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

bool ScenePlay::isPlaymodeDP() const
{
    switch (gPlayContext.mode)
    {
        case SkinType::PLAY10:
        case SkinType::PLAY14:
            return true;
    }
    return false;
}


bool ScenePlay::isHoldingStart(int player) const
{
    if (isPlaymodeDP())
    {
        return holdingStart[0] || holdingStart[1];
    }
    return holdingStart[player];
}

bool ScenePlay::isHoldingSelect(int player) const
{
    if (isPlaymodeDP())
    {
        return holdingSelect[0] || holdingSelect[1];
    }
    return holdingSelect[player];
}


int getLanecoverTop(int slot)
{
    IndexNumber lcTopInd, lcBottomInd;
    IndexOption lcTypeInd;
    if (slot == PLAYER_SLOT_PLAYER)
    {
        lcTopInd = IndexNumber::LANECOVER_TOP_1P;
        lcTypeInd = IndexOption::PLAY_LANE_EFFECT_TYPE_1P;
    }
    else
    {
        lcTopInd = IndexNumber::LANECOVER_TOP_2P;
        lcTypeInd = IndexOption::PLAY_LANE_EFFECT_TYPE_2P;
    }
    switch ((Option::e_lane_effect_type)State::get(lcTypeInd))
    {
    case Option::LANE_SUDDEN:
    case Option::LANE_SUDHID:
    case Option::LANE_LIFTSUD: return State::get(lcTopInd);
    }
    return 0;
}
int getLanecoverBottom(int slot)
{
    IndexNumber lcTopInd, lcBottomInd;
    IndexOption lcTypeInd;
    if (slot == PLAYER_SLOT_PLAYER)
    {
        lcTopInd = IndexNumber::LANECOVER_TOP_1P;
        lcBottomInd = IndexNumber::LANECOVER_BOTTOM_1P;
        lcTypeInd = IndexOption::PLAY_LANE_EFFECT_TYPE_1P;
    }
    else
    {
        lcTopInd = IndexNumber::LANECOVER_TOP_1P;
        lcBottomInd = IndexNumber::LANECOVER_BOTTOM_2P;
        lcTypeInd = IndexOption::PLAY_LANE_EFFECT_TYPE_2P;
    }
    switch ((Option::e_lane_effect_type)State::get(lcTypeInd))
    {
    case Option::LANE_SUDHID:  return State::get(lcTopInd);
    case Option::LANE_HIDDEN:
    case Option::LANE_LIFT:
    case Option::LANE_LIFTSUD: return State::get(lcBottomInd);
    }
    return 0;
}

double getLaneVisible(int slot)
{
    return std::max(0, (1000 - getLanecoverTop(slot) - getLanecoverBottom(slot))) / 1000.0;
}

// HiSpeed, InternalSpeedValue
std::pair<double, double> calcHiSpeed(double bpm, int slot, int green)
{
    double visible = getLaneVisible(slot);
    double hs = (green * bpm <= 0.0) ? 200 : std::min(visible * 120.0 * 1200 / green / bpm, 10.0);
    double speedValue = (visible == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / visible);
    return { hs, speedValue };
}

double getHiSpeed(double bpm, int slot, double speedValue)
{
    double visible = getLaneVisible(slot);
    return speedValue / bpm * visible;
}

// HiSpeed, InternalSpeedValue
std::pair<int, double> calcGreenNumber(double bpm, int slot, double hs)
{
    double visible = getLaneVisible(slot);
    double speedValue = (visible == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / visible);

    double den = hs * bpm;
    int green = den != 0.0 ? int(std::round(visible * 120.0 * 1200 / hs / bpm)) : 0;

    return { green, speedValue };
}


//////////////////////////////////////////////////////////////////////////////////////////////

ScenePlay::ScenePlay(): SceneBase(gPlayContext.mode, 1000, true)
{
    _type = SceneType::PLAY;
    state = ePlayState::PREPARE;

    assert(!isPlaymodeDP() || !gPlayContext.isBattle);

    // 2P inputs => 1P
    if (!isPlaymodeDP() && !gPlayContext.isBattle)
    {
        _input.setMergeInput();
    }
    _inputAvailable = INPUT_MASK_FUNC;
    _inputAvailable |= INPUT_MASK_1P | INPUT_MASK_2P;

    // which lanes should we use for 5K, 1-5 or 3-7? 
    gPlayContext.shift1PNotes5KFor7KSkin = false;
    gPlayContext.shift2PNotes5KFor7KSkin = false;
    if ((pSkin->info.mode == SkinType::PLAY7 || pSkin->info.mode == SkinType::PLAY7_2 || pSkin->info.mode == SkinType::PLAY14) &&
        (State::get(IndexOption::CHART_PLAY_KEYS) == Option::KEYS_5 || State::get(IndexOption::CHART_PLAY_KEYS) == Option::KEYS_10))
    {
        gPlayContext.shift1PNotes5KFor7KSkin = (pSkin->info.scratchSide1P == 1);
        gPlayContext.shift2PNotes5KFor7KSkin = (pSkin->info.scratchSide2P == 1);

        if (gPlayContext.shift1PNotes5KFor7KSkin)
        {
            replayCmdMapIndex = gPlayContext.shift2PNotes5KFor7KSkin ? 3 : 2;
        }
        else
        {
            replayCmdMapIndex = gPlayContext.shift2PNotes5KFor7KSkin ? 1 : 0;
        }
    }

    // ? 
    keySampleIndex.assign(Input::ESC, 0);

    // replay pitch
    if (gPlayContext.isReplay && gPlayContext.replay)
    {
        State::set(IndexSwitch::SOUND_PITCH, true);
        State::set(IndexOption::SOUND_PITCH_TYPE, gPlayContext.replay->pitchType);
        double ps = (gPlayContext.replay->pitchValue + 12) / 24.0;
        lr2skin::slider::pitch(ps);
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    // chart initialize, get basic info
    if (gChartContext.chart == nullptr || !gChartContext.chart->isLoaded())
    {
        if (gChartContext.path.empty())
        {
            LOG_ERROR << "[Play] Chart not specified!";
            gNextScene = gQuitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
            return;
        }
        if (gArenaData.isOnline())
        {
            gPlayContext.randomSeed = gArenaData.getRandomSeed();
        }
        else if (gPlayContext.replay && 
            gPlayContext.isReplay || (gPlayContext.replay && State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_GHOST))
        {
            gPlayContext.randomSeed = gPlayContext.replay->randomSeed;
        }
        gChartContext.chart = ChartFormatBase::createFromFile(gChartContext.path, gPlayContext.randomSeed);
    }
    if (gChartContext.chart == nullptr || !gChartContext.chart->isLoaded())
    {
        LOG_ERROR << "[Play] Invalid chart: " << gChartContext.path.u8string();
        gNextScene = gQuitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
        return;
    }

    LOG_DEBUG << "[Play] " << gChartContext.chart->title << " " << gChartContext.chart->title2 << " [" << gChartContext.chart->version << "]";
    LOG_DEBUG << "[Play] MD5: " << gChartContext.chart->fileHash.hexdigest();
    LOG_DEBUG << "[Play] Mode: " << gChartContext.chart->gamemode;
    LOG_DEBUG << "[Play] BPM: " << gChartContext.chart->startBPM
        << " (" << gChartContext.chart->minBPM
        << " - " << gChartContext.chart->maxBPM << ")";

    if (gPlayContext.replayMybest)
    {
        gChartContext.chartMybest = ChartFormatBase::createFromFile(gChartContext.path, gPlayContext.replayMybest->randomSeed);
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    clearGlobalDatas();
    // global info
    // 
    // basic info
    State::set(IndexText::PLAY_TITLE, gChartContext.title);
    State::set(IndexText::PLAY_SUBTITLE, gChartContext.title2);
    if (gChartContext.title2.empty())
        State::set(IndexText::PLAY_FULLTITLE, gChartContext.title);
    else
        State::set(IndexText::PLAY_FULLTITLE, gChartContext.title + " " + gChartContext.title2);
    State::set(IndexText::PLAY_ARTIST, gChartContext.artist);
    State::set(IndexText::PLAY_SUBARTIST, gChartContext.artist2);
    State::set(IndexText::PLAY_GENRE, gChartContext.genre);
    State::set(IndexNumber::PLAY_BPM, int(std::round(gChartContext.startBPM * gSelectContext.pitchSpeed)));
    State::set(IndexNumber::INFO_BPM_MIN, int(std::round(gChartContext.minBPM * gSelectContext.pitchSpeed)));
    State::set(IndexNumber::INFO_BPM_MAX, int(std::round(gChartContext.maxBPM * gSelectContext.pitchSpeed)));
    State::set(IndexNumber::INFO_RIVAL_BPM_MIN, int(std::round(gChartContext.minBPM * gSelectContext.pitchSpeed)));
    State::set(IndexNumber::INFO_RIVAL_BPM_MAX, int(std::round(gChartContext.maxBPM * gSelectContext.pitchSpeed)));

    State::set(IndexOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_NONE);
    State::set(IndexOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_NONE);
    State::set(IndexOption::PLAY_RANK_BORDER_1P, Option::RANK_NONE);
    State::set(IndexOption::PLAY_RANK_BORDER_2P, Option::RANK_NONE);

    lr2skin::button::target_type(0);

    gChartContext.title = gChartContext.chart->title;
    gChartContext.title2 = gChartContext.chart->title2;
    gChartContext.artist = gChartContext.chart->artist;
    gChartContext.artist2 = gChartContext.chart->artist2;
    gChartContext.genre = gChartContext.chart->genre;
    gChartContext.minBPM = gChartContext.chart->minBPM;
    gChartContext.startBPM = gChartContext.chart->startBPM;
    gChartContext.maxBPM = gChartContext.chart->maxBPM;

    // chartobj
    chartObjLoaded = createChartObj();
    gPlayContext.remainTime = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength();

    LOG_DEBUG << "[Play] Real BPM: " << gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM()
        << " (" << gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getPlayMinBPM()
        << " - " << gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getPlayMaxBPM()
        << ") / Avg: " << gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getAverageBPM()
        << " / Main: " << gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getMainBPM();
    LOG_DEBUG << "[Play] Notes: " << gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getNoteTotalCount();
    LOG_DEBUG << "[Play] Length: " << gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().norm() / 1000;

    //////////////////////////////////////////////////////////////////////////////////////////

    // set gauge type, health value
    if (gChartContext.chart)
    {
        switch (gChartContext.chart->type())
        {
        case eChartFormat::BMS:
        case eChartFormat::BMSON:
            setInitialHealthBMS();
            break;
        default:
            break;
        }
    }
    playerState[PLAYER_SLOT_PLAYER].healthLastTick = State::get(IndexNumber::PLAY_1P_GROOVEGAUGE);
    playerState[PLAYER_SLOT_TARGET].healthLastTick = State::get(IndexNumber::PLAY_2P_GROOVEGAUGE);

    auto initDisplayGaugeType = [&](int slot)
    {
        using namespace std::string_literals;
        GaugeDisplayType tmp = GaugeDisplayType::GROOVE;
        switch (gPlayContext.mods[slot].gauge)
        {
        case PlayModifierGaugeType::NORMAL:       tmp = GaugeDisplayType::GROOVE; break;
        case PlayModifierGaugeType::HARD:         tmp = GaugeDisplayType::SURVIVAL; break;
        case PlayModifierGaugeType::DEATH:        tmp = GaugeDisplayType::EX_SURVIVAL; break;
        case PlayModifierGaugeType::EASY:         tmp = GaugeDisplayType::GROOVE; break;
            // case PlayModifierGaugeType::PATTACK:      tmp = GaugeDisplayType::EX_SURVIVAL; break;
            // case PlayModifierGaugeType::GATTACK:      tmp = GaugeDisplayType::EX_SURVIVAL; break;
        case PlayModifierGaugeType::ASSISTEASY:   tmp = GaugeDisplayType::ASSIST_EASY; break;
        case PlayModifierGaugeType::EXHARD:       tmp = GaugeDisplayType::EX_SURVIVAL; break;
        case PlayModifierGaugeType::GRADE_NORMAL: tmp = GaugeDisplayType::SURVIVAL; break;
        case PlayModifierGaugeType::GRADE_HARD:   tmp = GaugeDisplayType::EX_SURVIVAL; break;
        case PlayModifierGaugeType::GRADE_DEATH:  tmp = GaugeDisplayType::EX_SURVIVAL; break;
        default: break;
        }
        if (slot == PLAYER_SLOT_PLAYER)
        {
            pSkin->setExtendedProperty("GAUGETYPE_1P"s, (void*)&tmp);
        }
        else
        {
            pSkin->setExtendedProperty("GAUGETYPE_2P"s, (void*)&tmp);
        }
    };
    initDisplayGaugeType(PLAYER_SLOT_PLAYER);
    if (gPlayContext.isBattle) initDisplayGaugeType(PLAYER_SLOT_TARGET);

    //////////////////////////////////////////////////////////////////////////////////////////

    // ruleset, should be called after initial health set
    rulesetLoaded = createRuleset();

    if (rulesetLoaded && gArenaData.isOnline())
    {
        if (gArenaData.isClient())
            g_pArenaClient->setCreatedRuleset();
        else
            g_pArenaHost->setCreatedRuleset();
    }

    // course: skip play scene if already failed
    if (gPlayContext.isCourse && gPlayContext.initialHealth[PLAYER_SLOT_PLAYER] <= 0.)
    {
        if (gPlayContext.courseStage < gPlayContext.courseCharts.size())
        {
            gPlayContext.courseStageReplayPathNew.push_back("");

            if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER])
            {
                gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->fail();
                gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->updateGlobals();
                gPlayContext.courseStageRulesetCopy[PLAYER_SLOT_PLAYER].push_back(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]);
            }
            if (gPlayContext.ruleset[PLAYER_SLOT_TARGET])
            {
                gPlayContext.ruleset[PLAYER_SLOT_TARGET]->fail();
                gPlayContext.ruleset[PLAYER_SLOT_TARGET]->updateGlobals();
                gPlayContext.courseStageRulesetCopy[PLAYER_SLOT_TARGET].push_back(gPlayContext.ruleset[PLAYER_SLOT_TARGET]);
            }

            // do not draw anything
            pSkin.reset();

            ++gPlayContext.courseStage;
            gNextScene = SceneType::COURSE_TRANS;
            return;
        }
        else
        {
            gNextScene = SceneType::COURSE_RESULT;
            return;
        }
    }

    if (gPlayContext.isCourse)
    {
        LOG_DEBUG << "[Play] Course stage " << gPlayContext.courseStage;
        LOG_DEBUG << "[Play] Running combo: " << gPlayContext.courseRunningCombo[PLAYER_SLOT_PLAYER] << " / " << gPlayContext.courseRunningCombo[PLAYER_SLOT_TARGET];
        LOG_DEBUG << "[Play] Health: " << gPlayContext.initialHealth[PLAYER_SLOT_PLAYER] << " / " << gPlayContext.initialHealth[PLAYER_SLOT_TARGET];
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    // Lanecover
    auto initLanecover = [&](int slot)
    {
        IndexOption indLanecoverType = IndexOption::PLAY_LANE_EFFECT_TYPE_1P;
        IndexSwitch indLanecoverEnabled = IndexSwitch::P1_LANECOVER_ENABLED;
        IndexNumber indNumLanecover = IndexNumber::LANECOVER100_1P;
        IndexNumber indNumSudden = IndexNumber::LANECOVER_TOP_1P;
        IndexSlider indSliSudden = IndexSlider::SUD_1P;
        IndexNumber indNumHidden = IndexNumber::LANECOVER_BOTTOM_1P;
        IndexSlider indSliHidden = IndexSlider::HID_1P;
        IndexSwitch indSwHasLcTop = IndexSwitch::P1_HAS_LANECOVER_TOP;
        IndexSwitch indSwHasLcBottom = IndexSwitch::P1_HAS_LANECOVER_BOTTOM;
        const char* cfgLanecoverTop = cfg::P_LANECOVER_TOP;
        const char* cfgLanecoverBottom = cfg::P_LANECOVER_BOTTOM;
        if (slot != PLAYER_SLOT_PLAYER)
        {
            indLanecoverType = IndexOption::PLAY_LANE_EFFECT_TYPE_2P;
            indLanecoverEnabled = IndexSwitch::P2_LANECOVER_ENABLED;
            indNumLanecover = IndexNumber::LANECOVER100_2P;
            indNumSudden = IndexNumber::LANECOVER_TOP_2P;
            indSliSudden = IndexSlider::SUD_2P;
            indNumHidden = IndexNumber::LANECOVER_BOTTOM_2P;
            indSliHidden = IndexSlider::HID_2P;
            indSwHasLcTop = IndexSwitch::P2_HAS_LANECOVER_TOP;
            indSwHasLcBottom = IndexSwitch::P2_HAS_LANECOVER_BOTTOM;
            cfgLanecoverTop = cfg::P_LANECOVER_TOP_2P;
            cfgLanecoverBottom = cfg::P_LANECOVER_BOTTOM_2P;
        }

        int lcTop = 0;
        int lcBottom = 0;
        int lc100 = 0;
        double sudden = 0.;
        double hidden = 0.;
        if (slot == PLAYER_SLOT_PLAYER && gPlayContext.isReplay && gPlayContext.replay)
        {
            switch ((PlayModifierLaneEffectType)gPlayContext.replay->laneEffectType)
            {
            case PlayModifierLaneEffectType::HIDDEN:  playerState[PLAYER_SLOT_PLAYER].origLanecoverType = Option::LANE_HIDDEN; break;
            case PlayModifierLaneEffectType::SUDDEN:  playerState[PLAYER_SLOT_PLAYER].origLanecoverType = Option::LANE_SUDDEN; break;
            case PlayModifierLaneEffectType::SUDHID:  playerState[PLAYER_SLOT_PLAYER].origLanecoverType = Option::LANE_SUDHID; break;
            case PlayModifierLaneEffectType::LIFT:    playerState[PLAYER_SLOT_PLAYER].origLanecoverType = Option::LANE_LIFT; break;
            case PlayModifierLaneEffectType::LIFTSUD: playerState[PLAYER_SLOT_PLAYER].origLanecoverType = Option::LANE_LIFTSUD; break;
            default:                                  playerState[PLAYER_SLOT_PLAYER].origLanecoverType = Option::LANE_OFF; break;
            }
            lcTop = gPlayContext.replay->lanecoverTop;
            lcBottom = gPlayContext.replay->lanecoverBottom;
            lc100 = lcTop / 10;
            sudden = lcTop / 1000.0;
            hidden = lcBottom / 1000.0;
        }
        else if (slot == PLAYER_SLOT_TARGET && isPlaymodeDP())
        {
            playerState[PLAYER_SLOT_TARGET].origLanecoverType = playerState[PLAYER_SLOT_PLAYER].origLanecoverType;
            lcTop = State::get(IndexNumber::LANECOVER_TOP_1P);
            lcBottom = State::get(IndexNumber::LANECOVER_BOTTOM_1P);
            lc100 = lcTop / 10;
            sudden = lcTop / 1000.0;
            hidden = lcBottom / 1000.0;
        }
        else
        {
            playerState[slot].origLanecoverType = (Option::e_lane_effect_type)State::get(indLanecoverType);

            sudden = ConfigMgr::get('P', cfgLanecoverTop, 0) / 1000.0;
            hidden = ConfigMgr::get('P', cfgLanecoverBottom, 0) / 1000.0;
            switch (playerState[slot].origLanecoverType)
            {
            case Option::LANE_OFF:
                sudden = 0.;
                hidden = 0.;
                break;

            case Option::LANE_SUDDEN:
                hidden = 0.;
                break;

            case Option::LANE_HIDDEN:
                sudden = 0.;
                break;

            case Option::LANE_SUDHID:
                hidden = sudden;
                break;

            case Option::LANE_LIFT:
                sudden = 0.;
                break;
            }
            lcTop = int(sudden * 1000);
            lcBottom = int(hidden * 1000);
            lc100 = lcTop / 10;
        }

        State::set(indLanecoverEnabled,
            (playerState[slot].origLanecoverType != Option::LANE_OFF && playerState[PLAYER_SLOT_PLAYER].origLanecoverType != Option::LANE_LIFT));
        State::set(indNumSudden, lcTop);
        State::set(indNumHidden, lcBottom);
        State::set(indNumLanecover, lc100);
        State::set(indSliSudden, sudden);
        State::set(indSliHidden, hidden);

        State::set(indSwHasLcTop, playerState[slot].origLanecoverType != Option::LANE_HIDDEN);
        switch (playerState[slot].origLanecoverType)
        {
        case Option::LANE_HIDDEN:
        case Option::LANE_SUDHID:
        case Option::LANE_LIFT:
        case Option::LANE_LIFTSUD:
            State::set(indSwHasLcBottom, true);
            break;
        default:
            State::set(indSwHasLcBottom, false);
            break;
        }
    };
    initLanecover(PLAYER_SLOT_PLAYER);
    initLanecover(PLAYER_SLOT_TARGET);

    //////////////////////////////////////////////////////////////////////////////////////////

    // Hispeed
    playerState[PLAYER_SLOT_PLAYER].savedHispeed = gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed;
    playerState[PLAYER_SLOT_TARGET].savedHispeed = gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed;

    // replay hispeed
    if (gPlayContext.isReplay && gPlayContext.replay)
    {
        gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed = gPlayContext.replay->hispeed;
    }

    auto initLockHispeed = [&](int slot)
    {
        auto cfg = cfg::P_GREENNUMBER;
        auto indNum = IndexNumber::HS_1P;
        auto indSli = IndexSlider::HISPEED_1P;
        double* pHispeed = &gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed;
        if (slot == PLAYER_SLOT_TARGET)
        {
            cfg = cfg::P_GREENNUMBER_2P;
            indNum = IndexNumber::HS_2P;
            indSli = IndexSlider::HISPEED_2P;
            pHispeed = &gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed;
        }

        double bpm = gChartContext.startBPM * gSelectContext.pitchSpeed;
        switch (gPlayContext.mods[slot].hispeedFix)
        {
        case PlayModifierHispeedFixType::MAXBPM:   bpm = gPlayContext.chartObj[slot]->getPlayMaxBPM(); break;
        case PlayModifierHispeedFixType::MINBPM:   bpm = gPlayContext.chartObj[slot]->getPlayMinBPM(); break;
        case PlayModifierHispeedFixType::AVERAGE:  bpm = gPlayContext.chartObj[slot]->getAverageBPM(); break;
        case PlayModifierHispeedFixType::CONSTANT: bpm = 150.0; break;
        case PlayModifierHispeedFixType::MAIN:     bpm = gPlayContext.chartObj[slot]->getMainBPM(); break;
        case PlayModifierHispeedFixType::INITIAL:
        case PlayModifierHispeedFixType::NONE:
        default:               bpm = gPlayContext.chartObj[slot]->getCurrentBPM(); break;
        }

        int green = ConfigMgr::get('P', cfg, 1200);
        const auto [hs, val] = calcHiSpeed(bpm, slot, green);

        *pHispeed = hs;
        State::set(indNum, (int)std::round(*pHispeed * 100));
        State::set(indSli, *pHispeed / 10.0);
        playerState[slot].lockspeedValueInternal = val;
        playerState[slot].lockspeedGreenNumber = green;
        playerState[slot].lockspeedHispeedBuffered = hs;
    };
    if (State::get(IndexSwitch::P1_LOCK_SPEED))
    {
        initLockHispeed(PLAYER_SLOT_PLAYER);
    }
    if (gPlayContext.isBattle && State::get(IndexSwitch::P2_LOCK_SPEED))
    {
        initLockHispeed(PLAYER_SLOT_TARGET);
    }
    gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeedGradientStart = TIMER_NEVER;
    gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeedGradientFrom = gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeedGradientNow;
    gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeedGradientNow = gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed;
    gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeedGradientStart = TIMER_NEVER;
    gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeedGradientFrom = gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeedGradientNow;
    gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeedGradientNow = gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed;

    State::set(IndexSlider::HISPEED_1P, gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed / 10.0);
    State::set(IndexSlider::HISPEED_2P, gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed / 10.0);

    //////////////////////////////////////////////////////////////////////////////////////////
    
    adjustHispeedWithUpDown = ConfigMgr::get('P', cfg::P_ADJUST_HISPEED_WITH_ARROWKEYS, false);
    adjustHispeedWithSelect = ConfigMgr::get('P', cfg::P_ADJUST_HISPEED_WITH_SELECT, false);
    adjustLanecoverWithStart67 = ConfigMgr::get('P', cfg::P_ADJUST_LANECOVER_WITH_START_67, false);
    adjustLanecoverWithMousewheel = ConfigMgr::get('P', cfg::P_ADJUST_LANECOVER_WITH_MOUSEWHEEL, false);
    adjustLanecoverWithLeftRight = ConfigMgr::get('P', cfg::P_ADJUST_LANECOVER_WITH_ARROWKEYS, false);

    if (adjustLanecoverWithMousewheel)
        _inputAvailable |= INPUT_MASK_MOUSE;

    poorBgaDuration = ConfigMgr::get("P", cfg::P_MISSBGA_LENGTH, 500);

    //////////////////////////////////////////////////////////////////////////////////////////

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&ScenePlay::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&ScenePlay::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&ScenePlay::inputGameRelease, this, _1, _2));
    _input.register_a("SCENE_AXIS", std::bind(&ScenePlay::inputGameAxis, this, _1, _2, _3));

    //////////////////////////////////////////////////////////////////////////////////////////

    imguiInit();

    //////////////////////////////////////////////////////////////////////////////////////////

    State::set(IndexTimer::PLAY_READY, TIMER_NEVER);
    State::set(IndexTimer::PLAY_START, TIMER_NEVER);
    State::set(IndexTimer::MUSIC_BEAT, TIMER_NEVER);
    SoundMgr::setSysVolume(1.0);
    SoundMgr::setNoteVolume(1.0);
}

void ScenePlay::clearGlobalDatas()
{
    // reset
    IndexNumber numbersReset[] =
    {
        IndexNumber::PLAY_1P_SCORE,
        IndexNumber::PLAY_1P_EXSCORE,
        IndexNumber::PLAY_1P_RATE,
        IndexNumber::PLAY_1P_RATEDECIMAL,
        IndexNumber::PLAY_1P_NOWCOMBO,
        IndexNumber::PLAY_1P_MAXCOMBO,
        IndexNumber::PLAY_1P_EXSCORE_DIFF,
        IndexNumber::PLAY_1P_NEXT_RANK_EX_DIFF,
        IndexNumber::PLAY_1P_PERFECT,
        IndexNumber::PLAY_1P_GREAT,
        IndexNumber::PLAY_1P_GOOD,
        IndexNumber::PLAY_1P_BAD,
        IndexNumber::PLAY_1P_POOR,
        IndexNumber::PLAY_1P_TOTAL_RATE,
        IndexNumber::PLAY_1P_TOTAL_RATE_DECIMAL2,
        IndexNumber::PLAY_2P_SCORE,
        IndexNumber::PLAY_2P_EXSCORE,
        IndexNumber::PLAY_2P_RATE,
        IndexNumber::PLAY_2P_RATEDECIMAL,
        IndexNumber::PLAY_2P_NOWCOMBO,
        IndexNumber::PLAY_2P_MAXCOMBO,
        IndexNumber::PLAY_2P_EXSCORE_DIFF,
        IndexNumber::PLAY_2P_NEXT_RANK_EX_DIFF,
        IndexNumber::PLAY_2P_PERFECT,
        IndexNumber::PLAY_2P_GREAT,
        IndexNumber::PLAY_2P_GOOD,
        IndexNumber::PLAY_2P_BAD,
        IndexNumber::PLAY_2P_POOR,
        IndexNumber::PLAY_2P_TOTAL_RATE,
        IndexNumber::PLAY_2P_TOTAL_RATE_DECIMAL2,
        IndexNumber::RESULT_MYBEST_EX,
        IndexNumber::RESULT_TARGET_EX,
        IndexNumber::RESULT_MYBEST_DIFF,
        IndexNumber::RESULT_TARGET_DIFF,
        IndexNumber::RESULT_NEXT_RANK_EX_DIFF,
        IndexNumber::RESULT_MYBEST_RATE,
        IndexNumber::RESULT_MYBEST_RATE_DECIMAL2,
        IndexNumber::RESULT_TARGET_RATE,
        IndexNumber::RESULT_TARGET_RATE_DECIMAL2,
        IndexNumber::PLAY_MIN,
        IndexNumber::PLAY_SEC,
        IndexNumber::PLAY_LOAD_PROGRESS_PERCENT,
        IndexNumber::PLAY_LOAD_PROGRESS_SYS,
        IndexNumber::PLAY_LOAD_PROGRESS_WAV,
        IndexNumber::PLAY_LOAD_PROGRESS_BGA,
        IndexNumber::RESULT_RECORD_EX_BEFORE,
        IndexNumber::RESULT_RECORD_EX_NOW,
        IndexNumber::RESULT_RECORD_EX_DIFF,
        IndexNumber::RESULT_RECORD_MAXCOMBO_BEFORE,
        IndexNumber::RESULT_RECORD_MAXCOMBO_NOW,
        IndexNumber::RESULT_RECORD_MAXCOMBO_DIFF,
        IndexNumber::RESULT_RECORD_BP_BEFORE,
        IndexNumber::RESULT_RECORD_BP_NOW,
        IndexNumber::RESULT_RECORD_BP_DIFF,
        IndexNumber::RESULT_RECORD_IR_RANK,
        IndexNumber::RESULT_RECORD_IR_TOTALPLAYER,
        IndexNumber::RESULT_RECORD_IR_CLEARRATE,
        IndexNumber::RESULT_RECORD_IR_RANK_BEFORE,
        IndexNumber::RESULT_RECORD_MYBEST_RATE,
        IndexNumber::RESULT_RECORD_MYBEST_RATE_DECIMAL2,
        IndexNumber::LR2IR_REPLACE_PLAY_1P_JUDGE_TIME_ERROR_MS,
        IndexNumber::LR2IR_REPLACE_PLAY_1P_FAST_SLOW,
        IndexNumber::LR2IR_REPLACE_PLAY_2P_FAST_SLOW,
        IndexNumber::LR2IR_REPLACE_PLAY_1P_FAST_COUNT,
        IndexNumber::LR2IR_REPLACE_PLAY_2P_JUDGE_TIME_ERROR_MS,
        IndexNumber::LR2IR_REPLACE_PLAY_1P_SLOW_COUNT,
        IndexNumber::LR2IR_REPLACE_PLAY_1P_COMBOBREAK,
        IndexNumber::LR2IR_REPLACE_PLAY_REMAIN_NOTES,
        IndexNumber::LR2IR_REPLACE_PLAY_RUNNING_NOTES,
        IndexNumber::PLAY_1P_MISS,
        IndexNumber::PLAY_1P_FAST_COUNT,
        IndexNumber::PLAY_1P_SLOW_COUNT,
        IndexNumber::PLAY_1P_BPOOR,
        IndexNumber::PLAY_1P_COMBOBREAK,
        IndexNumber::PLAY_1P_BP,
        IndexNumber::PLAY_1P_JUDGE_TIME_ERROR_MS,
        IndexNumber::PLAY_2P_MISS,
        IndexNumber::PLAY_2P_FAST_COUNT,
        IndexNumber::PLAY_2P_SLOW_COUNT,
        IndexNumber::PLAY_2P_BPOOR,
        IndexNumber::PLAY_2P_COMBOBREAK,
        IndexNumber::PLAY_2P_BP,
        IndexNumber::PLAY_2P_JUDGE_TIME_ERROR_MS,
        IndexNumber::_ANGLE_TT_1P,
        IndexNumber::_ANGLE_TT_2P
    };
    for (auto& e : numbersReset)
    {
        State::set(e, 0);
    }
    for (int i = (int)IndexNumber::ARENA_PLAYDATA_BASE; i <= (int)IndexNumber::ARENA_PLAYDATA_ALL_MAX; i++)
    {
        State::set((IndexNumber)i, 0);
    }

    IndexBargraph bargraphsReset[] =
    {
        IndexBargraph::PLAY_EXSCORE,
        IndexBargraph::PLAY_EXSCORE_PREDICT,
        IndexBargraph::PLAY_MYBEST_NOW,
        IndexBargraph::PLAY_MYBEST_FINAL,
        IndexBargraph::PLAY_RIVAL_EXSCORE,
        IndexBargraph::PLAY_RIVAL_EXSCORE_FINAL,
        IndexBargraph::PLAY_EXSCORE_BACKUP,
        IndexBargraph::PLAY_RIVAL_EXSCORE_BACKUP,
        IndexBargraph::RESULT_PG,
        IndexBargraph::RESULT_GR,
        IndexBargraph::RESULT_GD,
        IndexBargraph::RESULT_BD,
        IndexBargraph::RESULT_PR,
        IndexBargraph::RESULT_MAXCOMBO,
        IndexBargraph::RESULT_SCORE,
        IndexBargraph::RESULT_EXSCORE,
        IndexBargraph::RESULT_RIVAL_PG,
        IndexBargraph::RESULT_RIVAL_GR,
        IndexBargraph::RESULT_RIVAL_GD,
        IndexBargraph::RESULT_RIVAL_BD,
        IndexBargraph::RESULT_RIVAL_PR,
        IndexBargraph::RESULT_RIVAL_MAXCOMBO,
        IndexBargraph::RESULT_RIVAL_SCORE,
        IndexBargraph::RESULT_RIVAL_EXSCORE,
        IndexBargraph::PLAY_1P_SLOW_COUNT,
        IndexBargraph::PLAY_1P_FAST_COUNT,
        IndexBargraph::PLAY_2P_SLOW_COUNT,
        IndexBargraph::PLAY_2P_FAST_COUNT,
        IndexBargraph::MUSIC_LOAD_PROGRESS_SYS,
        IndexBargraph::MUSIC_LOAD_PROGRESS_WAV,
        IndexBargraph::MUSIC_LOAD_PROGRESS_BGA,
    };
    for (auto& e : bargraphsReset)
    {
        State::set(e, 0.0);
    }
    for (int i = (int)IndexBargraph::ARENA_PLAYDATA_BASE; i <= (int)IndexBargraph::ARENA_PLAYDATA_ALL_MAX; i++)
    {
        State::set((IndexBargraph)i, 0);
    }

    IndexSlider slidersReset[] =
    {
        IndexSlider::SONG_PROGRESS
    };
    for (auto& e : slidersReset)
    {
        State::set(e, 0.0);
    }

}

bool ScenePlay::createChartObj()
{
    //load chart object from Chart object
    switch (gChartContext.chart->type())
    {
    case eChartFormat::BMS:
    {
        auto bms = std::reinterpret_pointer_cast<ChartFormatBMS>(gChartContext.chart);

        gPlayContext.chartObj[PLAYER_SLOT_PLAYER] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);

        if (gPlayContext.isBattle)
            gPlayContext.chartObj[PLAYER_SLOT_TARGET] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_TARGET, bms);
        else
            gPlayContext.chartObj[PLAYER_SLOT_TARGET] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);    // create for rival; loading with 1P options

        if (gPlayContext.replayMybest)
            gPlayContext.chartObj[PLAYER_SLOT_MYBEST] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_MYBEST, bms);

        if (gPlayContext.isReplay && (!gPlayContext.isBattle || State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_GHOST))
            itReplayCommand = gPlayContext.replay->commands.begin();

        State::set(IndexNumber::PLAY_REMAIN_MIN, int(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().norm() / 1000 / 60));
        State::set(IndexNumber::PLAY_REMAIN_SEC, int(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().norm() / 1000 % 60));
        return true;
    }

    case eChartFormat::BMSON:
    default:
        LOG_WARNING << "[Play] chart format not supported.";
        gNextScene = gQuitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
        return false;
    }

}

bool ScenePlay::createRuleset()
{
    // build Ruleset object
    switch (gPlayContext.rulesetType)
    {
    case RulesetType::BMS:
    {
        // set judge diff
        RulesetBMS::JudgeDifficulty judgeDiff;
        switch (gChartContext.chart->type())
        {
        case eChartFormat::BMS:
            if (int rank = 0; gChartContext.chart->getExtendedProperty("RANK", &rank))
            {
                switch (rank)
                {
                case 1: judgeDiff = RulesetBMS::JudgeDifficulty::HARD; break;
                case 2: judgeDiff = RulesetBMS::JudgeDifficulty::NORMAL; break;
                case 3: judgeDiff = RulesetBMS::JudgeDifficulty::EASY; break;
                case 4: judgeDiff = RulesetBMS::JudgeDifficulty::VERYEASY; break;
                case 6: judgeDiff = RulesetBMS::JudgeDifficulty::WHAT; break;
                case 0:
                default:
                    judgeDiff = RulesetBMS::JudgeDifficulty::VERYHARD; break;
                }
            }
            break;
        case eChartFormat::BMSON:
        default:
            LOG_WARNING << "[Play] chart format not supported.";
            break;
        }

        unsigned keys = 7;
        switch (gPlayContext.mode)
        {
        case SkinType::PLAY5:
        case SkinType::PLAY5_2: keys = 5; break;
        case SkinType::PLAY7:
        case SkinType::PLAY7_2: keys = 7; break;
        case SkinType::PLAY9:
        case SkinType::PLAY9_2: keys = 9; break;
        case SkinType::PLAY10: keys = 10; break;
        case SkinType::PLAY14: keys = 14; break;
        default: break;
        }

        if (!gInCustomize)
        {
            unsigned skinKeys = 7;
            switch (pSkin->info.mode)
            {
            case SkinType::PLAY5:
            case SkinType::PLAY5_2: skinKeys = 5; break;
            case SkinType::PLAY7:
            case SkinType::PLAY7_2: skinKeys = 7; break;
            case SkinType::PLAY9:
            case SkinType::PLAY9_2: skinKeys = 9; break;
            case SkinType::PLAY10: skinKeys = 10; break;
            case SkinType::PLAY14: skinKeys = 14; break;
            default: break;
            }
            InputMgr::updateBindings(skinKeys);
        }

        auto rulesetFactoryFunc = [&](int slot) -> std::shared_ptr<RulesetBMS>
        {
            enum ObjType { EMPTY, BASE, AUTO, REPLAY };
            int objType = EMPTY;
            bool doublePlay = (keys == 10 || keys == 14);
            RulesetBMS::PlaySide playSide = RulesetBMS::PlaySide::SINGLE;

            auto setParamsPlayer = [&]()
            {
                if (gPlayContext.isAuto)
                {
                    objType = AUTO;
                    playSide = doublePlay ? RulesetBMS::PlaySide::AUTO_DOUBLE : RulesetBMS::PlaySide::AUTO;
                }
                else if (gPlayContext.isReplay)
                {
                    objType = REPLAY;
                    playSide = doublePlay ? RulesetBMS::PlaySide::AUTO_DOUBLE : RulesetBMS::PlaySide::AUTO;
                }
                else if (gPlayContext.isBattle)
                {
                    objType = BASE;
                    playSide = RulesetBMS::PlaySide::BATTLE_1P;
                }
                else
                {
                    objType = BASE;
                    playSide = doublePlay ? RulesetBMS::PlaySide::DOUBLE : RulesetBMS::PlaySide::SINGLE;
                }
            };

            auto setParamsTarget = [&]()
            {
                if (gPlayContext.isAuto)
                {
                    if (gPlayContext.isBattle)
                    {
                        objType = gPlayContext.replayMybest ? REPLAY : AUTO;
                        playSide = RulesetBMS::PlaySide::AUTO_2P;
                    }
                }
                else if (gPlayContext.isBattle)
                {
                    objType = BASE;
                    playSide = RulesetBMS::PlaySide::BATTLE_2P;
                }
                else
                {
                    objType = AUTO;
                    playSide = RulesetBMS::PlaySide::RIVAL;
                }
            };

            auto setParamsMybest = [&]()
            {
                if (!gPlayContext.isAuto && !gPlayContext.isBattle && gPlayContext.replayMybest)
                {
                    objType = REPLAY;
                    playSide = RulesetBMS::PlaySide::MYBEST;
                }
            };

            switch (slot)
            {
            case PLAYER_SLOT_PLAYER: setParamsPlayer(); break;
            case PLAYER_SLOT_TARGET: setParamsTarget(); break;
            case PLAYER_SLOT_MYBEST: setParamsMybest(); break;
            }

            switch (objType)
            {
            case BASE: 
                return std::make_shared<RulesetBMS>(
                gChartContext.chart, gPlayContext.chartObj[slot],
                gPlayContext.mods[slot].gauge, keys, judgeDiff,
                gPlayContext.initialHealth[slot], playSide);

            case AUTO:
                return std::make_shared<RulesetBMSAuto>(
                    gChartContext.chart, gPlayContext.chartObj[slot],
                    gPlayContext.mods[slot].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[slot], playSide);

            case REPLAY:
                assert(gPlayContext.replayMybest != nullptr);
                return std::make_shared<RulesetBMSReplay>(
                    gChartContext.chartMybest, gPlayContext.chartObj[slot], gPlayContext.replayMybest,
                    gPlayContext.replayMybest->gaugeType, keys, judgeDiff,
                    gPlayContext.initialHealth[slot], playSide);

            default:
                return nullptr;
            }
        };
        gPlayContext.ruleset[PLAYER_SLOT_PLAYER] = rulesetFactoryFunc(PLAYER_SLOT_PLAYER);
        gPlayContext.ruleset[PLAYER_SLOT_TARGET] = rulesetFactoryFunc(PLAYER_SLOT_TARGET);
        gPlayContext.ruleset[PLAYER_SLOT_MYBEST] = rulesetFactoryFunc(PLAYER_SLOT_MYBEST);

        if (gPlayContext.isCourse)
        {
            gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->setComboDisplay(gPlayContext.courseRunningCombo[PLAYER_SLOT_PLAYER]);
            gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->setMaxComboDisplay(gPlayContext.courseMaxCombo[PLAYER_SLOT_PLAYER]);
            if (gPlayContext.ruleset[PLAYER_SLOT_TARGET])
            {
                gPlayContext.ruleset[PLAYER_SLOT_TARGET]->setComboDisplay(gPlayContext.courseRunningCombo[PLAYER_SLOT_TARGET]);
                gPlayContext.ruleset[PLAYER_SLOT_TARGET]->setMaxComboDisplay(gPlayContext.courseMaxCombo[PLAYER_SLOT_TARGET]);
            }
        }

        if (!gPlayContext.isAuto && !gPlayContext.isReplay &&
            (!gPlayContext.isBattle || (gPlayContext.replay && State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_GHOST)))
        {
            // create replay
            gPlayContext.replayNew = std::make_shared<ReplayChart>();
            gPlayContext.replayNew->chartHash = gChartContext.hash;
            gPlayContext.replayNew->randomSeed = gPlayContext.randomSeed;
            gPlayContext.replayNew->gaugeType = gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge;
            gPlayContext.replayNew->randomTypeLeft = gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft;
            gPlayContext.replayNew->randomTypeRight = gPlayContext.mods[PLAYER_SLOT_PLAYER].randomRight;
            gPlayContext.replayNew->assistMask = gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask;
            gPlayContext.replayNew->hispeedFix = gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix;
            gPlayContext.replayNew->laneEffectType = (int8_t)gPlayContext.mods[PLAYER_SLOT_PLAYER].laneEffect;
            if (State::get(IndexSwitch::SOUND_PITCH))
            {
                gPlayContext.replayNew->pitchType = (int8_t)State::get(IndexOption::SOUND_PITCH_TYPE);
                gPlayContext.replayNew->pitchValue = (int8_t)std::round((State::get(IndexSlider::PITCH) - 0.5) * 2 * 12);
            }
            gPlayContext.replayNew->DPFlip = gPlayContext.mods[PLAYER_SLOT_PLAYER].DPFlip;
        }

        if (gPlayContext.ruleset[PLAYER_SLOT_TARGET] != nullptr && !gPlayContext.isBattle)
        {
            double targetRateReal = 0.0;
            switch (State::get(IndexOption::PLAY_TARGET_TYPE))
            {
            case Option::TARGET_AAA: targetRateReal = 8.0 / 9; State::set(IndexText::TARGET_NAME, "RANK AAA");   break;
            case Option::TARGET_AA:  targetRateReal = 7.0 / 9; State::set(IndexText::TARGET_NAME, "RANK AA");  break;
            case Option::TARGET_A:   targetRateReal = 6.0 / 9; State::set(IndexText::TARGET_NAME, "RANK A"); break;
            default:
            {
                // rename target
                int targetRate = State::get(IndexNumber::DEFAULT_TARGET_RATE);
                switch (targetRate)
                {
                case 22:  targetRateReal = 2.0 / 9; State::set(IndexText::TARGET_NAME, "RANK E"); break;  // E
                case 33:  targetRateReal = 3.0 / 9; State::set(IndexText::TARGET_NAME, "RANK D"); break;  // D
                case 44:  targetRateReal = 4.0 / 9; State::set(IndexText::TARGET_NAME, "RANK C"); break;  // C
                case 55:  targetRateReal = 5.0 / 9; State::set(IndexText::TARGET_NAME, "RANK B"); break;  // B
                case 66:  targetRateReal = 6.0 / 9; State::set(IndexText::TARGET_NAME, "RANK A");  break;  // A
                case 77:  targetRateReal = 7.0 / 9; State::set(IndexText::TARGET_NAME, "RANK AA"); break;  // AA
                case 88:  targetRateReal = 8.0 / 9; State::set(IndexText::TARGET_NAME, "RANK AAA"); break;  // AAA
                case 100: targetRateReal = 1.0;     State::set(IndexText::TARGET_NAME, "DJ AUTO"); break;  // MAX
                default:
                    targetRateReal = targetRate / 100.0;
                    State::set(IndexText::TARGET_NAME, "RATE "s + std::to_string(targetRate) + "%"s);
                    break;
                }
            }
            break;
            }

            std::dynamic_pointer_cast<RulesetBMSAuto>(gPlayContext.ruleset[PLAYER_SLOT_TARGET])->setTargetRate(targetRateReal);
            if (!gArenaData.isOnline())
            {
                State::set(IndexBargraph::PLAY_RIVAL_EXSCORE_FINAL, targetRateReal);
            }
        }

        // load mybest score
        auto pScore = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
        if (pScore)
        {
            if (!gArenaData.isOnline())
            {
                State::set(IndexBargraph::PLAY_MYBEST_FINAL, (double)pScore->exscore / gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getMaxScore());
            }
            if (!gPlayContext.replayMybest)
            {
                if (!gArenaData.isOnline())
                {
                    State::set(IndexBargraph::PLAY_MYBEST_NOW, (double)pScore->exscore / gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getMaxScore());
                }
                State::set(IndexNumber::RESULT_MYBEST_EX, pScore->exscore);
                State::set(IndexNumber::RESULT_MYBEST_RATE, (int)std::floor(pScore->rate * 100.0));
                State::set(IndexNumber::RESULT_MYBEST_RATE_DECIMAL2, (int)std::floor(pScore->rate * 10000.0) % 100);
                State::set(IndexNumber::RESULT_MYBEST_DIFF, -pScore->exscore);
            }
        }

        if (auto prb = std::dynamic_pointer_cast<RulesetBMS>(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]); prb)
        {
            State::set(IndexText::PLAYER_MODIFIER, prb->getModifierText());
            State::set(IndexText::PLAYER_MODIFIER_SHORT, prb->getModifierTextShort());
        }
        else
        {
            assert(false);
        }

        return true;
    }
    break;

    default:
        break;
    }

    return false;
}

ScenePlay::~ScenePlay()
{
    gPlayContext.bgaTexture->stopUpdate();
    gPlayContext.bgaTexture->reset();

    _input.loopEnd();
    loopEnd();
}

void ScenePlay::setInitialHealthBMS()
{
    if (!gPlayContext.isCourse || gPlayContext.courseStage == 0)
    {
        auto initHealth = [&](int slot)
        {
            PlayModifierGaugeType gaugeType = PlayModifierGaugeType::NORMAL;
            bool setInd = false;
            IndexNumber indNum = IndexNumber::PLAY_1P_GROOVEGAUGE;
            IndexOption indOpt = IndexOption::PLAY_HEALTH_1P;
            switch (slot)
            {
            case PLAYER_SLOT_PLAYER: 
                gaugeType = gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge; 
                setInd = true; 
                break;

            case PLAYER_SLOT_TARGET: 
                gaugeType = gPlayContext.mods[PLAYER_SLOT_TARGET].gauge; 
                setInd = true; 
                indNum = IndexNumber::PLAY_2P_GROOVEGAUGE; 
                indOpt = IndexOption::PLAY_HEALTH_2P; 
                break;

            case PLAYER_SLOT_MYBEST: 
                gaugeType = gPlayContext.replayMybest->gaugeType; 
                setInd = false;
                break;
            }

            switch (gaugeType)
            {
            case PlayModifierGaugeType::NORMAL:
            case PlayModifierGaugeType::EASY:
            case PlayModifierGaugeType::ASSISTEASY:
                gPlayContext.initialHealth[slot] = 0.2;
                if (setInd)
                {
                    State::set(indNum, 20);
                    State::set(indOpt, Option::HEALTH_20);
                }
                break;

            case PlayModifierGaugeType::HARD:
            case PlayModifierGaugeType::DEATH:
                // case PlayModifierGaugeType::PATTACK:
                // case PlayModifierGaugeType::GATTACK:
            case PlayModifierGaugeType::EXHARD:
            case PlayModifierGaugeType::GRADE_NORMAL:
            case PlayModifierGaugeType::GRADE_HARD:
            case PlayModifierGaugeType::GRADE_DEATH:
                gPlayContext.initialHealth[slot] = 1.0;
                if (setInd)
                {
                    State::set(indNum, 100);
                    State::set(indOpt, Option::HEALTH_100);
                }
                break;

            default: break;
            }
        };
        initHealth(PLAYER_SLOT_PLAYER);
        if (gPlayContext.isBattle) initHealth(PLAYER_SLOT_TARGET);
        if (gPlayContext.replayMybest) initHealth(PLAYER_SLOT_MYBEST);
    }
    else
    {
        State::set(IndexNumber::PLAY_1P_GROOVEGAUGE, (int)(gPlayContext.initialHealth[PLAYER_SLOT_PLAYER] * 100));

        if (gPlayContext.isBattle)
        {
            State::set(IndexNumber::PLAY_2P_GROOVEGAUGE, (int)(gPlayContext.initialHealth[PLAYER_SLOT_TARGET] * 100));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::loadChart()
{
    if (!gChartContext.chart) return;

    // always reload unstable resources
    if (!gChartContext.chart->resourceStable)
    {
        gChartContext.isSampleLoaded = false;
        gChartContext.sampleLoadedHash.reset();
        gChartContext.isBgaLoaded = false;
    }

    // load samples
    if ((!gChartContext.isSampleLoaded || gChartContext.hash != gChartContext.sampleLoadedHash) && !sceneEnding)
    {
        auto dtor = std::async(std::launch::async, [&]() {
            SetDebugThreadName("Chart sound sample loading thread");
            SoundMgr::freeNoteSamples();

            auto _pChart = gChartContext.chart;
            auto chartDir = gChartContext.chart->getDirectory();
            LOG_DEBUG << "[Play] Load files from " << chartDir.c_str();
            for (const auto& it : _pChart->wavFiles)
            {
				if (sceneEnding) break;
                if (it.empty()) continue;
                ++wavTotal;
            }
            if (wavTotal == 0)
            {
                wavLoaded = 1;
                gChartContext.isSampleLoaded = true;
                gChartContext.sampleLoadedHash = gChartContext.hash;
                return;
            }

            boost::asio::thread_pool pool(std::max(1u, std::thread::hardware_concurrency() - 2));
            for (size_t i = 0; i < _pChart->wavFiles.size(); ++i)
            {
				if (sceneEnding) break;

                const auto& wav = _pChart->wavFiles[i];
                if (wav.empty()) continue;

                boost::asio::post(pool, std::bind([&](size_t i)
                    {
                        Path pWav = fs::u8path(wav);
                        if (pWav.is_absolute())
                            SoundMgr::loadNoteSample(pWav, i);
                        else
                            SoundMgr::loadNoteSample((chartDir / pWav), i);
                        ++wavLoaded;
                    }, i));
            }
            pool.wait();

            if (!sceneEnding)
            {
                gChartContext.isSampleLoaded = true;
                gChartContext.sampleLoadedHash = gChartContext.hash;
            }
        });
    }
    else
    {
        gChartContext.isSampleLoaded = true;
    }

    // load bga
    if (State::get(IndexSwitch::_LOAD_BGA) && !sceneEnding)
    {
        if (!gChartContext.isBgaLoaded)
        {
            auto dtor = std::async(std::launch::async, [&]() {
                SetDebugThreadName("Chart BGA loading thread");
                gPlayContext.bgaTexture->clear();

                auto _pChart = gChartContext.chart;
                auto chartDir = gChartContext.chart->getDirectory();
                for (const auto& it : _pChart->bgaFiles)
                {
                    if (sceneEnding) return;
                    if (it.empty()) continue;
                    ++bmpTotal;
                }
                if (bmpTotal == 0)
                {
                    bmpLoaded = 1;
                    gChartContext.isBgaLoaded = true;
                    return;
                }

                std::list<std::pair<size_t, Path>> mapBgaFiles;
                auto loadBgaFiles = [&]
                {
                    for (auto& [i, pBmp] : mapBgaFiles)
                    {
                        if (pBmp.is_absolute())
                            gPlayContext.bgaTexture->addBmp(i, pBmp);
                        else
                            gPlayContext.bgaTexture->addBmp(i, chartDir / pBmp);
                        ++bmpLoaded;
                    }
                };
                for (size_t i = 0; i < _pChart->bgaFiles.size(); ++i)
                {
                    if (sceneEnding) return;
                    const auto& bmp = _pChart->bgaFiles[i];
                    if (bmp.empty()) continue;

                    mapBgaFiles.emplace_back(i, fs::u8path(bmp));

                    // load 8 bmps each frame
                    if (mapBgaFiles.size() >= 8)
                    {
                        pushAndWaitMainThreadTask<void>(loadBgaFiles);
                        mapBgaFiles.clear();
                    }
                }
                if (!sceneEnding)
                {
                    loadBgaFiles();
                    mapBgaFiles.clear();
                }
                if (!sceneEnding)
                {
                    if (bmpLoaded > 0)
                    {
                        gPlayContext.bgaTexture->setLoaded();
                    }
                    gPlayContext.bgaTexture->setSlotFromBMS(*std::reinterpret_pointer_cast<ChartObjectBMS>(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]));
                    gChartContext.isBgaLoaded = true;
                }
                });
        }
        else
        {
            // set playback speed on each play
            gPlayContext.bgaTexture->setVideoSpeed();
        }
    }
}

void ScenePlay::setInputJudgeCallback()
{
    using namespace std::placeholders;
    if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER] != nullptr)
    {
        auto fp = std::bind(&RulesetBase::updatePress, gPlayContext.ruleset[PLAYER_SLOT_PLAYER], _1, _2);
        _input.register_p("JUDGE_PRESS_1", fp);
        auto fh = std::bind(&RulesetBase::updateHold, gPlayContext.ruleset[PLAYER_SLOT_PLAYER], _1, _2);
        _input.register_h("JUDGE_HOLD_1", fh);
        auto fr = std::bind(&RulesetBase::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_PLAYER], _1, _2);
        _input.register_r("JUDGE_RELEASE_1", fr);
        auto fa = std::bind(&RulesetBase::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_PLAYER], _1, _2, _3);
        _input.register_a("JUDGE_AXIS_1", fa);
    }
    else
    {
        LOG_ERROR << "[Play] Ruleset of 1P not initialized!";
    }

    if (gPlayContext.ruleset[PLAYER_SLOT_TARGET] != nullptr)
    {
        auto fp = std::bind(&RulesetBase::updatePress, gPlayContext.ruleset[PLAYER_SLOT_TARGET], _1, _2);
        _input.register_p("JUDGE_PRESS_2", fp);
        auto fh = std::bind(&RulesetBase::updateHold, gPlayContext.ruleset[PLAYER_SLOT_TARGET], _1, _2);
        _input.register_h("JUDGE_HOLD_2", fh);
        auto fr = std::bind(&RulesetBase::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_TARGET], _1, _2);
        _input.register_r("JUDGE_RELEASE_2", fr);
        auto fa = std::bind(&RulesetBase::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_TARGET], _1, _2, _3);
        _input.register_a("JUDGE_AXIS_2", fa);
    }
    else if (!gPlayContext.isAuto)
    {
        LOG_ERROR << "[Play] Ruleset of 2P not initialized!";
    }
}

void ScenePlay::removeInputJudgeCallback()
{
    for (size_t i = 0; i < gPlayContext.ruleset.size(); ++i)
    {
        _input.unregister_p("JUDGE_PRESS_1");
        _input.unregister_h("JUDGE_HOLD_1");
        _input.unregister_r("JUDGE_RELEASE_1");
        _input.unregister_a("JUDGE_AXIS_1");
        _input.unregister_p("JUDGE_PRESS_2");
        _input.unregister_h("JUDGE_HOLD_2");
        _input.unregister_r("JUDGE_RELEASE_2");
        _input.unregister_a("JUDGE_AXIS_2");
    }
}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::_updateAsync()
{
    if (gNextScene != SceneType::PLAY) return;

    if (gAppIsExiting)
    {
        gNextScene = SceneType::EXIT_TRANS;
    }

    Time t;

    // update lanecover / hispeed change
    updateAsyncLanecover(t);

    // health + timer, reset per 2%
    updateAsyncGaugeUpTimer(t);

    // absolute axis scratch
    updateAsyncAbsoluteAxis(t);

    // record 
    if (gChartContext.started && gPlayContext.replayNew)
    {
        long long ms = t.norm() - State::get(IndexTimer::PLAY_START);
        if (playerState[PLAYER_SLOT_PLAYER].hispeedHasChanged)
        {
            gPlayContext.replayNew->commands.push_back({ int64_t(ms), ReplayChart::Commands::Type::HISPEED, gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed });
        }
        if (playerState[PLAYER_SLOT_PLAYER].lanecoverTopHasChanged)
        {
            gPlayContext.replayNew->commands.push_back({ int64_t(ms), ReplayChart::Commands::Type::LANECOVER_TOP, double(State::get(IndexNumber::LANECOVER_TOP_1P)) });
        }
        if (playerState[PLAYER_SLOT_PLAYER].lanecoverBottomHasChanged)
        {
            gPlayContext.replayNew->commands.push_back({ int64_t(ms), ReplayChart::Commands::Type::LANECOVER_BOTTOM, double(State::get(IndexNumber::LANECOVER_BOTTOM_1P)) });
        }
        if (playerState[PLAYER_SLOT_PLAYER].lanecoverStateHasChanged)
        {
            gPlayContext.replayNew->commands.push_back({ int64_t(ms), ReplayChart::Commands::Type::LANECOVER_ENABLE, double(int(State::get(IndexSwitch::P1_LANECOVER_ENABLED))) });
        }
    }

    // adjust lanecover display
    updateAsyncLanecoverDisplay(t);

    // HS gradient
    updateAsyncHSGradient(t);

    // update green number
    updateAsyncGreenNumber(t);

    // retry / exit (SELECT+START)
    if ((isHoldingStart(PLAYER_SLOT_PLAYER) && isHoldingSelect(PLAYER_SLOT_PLAYER)) ||
        (isHoldingStart(PLAYER_SLOT_TARGET) && isHoldingSelect(PLAYER_SLOT_TARGET)))
    {
        retryRequestTick++;
    }
    else
    {
        retryRequestTick = 0;
    }
    if (retryRequestTick >= getRate() * 1)
    {
        isManuallyRequestedExit = true;
        requestExit();
    }

    // state based callback
    switch (state)
    {
    case ePlayState::PREPARE:
		State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_PREPARE);
        updatePrepare();
        break;
    case ePlayState::LOADING:
        updateLoading();
        break;
    case ePlayState::LOAD_END:
        updateLoadEnd();
        break;
    case ePlayState::PLAYING:
        updatePlaying();
        break;
    case ePlayState::FADEOUT:
        updateFadeout();
        break;
    case ePlayState::FAILED:
        updateFailed();
        break;
    case ePlayState::WAIT_ARENA:
        updateWaitArena();
        break;
    }

    if (gArenaData.isOnline() && gArenaData.isExpired())
    {
        gArenaData.reset();
    }
}

void ScenePlay::updateAsyncLanecover(const Time& t)
{
    int lcThreshold = getRate() / 200 * _input.getRate() / 1000;  // lanecover, +200 per second
    int hsThreshold = getRate() / 25 * _input.getRate() / 1000;   // hispeed, +25 per second

    auto handleSide = [&](int slot)
    {
        Option::e_lane_effect_type lanecoverType = (Option::e_lane_effect_type)
            State::get(slot == PLAYER_SLOT_PLAYER ? IndexOption::PLAY_LANE_EFFECT_TYPE_1P : IndexOption::PLAY_LANE_EFFECT_TYPE_2P);

        int lc = 0;
        switch (lanecoverType)
        {
        case Option::LANE_SUDDEN:
        case Option::LANE_SUDHID:
            lc = State::get(slot == PLAYER_SLOT_PLAYER ? IndexNumber::LANECOVER_TOP_1P : IndexNumber::LANECOVER_TOP_2P);
            break;
        case Option::LANE_HIDDEN:
            lc = State::get(slot == PLAYER_SLOT_PLAYER ? IndexNumber::LANECOVER_BOTTOM_1P : IndexNumber::LANECOVER_BOTTOM_2P);
            break;
        case Option::LANE_LIFT:
        case Option::LANE_LIFTSUD:
            if (slot == PLAYER_SLOT_PLAYER)
                lc = State::get(State::get(IndexSwitch::P1_LANECOVER_ENABLED) ? IndexNumber::LANECOVER_TOP_1P : IndexNumber::LANECOVER_BOTTOM_1P);
            else
                lc = State::get(State::get(IndexSwitch::P2_LANECOVER_ENABLED) ? IndexNumber::LANECOVER_TOP_2P : IndexNumber::LANECOVER_BOTTOM_2P);
            break;
        }

        bool lcHasChanged = false;
        int lcOld = lc;
        bool inverted = false;
        switch (lanecoverType)
        {
        case Option::LANE_HIDDEN:
        case Option::LANE_LIFT:
            inverted = true;
            break;
        }
        int units = playerState[slot].lanecoverAddPending > 0 ? (playerState[slot].lanecoverAddPending / lcThreshold) : -(-playerState[slot].lanecoverAddPending / lcThreshold);
        if (units != 0)
        {
            playerState[slot].lanecoverAddPending -= units * lcThreshold;
            lc = std::clamp(lc + (inverted ? -units : units), 0, 1000);
            if (lanecoverType == Option::LANE_SUDHID && lc > 500) lc = 500;
            lcHasChanged = true;
        }

        if (lcHasChanged)
        {
            switch (lanecoverType)
            {
            case Option::LANE_SUDHID:
            case Option::LANE_HIDDEN:
            case Option::LANE_LIFT:
                State::set(slot == PLAYER_SLOT_PLAYER ? IndexNumber::LANECOVER_BOTTOM_1P : IndexNumber::LANECOVER_BOTTOM_2P, lc);
                playerState[slot].lanecoverBottomHasChanged = true;
                break;
            }
            switch (lanecoverType)
            {
            case Option::LANE_SUDHID:
            case Option::LANE_SUDDEN:
            case Option::LANE_LIFTSUD:
                State::set(slot == PLAYER_SLOT_PLAYER ? IndexNumber::LANECOVER_TOP_1P : IndexNumber::LANECOVER_TOP_2P, lc);
                playerState[slot].lanecoverTopHasChanged = true;
                break;
            }
        }

        bool lockSpeedEnabled = State::get(slot == PLAYER_SLOT_PLAYER ? IndexSwitch::P1_LOCK_SPEED : IndexSwitch::P2_LOCK_SPEED);
        double *pHispeed = slot == PLAYER_SLOT_PLAYER ? &gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed : &gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed;
        if (playerState[slot].lockspeedResetPending)
        {
            playerState[slot].lockspeedResetPending = false;
            if (lockSpeedEnabled)
            {
                double bpm = gPlayContext.mods[slot].hispeedFix == PlayModifierHispeedFixType::CONSTANT ?
                    150.0 : gPlayContext.chartObj[slot]->getCurrentBPM();
                *pHispeed = std::min(getHiSpeed(bpm, slot, playerState[slot].lockspeedValueInternal), 10.0);
                playerState[slot].hispeedHasChanged = true;
                playerState[slot].lockspeedHispeedBuffered = *pHispeed;
            }
        }
        else if (playerState[slot].hispeedAddPending <= -hsThreshold || playerState[slot].hispeedAddPending >= hsThreshold)
        {
            double hs = *pHispeed;
            double hsOld = hs;
            int units = playerState[slot].hispeedAddPending > 0 ? (playerState[slot].hispeedAddPending / hsThreshold) : -(-playerState[slot].hispeedAddPending / hsThreshold);
            if (units != 0)
            {
                playerState[slot].hispeedAddPending -= units * hsThreshold;
                hs = std::clamp(hs + (inverted ? -units : units) / 100.0, hiSpeedMinSoft, hiSpeedMax);
            }
            *pHispeed = hs;

            if (lockSpeedEnabled)
            {
                double bpm = gPlayContext.mods[slot].hispeedFix == PlayModifierHispeedFixType::CONSTANT ?
                    150.0 : gPlayContext.chartObj[slot]->getCurrentBPM();
                const auto [green, val] = calcGreenNumber(bpm, slot, hs);
                playerState[slot].lockspeedValueInternal = val;
                playerState[slot].lockspeedGreenNumber = green;
                playerState[slot].lockspeedHispeedBuffered = hs;
            }

            playerState[slot].hispeedHasChanged = true;
        }


    };
    handleSide(PLAYER_SLOT_PLAYER);
    if (gPlayContext.isBattle) handleSide(PLAYER_SLOT_TARGET);
}

void ScenePlay::updateAsyncGreenNumber(const Time& t)
{
    // 120BPM with 1.0x HS is 2000ms (500ms/beat, green number 1200)
    auto updateSide = [&](int slot)
    {
        int noteLaneHeight = pSkin->info.noteLaneHeight1P;
        IndexNumber indNow = IndexNumber::GREEN_NUMBER_1P;
        IndexNumber indMax = IndexNumber::GREEN_NUMBER_MAXBPM_1P;
        IndexNumber indMin = IndexNumber::GREEN_NUMBER_MINBPM_1P;
        IndexSwitch indSet = IndexSwitch::P1_SETTING_HISPEED;
        if (slot != PLAYER_SLOT_PLAYER)
        {
            noteLaneHeight = pSkin->info.noteLaneHeight2P;
            indNow = IndexNumber::GREEN_NUMBER_2P;
            indMax = IndexNumber::GREEN_NUMBER_MAXBPM_2P;
            indMin = IndexNumber::GREEN_NUMBER_MINBPM_2P;
            indSet = gPlayContext.isBattle ? IndexSwitch::P2_SETTING_HISPEED : IndexSwitch::P1_SETTING_HISPEED;
        }
        if (noteLaneHeight != 0 && gPlayContext.chartObj[slot] != nullptr)
        {
            double bpm, minBPM, maxBPM;
            if (gPlayContext.mods[slot].hispeedFix != PlayModifierHispeedFixType::CONSTANT)
            {
                bpm = gPlayContext.chartObj[slot]->getCurrentBPM();
                minBPM = gChartContext.minBPM;
                maxBPM = gChartContext.maxBPM;
            }
            else
            {
                bpm = minBPM = maxBPM = 150.0;
            }
            State::set(indNow, calcGreenNumber(bpm, slot, gPlayContext.playerState[slot].hispeed).first);
            State::set(indMax, calcGreenNumber(maxBPM, slot, gPlayContext.playerState[slot].hispeed).first);
            State::set(indMin, calcGreenNumber(minBPM, slot, gPlayContext.playerState[slot].hispeed).first);

            // setting speed / lanecover (if display white number / green number)
            State::set(indSet, State::get(indSet) || (isHoldingStart(slot) || isHoldingSelect(slot)));
        }
    };
    State::set(IndexSwitch::P1_SETTING_HISPEED, false);
    State::set(IndexSwitch::P2_SETTING_HISPEED, false);
    updateSide(PLAYER_SLOT_PLAYER);
    updateSide(PLAYER_SLOT_TARGET);

    State::set(IndexSwitch::P1_SETTING_LANECOVER, State::get(IndexSwitch::P1_LANECOVER_ENABLED) && State::get(IndexSwitch::P1_SETTING_HISPEED));
    State::set(IndexSwitch::P2_SETTING_LANECOVER, State::get(IndexSwitch::P2_LANECOVER_ENABLED) && State::get(IndexSwitch::P2_SETTING_HISPEED));

    // show greennumber on top-left for unsupported skins
    if (!pSkin->isSupportGreenNumber)
    {
        std::stringstream ss;
        if (holdingStart[0] || holdingSelect[0] || holdingStart[1] || holdingSelect[1])
        {
            ss << "G(1P): " << (State::get(IndexSwitch::P1_LOCK_SPEED) ? "FIX " : "") << State::get(IndexNumber::GREEN_NUMBER_1P) <<
                " (" << State::get(IndexNumber::GREEN_NUMBER_MINBPM_1P) << " - " << State::get(IndexNumber::GREEN_NUMBER_MAXBPM_1P) << ")";

            if (gPlayContext.isBattle)
            {
                ss << " | G(2P): " << (State::get(IndexSwitch::P2_LOCK_SPEED) ? "FIX " : "") << State::get(IndexNumber::GREEN_NUMBER_2P) <<
                    " (" << State::get(IndexNumber::GREEN_NUMBER_MINBPM_2P) << " - " << State::get(IndexNumber::GREEN_NUMBER_MAXBPM_2P) << ")";
            }
        }
        State::set(IndexText::_OVERLAY_TOPLEFT, ss.str());
    }
}

void ScenePlay::updateAsyncGaugeUpTimer(const Time& t)
{
    auto updateSide = [&](int slot)
    {
        IndexNumber indNum = IndexNumber::PLAY_1P_GROOVEGAUGE;
        IndexTimer indAdd = IndexTimer::PLAY_GAUGE_1P_ADD;
        IndexTimer indMax = IndexTimer::PLAY_GAUGE_1P_MAX;
        if (slot != PLAYER_SLOT_PLAYER)
        {
            indNum = IndexNumber::PLAY_2P_GROOVEGAUGE;
            indAdd = IndexTimer::PLAY_GAUGE_2P_ADD;
            indMax = IndexTimer::PLAY_GAUGE_2P_MAX;
        }
        int health = State::get(indNum);
        if (playerState[slot].healthLastTick / 2 != health / 2)
        {
            if (slot == 100)
            {
                State::set(indAdd, TIMER_NEVER);
                State::set(indMax, t.norm());
            }
            else if (health > playerState[slot].healthLastTick)
            {
                State::set(indAdd, t.norm());
            }
            else
            {
                State::set(indMax, TIMER_NEVER);
            }
        }
        playerState[slot].healthLastTick = health;
    };
    updateSide(PLAYER_SLOT_PLAYER);
    updateSide(PLAYER_SLOT_TARGET);
}

void ScenePlay::updateAsyncLanecoverDisplay(const Time& t)
{
    auto updateSide = [&](int slot)
    {
        IndexNumber indNumHispeed = IndexNumber::HS_1P;
        IndexSlider indSliHispeed = IndexSlider::HISPEED_1P;
        IndexNumber indNumLanecover = IndexNumber::LANECOVER100_1P;
        IndexNumber indNumSudden = IndexNumber::LANECOVER_TOP_1P;
        IndexSlider indSliSudden = IndexSlider::SUD_1P;
        IndexNumber indNumHidden = IndexNumber::LANECOVER_BOTTOM_1P;
        IndexSlider indSliHidden = IndexSlider::HID_1P;
        IndexSwitch indLanecoverEnabled = IndexSwitch::P1_LANECOVER_ENABLED;
        if (slot != PLAYER_SLOT_PLAYER)
        {
            indNumHispeed = IndexNumber::HS_2P;
            indSliHispeed = IndexSlider::HISPEED_2P;
            indNumLanecover = IndexNumber::LANECOVER100_2P;
            indNumSudden = IndexNumber::LANECOVER_TOP_2P;
            indSliSudden = IndexSlider::SUD_2P;
            indNumHidden = IndexNumber::LANECOVER_BOTTOM_2P;
            indSliHidden = IndexSlider::HID_2P;
            indLanecoverEnabled = IndexSwitch::P2_LANECOVER_ENABLED;
        }

        if (playerState[slot].hispeedHasChanged)
        {
            playerState[slot].hispeedHasChanged = false;

            gPlayContext.playerState[slot].hispeedGradientStart = t;
            gPlayContext.playerState[slot].hispeedGradientFrom = gPlayContext.playerState[slot].hispeedGradientNow;
            State::set(indNumHispeed, (int)std::round(gPlayContext.playerState[slot].hispeed * 100));
            State::set(indSliHispeed, gPlayContext.playerState[slot].hispeed / 10.0);
        }
        if (playerState[slot].lanecoverBottomHasChanged)
        {
            playerState[slot].lanecoverBottomHasChanged = false;

            int lcBottom = State::get(indNumHidden);
            int lc100 = lcBottom / 10;
            double hid = lcBottom / 1000.0;
            State::set(indNumLanecover, lc100);
            State::set(indSliHidden, hid);
        }
        if (playerState[slot].lanecoverTopHasChanged)
        {
            playerState[slot].lanecoverTopHasChanged = false;

            int lcTop = State::get(indNumSudden);
            int lc100 = lcTop / 10;
            double sud = lcTop / 1000.0;
            State::set(indNumLanecover, lc100);
            State::set(indSliSudden, sud);
        }
        if (playerState[slot].lanecoverStateHasChanged)
        {
            playerState[slot].lanecoverStateHasChanged = false;

            toggleLanecover(slot, State::get(indLanecoverEnabled));
        }
    };
    updateSide(PLAYER_SLOT_PLAYER);
    updateSide(PLAYER_SLOT_TARGET);
}

void ScenePlay::updateAsyncHSGradient(const Time& t)
{
    const long long HS_GRADIENT_LENGTH_MS = 200;
    for (int slot = PLAYER_SLOT_PLAYER; slot <= PLAYER_SLOT_TARGET; slot++)
    {
        if (gPlayContext.playerState[slot].hispeedGradientStart != TIMER_NEVER)
        {
            Time hsGradient = t - gPlayContext.playerState[slot].hispeedGradientStart;
            if (hsGradient.norm() < HS_GRADIENT_LENGTH_MS)
            {
                double prog = std::sin((hsGradient.norm() / (double)HS_GRADIENT_LENGTH_MS) * 1.57079632679);
                gPlayContext.playerState[slot].hispeedGradientNow = gPlayContext.playerState[slot].hispeedGradientFrom +
                    prog * (gPlayContext.playerState[slot].hispeed - gPlayContext.playerState[slot].hispeedGradientFrom);
            }
            else
            {
                gPlayContext.playerState[slot].hispeedGradientNow = gPlayContext.playerState[slot].hispeed;
                gPlayContext.playerState[slot].hispeedGradientStart = TIMER_NEVER;
            }
        }
    }
}

void ScenePlay::updateAsyncAbsoluteAxis(const Time& t)
{
    auto Scratch = [&](const Time& t, Input::Pad up, Input::Pad dn, double& val, int slot)
    {
        double scratchThreshold = 0.001;
        double scratchRewind = 0.0001;

        int axisDir = AxisDir::AXIS_NONE;
        if (val > scratchThreshold)
        {
            val -= scratchThreshold;
            axisDir = AxisDir::AXIS_DOWN;
        }
        else if (val < -scratchThreshold)
        {
            val += scratchThreshold;
            axisDir = AxisDir::AXIS_UP;
        }

        if (playerState[slot].scratchDirection != axisDir)
        {
            if (axisDir == AxisDir::AXIS_DOWN)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    State::set(IndexTimer::S1_DOWN, t.norm());
                    State::set(IndexTimer::S1_UP, TIMER_NEVER);
                    State::set(IndexSwitch::S1_DOWN, true);
                }
                else
                {
                    State::set(IndexTimer::S2_DOWN, t.norm());
                    State::set(IndexTimer::S2_UP, TIMER_NEVER);
                    State::set(IndexSwitch::S2_DOWN, true);
                }
            }
            else if (axisDir == AxisDir::AXIS_UP)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    State::set(IndexTimer::S1_DOWN, t.norm());
                    State::set(IndexTimer::S1_UP, TIMER_NEVER);
                    State::set(IndexSwitch::S1_DOWN, true);
                }
                else
                {
                    State::set(IndexTimer::S2_DOWN, t.norm());
                    State::set(IndexTimer::S2_UP, TIMER_NEVER);
                    State::set(IndexSwitch::S2_DOWN, true);
                }
            }
        }

        // push replay command
        if (gChartContext.started && gPlayContext.replayNew)
        {
            long long ms = t.norm() - State::get(IndexTimer::PLAY_START);
            if (axisDir != AxisDir::AXIS_NONE)
            {
                ReplayChart::Commands cmd;
                cmd.ms = ms;
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    cmd.type = axisDir != AxisDir::AXIS_DOWN ? ReplayChart::Commands::Type::S1A_PLUS : ReplayChart::Commands::Type::S1A_MINUS;
                    replayKeyPressing[Input::Pad::S1A] = true;
                }
                else
                {
                    cmd.type = axisDir != AxisDir::AXIS_DOWN ? ReplayChart::Commands::Type::S2A_PLUS : ReplayChart::Commands::Type::S2A_MINUS;
                    replayKeyPressing[Input::Pad::S2A] = true;
                }
                gPlayContext.replayNew->commands.push_back(cmd);
            }
            else
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    if (replayKeyPressing[Input::Pad::S1A])
                    {
                        ReplayChart::Commands cmd;
                        cmd.ms = ms;
                        cmd.type = ReplayChart::Commands::Type::S1A_STOP;
                        replayKeyPressing[Input::Pad::S1A] = false;
                        gPlayContext.replayNew->commands.push_back(cmd);
                    }
                }
                else
                {
                    if (replayKeyPressing[Input::Pad::S2A])
                    {
                        ReplayChart::Commands cmd;
                        cmd.ms = ms;
                        cmd.type = ReplayChart::Commands::Type::S2A_STOP;
                        replayKeyPressing[Input::Pad::S2A] = false;
                        gPlayContext.replayNew->commands.push_back(cmd);
                    }
                }
            }
        }

        if (axisDir != AxisDir::AXIS_NONE && playerState[slot].scratchDirection != axisDir)
        {
            std::array<size_t, 4> keySampleIdxBufScratch;
            size_t sampleCount = 0;

            if (slot == PLAYER_SLOT_PLAYER && !gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isFailed())
            {
                if (playerState[slot].scratchDirection == AxisDir::AXIS_UP && keySampleIndex[Input::S1L])
                    keySampleIdxBufScratch[sampleCount++] = keySampleIndex[Input::S1L];
                if (playerState[slot].scratchDirection == AxisDir::AXIS_DOWN && keySampleIndex[Input::S1R])
                    keySampleIdxBufScratch[sampleCount++] = keySampleIndex[Input::S1R];

                SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, sampleCount, keySampleIdxBufScratch.data());
            }
            if (slot != PLAYER_SLOT_PLAYER && (isPlaymodeDP() || (gPlayContext.isBattle && !gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFailed())))
            {
                if (playerState[slot].scratchDirection == AxisDir::AXIS_UP && keySampleIndex[Input::S2L])
                    keySampleIdxBufScratch[sampleCount++] = keySampleIndex[Input::S2L];
                if (playerState[slot].scratchDirection == AxisDir::AXIS_DOWN && keySampleIndex[Input::S2R])
                    keySampleIdxBufScratch[sampleCount++] = keySampleIndex[Input::S2R];

                SoundMgr::playNoteSample((!gPlayContext.isBattle ? SoundChannelType::KEY_LEFT : SoundChannelType::KEY_RIGHT), sampleCount, keySampleIdxBufScratch.data());
            }

            playerState[slot].scratchLastUpdate = t;
            playerState[slot].scratchDirection = axisDir;
        }

        if (val > scratchRewind)
            val -= scratchRewind;
        else if (val < -scratchRewind)
            val += scratchRewind;
        else
            val = 0.;

        if ((t - playerState[slot].scratchLastUpdate).norm() > 133)
        {
            // release
            if (playerState[slot].scratchDirection != AxisDir::AXIS_NONE)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    State::set(IndexTimer::S1_DOWN, TIMER_NEVER);
                    State::set(IndexTimer::S1_UP, t.norm());
                    State::set(IndexSwitch::S1_DOWN, false);
                }
                else
                {
                    State::set(IndexTimer::S2_DOWN, TIMER_NEVER);
                    State::set(IndexTimer::S2_UP, t.norm());
                    State::set(IndexSwitch::S2_DOWN, false);
                }
            }

            playerState[slot].scratchDirection = AxisDir::AXIS_NONE;
            playerState[slot].scratchLastUpdate = TIMER_NEVER;
        }
    };
    Scratch(t, Input::S1L, Input::S1R, playerState[PLAYER_SLOT_PLAYER].scratchAccumulator, PLAYER_SLOT_PLAYER);
    Scratch(t, Input::S2L, Input::S2R, playerState[PLAYER_SLOT_TARGET].scratchAccumulator, PLAYER_SLOT_TARGET);
}


////////////////////////////////////////////////////////////////////////////////

void ScenePlay::updatePrepare()
{
	auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);
    if (rt.norm() > pSkin->info.timeIntro)
    {
        State::set(IndexTimer::_LOAD_START, t.norm());
		State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_LOADING);
		_loadChartFuture = std::async(std::launch::async, std::bind(&ScenePlay::loadChart, this));
        state = ePlayState::LOADING;
        LOG_DEBUG << "[Play] State changed to LOADING";
    }
}

void ScenePlay::updateLoading()
{
	auto t = Time();
    auto rt = t - State::get(IndexTimer::_LOAD_START);

    State::set(IndexNumber::PLAY_LOAD_PROGRESS_SYS, int(chartObjLoaded * 50 + rulesetLoaded * 50));
    State::set(IndexNumber::PLAY_LOAD_PROGRESS_WAV, int(getWavLoadProgress() * 100));
    State::set(IndexNumber::PLAY_LOAD_PROGRESS_BGA, int(getBgaLoadProgress() * 100));
    State::set(IndexNumber::PLAY_LOAD_PROGRESS_PERCENT, int(
        getWavLoadProgress() * 100 + getBgaLoadProgress() * 100) / 2);

    State::set(IndexBargraph::MUSIC_LOAD_PROGRESS_SYS, int(chartObjLoaded) * 0.5 + int(rulesetLoaded) * 0.5);
    State::set(IndexBargraph::MUSIC_LOAD_PROGRESS_WAV, getWavLoadProgress());
    State::set(IndexBargraph::MUSIC_LOAD_PROGRESS_BGA, getBgaLoadProgress());
    State::set(IndexBargraph::MUSIC_LOAD_PROGRESS, int(
        getWavLoadProgress() + getBgaLoadProgress()) / 2.0);

    if (chartObjLoaded && 
        rulesetLoaded &&
        gChartContext.isSampleLoaded && 
        (!State::get(IndexSwitch::_LOAD_BGA) || gChartContext.isBgaLoaded) &&
        (t - delayedReadyTime) > 1000 &&
		rt > pSkin->info.timeMinimumLoad)
    {
        bool trans = true;
        if (gArenaData.isOnline())
        {
            trans = gArenaData.isPlaying();
            if (!gArenaData.isPlaying())
            {
                if (gArenaData.isClient())
                    g_pArenaClient->setLoadingFinished(pSkin->info.timeGetReady);
                else
                    g_pArenaHost->setLoadingFinished(pSkin->info.timeGetReady);
            }
            else
            {
                pSkin->info.timeGetReady = gArenaData.getPlayStartTimeMs();
            }
        }
        if (trans)
        {
            State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_READY);
            if (gPlayContext.bgaTexture) gPlayContext.bgaTexture->reset();
            State::set(IndexTimer::PLAY_READY, t.norm());
            state = ePlayState::LOAD_END;
            LOG_DEBUG << "[Play] State changed to READY";
        }
    }
}

void ScenePlay::updateLoadEnd()
{
	auto t = Time();
    auto rt = t - State::get(IndexTimer::PLAY_READY);
    spinTurntable(false);
    if (rt > pSkin->info.timeGetReady)
    {
        changeKeySampleMapping(0);
		State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_PLAYING);
        State::set(IndexTimer::PLAY_START, t.norm());
        setInputJudgeCallback();
		gChartContext.started = true;
        if (gPlayContext.replayNew)
        {
            gPlayContext.replayNew->hispeed = gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed;
            gPlayContext.replayNew->lanecoverTop = State::get(IndexNumber::LANECOVER_TOP_1P);
            gPlayContext.replayNew->lanecoverBottom = State::get(IndexNumber::LANECOVER_BOTTOM_1P);
            gPlayContext.replayNew->lanecoverEnabled = State::get(IndexSwitch::P1_LANECOVER_ENABLED);
        }
        state = ePlayState::PLAYING;
        LOG_DEBUG << "[Play] State changed to PLAY_START";
    }
}

void ScenePlay::updatePlaying()
{
	auto t = Time();
	auto rt = t - State::get(IndexTimer::PLAY_START);
    State::set(IndexTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre() * 4.0)) % 1000);

    assert(gPlayContext.ruleset[PLAYER_SLOT_PLAYER] != nullptr);
    {
        gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->update(rt);
        gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->update(t);
    }
    if (gPlayContext.ruleset[PLAYER_SLOT_MYBEST] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_MYBEST]->update(rt);
        gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->update(t);
    }
    if (gPlayContext.ruleset[PLAYER_SLOT_TARGET] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_TARGET]->update(rt);
        gPlayContext.ruleset[PLAYER_SLOT_TARGET]->update(t);
    }

    // update replay key timers and lanecover values
    if (gPlayContext.isReplay ||
        (gPlayContext.isBattle && State::get(IndexOption::PLAY_BATTLE_TYPE) == Option::BATTLE_GHOST))
    {
        int slot = !gPlayContext.isBattle ? PLAYER_SLOT_PLAYER : PLAYER_SLOT_TARGET;
        InputMask prev = replayKeyPressing;
        while (itReplayCommand != gPlayContext.replay->commands.end() && rt.norm() >= itReplayCommand->ms)
        {
            auto cmd = itReplayCommand->type;
            if (gPlayContext.isBattle)
            {
                // Ghost battle, replace 1P input with 2P input
                switch (itReplayCommand->type)
                {
                case ReplayChart::Commands::Type::S1L_DOWN: cmd = ReplayChart::Commands::Type::S2L_DOWN; break;
                case ReplayChart::Commands::Type::S1R_DOWN: cmd = ReplayChart::Commands::Type::S2R_DOWN; break;
                case ReplayChart::Commands::Type::K11_DOWN: cmd = ReplayChart::Commands::Type::K21_DOWN; break;
                case ReplayChart::Commands::Type::K12_DOWN: cmd = ReplayChart::Commands::Type::K22_DOWN; break;
                case ReplayChart::Commands::Type::K13_DOWN: cmd = ReplayChart::Commands::Type::K23_DOWN; break;
                case ReplayChart::Commands::Type::K14_DOWN: cmd = ReplayChart::Commands::Type::K24_DOWN; break;
                case ReplayChart::Commands::Type::K15_DOWN: cmd = ReplayChart::Commands::Type::K25_DOWN; break;
                case ReplayChart::Commands::Type::K16_DOWN: cmd = ReplayChart::Commands::Type::K26_DOWN; break;
                case ReplayChart::Commands::Type::K17_DOWN: cmd = ReplayChart::Commands::Type::K27_DOWN; break;
                case ReplayChart::Commands::Type::K18_DOWN: cmd = ReplayChart::Commands::Type::K28_DOWN; break;
                case ReplayChart::Commands::Type::K19_DOWN: cmd = ReplayChart::Commands::Type::K29_DOWN; break;
                case ReplayChart::Commands::Type::S1L_UP: cmd = ReplayChart::Commands::Type::S2L_UP; break;
                case ReplayChart::Commands::Type::S1R_UP: cmd = ReplayChart::Commands::Type::S2R_UP; break;
                case ReplayChart::Commands::Type::K11_UP: cmd = ReplayChart::Commands::Type::K21_UP; break;
                case ReplayChart::Commands::Type::K12_UP: cmd = ReplayChart::Commands::Type::K22_UP; break;
                case ReplayChart::Commands::Type::K13_UP: cmd = ReplayChart::Commands::Type::K23_UP; break;
                case ReplayChart::Commands::Type::K14_UP: cmd = ReplayChart::Commands::Type::K24_UP; break;
                case ReplayChart::Commands::Type::K15_UP: cmd = ReplayChart::Commands::Type::K25_UP; break;
                case ReplayChart::Commands::Type::K16_UP: cmd = ReplayChart::Commands::Type::K26_UP; break;
                case ReplayChart::Commands::Type::K17_UP: cmd = ReplayChart::Commands::Type::K27_UP; break;
                case ReplayChart::Commands::Type::K18_UP: cmd = ReplayChart::Commands::Type::K28_UP; break;
                case ReplayChart::Commands::Type::K19_UP: cmd = ReplayChart::Commands::Type::K29_UP; break;
                case ReplayChart::Commands::Type::S1A_PLUS:  cmd = ReplayChart::Commands::Type::S2A_PLUS; break;
                case ReplayChart::Commands::Type::S1A_MINUS: cmd = ReplayChart::Commands::Type::S2A_MINUS; break;
                case ReplayChart::Commands::Type::S1A_STOP:  cmd = ReplayChart::Commands::Type::S2A_STOP; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_EXACT_0:   cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_EXACT_0; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_0:   cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_0; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_1:   cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_1; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_2:   cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_2; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_3:   cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_3; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_4:   cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_4; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_5:   cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_5; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_0:    cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_0; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_1:    cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_1; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_2:    cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_2; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_3:    cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_3; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4:    cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_5:    cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_5; break;
                case ReplayChart::Commands::Type::JUDGE_LEFT_LANDMINE:  cmd = cmd = ReplayChart::Commands::Type::JUDGE_RIGHT_LANDMINE; break;
                }
            }

            if (gPlayContext.mode == SkinType::PLAY5 || gPlayContext.mode == SkinType::PLAY5_2)
            {
                if (REPLAY_CMD_INPUT_DOWN_MAP_5K[replayCmdMapIndex].find(cmd) != REPLAY_CMD_INPUT_DOWN_MAP_5K[replayCmdMapIndex].end())
                {
                    replayKeyPressing[REPLAY_CMD_INPUT_DOWN_MAP_5K[replayCmdMapIndex].at(cmd)] = true;
                }
                else if (REPLAY_CMD_INPUT_UP_MAP_5K[replayCmdMapIndex].find(cmd) != REPLAY_CMD_INPUT_UP_MAP_5K[replayCmdMapIndex].end())
                {
                    replayKeyPressing[REPLAY_CMD_INPUT_UP_MAP_5K[replayCmdMapIndex].at(cmd)] = false;
                }
            }
            else
            {
                if (REPLAY_CMD_INPUT_DOWN_MAP.find(cmd) != REPLAY_CMD_INPUT_DOWN_MAP.end())
                {
                    replayKeyPressing[REPLAY_CMD_INPUT_DOWN_MAP.at(cmd)] = true;
                }
                else if (REPLAY_CMD_INPUT_UP_MAP.find(cmd) != REPLAY_CMD_INPUT_UP_MAP.end())
                {
                    replayKeyPressing[REPLAY_CMD_INPUT_UP_MAP.at(cmd)] = false;
                }
            }

            switch (cmd)
            {
            case ReplayChart::Commands::Type::S1A_PLUS:  playerState[PLAYER_SLOT_PLAYER].scratchAccumulator = 0.0015; break;
            case ReplayChart::Commands::Type::S1A_MINUS: playerState[PLAYER_SLOT_PLAYER].scratchAccumulator = -0.0015; break;
            case ReplayChart::Commands::Type::S1A_STOP:  playerState[PLAYER_SLOT_PLAYER].scratchAccumulator = 0; break;
            case ReplayChart::Commands::Type::S2A_PLUS:  playerState[PLAYER_SLOT_TARGET].scratchAccumulator = 0.0015; break;
            case ReplayChart::Commands::Type::S2A_MINUS: playerState[PLAYER_SLOT_TARGET].scratchAccumulator = -0.0015; break;
            case ReplayChart::Commands::Type::S2A_STOP:  playerState[PLAYER_SLOT_TARGET].scratchAccumulator = 0; break;

            case ReplayChart::Commands::Type::HISPEED:
                gPlayContext.playerState[slot].hispeed = itReplayCommand->value;
                playerState[slot].hispeedHasChanged = true;
                break;

            case ReplayChart::Commands::Type::LANECOVER_TOP: 
                State::set(slot == PLAYER_SLOT_PLAYER ? IndexNumber::LANECOVER_TOP_1P : IndexNumber::LANECOVER_TOP_2P, itReplayCommand->value);
                playerState[slot].lanecoverTopHasChanged = true;
                break;

            case ReplayChart::Commands::Type::LANECOVER_BOTTOM:
                State::set(slot == PLAYER_SLOT_PLAYER ? IndexNumber::LANECOVER_BOTTOM_1P : IndexNumber::LANECOVER_BOTTOM_2P, itReplayCommand->value);
                playerState[slot].lanecoverBottomHasChanged = true;
                break;

            case ReplayChart::Commands::Type::LANECOVER_ENABLE:
                State::set(slot == PLAYER_SLOT_PLAYER ? IndexSwitch::P1_LANECOVER_ENABLED : IndexSwitch::P2_LANECOVER_ENABLED, (bool)(int)itReplayCommand->value);
                playerState[slot].lanecoverStateHasChanged = true;
                break;
            }

            // do not accept replay-requested ESC in battle mode
            if (gPlayContext.isReplay && cmd == ReplayChart::Commands::Type::ESC)
            {
                isReplayRequestedExit = true;
                requestExit();
            }

            itReplayCommand++;
        }
        InputMask pressed = replayKeyPressing & ~prev;
        InputMask released = ~replayKeyPressing & prev;
        if (pressed.any())
        {
            inputGamePressTimer(pressed, t);
            inputGamePressPlayKeysounds(pressed, t);
        }
        if (released.any())
        {
            inputGameReleaseTimer(released, t);
        }

        holdingStart[0] = replayKeyPressing[Input::Pad::K1START];
        holdingSelect[0] = replayKeyPressing[Input::Pad::K1SELECT];
        holdingStart[1] = replayKeyPressing[Input::Pad::K2START];
        holdingSelect[1] = replayKeyPressing[Input::Pad::K2SELECT];

    }
    
    // update score numbers
    int miss1 = 0;
    int miss2 = 0;
    if (true)
    {
        auto dp1 = gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData();

        long long exScore1P = 0;
        long long exScore2P = 0;
        long long exScoreMybest = 0;
        int missMybest = 0;
        if (auto pr = std::dynamic_pointer_cast<RulesetBMS>(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]); pr)
        {
            exScore1P = pr->getExScore();
            miss1 = pr->getJudgeCountEx(RulesetBMS::JUDGE_BP);
        }

        if (gPlayContext.ruleset[PLAYER_SLOT_MYBEST] != nullptr)
        {
            auto dpb = gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->getData();

            State::set(IndexNumber::RESULT_MYBEST_RATE, (int)std::floor(dpb.acc * 100.0));
            State::set(IndexNumber::RESULT_MYBEST_RATE_DECIMAL2, (int)std::floor(dpb.acc * 10000.0) % 100);

            if (auto prpb = std::dynamic_pointer_cast<RulesetBMS>(gPlayContext.ruleset[PLAYER_SLOT_MYBEST]); prpb)
            {
                exScoreMybest = prpb->getExScore();
                missMybest = prpb->getJudgeCountEx(RulesetBMS::JUDGE_BP);
            }
        }

        auto targetType = State::get(IndexOption::PLAY_TARGET_TYPE);

        if (gPlayContext.ruleset[PLAYER_SLOT_TARGET] != nullptr)
        {
            if (targetType == Option::TARGET_MYBEST && gPlayContext.replayMybest)
            {
                auto dp2 = gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->getData();

                State::set(IndexNumber::RESULT_TARGET_RATE, (int)std::floor(dp2.acc * 100.0) / 100);
                State::set(IndexNumber::RESULT_TARGET_RATE_DECIMAL2, (int)std::floor(dp2.acc * 10000.0) % 100);
            }
            else if (targetType == Option::TARGET_0)
            {
                State::set(IndexNumber::RESULT_TARGET_RATE, 0);
                State::set(IndexNumber::RESULT_TARGET_RATE_DECIMAL2, 0);
            }
            else
            {
                auto dp2 = gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData();

                State::set(IndexNumber::RESULT_TARGET_RATE, (int)std::floor(dp2.acc * 100.0) / 100);
                State::set(IndexNumber::RESULT_TARGET_RATE_DECIMAL2, (int)std::floor(dp2.acc * 10000.0) % 100);
            }

            if (auto pr2 = std::dynamic_pointer_cast<RulesetBMS>(gPlayContext.ruleset[PLAYER_SLOT_TARGET]); pr2)
            {
                exScore2P = pr2->getExScore();
                miss2 = pr2->getJudgeCountEx(RulesetBMS::JUDGE_BP);
            }
        }
        State::set(IndexNumber::RESULT_MYBEST_EX, exScoreMybest);
        State::set(IndexNumber::RESULT_MYBEST_DIFF, exScore1P - exScoreMybest);

        if (!gPlayContext.isBattle)
        {
            if (targetType == Option::TARGET_MYBEST && gPlayContext.replayMybest)
            {
                exScore2P = exScoreMybest;
            }
            else if (targetType == Option::TARGET_0)
            {
                exScore2P = 0;
            }
            State::set(IndexNumber::PLAY_2P_EXSCORE, exScore2P);
        }
        State::set(IndexNumber::PLAY_1P_EXSCORE_DIFF, exScore1P - exScore2P);
        State::set(IndexNumber::PLAY_2P_EXSCORE_DIFF, exScore2P - exScore1P);

        State::set(IndexNumber::RESULT_TARGET_EX, exScore2P);
        State::set(IndexNumber::RESULT_TARGET_DIFF, exScore1P - exScore2P);
    }

    // update poor bga
    if (playerState[PLAYER_SLOT_PLAYER].judgeBP != miss1)
    {
        playerState[PLAYER_SLOT_PLAYER].judgeBP = miss1;
        poorBgaStartTime = t;
    }
    if (playerState[PLAYER_SLOT_TARGET].judgeBP != miss2)
    {
        playerState[PLAYER_SLOT_TARGET].judgeBP = miss2;
        poorBgaStartTime = t;
    }
    gPlayContext.bgaTexture->update(rt, t.norm() - poorBgaStartTime.norm() < poorBgaDuration);

    // BPM
    State::set(IndexNumber::PLAY_BPM, int(std::round(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM())));

    // play time / remain time
    updatePlayTime(rt);

    // play bgm lanes
    procCommonNotes();

    // update keysound bindings
    changeKeySampleMapping(rt);

    // graphs
    if (rt.norm() / 500 >= gPlayContext.graphGauge[PLAYER_SLOT_PLAYER].size())
    {
        auto& g = gPlayContext.graphGauge[PLAYER_SLOT_PLAYER];
        auto& r = gPlayContext.ruleset[PLAYER_SLOT_PLAYER];
        int h = r->getClearHealth() * 100;
        if (!g.empty())
        {
            int ch = r->getData().health * 100;
            if ((g.back() < h && ch > h) || (g.back() > h && ch < h))
            {
                // just insert an interim point, as for a game we don't need to be too precise
                g.push_back(h);
            }
        }

        pushGraphPoints();
    }

    // health check (-> to failed)
    if (!playInterrupted)
    {
        if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isFailed() && gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->failWhenNoHealth() &&
            (!gPlayContext.isBattle || gPlayContext.ruleset[PLAYER_SLOT_TARGET] == nullptr || gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFailed() && gPlayContext.ruleset[PLAYER_SLOT_TARGET]->failWhenNoHealth()))
        {
            pushGraphPoints();

            playInterrupted = true;
            if (gArenaData.isOnline())
            {
                State::set(IndexTimer::ARENA_PLAY_WAIT, t.norm());
                state = ePlayState::WAIT_ARENA;
                LOG_DEBUG << "[Play] State changed to WAIT_ARENA";
            }
            else
            {
                State::set(IndexTimer::FAIL_BEGIN, t.norm());
                State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_FAILED);
                state = ePlayState::FAILED;
                SoundMgr::stopSysSamples();
                SoundMgr::stopNoteSamples();
                SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_PLAYSTOP);
                LOG_DEBUG << "[Play] State changed to PLAY_FAILED";
            }
            for (size_t i = 0; i < gPlayContext.ruleset.size(); ++i)
            {
                _input.unregister_p("SCENE_PRESS");
            }
        }
    }

    // finish check
    auto finishCheckSide = [&](int slot)
    {
        if (!playerState[slot].finished)
        {
            bool fullCombo = gPlayContext.ruleset[slot]->getData().combo == gPlayContext.ruleset[slot]->getMaxCombo();
            if (gPlayContext.ruleset[slot]->isFinished() || fullCombo)
            {
                State::set(slot == PLAYER_SLOT_PLAYER ? IndexTimer::PLAY_P1_FINISHED : IndexTimer::PLAY_P2_FINISHED, t.norm());

                if (fullCombo)
                {
                    State::set(slot == PLAYER_SLOT_PLAYER ? IndexTimer::PLAY_FULLCOMBO_1P : IndexTimer::PLAY_FULLCOMBO_2P, t.norm());
                }

                playerState[slot].finished = true;

                LOG_INFO << "[Play] " << slot + 1 << "P finished";
            }
        }
    };
    finishCheckSide(PLAYER_SLOT_PLAYER);
    if (gPlayContext.ruleset[PLAYER_SLOT_TARGET]) finishCheckSide(PLAYER_SLOT_TARGET);

    if (isPlaymodeDP())
    {
        State::set(IndexTimer::PLAY_P2_FINISHED, t.norm());

        if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().combo == gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getMaxCombo())
        {
            State::set(IndexTimer::PLAY_FULLCOMBO_2P, t.norm());
        }
    }

    if (gArenaData.isOnline())
    {
        if (gArenaData.isClient())
            g_pArenaClient->setPlayingFinished();
        else
            g_pArenaHost->setPlayingFinished();
    }

    playFinished = !playInterrupted && playerState[PLAYER_SLOT_PLAYER].finished && (!gPlayContext.isBattle || playerState[PLAYER_SLOT_TARGET].finished);

    // 
    spinTurntable(true);

    // outro check
    if (rt.hres() - gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().hres() >= 0)
    {
        if (gArenaData.isOnline())
        {
            State::set(IndexTimer::ARENA_PLAY_WAIT, t.norm());
            state = ePlayState::WAIT_ARENA;
            LOG_DEBUG << "[Play] State changed to WAIT_ARENA";
        }
        else
        {
            State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
            State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
            state = ePlayState::FADEOUT;
            LOG_DEBUG << "[Play] State changed to FADEOUT";
        }
    }
}

void ScenePlay::updateFadeout()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::PLAY_START);
    auto ft = t - State::get(IndexTimer::FADEOUT_BEGIN);

    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->update(rt);
    }
    if (gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_TARGET]->update(rt);
    }
    if (gPlayContext.chartObj[PLAYER_SLOT_MYBEST] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_MYBEST]->update(rt);
    }

    if (gChartContext.started)
    {
        State::set(IndexTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre() * 4.0)) % 1000);

        gPlayContext.bgaTexture->update(rt, false);

        State::set(IndexNumber::PLAY_BPM, int(std::round(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM())));
    }

    // play time / remain time
    updatePlayTime(rt);

    //
    spinTurntable(gChartContext.started);

    if (ft >= pSkin->info.timeOutro)
    {
        sceneEnding = true;
        if (_loadChartFuture.valid())
            _loadChartFuture.wait();

        removeInputJudgeCallback();

        bool cleared = false;
        if (gPlayContext.isBattle)
        {
            if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isCleared() ||
                gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isCleared())
                cleared = true;
        }
        else
        {
            if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isCleared())
                cleared = true;
        }
        State::set(IndexSwitch::RESULT_CLEAR, cleared);

        // restore hispeed if FHS
        if (State::get(IndexSwitch::P1_LOCK_SPEED))
        {
            gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed = playerState[PLAYER_SLOT_PLAYER].savedHispeed;
        }
        State::set(IndexNumber::HS_1P, (int)std::round(gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed * 100));
        State::set(IndexSlider::HISPEED_1P, gPlayContext.playerState[PLAYER_SLOT_PLAYER].hispeed / 10.0);
        if (gPlayContext.isBattle)
        {
            if (State::get(IndexSwitch::P2_LOCK_SPEED))
            {
                gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed = playerState[PLAYER_SLOT_TARGET].savedHispeed;
            }
            State::set(IndexNumber::HS_2P, (int)std::round(gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed * 100));
            State::set(IndexSlider::HISPEED_2P, gPlayContext.playerState[PLAYER_SLOT_TARGET].hispeed / 10.0);
        }

        // save lanecover settings
        if (!gPlayContext.isReplay)
        {
            auto saveLanecoverSide = [&](int slot)
            {
                IndexOption indLanecoverType = IndexOption::PLAY_LANE_EFFECT_TYPE_1P;
				IndexSwitch indLockSpeed = IndexSwitch::P1_LOCK_SPEED;
				IndexNumber indLanecoverTop = IndexNumber::LANECOVER_TOP_1P;
				IndexNumber indLanecoverBottom = IndexNumber::LANECOVER_BOTTOM_1P;
				IndexSwitch indLanecoverEnabled = IndexSwitch::P1_LANECOVER_ENABLED;
				const char* cfgLaneEffect = cfg::P_LANE_EFFECT_OP;
				if (slot != PLAYER_SLOT_PLAYER)
				{
					indLanecoverType = IndexOption::PLAY_LANE_EFFECT_TYPE_2P;
					indLockSpeed = IndexSwitch::P2_LOCK_SPEED;
					indLanecoverTop = IndexNumber::LANECOVER_TOP_2P;
					indLanecoverBottom = IndexNumber::LANECOVER_BOTTOM_2P;
					indLanecoverEnabled = IndexSwitch::P2_LANECOVER_ENABLED;
					cfgLaneEffect = cfg::P_LANE_EFFECT_OP_2P;
				}

				auto lanecoverType = State::get(indLanecoverType);
				bool saveTop = false;
				bool saveBottom = false;
				switch (lanecoverType)
				{
				case Option::LANE_SUDDEN:
				case Option::LANE_SUDHID:
				case Option::LANE_LIFTSUD:
					saveTop = true;
					break;
				}
				switch (lanecoverType)
				{
				case Option::LANE_HIDDEN:
				case Option::LANE_SUDHID:
				case Option::LANE_LIFT:
				case Option::LANE_LIFTSUD:
					saveBottom = true;
					break;
				}
				if (saveTop)
					ConfigMgr::set('P', cfg::P_LANECOVER_TOP, State::get(indLanecoverTop));
				if (saveBottom)
					ConfigMgr::set('P', cfg::P_LANECOVER_BOTTOM, State::get(indLanecoverBottom));

				if (State::get(indLockSpeed))
				{
					ConfigMgr::set('P', cfg::P_GREENNUMBER, playerState[slot].lockspeedGreenNumber);

					if (State::get(IndexOption::PLAY_HSFIX_TYPE) == Option::SPEED_NORMAL)
					{
						ConfigMgr::set('P', cfg::P_SPEED_TYPE, cfg::P_SPEED_TYPE_INITIAL);
					}
				}

				// only save OFF -> SUD+
				if (playerState[slot].origLanecoverType == Option::LANE_OFF && State::get(indLanecoverEnabled))
				{
					ConfigMgr::set('P', cfgLaneEffect, cfg::P_LANE_EFFECT_OP_SUDDEN);
				}
            };
            saveLanecoverSide(PLAYER_SLOT_PLAYER);
            if (gPlayContext.isBattle) saveLanecoverSide(PLAYER_SLOT_TARGET);
        }

        // reset BGA
        gPlayContext.bgaTexture->reset();

        // check and set next scene
        gNextScene = SceneType::SELECT;

        // check quick retry (start+select / white+black)
        bool wantRetry = false;
        bool wantNewRandomSeed = false;
        if (gPlayContext.canRetry && gChartContext.started && playInterrupted && !playFinished)
        {
            auto h = _input.Holding();
            using namespace Input;

            bool ss = isHoldingStart(PLAYER_SLOT_PLAYER) && isHoldingSelect(PLAYER_SLOT_PLAYER);
            bool ss2 = isHoldingStart(PLAYER_SLOT_TARGET) && isHoldingSelect(PLAYER_SLOT_TARGET);
            bool white = h.test(K11) || h.test(K13) || h.test(K15) || h.test(K17) || h.test(K19);
            bool black = h.test(K12) || h.test(K14) || h.test(K16) || h.test(K18);
            bool white2 = h.test(K21) || h.test(K23) || h.test(K25) || h.test(K27) || h.test(K29);
            bool black2 = h.test(K22) || h.test(K24) || h.test(K26) || h.test(K28);

            if (gPlayContext.isBattle)
            {
                if (ss || ss2 || (white && black) || (white2 && black2))
                {
                    wantRetry = true;
                    wantNewRandomSeed = ss || ss2;
                }
            }
            else if (isPlaymodeDP())
            {
                bool ss3 = isHoldingStart(PLAYER_SLOT_PLAYER) && isHoldingSelect(PLAYER_SLOT_TARGET);
                bool ss4 = isHoldingStart(PLAYER_SLOT_TARGET) && isHoldingSelect(PLAYER_SLOT_PLAYER);
                if (ss || ss2 || ss3 || ss4 || ((white || white2) && (black || black2)))
                {
                    wantRetry = true;
                    wantNewRandomSeed = ss || ss2 || ss3 || ss4;
                }
            }
            else
            {
                if (ss || (white && black))
                {
                    wantRetry = true;
                    wantNewRandomSeed = ss;
                }
            }
        }
        if (wantRetry)
        {
            if (wantNewRandomSeed)
            {
                // the retry is requested by START+SELECT
                static std::random_device rd;
                gPlayContext.randomSeed = ((uint64_t)rd() << 32) | rd();
            }
            SoundMgr::stopNoteSamples();
            gNextScene = SceneType::RETRY_TRANS;
        }
        else if (gPlayContext.isAuto)
        {
            // Auto mode skips single result

            if (gPlayContext.isCourse)
            {
                // If playing course mode, just go straight to next stage
                if (playerState[PLAYER_SLOT_PLAYER].finished && gPlayContext.courseStage < gPlayContext.courseCharts.size())
                {
                    ++gPlayContext.courseStage;
                    gNextScene = SceneType::COURSE_TRANS;
                }
            }
        }
        else if (gPlayContext.isCourse && gPlayContext.courseStage > 0)
        {
            // Course Stage 2+ should go to result, regardless of whether any notes are hit
            gNextScene = SceneType::RESULT;
        }
        else if (gChartContext.started)
        {
            // Skip result if no score
            if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER] && !gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isNoScore() ||
                gPlayContext.isBattle && gPlayContext.ruleset[PLAYER_SLOT_TARGET] && !gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isNoScore())
            {
                gNextScene = SceneType::RESULT;
            }
        }

        if (gNextScene == SceneType::SELECT && gQuitOnFinish)
        {
            gNextScene = SceneType::EXIT_TRANS;
        }

        // protect
        if (gPlayContext.ruleset[PLAYER_SLOT_MYBEST] &&
            !gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->isFailed() &&
            !gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->isFinished())
        {
            gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->fail();
            gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->updateGlobals();
        }

        if (!gPlayContext.isBattle &&
            gPlayContext.ruleset[PLAYER_SLOT_TARGET] && 
            !gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFailed() && 
            !gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFinished())
        {
            gPlayContext.ruleset[PLAYER_SLOT_TARGET]->fail();
            gPlayContext.ruleset[PLAYER_SLOT_TARGET]->updateGlobals();
        }
    }
}

void ScenePlay::updateFailed()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::PLAY_START);
    auto ft = t - State::get(IndexTimer::FAIL_BEGIN);

    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->update(rt);
    }
    if (gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_TARGET]->update(rt);
    }
    if (gPlayContext.chartObj[PLAYER_SLOT_MYBEST] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_MYBEST]->update(rt);
    }

    if (gChartContext.started)
    {
        State::set(IndexTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre() * 4.0)) % 1000);

        gPlayContext.bgaTexture->update(rt, false);

        State::set(IndexNumber::PLAY_BPM, int(std::round(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM())));
    }

    // play time / remain time
    updatePlayTime(rt);

    //
    spinTurntable(gChartContext.started);

    //failed play finished, move to next scene. No fadeout
    if (ft.norm() >= pSkin->info.timeFailed)
    {
        State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
        State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        state = ePlayState::FADEOUT;
        LOG_DEBUG << "[Play] State changed to FADEOUT";
    }
}

void ScenePlay::updateWaitArena()
{
    Time t;
    auto rt = t - State::get(IndexTimer::PLAY_START);

    gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->update(rt);
    gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->update(t);

    if (gChartContext.started)
    {
        State::set(IndexTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre() * 4.0)) % 1000);

        gPlayContext.bgaTexture->update(rt, false);

        State::set(IndexNumber::PLAY_BPM, int(std::round(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM())));
    }

    // play time / remain time
    updatePlayTime(rt);

    // play bgm lanes
    procCommonNotes();

    if (!gArenaData.isOnline() || gArenaData.isPlayingFinished())
    {
        State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
        State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        state = ePlayState::FADEOUT;
    }
}

void ScenePlay::updatePlayTime(const Time& rt)
{
    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        auto startTime = rt - State::get(IndexTimer::PLAY_START);
        auto totalTime = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().norm();
        auto playtime_s = rt.norm() / 1000;
        auto remaintime_s = totalTime / 1000 - playtime_s;
        State::set(IndexNumber::PLAY_MIN, int(playtime_s / 60));
        State::set(IndexNumber::PLAY_SEC, int(playtime_s % 60));
        State::set(IndexNumber::PLAY_REMAIN_MIN, int(remaintime_s / 60));
        State::set(IndexNumber::PLAY_REMAIN_SEC, int(remaintime_s % 60));
        State::set(IndexSlider::SONG_PROGRESS, (double)rt.norm() / totalTime);
    }
}

void ScenePlay::procCommonNotes()
{
    assert(gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr);
    auto it = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteBgmExpired.begin();
    size_t max = std::min(_bgmSampleIdxBuf.size(), gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteBgmExpired.size());
    size_t i = 0;
    for (; i < max && it != gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteBgmExpired.end(); ++i, ++it)
    {
        _bgmSampleIdxBuf[i] = (unsigned)it->dvalue;
    }
    SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_bgmSampleIdxBuf.data());

    // also play keysound in auto
    if (gPlayContext.isAuto)
    {
        i = 0;
        auto it = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.size());
        while (i < max2 && it != gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.end())
        {
            if ((it->flags & ~(Note::SCRATCH | Note::KEY_6_7)) == 0)
            {
                _keySampleIdxBuf[i] = (unsigned)it->dvalue;
                ++i;
            }
            ++it;
        }
        SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_keySampleIdxBuf.data());
    }

    // play auto-scratch keysound
    if (gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR)
    {
        i = 0;
        auto it = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.size());
        while (i < max2 && it != gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.end())
        {
            if ((it->flags & (Note::SCRATCH | Note::LN_TAIL)) == Note::SCRATCH)
            {
                _keySampleIdxBuf[i] = (unsigned)it->dvalue;
                ++i;
            }
            ++it;
        }
        SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_keySampleIdxBuf.data());
    }
    if (gPlayContext.isBattle && gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask & PLAY_MOD_ASSIST_AUTOSCR)
    {
        i = 0;
        auto it = gPlayContext.chartObj[PLAYER_SLOT_TARGET]->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + gPlayContext.chartObj[PLAYER_SLOT_TARGET]->noteExpired.size());
        while (i < max2 && it != gPlayContext.chartObj[PLAYER_SLOT_TARGET]->noteExpired.end())
        {
            if ((it->flags & (Note::SCRATCH | Note::LN_TAIL)) == Note::SCRATCH)
            {
                _keySampleIdxBuf[i] = (unsigned)it->dvalue;
                ++i;
            }
            ++it;
        }
        SoundMgr::playNoteSample(SoundChannelType::KEY_RIGHT, i, (size_t*)_keySampleIdxBuf.data());
    }
}

void ScenePlay::changeKeySampleMapping(const Time& t)
{
    static const Time MIN_REMAP_INTERVAL{ 1000 };

    auto changeKeySample = [&](Input::Pad k, int slot)
    {
        chart::NoteLaneIndex idx[3] = { chart::NoteLaneIndex::_, chart::NoteLaneIndex::_, chart::NoteLaneIndex::_ };
        HitableNote* pNote[3] = { nullptr, nullptr, nullptr };
        long long time[3] = { TIMER_NEVER, TIMER_NEVER, TIMER_NEVER };

        idx[0] = gPlayContext.chartObj[slot]->getLaneFromKey(chart::NoteLaneCategory::Note, k);
        if (idx[0] != chart::NoteLaneIndex::_)
        {
            pNote[0] = &*gPlayContext.chartObj[slot]->incomingNote(chart::NoteLaneCategory::Note, idx[0]);
            time[0] = pNote[0]->time.hres();
        }

        idx[1] = gPlayContext.chartObj[slot]->getLaneFromKey(chart::NoteLaneCategory::LN, k);
        if (idx[1] != chart::NoteLaneIndex::_)
        {
            auto itLNNote = gPlayContext.chartObj[slot]->incomingNote(chart::NoteLaneCategory::LN, idx[1]);
            while (!gPlayContext.chartObj[slot]->isLastNote(chart::NoteLaneCategory::LN, idx[1], itLNNote))
            {
                if (!(itLNNote->flags & Note::Flags::LN_TAIL))
                {
                    pNote[1] = &*itLNNote;
                    time[1] = pNote[1]->time.hres();
                    break;
                }
                itLNNote++;
            }
        }

        idx[2] = gPlayContext.chartObj[slot]->getLaneFromKey(chart::NoteLaneCategory::Invs, k);
        if (idx[2] != chart::NoteLaneIndex::_)
        {
            pNote[2] = &*gPlayContext.chartObj[slot]->incomingNote(chart::NoteLaneCategory::Invs, idx[2]);
            time[2] = pNote[2]->time.hres();
        }

        HitableNote* pNoteKey = nullptr;
        std::vector<std::pair<long long, size_t>> sortTmp;
        for (size_t i = 0; i < 3; ++i)
        {
            sortTmp.push_back(std::make_pair(time[i], i));
        }
        std::sort(sortTmp.begin(), sortTmp.end());
        for (size_t i = 0; i < 3; ++i)
        {
            size_t idxNoteKey = sortTmp[i].second;
            if (pNote[idxNoteKey])
            {
                pNoteKey = pNote[idxNoteKey];
                break;
            }
        }

        if (pNoteKey && pNoteKey->time - t <= MIN_REMAP_INTERVAL)
        {
            keySampleIndex[(size_t)k] = (size_t)pNoteKey->dvalue;

            if (k == Input::S1L) keySampleIndex[Input::S1R] = (size_t)pNoteKey->dvalue;
            if (k == Input::S2L) keySampleIndex[Input::S2R] = (size_t)pNoteKey->dvalue;
        }
    };

    assert(gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr);
    for (size_t i = Input::K11; i <= Input::K19; ++i)
    {
        if (_inputAvailable[i])
            changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
    }
    if (!(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
    {
        for (size_t i = Input::S1L; i <= Input::S1R; ++i)
        {
            if (_inputAvailable[i])
                changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
        }
    }
    if (isPlaymodeDP())
    {
        for (size_t i = Input::K21; i <= Input::K29; ++i)
        {
            if (_inputAvailable[i])
                changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
        }
        if (!(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
        {
            for (size_t i = Input::S2L; i <= Input::S2R; ++i)
            {
                if (_inputAvailable[i])
                    changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
            }
        }
    }
	if (gPlayContext.isBattle)
	{
		assert(gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr);
		for (size_t i = Input::K21; i <= Input::K29; ++i)
		{
			if (_inputAvailable[i])
				changeKeySample((Input::Pad)i, PLAYER_SLOT_TARGET);
		}
        if (!(gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
        {
            for (size_t i = Input::S2L; i <= Input::S2R; ++i)
            {
                if (_inputAvailable[i])
                    changeKeySample((Input::Pad)i, PLAYER_SLOT_TARGET);
            }
        }
	}
}

void ScenePlay::spinTurntable(bool startedPlaying)
{
    auto rt = startedPlaying ? Time().norm() - State::get(IndexTimer::PLAY_START) : 0;
    auto angle = rt * 360 / 2000;
    State::set(IndexNumber::_ANGLE_TT_1P, (angle + (int)playerState[0].turntableAngleAdd) % 360);
    State::set(IndexNumber::_ANGLE_TT_2P, (angle + (int)playerState[1].turntableAngleAdd) % 360);
}

void ScenePlay::requestExit()
{
    if (state == ePlayState::FADEOUT || state == ePlayState::WAIT_ARENA)
        return;

    Time t;

    if (gChartContext.started)
    {
        playInterrupted = true;

        if (!playerState[PLAYER_SLOT_PLAYER].finished)
        {
            gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->fail();
            gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->updateGlobals();
            LOG_INFO << "[Play] 1P finished";
        }
        if (!playerState[PLAYER_SLOT_TARGET].finished && gPlayContext.isBattle && gPlayContext.ruleset[PLAYER_SLOT_TARGET])
        {
            gPlayContext.ruleset[PLAYER_SLOT_TARGET]->fail();
            gPlayContext.ruleset[PLAYER_SLOT_TARGET]->updateGlobals();
            LOG_INFO << "[Play] 2P finished";
        }

        if (gArenaData.isOnline())
        {
            if (gArenaData.isClient())
                g_pArenaClient->setPlayingFinished();
            else
                g_pArenaHost->setPlayingFinished();
        }

        if (gPlayContext.replayNew)
        {
            long long ms = t.norm() - State::get(IndexTimer::PLAY_START);
            gPlayContext.replayNew->commands.push_back({ ms, ReplayChart::Commands::Type::ESC, 0 });
        }

        pushGraphPoints();
    }

    if (gArenaData.isOnline())
    {
        State::set(IndexTimer::ARENA_PLAY_WAIT, t.norm());
        state = ePlayState::WAIT_ARENA;
        LOG_DEBUG << "[Play] State changed to WAIT_ARENA";
    }
    else
    { 
        SoundMgr::setNoteVolume(0.0, 1000);
        State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
        State::set(IndexOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        state = ePlayState::FADEOUT;
        LOG_DEBUG << "[Play] State changed to FADEOUT";
    }
}

void ScenePlay::toggleLanecover(int slot, bool state)
{
    IndexSwitch indLanecoverEnabled = IndexSwitch::P1_LANECOVER_ENABLED;
    IndexOption indLanecoverType = IndexOption::PLAY_LANE_EFFECT_TYPE_1P;
    IndexSlider indSliSudden = IndexSlider::SUD_1P;
    IndexNumber indNumSudden = IndexNumber::LANECOVER_TOP_1P;
    IndexSlider indSliHidden = IndexSlider::HID_1P;
    IndexNumber indNumHidden = IndexNumber::LANECOVER_BOTTOM_1P;
    IndexSwitch indLockSpeed = IndexSwitch::P1_LOCK_SPEED;
    if (slot != PLAYER_SLOT_PLAYER)
    {
        indLanecoverEnabled = IndexSwitch::P2_LANECOVER_ENABLED;
        indLanecoverType = IndexOption::PLAY_LANE_EFFECT_TYPE_2P;
        indSliSudden = IndexSlider::SUD_2P;
        indNumSudden = IndexNumber::LANECOVER_TOP_2P;
        indSliHidden = IndexSlider::HID_2P;
        indNumHidden = IndexNumber::LANECOVER_BOTTOM_2P;
        indLockSpeed = IndexSwitch::P2_LOCK_SPEED;
    }

    State::set(indLanecoverEnabled, state);

    Option::e_lane_effect_type lcType = (Option::e_lane_effect_type)State::get(indLanecoverType);
    switch (lcType)
    {
    case Option::LANE_OFF:      lcType = playerState[slot].origLanecoverType == Option::LANE_OFF ? Option::LANE_SUDDEN : playerState[slot].origLanecoverType; break;
    case Option::LANE_HIDDEN:   lcType = Option::LANE_OFF; break;
    case Option::LANE_SUDDEN:   lcType = Option::LANE_OFF; break;
    case Option::LANE_SUDHID:   lcType = Option::LANE_OFF; break;
    case Option::LANE_LIFT:     lcType = Option::LANE_LIFTSUD; break;
    case Option::LANE_LIFTSUD:  lcType = Option::LANE_LIFT; break;
    }
    State::set(indLanecoverType, lcType);

    double sud = State::get(indNumSudden) / 1000.0;
    double hid = State::get(indNumHidden) / 1000.0;

    switch (lcType)
    {
    case Option::LANE_OFF:     sud = 0.; hid = 0.;  break;
    case Option::LANE_HIDDEN:  sud = 0.;            break;
    case Option::LANE_SUDDEN:            hid = 0.;  break;
    case Option::LANE_SUDHID:            hid = sud; break;
    case Option::LANE_LIFT:    sud = 0.;            break;
    case Option::LANE_LIFTSUD:                      break;
    }
    State::set(indSliSudden, sud);
    State::set(indSliHidden, hid);

    if (state && State::get(indLockSpeed) && playerState[slot].lockspeedHispeedBuffered != 0.0)
    {
        gPlayContext.playerState[slot].hispeed = playerState[slot].lockspeedHispeedBuffered;
        double bpm = gPlayContext.mods[slot].hispeedFix == PlayModifierHispeedFixType::CONSTANT ?
            150.0 : gPlayContext.chartObj[slot]->getCurrentBPM();
        const auto [green, val] = calcGreenNumber(bpm, slot, gPlayContext.playerState[slot].hispeed);
        playerState[slot].lockspeedValueInternal = val;
        playerState[slot].lockspeedGreenNumber = green;
        playerState[slot].hispeedHasChanged = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CALLBACK
void ScenePlay::inputGamePress(InputMask& m, const Time& t)
{
    using namespace Input;

    auto input = _inputAvailable & m;

    // individual keys
    if (!gPlayContext.isAuto && !gPlayContext.isReplay)
    {
        inputGamePressTimer(input, t);
        inputGamePressPlayKeysounds(input, t);
    }
    if (gChartContext.started && gPlayContext.replayNew)
    {
        long long ms = t.norm() - State::get(IndexTimer::PLAY_START);
        ReplayChart::Commands cmd;
        cmd.ms = ms;
        for (size_t k = S1L; k < LANE_COUNT; ++k)
        {
            if (!input[k]) continue;

            if (gPlayContext.mode == SkinType::PLAY5 || gPlayContext.mode == SkinType::PLAY5_2)
            {
                if (REPLAY_INPUT_DOWN_CMD_MAP_5K[replayCmdMapIndex].find((Input::Pad)k) != REPLAY_INPUT_DOWN_CMD_MAP_5K[replayCmdMapIndex].end())
                {
                    cmd.type = REPLAY_INPUT_DOWN_CMD_MAP_5K[replayCmdMapIndex].at((Input::Pad)k);
                    gPlayContext.replayNew->commands.push_back(cmd);
                }
            }
            else
            {
                if (REPLAY_INPUT_DOWN_CMD_MAP.find((Input::Pad)k) != REPLAY_INPUT_DOWN_CMD_MAP.end())
                {
                    cmd.type = REPLAY_INPUT_DOWN_CMD_MAP.at((Input::Pad)k);
                    gPlayContext.replayNew->commands.push_back(cmd);
                }
            }
        }
    }

    bool pressedStart[2]{ input[K1START], input[K2START] };
    bool pressedSelect[2]{ input[K1SELECT], input[K2SELECT] };
    bool pressedSpeedUp[2]{ input[K1SPDUP], input[K2SPDDN] };
    bool pressedSpeedDown[2]{ input[K1SPDDN], input[K2SPDDN] };
    std::array<bool, 2> white;
    std::array<bool, 2> black;
    if (!adjustLanecoverWithStart67)
    {
        white = { (input[K11] || input[K13] || input[K15] || input[K17] || input[K19]), (input[K21] || input[K23] || input[K25] || input[K27] || input[K29]) };
        black = { (input[K12] || input[K14] || input[K16] || input[K18]), (input[K22] || input[K24] || input[K26] || input[K28]) };
    }
    else
    {
        white = { (input[K11] || input[K13] || input[K15] || input[K19]), (input[K21] || input[K23] || input[K25] || input[K29]) };
        black = { (input[K12] || input[K14]|| input[K18]), (input[K22] || input[K24] || input[K28]) };
    }
    if (isPlaymodeDP())
    {
        pressedStart[PLAYER_SLOT_PLAYER] |= pressedStart[PLAYER_SLOT_TARGET];
        pressedSelect[PLAYER_SLOT_PLAYER] |= pressedSelect[PLAYER_SLOT_TARGET];
        pressedSpeedUp[PLAYER_SLOT_PLAYER] |= pressedSpeedUp[PLAYER_SLOT_TARGET];
        pressedSpeedDown[PLAYER_SLOT_PLAYER] |= pressedSpeedDown[PLAYER_SLOT_TARGET];
        white[PLAYER_SLOT_PLAYER] |= white[PLAYER_SLOT_TARGET];
        black[PLAYER_SLOT_PLAYER] |= black[PLAYER_SLOT_TARGET];
    }

    // double click START: toggle top lanecover
    auto toggleLanecoverSide = [&](int slot)
    {
        IndexSwitch indLanecoverEnabled = slot == PLAYER_SLOT_PLAYER ? IndexSwitch::P1_LANECOVER_ENABLED : IndexSwitch::P2_LANECOVER_ENABLED;

        if (t > playerState[slot].startPressedTime && (t - playerState[slot].startPressedTime).norm() < 200)
        {
            playerState[slot].startPressedTime = TIMER_NEVER;
            State::set(indLanecoverEnabled, !State::get(indLanecoverEnabled));
            playerState[slot].lanecoverStateHasChanged = true;
        }
        else
        {
            playerState[slot].startPressedTime = t;
        }
    };
    if (pressedStart[PLAYER_SLOT_PLAYER]) 
        toggleLanecoverSide(PLAYER_SLOT_PLAYER);
    if (gPlayContext.isBattle && pressedStart[PLAYER_SLOT_TARGET])
        toggleLanecoverSide(PLAYER_SLOT_TARGET);

    // double click SELECT when lanecover enabled: lock green number
    auto toggleLockspeedSide = [&](int slot)
    {
        IndexSwitch indLockspeedEnabled = slot == PLAYER_SLOT_PLAYER ? IndexSwitch::P1_LOCK_SPEED : IndexSwitch::P2_LOCK_SPEED;

        if (t > playerState[slot].selectPressedTime && (t - playerState[slot].selectPressedTime).norm() < 200)
        {
            State::set(indLockspeedEnabled, !State::get(indLockspeedEnabled));
            playerState[slot].selectPressedTime = TIMER_NEVER;

            if (State::get(indLockspeedEnabled))
            {
                double bpm = gPlayContext.mods[slot].hispeedFix == PlayModifierHispeedFixType::CONSTANT ?
                    150.0 : gPlayContext.chartObj[slot]->getCurrentBPM();
                double hs = gPlayContext.playerState[slot].hispeed;
                const auto [green, val] = calcGreenNumber(bpm, slot, hs);
                playerState[slot].lockspeedValueInternal = val;
                playerState[slot].lockspeedGreenNumber = green;
                playerState[slot].lockspeedHispeedBuffered = hs;
            }
        }
        else
        {
            playerState[slot].selectPressedTime = t;
        }
    };
    if (pressedSelect[PLAYER_SLOT_PLAYER])
        toggleLockspeedSide(PLAYER_SLOT_PLAYER);
    if (gPlayContext.isBattle && pressedSelect[PLAYER_SLOT_TARGET])
        toggleLockspeedSide(PLAYER_SLOT_TARGET);

    // hs adjusted by key
    auto adjustHispeedSide = [&](int slot)
    {
        if (pressedSpeedUp[slot] ||
            ((isHoldingStart(slot) || isHoldingSelect(slot)) && black[slot]) ||
            (slot == PLAYER_SLOT_PLAYER && adjustHispeedWithUpDown && input[UP]))
        {
            if (gPlayContext.playerState[slot].hispeed < hiSpeedMax)
            {
                gPlayContext.playerState[slot].hispeed = std::min(gPlayContext.playerState[slot].hispeed + hiSpeedMargin, hiSpeedMax);
                playerState[slot].hispeedHasChanged = true;
            }
        }

        if (pressedSpeedDown[slot] ||
            ((isHoldingStart(slot) || isHoldingSelect(slot)) && white[slot]) ||
            (slot == PLAYER_SLOT_PLAYER && adjustHispeedWithUpDown && input[DOWN]))
        {
            if (gPlayContext.playerState[slot].hispeed > hiSpeedMinSoft)
            {
                gPlayContext.playerState[slot].hispeed = std::max(gPlayContext.playerState[slot].hispeed - hiSpeedMargin, hiSpeedMinSoft);
                playerState[slot].hispeedHasChanged = true;
            }
        }
    };
    adjustHispeedSide(PLAYER_SLOT_PLAYER);
    if (gPlayContext.isBattle) 
        adjustHispeedSide(PLAYER_SLOT_TARGET);

    // lanecover adjusted by key
    if (State::get(IndexSwitch::P1_LANECOVER_ENABLED) || State::get(IndexOption::PLAY_LANE_EFFECT_TYPE_1P) == Option::LANE_LIFT)
    {
        int lcThreshold = getRate() / 200 * _input.getRate() / 1000;  // updateAsyncLanecover()
        if (adjustLanecoverWithMousewheel)
        {
            if (input[MWHEELUP])
            {
                playerState[PLAYER_SLOT_PLAYER].lanecoverAddPending -= lcThreshold * 10;
                playerState[PLAYER_SLOT_PLAYER].lockspeedResetPending = true;
            }
            if (input[MWHEELDOWN])
            {
                playerState[PLAYER_SLOT_PLAYER].lanecoverAddPending += lcThreshold * 10;
                playerState[PLAYER_SLOT_PLAYER].lockspeedResetPending = true;
            }
        }
        if (adjustLanecoverWithLeftRight)
        {
            if (input[LEFT])
            {
                playerState[PLAYER_SLOT_PLAYER].lanecoverAddPending -= lcThreshold * lanecoverMargin;
                playerState[PLAYER_SLOT_PLAYER].lockspeedResetPending = true;
            }
            if (input[RIGHT])
            {
                playerState[PLAYER_SLOT_PLAYER].lanecoverAddPending += lcThreshold * lanecoverMargin;
                playerState[PLAYER_SLOT_PLAYER].lockspeedResetPending = true;
            }
        }
        if (adjustLanecoverWithStart67)
        {
            auto adjustLanecoverWithStart67Side = [&](int slot, Pad k6, Pad k7)
            {
                if ((isHoldingStart(slot) || isHoldingSelect(slot)))
                {
                    if (input[k6])
                    {
                        playerState[slot].lanecoverAddPending -= lcThreshold * lanecoverMargin;
                        playerState[slot].lockspeedResetPending = true;
                    }
                    if (input[k7])
                    {
                        playerState[slot].lanecoverAddPending += lcThreshold * lanecoverMargin;
                        playerState[slot].lockspeedResetPending = true;
                    }
                }
            };
            adjustLanecoverWithStart67Side(PLAYER_SLOT_PLAYER, K16, K17);
            if (gPlayContext.isBattle)
                adjustLanecoverWithStart67Side(PLAYER_SLOT_TARGET, K26, K27);
            else
                adjustLanecoverWithStart67Side(PLAYER_SLOT_PLAYER, K26, K27);
        }
    }

    holdingStart[0] |= input[K1START];
    holdingSelect[0] |= input[K1SELECT];
    holdingStart[1] |= input[K2START];
    holdingSelect[1] |= input[K2SELECT];

    auto holding = _input.Holding();
    if (state != ePlayState::FADEOUT)
    {
        if (input[Input::F1])
        {
            imguiShowAdjustMenu = !imguiShowAdjustMenu;
        }
        if (!gArenaData.isOnline() || state == ePlayState::PLAYING)
        {
            if (input[Input::ESC])
            {
                if (imguiShowAdjustMenu)
                {
                    imguiShowAdjustMenu = false;
                }
                else
                {
                    isManuallyRequestedExit = true;
                    requestExit();
                }
            }
        }
    }
}

void ScenePlay::inputGamePressTimer(InputMask& input, const Time& t)
{
    using namespace Input;

    if (true)
    {
        if (input[S1L] || input[S1R])
        {
            playerState[PLAYER_SLOT_PLAYER].scratchDirection = input[S1L] ? AxisDir::AXIS_UP : AxisDir::AXIS_DOWN;
            State::set(IndexTimer::S1_DOWN, t.norm());
            State::set(IndexTimer::S1_UP, TIMER_NEVER);
            State::set(IndexSwitch::S1_DOWN, true);
        }
    }
    if (gPlayContext.isBattle || isPlaymodeDP())
    {
        if (input[S2L] || input[S2R])
        {
            playerState[PLAYER_SLOT_TARGET].scratchDirection = input[S2L] ? AxisDir::AXIS_UP : AxisDir::AXIS_DOWN;
            State::set(IndexTimer::S2_DOWN, t.norm());
            State::set(IndexTimer::S2_UP, TIMER_NEVER);
            State::set(IndexSwitch::S2_DOWN, true);
        }
    }
}

void ScenePlay::inputGamePressPlayKeysounds(InputMask inputSample, const Time& t)
{
    using namespace Input;

    // do not play keysounds if player is failed
    if (gPlayContext.isBattle)
    {
        if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isFailed()) inputSample &= ~INPUT_MASK_1P;
        if (gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFailed()) inputSample &= ~INPUT_MASK_2P;
    }

    size_t sampleCount = 0;
    for (size_t i = S1L; i < LANE_COUNT; ++i)
    {
        if (inputSample[i])
        {
            if (keySampleIndex[i])
            {
                _keySampleIdxBuf[sampleCount++] = keySampleIndex[i];
            }
        }
        if (inputSample[i])
        {
            State::set(InputGamePressMap[i].tm, t.norm());
            State::set(InputGameReleaseMap[i].tm, TIMER_NEVER);
            State::set(InputGamePressMap[i].sw, true);
        }
    }
    SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, sampleCount, (size_t*)&_keySampleIdxBuf[0]);
}

// CALLBACK
void ScenePlay::inputGameHold(InputMask& m, const Time& t)
{
    using namespace Input;

    auto input = _inputAvailable & m;

    // delay start
    if (!gChartContext.started && (input[K1START] || input[K1SELECT] || input[K2START] || input[K2SELECT]))
    {
        delayedReadyTime = t;
    }

    auto ttUpdateSide = [&](int slot, Pad ttUp, Pad ttDn)
    {
        bool lanecover = State::get(slot == PLAYER_SLOT_PLAYER ? IndexOption::PLAY_LANE_EFFECT_TYPE_1P : IndexOption::PLAY_LANE_EFFECT_TYPE_2P) != Option::LANE_OFF;
        bool fnLanecover = isHoldingStart(slot) || !adjustHispeedWithSelect && isHoldingSelect(slot);
        bool fnHispeed = adjustHispeedWithSelect && isHoldingSelect(slot);

        int val = 0;
        if (input[ttUp]) val--;  // -1 per tick
        if (input[ttDn]) val++;  // +1 per tick

        // turntable spin
        playerState[slot].turntableAngleAdd += val * 0.25;

        if (lanecover && fnLanecover)
        {
            playerState[slot].lanecoverAddPending += val;
            playerState[slot].lockspeedResetPending |= val != 0;
        }
        else if (!lanecover && fnLanecover || fnHispeed)
        {
            playerState[slot].hispeedAddPending += val;
        }
    };
    ttUpdateSide(PLAYER_SLOT_PLAYER, S1L, S1R);
    if (isPlaymodeDP())
        ttUpdateSide(PLAYER_SLOT_PLAYER, S2L, S2R);
    else if (gPlayContext.isBattle)
        ttUpdateSide(PLAYER_SLOT_TARGET, S2L, S2R);
}

// CALLBACK
void ScenePlay::inputGameRelease(InputMask& m, const Time& t)
{
    using namespace Input;
    auto input = _inputAvailable & m;

    if (!gPlayContext.isAuto && !gPlayContext.isReplay)
    {
        inputGameReleaseTimer(input, t);
    }
    
    if (gChartContext.started && gPlayContext.replayNew)
    {
        long long ms = t.norm() - State::get(IndexTimer::PLAY_START);
        ReplayChart::Commands cmd;
        cmd.ms = ms;
        for (size_t k = S1L; k < LANE_COUNT; ++k)
        {
            if (!input[k]) continue;

            if (gPlayContext.mode == SkinType::PLAY5 || gPlayContext.mode == SkinType::PLAY5_2)
            {
                if (REPLAY_INPUT_UP_CMD_MAP_5K[replayCmdMapIndex].find((Input::Pad)k) != REPLAY_INPUT_UP_CMD_MAP_5K[replayCmdMapIndex].end())
                {
                    cmd.type = REPLAY_INPUT_UP_CMD_MAP_5K[replayCmdMapIndex].at((Input::Pad)k);
                    gPlayContext.replayNew->commands.push_back(cmd);
                }
            }
            else
            {
                if (REPLAY_INPUT_UP_CMD_MAP.find((Input::Pad)k) != REPLAY_INPUT_UP_CMD_MAP.end())
                {
                    cmd.type = REPLAY_INPUT_UP_CMD_MAP.at((Input::Pad)k);
                    gPlayContext.replayNew->commands.push_back(cmd);
                }
            }
        }
    }

    holdingStart[0] &= !input[K1START];
    holdingSelect[0] &= !input[K1SELECT];
    holdingStart[1] &= !input[K2START];
    holdingSelect[1] &= !input[K2SELECT];
}

void ScenePlay::inputGameReleaseTimer(InputMask& input, const Time& t)
{
    using namespace Input;

    size_t count = 0;
    for (size_t i = Input::S1L; i < Input::LANE_COUNT; ++i)
        if (input[i])
        {
            State::set(InputGamePressMap[i].tm, TIMER_NEVER);
            State::set(InputGameReleaseMap[i].tm, t.norm());
            State::set(InputGameReleaseMap[i].sw, false);

            // TODO stop sample playing while release in LN notes
        }

    if (true)
    {
        if (input[S1L] || input[S1R])
        {
            if ((input[S1L] && playerState[PLAYER_SLOT_PLAYER].scratchDirection == AxisDir::AXIS_UP) ||
                (input[S1R] && playerState[PLAYER_SLOT_PLAYER].scratchDirection == AxisDir::AXIS_DOWN))
            {
                State::set(IndexTimer::S1_DOWN, TIMER_NEVER);
                State::set(IndexTimer::S1_UP, t.norm());
                State::set(IndexSwitch::S1_DOWN, false);
                playerState[PLAYER_SLOT_PLAYER].scratchDirection = AxisDir::AXIS_NONE;
            }
        }
    }
    if (gPlayContext.isBattle || isPlaymodeDP())
    {
        if (input[S2L] || input[S2R])
        {
            if ((input[S2L] && playerState[PLAYER_SLOT_TARGET].scratchDirection == AxisDir::AXIS_UP) ||
                (input[S2R] && playerState[PLAYER_SLOT_TARGET].scratchDirection == AxisDir::AXIS_DOWN))
            {
                State::set(IndexTimer::S2_DOWN, TIMER_NEVER);
                State::set(IndexTimer::S2_UP, t.norm());
                State::set(IndexSwitch::S2_DOWN, false);
                playerState[PLAYER_SLOT_TARGET].scratchDirection = AxisDir::AXIS_NONE;
            }
        }
    }
}

// CALLBACK
void ScenePlay::inputGameAxis(double S1, double S2, const Time& t)
{
	using namespace Input;

    // turntable spin
    playerState[PLAYER_SLOT_PLAYER].turntableAngleAdd += S1 * 2.0 * 360;
    playerState[PLAYER_SLOT_TARGET].turntableAngleAdd += S2 * 2.0 * 360;

	if (!gPlayContext.isAuto && (!gPlayContext.isReplay || !gChartContext.started))
	{
		auto ttUpdateSide = [&](int slot, double S)
		{
			bool lanecover = State::get(slot == PLAYER_SLOT_PLAYER ? IndexOption::PLAY_LANE_EFFECT_TYPE_1P : IndexOption::PLAY_LANE_EFFECT_TYPE_2P) != Option::LANE_OFF;
			bool fnLanecover = isHoldingStart(slot) || !adjustHispeedWithSelect && isHoldingSelect(slot);
			bool fnHispeed = adjustHispeedWithSelect && isHoldingSelect(slot);

			playerState[slot].scratchAccumulator += S;

			double lanecoverThreshold = 0.0002;

			int val = (int)std::round(S2 / lanecoverThreshold);
			if (lanecover && fnLanecover)
			{
				playerState[slot].lanecoverAddPending += val;
				playerState[slot].lockspeedResetPending |= val != 0;
			}
			else if (!lanecover && fnLanecover || fnHispeed)
			{
				playerState[slot].hispeedAddPending += val;
			}

		};
		if (isPlaymodeDP())
			ttUpdateSide(PLAYER_SLOT_PLAYER, S2);
		else if (gPlayContext.isBattle)
			ttUpdateSide(PLAYER_SLOT_TARGET, S2);
		else
			ttUpdateSide(PLAYER_SLOT_PLAYER, S1 + S2);
	}
}