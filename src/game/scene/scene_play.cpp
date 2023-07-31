#include "common/pch.h"
#include "scene_play.h"
#include "game/sound/sound_mgr.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_auto.h"
#include "game/ruleset/ruleset_bms_replay.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/chart/chart_bms.h"
#include "game/graphics/sprite_video.h"
#include "config/config_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/skin/skin_lr2_button_callbacks.h"
#include "game/skin/skin_lr2_slider_callbacks.h"
#include "game/data/data_types.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

namespace lunaticvibes
{

bool ScenePlay::isPlaymodeDP() const
{
    switch (PlayData.mode)
    {
        case SkinType::PLAY10:
        case SkinType::PLAY14:
            return true;
    }
    return false;
}

bool isBattle()
{
    return PlayData.battleType != PlayModifierBattleType::Off;
}

bool isCourse()
{
    return PlayData.courseStage >= 0;
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

bool isPlayerEnabledLanecover(int slot)
{
    switch (PlayData.player[slot].mods.laneEffect)
    {
    case PlayModifierLaneEffectType::SUDDEN:
    case PlayModifierLaneEffectType::HIDDEN:
    case PlayModifierLaneEffectType::SUDHID:
    case PlayModifierLaneEffectType::LIFTSUD:
        return true;
    }
    return false;
}

int getLanecoverTop(int slot)
{
    switch (PlayData.player[slot].mods.laneEffect)
    {
    case PlayModifierLaneEffectType::SUDDEN:
    case PlayModifierLaneEffectType::SUDHID:
    case PlayModifierLaneEffectType::LIFTSUD:
        return PlayData.player[slot].lanecoverTop;
    }
    return 0;
}
int getLanecoverBottom(int slot)
{
    switch (PlayData.player[slot].mods.laneEffect)
    {
    case PlayModifierLaneEffectType::HIDDEN:
    case PlayModifierLaneEffectType::SUDHID:
    case PlayModifierLaneEffectType::LIFT:
    case PlayModifierLaneEffectType::LIFTSUD:
        return PlayData.player[slot].lanecoverBottom;
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

bool isPlayerLockSpeed(int slot)
{
    // FIXME load P2 hispeedFix type
    return PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix != PlayModifierHispeedFixType::NONE;
}

//////////////////////////////////////////////////////////////////////////////////////////////

ScenePlay::ScenePlay(): SceneBase(PlayData.mode, 1000, true)
{
    _type = SceneType::PLAY;
    state = ePlayState::PREPARE;

    assert(!isPlaymodeDP() || !isBattle());

    // 2P inputs => 1P
    if (!isPlaymodeDP() && !isBattle())
    {
        _input.setMergeInput();
    }
    _inputAvailable = INPUT_MASK_FUNC;
    _inputAvailable |= INPUT_MASK_1P | INPUT_MASK_2P;

    PlayData.loadHasFinished = false;

    // which lanes should we use for 5K, 1-5 or 3-7? 
    PlayData.shift1PNotes5KFor7KSkin = false;
    PlayData.shift2PNotes5KFor7KSkin = false;
    if ((pSkin->info.mode == SkinType::PLAY7 || pSkin->info.mode == SkinType::PLAY7_2 || pSkin->info.mode == SkinType::PLAY14) &&
        (PlayData.mode == SkinType::PLAY5 || PlayData.mode == SkinType::PLAY10))
    {
        PlayData.shift1PNotes5KFor7KSkin = (pSkin->info.scratchSide1P == 1);
        PlayData.shift2PNotes5KFor7KSkin = (pSkin->info.scratchSide2P == 1);

        if (PlayData.shift1PNotes5KFor7KSkin)
        {
            replayCmdMapIndex = PlayData.shift2PNotes5KFor7KSkin ? 3 : 2;
        }
        else
        {
            replayCmdMapIndex = PlayData.shift2PNotes5KFor7KSkin ? 1 : 0;
        }
    }

    // ? 
    keySampleIndex.assign(Input::ESC, 0);

    // replay pitch
    if (PlayData.isReplay && PlayData.replay)
    {
        SystemData.freqType = FreqModifierType::Off;
        if (PlayData.replay->pitchValue != 0)
        {
            switch (PlayData.replay->pitchType)
            {
            case 0: SystemData.freqType = FreqModifierType::Frequency; break;
            case 1: SystemData.freqType = FreqModifierType::PitchOnly; break;
            case 2: SystemData.freqType = FreqModifierType::SpeedOnly; break;
            }
            SystemData.freqVal = PlayData.replay->pitchValue;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    // chart initialize, get basic info
    if (SelectData.selectedChart.chart == nullptr || !SelectData.selectedChart.chart->isLoaded())
    {
        if (SelectData.selectedChart.path.empty())
        {
            LOG_ERROR << "[Play] Chart not specified!";
            SystemData.gNextScene = SystemData.quitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
            return;
        }
        if (ArenaData.isOnline())
        {
            PlayData.randomSeed = ArenaData.getRandomSeed();
        }
        else if (PlayData.replay && 
            PlayData.isReplay || (PlayData.replay && PlayData.battleType == PlayModifierBattleType::GhostBattle))
        {
            PlayData.randomSeed = PlayData.replay->randomSeed;
        }
        SelectData.selectedChart.chart = ChartFormatBase::createFromFile(SelectData.selectedChart.path, PlayData.randomSeed);
    }
    if (SelectData.selectedChart.chart == nullptr || !SelectData.selectedChart.chart->isLoaded())
    {
        LOG_ERROR << "[Play] Invalid chart: " << SelectData.selectedChart.path.u8string();
        SystemData.gNextScene = SystemData.quitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
        return;
    }

    LOG_DEBUG << "[Play] " << SelectData.selectedChart.chart->title << " " << SelectData.selectedChart.chart->title2 << " [" << SelectData.selectedChart.chart->version << "]";
    LOG_DEBUG << "[Play] MD5: " << SelectData.selectedChart.chart->fileHash.hexdigest();
    LOG_DEBUG << "[Play] Mode: " << SelectData.selectedChart.chart->gamemode;
    LOG_DEBUG << "[Play] BPM: " << SelectData.selectedChart.chart->startBPM
        << " (" << SelectData.selectedChart.chart->minBPM
        << " - " << SelectData.selectedChart.chart->maxBPM << ")";

    if (PlayData.replayMybest)
    {
        SelectData.selectedChart.chartMybest = ChartFormatBase::createFromFile(SelectData.selectedChart.path, PlayData.replayMybest->randomSeed);
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    lr2skin::button::target_type(0);

    SelectData.selectedChart.title = SelectData.selectedChart.chart->title;
    SelectData.selectedChart.title2 = SelectData.selectedChart.chart->title2;
    SelectData.selectedChart.artist = SelectData.selectedChart.chart->artist;
    SelectData.selectedChart.artist2 = SelectData.selectedChart.chart->artist2;
    SelectData.selectedChart.genre = SelectData.selectedChart.chart->genre;
    SelectData.selectedChart.minBPM = SelectData.selectedChart.chart->minBPM;
    SelectData.selectedChart.startBPM = SelectData.selectedChart.chart->startBPM;
    SelectData.selectedChart.maxBPM = SelectData.selectedChart.chart->maxBPM;

    // chartobj
    chartObjLoaded = createChartObj();

    LOG_DEBUG << "[Play] Real BPM: " << PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getCurrentBPM()
        << " (" << PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getPlayMinBPM()
        << " - " << PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getPlayMaxBPM()
        << ") / Avg: " << PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getAverageBPM()
        << " / Main: " << PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getMainBPM();
    LOG_DEBUG << "[Play] Notes: " << PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getNoteTotalCount();
    LOG_DEBUG << "[Play] Length: " << PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getTotalLength().norm() / 1000;

    //////////////////////////////////////////////////////////////////////////////////////////

    // set gauge type, health value
    if (SelectData.selectedChart.chart)
    {
        switch (SelectData.selectedChart.chart->type())
        {
        case eChartFormat::BMS:
        case eChartFormat::BMSON:
            setInitialHealthBMS();
            break;
        default:
            break;
        }
    }
    playerState[PLAYER_SLOT_PLAYER].healthLastTick = 
        PlayData.player[PLAYER_SLOT_PLAYER].ruleset ? PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getData().health : 0.;
    playerState[PLAYER_SLOT_TARGET].healthLastTick = 
        PlayData.player[PLAYER_SLOT_TARGET].ruleset ? PlayData.player[PLAYER_SLOT_TARGET].ruleset->getData().health : 0.;

    auto initDisplayGaugeType = [&](int slot)
    {
        using namespace std::string_literals;
        GaugeDisplayType tmp = GaugeDisplayType::GROOVE;
        switch (PlayData.player[slot].mods.gauge)
        {
        case PlayModifierGaugeType::NORMAL:       tmp = GaugeDisplayType::GROOVE; break;
        case PlayModifierGaugeType::HARD:         tmp = GaugeDisplayType::SURVIVAL; break;
        case PlayModifierGaugeType::DEATH:        tmp = GaugeDisplayType::EX_SURVIVAL; break;
        case PlayModifierGaugeType::EASY:         tmp = GaugeDisplayType::GROOVE; break;
            // case PlayModifierGaugeType::PATTACK:      tmp = GaugeDisplayType::EX_SURVIVAL; break;
            // case PlayModifierGaugeType::GATTACK:      tmp = GaugeDisplayType::EX_SURVIVAL; break;
        case PlayModifierGaugeType::ASSISTEASY:   tmp = GaugeDisplayType::ASSIST_EASY; break;
        case PlayModifierGaugeType::EXHARD:       tmp = GaugeDisplayType::EX_SURVIVAL; break;
        case PlayModifierGaugeType::CLASS_NORMAL: tmp = GaugeDisplayType::SURVIVAL; break;
        case PlayModifierGaugeType::CLASS_HARD:   tmp = GaugeDisplayType::EX_SURVIVAL; break;
        case PlayModifierGaugeType::CLASS_DEATH:  tmp = GaugeDisplayType::EX_SURVIVAL; break;
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
    if (isBattle()) initDisplayGaugeType(PLAYER_SLOT_TARGET);

    //////////////////////////////////////////////////////////////////////////////////////////

    // ruleset, should be called after initial health set
    rulesetLoaded = createRuleset();

    if (rulesetLoaded && ArenaData.isOnline())
    {
        if (ArenaData.isClient())
            g_pArenaClient->setCreatedRuleset();
        else
            g_pArenaHost->setCreatedRuleset();
    }

    // course: skip play scene if already failed
    if (isCourse() && PlayData.player[PLAYER_SLOT_PLAYER].initialHealth <= 0.)
    {
        if (PlayData.courseStage < PlayData.courseStageData.size())
        {
            PlayData.courseStageData[PlayData.courseStage].replayPathNew = "";

            if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset)
            {
                PlayData.player[PLAYER_SLOT_PLAYER].ruleset->fail();
                PlayData.courseStageData[PlayData.courseStage].rulesetCopy[PLAYER_SLOT_PLAYER] = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
            }
            if (PlayData.player[PLAYER_SLOT_TARGET].ruleset)
            {
                PlayData.player[PLAYER_SLOT_TARGET].ruleset->fail();
                PlayData.courseStageData[PlayData.courseStage].rulesetCopy[PLAYER_SLOT_TARGET] = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
            }

            // do not draw anything
            pSkin.reset();

            ++PlayData.courseStage;
            SystemData.gNextScene = SceneType::COURSE_TRANS;
            return;
        }
        else
        {
            SystemData.gNextScene = SceneType::COURSE_RESULT;
            return;
        }
    }

    if (isCourse())
    {
        LOG_DEBUG << "[Play] Course stage " << PlayData.courseStage;
        LOG_DEBUG << "[Play] Running combo: " << PlayData.player[PLAYER_SLOT_PLAYER].courseRunningCombo << " / " << PlayData.player[PLAYER_SLOT_TARGET].courseRunningCombo;
        LOG_DEBUG << "[Play] Health: " << PlayData.player[PLAYER_SLOT_PLAYER].initialHealth << " / " << PlayData.player[PLAYER_SLOT_TARGET].initialHealth;
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    // Lanecover
    auto initLanecover = [&](int slot)
    {
        int lcTop = 0;
        int lcBottom = 0;
        int lc100 = 0;
        double sudden = 0.;
        double hidden = 0.;
        if (slot == PLAYER_SLOT_PLAYER && PlayData.isReplay && PlayData.replay)
        {
            playerState[PLAYER_SLOT_PLAYER].origLanecoverType = (PlayModifierLaneEffectType)PlayData.replay->laneEffectType;
            lcTop = PlayData.replay->lanecoverTop;
            lcBottom = PlayData.replay->lanecoverBottom;
            lc100 = lcTop / 10;
            sudden = lcTop / 1000.0;
            hidden = lcBottom / 1000.0;
        }
        else if (slot == PLAYER_SLOT_TARGET && isPlaymodeDP())
        {
            playerState[PLAYER_SLOT_TARGET].origLanecoverType = playerState[PLAYER_SLOT_PLAYER].origLanecoverType;
            lcTop = PlayData.player[slot].lanecoverTop;
            lcBottom = PlayData.player[slot].lanecoverBottom;
            lc100 = lcTop / 10;
            sudden = lcTop / 1000.0;
            hidden = lcBottom / 1000.0;
        }
        else
        {
            playerState[slot].origLanecoverType = PlayData.player[slot].mods.laneEffect;

            sudden = PlayData.player[slot].lanecoverTop / 1000.0;
            hidden = PlayData.player[slot].lanecoverBottom / 1000.0;
            switch (playerState[slot].origLanecoverType)
            {
            case PlayModifierLaneEffectType::OFF:
                sudden = 0.;
                hidden = 0.;
                break;

            case PlayModifierLaneEffectType::SUDDEN:
                hidden = 0.;
                break;

            case PlayModifierLaneEffectType::HIDDEN:
                sudden = 0.;
                break;

            case PlayModifierLaneEffectType::SUDHID:
                hidden = sudden;
                break;

            case PlayModifierLaneEffectType::LIFT:
                sudden = 0.;
                break;
            }
            lcTop = int(sudden * 1000);
            lcBottom = int(hidden * 1000);
            lc100 = lcTop / 10;
        }
    };
    initLanecover(PLAYER_SLOT_PLAYER);
    initLanecover(PLAYER_SLOT_TARGET);

    //////////////////////////////////////////////////////////////////////////////////////////

    // Hispeed
    playerState[PLAYER_SLOT_PLAYER].savedHispeed = PlayData.player[PLAYER_SLOT_PLAYER].hispeed;
    playerState[PLAYER_SLOT_TARGET].savedHispeed = PlayData.player[PLAYER_SLOT_PLAYER].hispeed;

    // FIXME load P2 hispeedFix type
    playerState[PLAYER_SLOT_PLAYER].origLockspeedType = PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix;
    playerState[PLAYER_SLOT_TARGET].origLockspeedType = PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix;

    // replay hispeed
    if (PlayData.isReplay && PlayData.replay)
    {
        PlayData.player[PLAYER_SLOT_PLAYER].hispeed = PlayData.replay->hispeed;
    }

    auto initLockHispeed = [&](int slot)
    {
        double* pHispeed = &PlayData.player[PLAYER_SLOT_PLAYER].hispeed;
        if (slot == PLAYER_SLOT_TARGET)
        {
            pHispeed = &PlayData.player[PLAYER_SLOT_PLAYER].hispeed;
        }

        double bpm = SelectData.selectedChart.startBPM * SystemData.pitchSpeed;
        switch (PlayData.player[slot].mods.hispeedFix)
        {
        case PlayModifierHispeedFixType::MAXBPM:   bpm = PlayData.player[slot].chartObj->getPlayMaxBPM(); break;
        case PlayModifierHispeedFixType::MINBPM:   bpm = PlayData.player[slot].chartObj->getPlayMinBPM(); break;
        case PlayModifierHispeedFixType::AVERAGE:  bpm = PlayData.player[slot].chartObj->getAverageBPM(); break;
        case PlayModifierHispeedFixType::CONSTANT: bpm = 150.0; break;
        case PlayModifierHispeedFixType::MAIN:     bpm = PlayData.player[slot].chartObj->getMainBPM(); break;
        case PlayModifierHispeedFixType::INITIAL:
        case PlayModifierHispeedFixType::NONE:
        default:               bpm = PlayData.player[slot].chartObj->getCurrentBPM(); break;
        }

        int green = PlayData.player[slot].greenNumber;
        const auto [hs, val] = calcHiSpeed(bpm, slot, green);

        *pHispeed = hs;
        playerState[slot].lockspeedValueInternal = val;
        playerState[slot].lockspeedGreenNumber = green;
        playerState[slot].lockspeedHispeedBuffered = hs;
    };
    if (isPlayerLockSpeed(PLAYER_SLOT_PLAYER))
    {
        initLockHispeed(PLAYER_SLOT_PLAYER);
    }
    if (isBattle() && isPlayerLockSpeed(PLAYER_SLOT_TARGET))
    {
        initLockHispeed(PLAYER_SLOT_TARGET);
    }
    PlayData.player[PLAYER_SLOT_PLAYER].hispeedGradientStart = TIMER_NEVER;
    PlayData.player[PLAYER_SLOT_PLAYER].hispeedGradientFrom = PlayData.player[PLAYER_SLOT_PLAYER].hispeedGradientNow;
    PlayData.player[PLAYER_SLOT_PLAYER].hispeedGradientNow = PlayData.player[PLAYER_SLOT_PLAYER].hispeed;
    PlayData.player[PLAYER_SLOT_PLAYER].hispeedGradientStart = TIMER_NEVER;
    PlayData.player[PLAYER_SLOT_PLAYER].hispeedGradientFrom = PlayData.player[PLAYER_SLOT_PLAYER].hispeedGradientNow;
    PlayData.player[PLAYER_SLOT_PLAYER].hispeedGradientNow = PlayData.player[PLAYER_SLOT_PLAYER].hispeed;

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

    PlayData.timers["ready"] = TIMER_NEVER;
    PlayData.timers["play_start"] = TIMER_NEVER;
    PlayData.timers["beat"] = TIMER_NEVER;
    SoundMgr::setSysVolume(1.0);
    SoundMgr::setNoteVolume(1.0);
}

bool ScenePlay::createChartObj()
{
    //load chart object from Chart object
    switch (SelectData.selectedChart.chart->type())
    {
    case eChartFormat::BMS:
    {
        auto bms = std::reinterpret_pointer_cast<ChartFormatBMS>(SelectData.selectedChart.chart);

        PlayData.player[PLAYER_SLOT_PLAYER].chartObj = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);
        PlayData.player[PLAYER_SLOT_PLAYER].chartObj->setVisualOffset(PlayData.player[PLAYER_SLOT_PLAYER].offsetVisual);

        if (isBattle())
        {
            PlayData.player[PLAYER_SLOT_TARGET].chartObj = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_TARGET, bms);
            PlayData.player[PLAYER_SLOT_TARGET].chartObj->setVisualOffset(PlayData.player[PLAYER_SLOT_TARGET].offsetVisual);
        }
        else
        {
            // create for rival; loading with 1P options
            PlayData.player[PLAYER_SLOT_TARGET].chartObj = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);
            PlayData.player[PLAYER_SLOT_TARGET].chartObj->setVisualOffset(PlayData.player[PLAYER_SLOT_PLAYER].offsetVisual);
        }

        if (PlayData.replayMybest)
        {
            PlayData.player[PLAYER_SLOT_MYBEST].chartObj = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_MYBEST, bms);
            PlayData.player[PLAYER_SLOT_MYBEST].chartObj->setVisualOffset(PlayData.player[PLAYER_SLOT_MYBEST].offsetVisual);
        }

        if (PlayData.isReplay && (!isBattle() || PlayData.battleType == PlayModifierBattleType::GhostBattle))
            itReplayCommand = PlayData.replay->commands.begin();

        return true;
    }

    case eChartFormat::BMSON:
    default:
        LOG_WARNING << "[Play] chart format not supported.";
        SystemData.gNextScene = SystemData.quitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
        return false;
    }

}

bool ScenePlay::createRuleset()
{
    // build Ruleset object
    //switch (PlayData.rulesetType)
    //{
    //case RulesetType::BMS:
    {
        // set judge diff
        RulesetBMS::JudgeDifficulty judgeDiff;
        switch (SelectData.selectedChart.chart->type())
        {
        case eChartFormat::BMS:
            if (int rank = 0; SelectData.selectedChart.chart->getExtendedProperty("RANK", &rank))
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
        switch (PlayData.mode)
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

        //if (!LR2CustomizeData.isInCustomize)
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
                if (PlayData.isAuto)
                {
                    objType = AUTO;
                    playSide = doublePlay ? RulesetBMS::PlaySide::AUTO_DOUBLE : RulesetBMS::PlaySide::AUTO;
                }
                else if (PlayData.isReplay)
                {
                    objType = REPLAY;
                    playSide = doublePlay ? RulesetBMS::PlaySide::AUTO_DOUBLE : RulesetBMS::PlaySide::AUTO;
                }
                else if (isBattle())
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
                if (PlayData.isAuto)
                {
                    if (isBattle())
                    {
                        objType = PlayData.replayMybest ? REPLAY : AUTO;
                        playSide = RulesetBMS::PlaySide::AUTO_2P;
                    }
                }
                else if (isBattle())
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
                if (!PlayData.isAuto && !isBattle() && PlayData.replayMybest)
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
                SelectData.selectedChart.chart, PlayData.player[slot].chartObj,
                PlayData.player[slot].mods.gauge, keys, judgeDiff,
                PlayData.player[slot].initialHealth, playSide);

            case AUTO:
                return std::make_shared<RulesetBMSAuto>(
                    SelectData.selectedChart.chart, PlayData.player[slot].chartObj,
                    PlayData.player[slot].mods.gauge, keys, judgeDiff,
                    PlayData.player[slot].initialHealth, playSide);

            case REPLAY:
                assert(PlayData.replayMybest != nullptr);
                return std::make_shared<RulesetBMSReplay>(
                    SelectData.selectedChart.chartMybest, PlayData.player[slot].chartObj, PlayData.replayMybest,
                    PlayData.replayMybest->gaugeType, keys, judgeDiff,
                    PlayData.player[slot].initialHealth, playSide);

            default:
                return nullptr;
            }
        };
        PlayData.player[PLAYER_SLOT_PLAYER].ruleset = rulesetFactoryFunc(PLAYER_SLOT_PLAYER);
        PlayData.player[PLAYER_SLOT_TARGET].ruleset = rulesetFactoryFunc(PLAYER_SLOT_TARGET);
        PlayData.player[PLAYER_SLOT_MYBEST].ruleset = rulesetFactoryFunc(PLAYER_SLOT_MYBEST);

        if (isCourse())
        {
            PlayData.player[PLAYER_SLOT_PLAYER].ruleset->setComboDisplay(PlayData.player[PLAYER_SLOT_PLAYER].courseRunningCombo);
            PlayData.player[PLAYER_SLOT_PLAYER].ruleset->setMaxComboDisplay(PlayData.player[PLAYER_SLOT_PLAYER].courseMaxCombo);
            if (PlayData.player[PLAYER_SLOT_TARGET].ruleset)
            {
                PlayData.player[PLAYER_SLOT_TARGET].ruleset->setComboDisplay(PlayData.player[PLAYER_SLOT_TARGET].courseRunningCombo);
                PlayData.player[PLAYER_SLOT_TARGET].ruleset->setMaxComboDisplay(PlayData.player[PLAYER_SLOT_TARGET].courseMaxCombo);
            }
        }

        if (!PlayData.isAuto && !PlayData.isReplay &&
            (!isBattle() || (PlayData.replay && PlayData.battleType == PlayModifierBattleType::GhostBattle)))
        {
            // create replay
            PlayData.replayNew = std::make_shared<ReplayChart>();
            PlayData.replayNew->chartHash = SelectData.selectedChart.hash;
            PlayData.replayNew->randomSeed = PlayData.randomSeed;
            PlayData.replayNew->gaugeType = PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge;
            PlayData.replayNew->randomTypeLeft = PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft;
            PlayData.replayNew->randomTypeRight = PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight;
            PlayData.replayNew->assistMask = PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask;
            PlayData.replayNew->hispeedFix = PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix;
            PlayData.replayNew->laneEffectType = (int8_t)PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect;
            if (SystemData.freqType != FreqModifierType::Off)
            {
                PlayData.replayNew->pitchType = (int8_t)SystemData.freqType - 1;    // -1 for compatibility
                PlayData.replayNew->pitchValue = (int8_t)SystemData.freqVal;
            }
            PlayData.replayNew->DPFlip = PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip;
        }

        if (PlayData.player[PLAYER_SLOT_TARGET].ruleset != nullptr && !isBattle())
        {
            double targetRateReal = 0.0;
            switch (PlayData.targetType)
            {
            case TargetType::RankAAA: targetRateReal = 8.0 / 9; break;
            case TargetType::RankAA:  targetRateReal = 7.0 / 9; break;
            case TargetType::RankA:   targetRateReal = 6.0 / 9; break;
            default:
            {
                // convert rank border to decimal rate values (88.88%, etc.)
                int targetRate = PlayData.targetRate;
                switch (targetRate)
                {
                case 22:  targetRateReal = 2.0 / 9; break;
                case 33:  targetRateReal = 3.0 / 9; break;
                case 44:  targetRateReal = 4.0 / 9; break;
                case 55:  targetRateReal = 5.0 / 9; break;
                case 66:  targetRateReal = 6.0 / 9; break;
                case 77:  targetRateReal = 7.0 / 9; break;
                case 88:  targetRateReal = 8.0 / 9; break;
                case 100: targetRateReal = 1.0;     break;
                default:  targetRateReal = targetRate / 100.0; break;
                }
            }
            break;
            }

            std::dynamic_pointer_cast<RulesetBMSAuto>(PlayData.player[PLAYER_SLOT_TARGET].ruleset)->setTargetRate(targetRateReal);
        }

        return true;
    }
    //break;

    //default:
    //    break;
    //}

    return false;
}

ScenePlay::~ScenePlay()
{
    PlayData.bgaTexture->stopUpdate();
    PlayData.bgaTexture->reset();

    _input.loopEnd();
    loopEnd();
}

void ScenePlay::setInitialHealthBMS()
{
    // FIXME this function was intended to display gauge type correctly before ruleset initializes
    if (!isCourse() || PlayData.courseStage == 0)
    {
        auto initHealth = [&](int slot)
        {
            PlayModifierGaugeType gaugeType = PlayModifierGaugeType::NORMAL;
            bool setInd = false;
            switch (slot)
            {
            case PLAYER_SLOT_PLAYER: 
                gaugeType = PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge; 
                setInd = true; 
                break;

            case PLAYER_SLOT_TARGET: 
                gaugeType = PlayData.player[PLAYER_SLOT_TARGET].mods.gauge; 
                setInd = true; 
                break;

            case PLAYER_SLOT_MYBEST: 
                gaugeType = PlayData.replayMybest->gaugeType; 
                setInd = false;
                break;
            }

            switch (gaugeType)
            {
            case PlayModifierGaugeType::NORMAL:
            case PlayModifierGaugeType::EASY:
            case PlayModifierGaugeType::ASSISTEASY:
                PlayData.player[slot].initialHealth = 0.2;
                break;

            case PlayModifierGaugeType::HARD:
            case PlayModifierGaugeType::DEATH:
                // case PlayModifierGaugeType::PATTACK:
                // case PlayModifierGaugeType::GATTACK:
            case PlayModifierGaugeType::EXHARD:
            case PlayModifierGaugeType::CLASS_NORMAL:
            case PlayModifierGaugeType::CLASS_HARD:
            case PlayModifierGaugeType::CLASS_DEATH:
                PlayData.player[slot].initialHealth = 1.0;
                break;

            default: break;
            }
        };
        initHealth(PLAYER_SLOT_PLAYER);
        if (isBattle()) initHealth(PLAYER_SLOT_TARGET);
        if (PlayData.replayMybest) initHealth(PLAYER_SLOT_MYBEST);
    }
    else
    {
        /*
        State::set(IndexNumber::PLAY_1P_GROOVEGAUGE, (int)(PlayData.player[PLAYER_SLOT_PLAYER].initialHealth * 100));

        if (isBattle())
        {
            State::set(IndexNumber::PLAY_2P_GROOVEGAUGE, (int)(PlayData.player[PLAYER_SLOT_TARGET].initialHealth * 100));
        }
        */
    }
}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::loadChart()
{
    if (!SelectData.selectedChart.chart) return;

    // always reload unstable resources
    if (!SelectData.selectedChart.chart->resourceStable)
    {
        SelectData.selectedChart.isSampleLoaded = false;
        SelectData.selectedChart.sampleLoadedHash.reset();
        SelectData.selectedChart.isBgaLoaded = false;
        SelectData.selectedChart.bgaLoadedHash.reset();
    }

    // load samples
    if ((!SelectData.selectedChart.isSampleLoaded || SelectData.selectedChart.hash != SelectData.selectedChart.sampleLoadedHash) && !sceneEnding)
    {
        auto dtor = std::async(std::launch::async, [&]() {
            SetDebugThreadName("Chart sound sample loading thread");
            SoundMgr::freeNoteSamples();

            auto _pChart = SelectData.selectedChart.chart;
            auto chartDir = SelectData.selectedChart.chart->getDirectory();
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
                SelectData.selectedChart.isSampleLoaded = true;
                SelectData.selectedChart.sampleLoadedHash = SelectData.selectedChart.hash;
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
                SelectData.selectedChart.isSampleLoaded = true;
                SelectData.selectedChart.sampleLoadedHash = SelectData.selectedChart.hash;
            }
        });
    }
    else
    {
        SelectData.selectedChart.isSampleLoaded = true;
    }

    // load bga
    if ((PlayData.panelStyle & PANEL_STYLE_BGA_MASK) != PANEL_STYLE_BGA_OFF && !sceneEnding)
    {
        if (!SelectData.selectedChart.isBgaLoaded)
        {
            auto dtor = std::async(std::launch::async, [&]() {
                SetDebugThreadName("Chart BGA loading thread");
                PlayData.bgaTexture->clear();

                auto _pChart = SelectData.selectedChart.chart;
                auto chartDir = SelectData.selectedChart.chart->getDirectory();
                for (const auto& it : _pChart->bgaFiles)
                {
                    if (sceneEnding) return;
                    if (it.empty()) continue;
                    ++bmpTotal;
                }
                if (bmpTotal == 0)
                {
                    bmpLoaded = 1;
                    SelectData.selectedChart.isBgaLoaded = true;
                    SelectData.selectedChart.bgaLoadedHash = SelectData.selectedChart.hash;
                    return;
                }

                std::list<std::pair<size_t, Path>> mapBgaFiles;
                auto loadBgaFiles = [&]
                {
                    for (auto& [i, pBmp] : mapBgaFiles)
                    {
                        if (pBmp.is_absolute())
                            PlayData.bgaTexture->addBmp(i, pBmp);
                        else
                            PlayData.bgaTexture->addBmp(i, chartDir / pBmp);
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
                        PlayData.bgaTexture->setLoaded();
                    }
                    PlayData.bgaTexture->setSlotFromBMS(*std::reinterpret_pointer_cast<ChartObjectBMS>(PlayData.player[PLAYER_SLOT_PLAYER].chartObj));
                    SelectData.selectedChart.isBgaLoaded = true;
                    SelectData.selectedChart.bgaLoadedHash = SelectData.selectedChart.hash;
                }
                });
        }
        else
        {
            // set playback speed on each play
            PlayData.bgaTexture->setVideoSpeed();
        }
    }
}

void ScenePlay::setInputJudgeCallback()
{
    using namespace std::placeholders;
    if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset != nullptr)
    {
        auto fp = std::bind(&RulesetBase::updatePress, PlayData.player[PLAYER_SLOT_PLAYER].ruleset, _1, _2);
        _input.register_p("JUDGE_PRESS_1", fp);
        auto fh = std::bind(&RulesetBase::updateHold, PlayData.player[PLAYER_SLOT_PLAYER].ruleset, _1, _2);
        _input.register_h("JUDGE_HOLD_1", fh);
        auto fr = std::bind(&RulesetBase::updateRelease, PlayData.player[PLAYER_SLOT_PLAYER].ruleset, _1, _2);
        _input.register_r("JUDGE_RELEASE_1", fr);
        auto fa = std::bind(&RulesetBase::updateAxis, PlayData.player[PLAYER_SLOT_PLAYER].ruleset, _1, _2, _3);
        _input.register_a("JUDGE_AXIS_1", fa);
    }
    else
    {
        LOG_ERROR << "[Play] Ruleset of 1P not initialized!";
    }

    if (PlayData.player[PLAYER_SLOT_TARGET].ruleset != nullptr)
    {
        auto fp = std::bind(&RulesetBase::updatePress, PlayData.player[PLAYER_SLOT_TARGET].ruleset, _1, _2);
        _input.register_p("JUDGE_PRESS_2", fp);
        auto fh = std::bind(&RulesetBase::updateHold, PlayData.player[PLAYER_SLOT_TARGET].ruleset, _1, _2);
        _input.register_h("JUDGE_HOLD_2", fh);
        auto fr = std::bind(&RulesetBase::updateRelease, PlayData.player[PLAYER_SLOT_TARGET].ruleset, _1, _2);
        _input.register_r("JUDGE_RELEASE_2", fr);
        auto fa = std::bind(&RulesetBase::updateAxis, PlayData.player[PLAYER_SLOT_TARGET].ruleset, _1, _2, _3);
        _input.register_a("JUDGE_AXIS_2", fa);
    }
    else if (!PlayData.isAuto)
    {
        LOG_ERROR << "[Play] Ruleset of 2P not initialized!";
    }
}

void ScenePlay::removeInputJudgeCallback()
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

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::_updateAsync()
{
    if (SystemData.gNextScene != SceneType::PLAY) return;

    if (SystemData.isAppExiting)
    {
        SystemData.gNextScene = SceneType::EXIT_TRANS;
    }

    Time t;

    // update lanecover / hispeed change
    updateAsyncLanecover(t);

    // health + timer, reset per 2%
    updateAsyncGaugeUpTimer(t);

    // absolute axis scratch
    updateAsyncAbsoluteAxis(t);

    // record 
    if (PlayData.playStarted && PlayData.replayNew)
    {
        long long ms = t.norm() - PlayData.timers["play_start"];
        if (playerState[PLAYER_SLOT_PLAYER].hispeedHasChanged)
        {
            PlayData.replayNew->commands.push_back({ int64_t(ms), ReplayChart::Commands::Type::HISPEED, PlayData.player[PLAYER_SLOT_PLAYER].hispeed });
        }
        if (playerState[PLAYER_SLOT_PLAYER].lanecoverTopHasChanged)
        {
            PlayData.replayNew->commands.push_back({ int64_t(ms), ReplayChart::Commands::Type::LANECOVER_TOP, (double)PlayData.player[PLAYER_SLOT_PLAYER].lanecoverTop });
        }
        if (playerState[PLAYER_SLOT_PLAYER].lanecoverBottomHasChanged)
        {
            PlayData.replayNew->commands.push_back({ int64_t(ms), ReplayChart::Commands::Type::LANECOVER_BOTTOM, (double)PlayData.player[PLAYER_SLOT_PLAYER].lanecoverBottom });
        }
        if (playerState[PLAYER_SLOT_PLAYER].lanecoverStateHasChanged)
        {
            PlayData.replayNew->commands.push_back({ int64_t(ms), ReplayChart::Commands::Type::LANECOVER_ENABLE, 
                (double)isPlayerEnabledLanecover(PLAYER_SLOT_PLAYER) });
        }
    }

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

    for (int slot = PLAYER_SLOT_PLAYER; slot <= PLAYER_SLOT_TARGET; slot++)
    {
        if (playerState[slot].hispeedHasChanged)
        {
            playerState[slot].hispeedHasChanged = false;
        }
        if (playerState[slot].lanecoverTopHasChanged)
        {
            playerState[slot].lanecoverTopHasChanged = false;
        }
        if (playerState[slot].lanecoverBottomHasChanged)
        {
            playerState[slot].lanecoverBottomHasChanged = false;
        }
        if (playerState[slot].lanecoverStateHasChanged)
        {
            playerState[slot].lanecoverStateHasChanged = false;
        }
    }

    // state based callback
    switch (state)
    {
    case ePlayState::PREPARE:
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

    if (ArenaData.isOnline() && ArenaData.isExpired())
    {
        ArenaData.reset();
    }
}

void ScenePlay::updateAsyncLanecover(const Time& t)
{
    int lcThreshold = getRate() / 200 * _input.getRate() / 1000;  // lanecover, +200 per second
    int hsThreshold = getRate() / 25 * _input.getRate() / 1000;   // hispeed, +25 per second

    auto handleSide = [&](int slot)
    {
        auto lanecoverType = PlayData.player[slot].mods.laneEffect;
        bool adjustTop = false; // true: top / false: bottom

        int lc = 0;
        switch (lanecoverType)
        {
        case PlayModifierLaneEffectType::SUDDEN:
        case PlayModifierLaneEffectType::SUDHID:
        case PlayModifierLaneEffectType::LIFTSUD:
            adjustTop = true;
            break;
        case PlayModifierLaneEffectType::HIDDEN:
        case PlayModifierLaneEffectType::LIFT:
            adjustTop = false;
            break;
        }
        lc = adjustTop ? PlayData.player[slot].lanecoverTop : PlayData.player[slot].lanecoverBottom;

        bool lcHasChanged = false;
        int lcOld = lc;
        bool inverted = !adjustTop;
        int units = playerState[slot].lanecoverAddPending > 0 ? (playerState[slot].lanecoverAddPending / lcThreshold) : -(-playerState[slot].lanecoverAddPending / lcThreshold);
        if (units != 0)
        {
            playerState[slot].lanecoverAddPending -= units * lcThreshold;
            lc = std::clamp(lc + (inverted ? -units : units), 0, 1000);
            if (lanecoverType == PlayModifierLaneEffectType::SUDHID && lc > 500) lc = 500;
            lcHasChanged = true;
        }

        if (lcHasChanged)
        {
            if (adjustTop)
            {
                PlayData.player[slot].lanecoverTop = lc;
                playerState[slot].lanecoverTopHasChanged = true;
            }
            if (!adjustTop || lanecoverType == PlayModifierLaneEffectType::SUDHID)
            {
                PlayData.player[slot].lanecoverBottom = lc;
                playerState[slot].lanecoverBottomHasChanged = true;
            }
        }

        bool lockSpeedEnabled = isPlayerLockSpeed(slot);
        double *pHispeed = slot == PLAYER_SLOT_PLAYER ? &PlayData.player[PLAYER_SLOT_PLAYER].hispeed : &PlayData.player[PLAYER_SLOT_PLAYER].hispeed;
        if (playerState[slot].lockspeedResetPending)
        {
            playerState[slot].lockspeedResetPending = false;
            if (lockSpeedEnabled)
            {
                double bpm = PlayData.player[slot].mods.hispeedFix == PlayModifierHispeedFixType::CONSTANT ?
                    150.0 : PlayData.player[slot].chartObj->getCurrentBPM();
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
                double bpm = PlayData.player[slot].mods.hispeedFix == PlayModifierHispeedFixType::CONSTANT ?
                    150.0 : PlayData.player[slot].chartObj->getCurrentBPM();
                const auto [green, val] = calcGreenNumber(bpm, slot, hs);
                playerState[slot].lockspeedValueInternal = val;
                playerState[slot].lockspeedGreenNumber = green;
                playerState[slot].lockspeedHispeedBuffered = hs;
            }

            playerState[slot].hispeedHasChanged = true;
        }


    };
    handleSide(PLAYER_SLOT_PLAYER);
    if (isBattle()) handleSide(PLAYER_SLOT_TARGET);
}

void ScenePlay::updateAsyncGreenNumber(const Time& t)
{
    // 120BPM with 1.0x HS is 2000ms (500ms/beat, green number 1200)
    auto updateSide = [&](int slot)
    {
        int noteLaneHeight = pSkin->info.noteLaneHeight1P;
        if (slot != PLAYER_SLOT_PLAYER)
        {
            noteLaneHeight = pSkin->info.noteLaneHeight2P;
        }
        if (noteLaneHeight != 0 && PlayData.player[slot].chartObj != nullptr)
        {
            double bpm, minBPM, maxBPM;
            if (PlayData.player[slot].mods.hispeedFix != PlayModifierHispeedFixType::CONSTANT)
            {
                bpm = PlayData.player[slot].chartObj->getCurrentBPM();
                minBPM = SelectData.selectedChart.minBPM;
                maxBPM = SelectData.selectedChart.maxBPM;
            }
            else
            {
                bpm = minBPM = maxBPM = 150.0;
            }
            PlayData.player[slot].greenNumber = calcGreenNumber(bpm, slot, PlayData.player[slot].hispeed).first;
            PlayData.player[slot].greenNumberMaxBPM = calcGreenNumber(maxBPM, slot, PlayData.player[slot].hispeed).first;
            PlayData.player[slot].greenNumberMinBPM = calcGreenNumber(minBPM, slot, PlayData.player[slot].hispeed).first;

            // setting speed / lanecover (if display white number / green number)
            PlayData.player[slot].adjustingHispeed = isHoldingStart(slot) || isHoldingSelect(slot);
            PlayData.player[slot].adjustingLanecover = isPlayerEnabledLanecover(slot) && PlayData.player[slot].adjustingHispeed;
        }
    };
    updateSide(PLAYER_SLOT_PLAYER);
    updateSide(PLAYER_SLOT_TARGET);

    // show greennumber on top-left for unsupported skins
    if (!pSkin->isSupportGreenNumber)
    {
        std::stringstream ss;
        if (holdingStart[0] || holdingSelect[0] || holdingStart[1] || holdingSelect[1])
        {
            ss << "G(1P): " << (isPlayerLockSpeed(PLAYER_SLOT_PLAYER) ? "FIX " : "") << PlayData.player[PLAYER_SLOT_PLAYER].greenNumber <<
                " (" << PlayData.player[PLAYER_SLOT_PLAYER].greenNumberMinBPM << " - " << PlayData.player[PLAYER_SLOT_PLAYER].greenNumberMaxBPM << ")";

            if (isBattle())
            {
                ss << " | G(2P): " << (isPlayerLockSpeed(PLAYER_SLOT_TARGET) ? "FIX " : "") << PlayData.player[PLAYER_SLOT_TARGET].greenNumber <<
                    " (" << PlayData.player[PLAYER_SLOT_TARGET].greenNumberMinBPM << " - " << PlayData.player[PLAYER_SLOT_TARGET].greenNumberMaxBPM << ")";
            }
        }
        SystemData.overlayTopLeftText[0] = ss.str();
    }
}

void ScenePlay::updateAsyncGaugeUpTimer(const Time& t)
{
    auto updateSide = [&](int slot)
    {
        int health = int(100.0 * (PlayData.player[slot].ruleset ? PlayData.player[slot].ruleset->getData().health : 0.));
        if (playerState[slot].healthLastTick / 2 != health / 2)
        {
            if (health == 100)
            {
                PlayData.timers[slot == PLAYER_SLOT_PLAYER ? "gauge_up_1p" : "gauge_up_2p"] = TIMER_NEVER;
                PlayData.timers[slot == PLAYER_SLOT_PLAYER ? "gauge_max_1p" : "gauge_max_2p"] = t.norm();
            }
            else if (health > playerState[slot].healthLastTick)
            {
                PlayData.timers[slot == PLAYER_SLOT_PLAYER ? "gauge_up_1p" : "gauge_up_2p"] = t.norm();
                PlayData.timers[slot == PLAYER_SLOT_PLAYER ? "gauge_max_1p" : "gauge_max_2p"] = TIMER_NEVER;
            }
            else
            {
                PlayData.timers[slot == PLAYER_SLOT_PLAYER ? "gauge_max_1p" : "gauge_max_2p"] = TIMER_NEVER;
            }
        }
        playerState[slot].healthLastTick = health;
    };
    updateSide(PLAYER_SLOT_PLAYER);
    updateSide(PLAYER_SLOT_TARGET);
}

void ScenePlay::updateAsyncHSGradient(const Time& t)
{
    const long long HS_GRADIENT_LENGTH_MS = 200;
    for (int slot = PLAYER_SLOT_PLAYER; slot <= PLAYER_SLOT_TARGET; slot++)
    {
        if (playerState[slot].hispeedHasChanged)
        {
            PlayData.player[slot].hispeedGradientStart = t;
            PlayData.player[slot].hispeedGradientFrom = PlayData.player[slot].hispeedGradientNow;
        }

        if (PlayData.player[slot].hispeedGradientStart != TIMER_NEVER)
        {
            Time hsGradient = t - PlayData.player[slot].hispeedGradientStart;
            if (hsGradient.norm() < HS_GRADIENT_LENGTH_MS)
            {
                double prog = std::sin((hsGradient.norm() / (double)HS_GRADIENT_LENGTH_MS) * 1.57079632679);
                PlayData.player[slot].hispeedGradientNow = PlayData.player[slot].hispeedGradientFrom +
                    prog * (PlayData.player[slot].hispeed - PlayData.player[slot].hispeedGradientFrom);
            }
            else
            {
                PlayData.player[slot].hispeedGradientNow = PlayData.player[slot].hispeed;
                PlayData.player[slot].hispeedGradientStart = TIMER_NEVER;
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
                    SystemData.timers["key_on_s_1p"] = t.norm();
                    SystemData.timers["key_off_s_1p"] = TIMER_NEVER;
                    SystemData.timers["key_on_sr_1p"] = t.norm();
                    SystemData.timers["key_off_sr_1p"] = TIMER_NEVER;
                }
                else
                {
                    SystemData.timers["key_on_s_2p"] = t.norm();
                    SystemData.timers["key_off_s_2p"] = TIMER_NEVER;
                    SystemData.timers["key_on_sr_2p"] = t.norm();
                    SystemData.timers["key_off_sr_2p"] = TIMER_NEVER;
                }
                playerState[slot].scratchLastUpdate = t.norm();
            }
            else if (axisDir == AxisDir::AXIS_UP)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    SystemData.timers["key_on_s_1p"] = t.norm();
                    SystemData.timers["key_off_s_1p"] = TIMER_NEVER;
                    SystemData.timers["key_on_sl_1p"] = t.norm();
                    SystemData.timers["key_off_sl_1p"] = TIMER_NEVER;
                }
                else
                {
                    SystemData.timers["key_on_s_2p"] = t.norm();
                    SystemData.timers["key_off_s_2p"] = TIMER_NEVER;
                    SystemData.timers["key_on_sl_2p"] = t.norm();
                    SystemData.timers["key_off_sl_2p"] = TIMER_NEVER;
                }
                playerState[slot].scratchLastUpdate = t.norm();
            }
        }

        // push replay command
        if (PlayData.playStarted && PlayData.replayNew)
        {
            long long ms = t.norm() - PlayData.timers["play_start"];
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
                PlayData.replayNew->commands.push_back(cmd);
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
                        PlayData.replayNew->commands.push_back(cmd);
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
                        PlayData.replayNew->commands.push_back(cmd);
                    }
                }
            }
        }

        if (axisDir != AxisDir::AXIS_NONE && playerState[slot].scratchDirection != axisDir)
        {
            std::array<size_t, 4> keySampleIdxBufScratch;
            size_t sampleCount = 0;

            if (slot == PLAYER_SLOT_PLAYER && !PlayData.player[PLAYER_SLOT_PLAYER].ruleset->isFailed())
            {
                if (playerState[slot].scratchDirection == AxisDir::AXIS_UP && keySampleIndex[Input::S1L])
                    keySampleIdxBufScratch[sampleCount++] = keySampleIndex[Input::S1L];
                if (playerState[slot].scratchDirection == AxisDir::AXIS_DOWN && keySampleIndex[Input::S1R])
                    keySampleIdxBufScratch[sampleCount++] = keySampleIndex[Input::S1R];

                SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, sampleCount, keySampleIdxBufScratch.data());
            }
            if (slot != PLAYER_SLOT_PLAYER && (isPlaymodeDP() || (isBattle() && !PlayData.player[PLAYER_SLOT_TARGET].ruleset->isFailed())))
            {
                if (playerState[slot].scratchDirection == AxisDir::AXIS_UP && keySampleIndex[Input::S2L])
                    keySampleIdxBufScratch[sampleCount++] = keySampleIndex[Input::S2L];
                if (playerState[slot].scratchDirection == AxisDir::AXIS_DOWN && keySampleIndex[Input::S2R])
                    keySampleIdxBufScratch[sampleCount++] = keySampleIndex[Input::S2R];

                SoundMgr::playNoteSample((!isBattle() ? SoundChannelType::KEY_LEFT : SoundChannelType::KEY_RIGHT), sampleCount, keySampleIdxBufScratch.data());
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
                    SystemData.timers["key_on_s_1p"] = TIMER_NEVER;
                    SystemData.timers["key_off_s_1p"] = t.norm();
                    SystemData.timers["key_on_sl_1p"] = TIMER_NEVER;
                    SystemData.timers["key_off_sl_1p"] = t.norm();
                    SystemData.timers["key_on_sr_1p"] = TIMER_NEVER;
                    SystemData.timers["key_off_sr_1p"] = t.norm();
                }
                else
                {
                    SystemData.timers["key_on_s_2p"] = TIMER_NEVER;
                    SystemData.timers["key_off_s_2p"] = t.norm();
                    SystemData.timers["key_on_sl_2p"] = TIMER_NEVER;
                    SystemData.timers["key_off_sl_2p"] = t.norm();
                    SystemData.timers["key_on_sr_2p"] = TIMER_NEVER;
                    SystemData.timers["key_off_sr_2p"] = t.norm();
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
    auto rt = t - SystemData.timers["scene_start"];
    if (rt.norm() > pSkin->info.timeIntro)
    {
        PlayData.timers["load_start"] = t.norm();
		_loadChartFuture = std::async(std::launch::async, std::bind(&ScenePlay::loadChart, this));
        state = ePlayState::LOADING;
        LOG_DEBUG << "[Play] State changed to LOADING";
    }
}

void ScenePlay::updateLoading()
{
	auto t = Time();
    auto rt = t - PlayData.timers["load_start"];

    PlayData.loadProgressWav = getWavLoadProgress();
    PlayData.loadProgressBga = getBgaLoadProgress();

    if (chartObjLoaded && 
        rulesetLoaded &&
        SelectData.selectedChart.isSampleLoaded && 
        ((PlayData.panelStyle & PANEL_STYLE_BGA_OFF) == PANEL_STYLE_BGA_OFF || SelectData.selectedChart.isBgaLoaded) &&
        (t - delayedReadyTime) > 1000 &&
		rt > pSkin->info.timeMinimumLoad)
    {
        bool trans = true;
        if (ArenaData.isOnline())
        {
            trans = ArenaData.isPlaying();
            if (!ArenaData.isPlaying())
            {
                if (ArenaData.isClient())
                    g_pArenaClient->setLoadingFinished(pSkin->info.timeGetReady);
                else
                    g_pArenaHost->setLoadingFinished(pSkin->info.timeGetReady);
            }
            else
            {
                pSkin->info.timeGetReady = ArenaData.getPlayStartTimeMs();
            }
        }
        if (trans)
        {
            if (PlayData.bgaTexture) PlayData.bgaTexture->reset();
            PlayData.loadHasFinished = true;
            PlayData.timers["ready"] = t.norm();
            state = ePlayState::LOAD_END;
            LOG_DEBUG << "[Play] State changed to READY";
        }
    }
}

void ScenePlay::updateLoadEnd()
{
	auto t = Time();
    auto rt = t - PlayData.timers["ready"];
    spinTurntable(false);
    if (rt > pSkin->info.timeGetReady)
    {
        changeKeySampleMapping(0);
        PlayData.timers["play_start"] = t.norm();
        setInputJudgeCallback();
		PlayData.playStarted = true;
        if (PlayData.replayNew)
        {
            PlayData.replayNew->hispeed = PlayData.player[PLAYER_SLOT_PLAYER].hispeed;
            PlayData.replayNew->lanecoverTop = PlayData.player[PLAYER_SLOT_PLAYER].lanecoverTop;
            PlayData.replayNew->lanecoverBottom = PlayData.player[PLAYER_SLOT_PLAYER].lanecoverBottom;
            PlayData.replayNew->lanecoverEnabled = isPlayerEnabledLanecover(PLAYER_SLOT_PLAYER);
        }
        state = ePlayState::PLAYING;
        LOG_DEBUG << "[Play] State changed to PLAY_START";
    }
}

void ScenePlay::updatePlaying()
{
	auto t = Time();
	auto rt = t - PlayData.timers["play_start"];

    assert(PlayData.player[PLAYER_SLOT_PLAYER].ruleset != nullptr);
    {
        PlayData.player[PLAYER_SLOT_PLAYER].chartObj->update(rt);
        PlayData.player[PLAYER_SLOT_PLAYER].ruleset->update(t);
    }
    if (PlayData.player[PLAYER_SLOT_MYBEST].ruleset != nullptr)
    {
        PlayData.player[PLAYER_SLOT_MYBEST].chartObj->update(rt);
        PlayData.player[PLAYER_SLOT_MYBEST].ruleset->update(t);
    }
    if (PlayData.player[PLAYER_SLOT_TARGET].ruleset != nullptr)
    {
        PlayData.player[PLAYER_SLOT_TARGET].chartObj->update(rt);
        PlayData.player[PLAYER_SLOT_TARGET].ruleset->update(t);
    }

    // update replay key timers and lanecover values
    if (PlayData.isReplay ||
        (isBattle() && PlayData.battleType == PlayModifierBattleType::GhostBattle))
    {
        int slot = !isBattle() ? PLAYER_SLOT_PLAYER : PLAYER_SLOT_TARGET;
        InputMask prev = replayKeyPressing;
        while (itReplayCommand != PlayData.replay->commands.end() && rt.norm() >= itReplayCommand->ms)
        {
            auto cmd = itReplayCommand->type;
            if (isBattle())
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

            if (PlayData.mode == SkinType::PLAY5 || PlayData.mode == SkinType::PLAY5_2)
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
                PlayData.player[slot].hispeed = itReplayCommand->value;
                playerState[slot].hispeedHasChanged = true;
                break;

            case ReplayChart::Commands::Type::LANECOVER_TOP: 
                PlayData.player[slot].lanecoverTop = itReplayCommand->value;
                playerState[slot].lanecoverTopHasChanged = true;
                break;

            case ReplayChart::Commands::Type::LANECOVER_BOTTOM:
                PlayData.player[slot].lanecoverBottom = itReplayCommand->value;
                playerState[slot].lanecoverBottomHasChanged = true;
                break;

            case ReplayChart::Commands::Type::LANECOVER_ENABLE:
                toggleLanecover(slot, (bool)(int)itReplayCommand->value);
                playerState[slot].lanecoverStateHasChanged = true;
                break;
            }

            // do not accept replay-requested ESC in battle mode
            if (PlayData.isReplay && cmd == ReplayChart::Commands::Type::ESC)
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
        auto dp1 = PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getData();

        if (auto pr = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset); pr)
        {
            miss1 = pr->getJudgeCountEx(RulesetBMS::JUDGE_BP);
        }

        auto targetType = PlayData.targetType;

        if (PlayData.player[PLAYER_SLOT_TARGET].ruleset != nullptr)
        {
            if (auto pr2 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset); pr2)
            {
                miss2 = pr2->getJudgeCountEx(RulesetBMS::JUDGE_BP);
            }
        }
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
    PlayData.bgaTexture->update(rt, t.norm() - poorBgaStartTime.norm() < poorBgaDuration);

    // play time / remain time
    updatePlayTime(rt);

    // play bgm lanes
    procCommonNotes();

    // update keysound bindings
    changeKeySampleMapping(rt);

    // graphs
    if (rt.norm() / 500 >= PlayData.player[PLAYER_SLOT_PLAYER].graphGauge.size())
    {
        auto& g = PlayData.player[PLAYER_SLOT_PLAYER].graphGauge;
        auto& r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
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

        PlayData.pushGraphPoints();
    }

    // health check (-> to failed)
    if (!playInterrupted)
    {
        if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset->isFailed() && PlayData.player[PLAYER_SLOT_PLAYER].ruleset->failWhenNoHealth() &&
            (!isBattle() || PlayData.player[PLAYER_SLOT_TARGET].ruleset == nullptr || PlayData.player[PLAYER_SLOT_TARGET].ruleset->isFailed() && PlayData.player[PLAYER_SLOT_TARGET].ruleset->failWhenNoHealth()))
        {
            PlayData.pushGraphPoints();

            playInterrupted = true;
            if (ArenaData.isOnline())
            {
                ArenaData.timers["play_finish_wait"] = t.norm();
                state = ePlayState::WAIT_ARENA;
                LOG_DEBUG << "[Play] State changed to WAIT_ARENA";
            }
            else
            {
                PlayData.timers["fail"] = t.norm();
                state = ePlayState::FAILED;
                SoundMgr::stopSysSamples();
                SoundMgr::stopNoteSamples();
                SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_PLAYSTOP);
                LOG_DEBUG << "[Play] State changed to PLAY_FAILED";
            }
            _input.unregister_p("SCENE_PRESS");
        }
    }

    // finish check
    auto finishCheckSide = [&](int slot)
    {
        if (!playerState[slot].finished)
        {
            bool fullCombo = PlayData.player[slot].ruleset->getData().combo == PlayData.player[slot].ruleset->getMaxCombo();
            if (PlayData.player[slot].ruleset->isFinished() || fullCombo)
            {
                PlayData.timers[slot == PLAYER_SLOT_PLAYER ? "last_note_1p" : "last_note_2p"] = t.norm();

                if (fullCombo && !isManuallyRequestedExit && !isReplayRequestedExit)
                {
                    PlayData.timers[slot == PLAYER_SLOT_PLAYER ? "fullcombo_1p" : "fullcombo_2p"] = t.norm();
                }

                playerState[slot].finished = true;

                LOG_INFO << "[Play] " << slot + 1 << "P finished";
            }
        }
    };
    finishCheckSide(PLAYER_SLOT_PLAYER);
    if (PlayData.player[PLAYER_SLOT_TARGET].ruleset) finishCheckSide(PLAYER_SLOT_TARGET);

    if (isPlaymodeDP())
    {
        PlayData.timers["last_note_2p"] = t.norm();

        bool fullCombo = PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getData().combo == PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getMaxCombo();
        if (fullCombo && !isManuallyRequestedExit && !isReplayRequestedExit)
        {
            PlayData.timers["fullcombo_2p"] = t.norm();
        }
    }

    if (ArenaData.isOnline())
    {
        if (ArenaData.isClient())
            g_pArenaClient->setPlayingFinished();
        else
            g_pArenaHost->setPlayingFinished();
    }

    playFinished = !playInterrupted && playerState[PLAYER_SLOT_PLAYER].finished && (!isBattle() || playerState[PLAYER_SLOT_TARGET].finished);

    // 
    spinTurntable(true);

    // outro check
    if (rt.hres() - PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getTotalLength().hres() >= 0)
    {
        if (ArenaData.isOnline())
        {
            ArenaData.timers["play_finish_wait"] = t.norm();
            state = ePlayState::WAIT_ARENA;
            LOG_DEBUG << "[Play] State changed to WAIT_ARENA";
        }
        else
        {
            SystemData.timers["fadeout_start"] = t.norm();
            state = ePlayState::FADEOUT;
            LOG_DEBUG << "[Play] State changed to FADEOUT";
        }
    }
}

void ScenePlay::updateFadeout()
{
    auto t = Time();
    auto rt = t - PlayData.timers["play_start"];
    auto ft = t - SystemData.timers["fadeout_start"];

    if (PlayData.player[PLAYER_SLOT_PLAYER].chartObj != nullptr)
    {
        PlayData.player[PLAYER_SLOT_PLAYER].chartObj->update(rt);
    }
    if (PlayData.player[PLAYER_SLOT_TARGET].chartObj != nullptr)
    {
        PlayData.player[PLAYER_SLOT_TARGET].chartObj->update(rt);
    }
    if (PlayData.player[PLAYER_SLOT_MYBEST].chartObj != nullptr)
    {
        PlayData.player[PLAYER_SLOT_MYBEST].chartObj->update(rt);
    }

    if (PlayData.playStarted)
    {
        PlayData.bgaTexture->update(rt, false);
    }

    // play time / remain time
    updatePlayTime(rt);

    //
    spinTurntable(PlayData.playStarted);

    if (ft >= pSkin->info.timeOutro)
    {
        sceneEnding = true;
        if (_loadChartFuture.valid())
            _loadChartFuture.wait();

        removeInputJudgeCallback();

        bool cleared = false;
        if (isBattle())
        {
            if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset->isCleared() ||
                PlayData.player[PLAYER_SLOT_TARGET].ruleset->isCleared())
                cleared = true;
        }
        else
        {
            if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset->isCleared())
                cleared = true;
        }
        ResultData.cleared = cleared;

        // restore hispeed if FHS
        if (isPlayerLockSpeed(PLAYER_SLOT_PLAYER))
        {
            PlayData.player[PLAYER_SLOT_PLAYER].hispeed = playerState[PLAYER_SLOT_PLAYER].savedHispeed;
        }
        if (isBattle())
        {
            if (isPlayerLockSpeed(PLAYER_SLOT_TARGET))
            {
                PlayData.player[PLAYER_SLOT_TARGET].hispeed = playerState[PLAYER_SLOT_TARGET].savedHispeed;
            }
        }

        // save lanecover settings
        if (!PlayData.isReplay)
        {
            auto saveLanecoverSide = [&](int slot)
            {
				const char* cfgLaneEffect = cfg::P_LANE_EFFECT_OP;
				if (slot != PLAYER_SLOT_PLAYER)
				{
					cfgLaneEffect = cfg::P_LANE_EFFECT_OP_2P;
				}

				bool saveTop = false;
				bool saveBottom = false;
				switch (PlayData.player[slot].mods.laneEffect)
				{
				case PlayModifierLaneEffectType::SUDDEN:
				case PlayModifierLaneEffectType::SUDHID:
				case PlayModifierLaneEffectType::LIFTSUD:
					saveTop = true;
					break;
				}
				switch (PlayData.player[slot].mods.laneEffect)
				{
				case PlayModifierLaneEffectType::HIDDEN:
				case PlayModifierLaneEffectType::SUDHID:
				case PlayModifierLaneEffectType::LIFT:
				case PlayModifierLaneEffectType::LIFTSUD:
					saveBottom = true;
					break;
				}
				if (saveTop)
					ConfigMgr::set('P', cfg::P_LANECOVER_TOP, PlayData.player[slot].lanecoverTop);
				if (saveBottom)
					ConfigMgr::set('P', cfg::P_LANECOVER_BOTTOM, PlayData.player[slot].lanecoverBottom);

				if (isPlayerLockSpeed(slot))
				{
					ConfigMgr::set('P', cfg::P_GREENNUMBER, playerState[slot].lockspeedGreenNumber);

					if (ConfigMgr::get('P', cfg::P_SPEED_TYPE, cfg::P_SPEED_TYPE_NORMAL) == cfg::P_SPEED_TYPE_NORMAL)
					{
						ConfigMgr::set('P', cfg::P_SPEED_TYPE, cfg::P_SPEED_TYPE_INITIAL);
					}
				}

				// only save OFF -> SUD+
				if (playerState[slot].origLanecoverType == PlayModifierLaneEffectType::OFF && isPlayerEnabledLanecover(slot))
				{
					ConfigMgr::set('P', slot == PLAYER_SLOT_PLAYER ? cfg::P_LANE_EFFECT_OP : cfg::P_LANE_EFFECT_OP_2P, cfg::P_LANE_EFFECT_OP_SUDDEN);
				}
            };
            saveLanecoverSide(PLAYER_SLOT_PLAYER);
            if (isBattle()) 
                saveLanecoverSide(PLAYER_SLOT_TARGET);
        }

        // reset BGA
        PlayData.bgaTexture->reset();

        // check and set next scene
        SystemData.gNextScene = SceneType::SELECT;

        // check quick retry (start+select / white+black)
        bool wantRetry = false;
        bool wantNewRandomSeed = false;
        if (PlayData.canRetry && PlayData.playStarted && playInterrupted && !playFinished)
        {
            auto h = _input.Holding();
            using namespace Input;

            bool ss = isHoldingStart(PLAYER_SLOT_PLAYER) && isHoldingSelect(PLAYER_SLOT_PLAYER);
            bool ss2 = isHoldingStart(PLAYER_SLOT_TARGET) && isHoldingSelect(PLAYER_SLOT_TARGET);
            bool white = h.test(K11) || h.test(K13) || h.test(K15) || h.test(K17) || h.test(K19);
            bool black = h.test(K12) || h.test(K14) || h.test(K16) || h.test(K18);
            bool white2 = h.test(K21) || h.test(K23) || h.test(K25) || h.test(K27) || h.test(K29);
            bool black2 = h.test(K22) || h.test(K24) || h.test(K26) || h.test(K28);

            if (isBattle())
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
                PlayData.randomSeed = ((uint64_t)rd() << 32) | rd();
            }
            SoundMgr::stopNoteSamples();
            SystemData.gNextScene = SceneType::RETRY_TRANS;
        }
        else if (PlayData.isAuto)
        {
            // Auto mode skips single result

            if (isCourse())
            {
                // If playing course mode, just go straight to next stage
                if (playerState[PLAYER_SLOT_PLAYER].finished && PlayData.courseStage < PlayData.courseStageData.size())
                {
                    ++PlayData.courseStage;
                    SystemData.gNextScene = SceneType::COURSE_TRANS;
                }
            }
        }
        else if (isCourse() && PlayData.courseStage > 0)
        {
            // Course Stage 2+ should go to result, regardless of whether any notes are hit
            SystemData.gNextScene = SceneType::RESULT;
        }
        else if (PlayData.playStarted)
        {
            // Skip result if no score
            if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset && !PlayData.player[PLAYER_SLOT_PLAYER].ruleset->isNoScore() ||
                isBattle() && PlayData.player[PLAYER_SLOT_TARGET].ruleset && !PlayData.player[PLAYER_SLOT_TARGET].ruleset->isNoScore())
            {
                SystemData.gNextScene = SceneType::RESULT;
            }
        }

        if (SystemData.gNextScene == SceneType::SELECT && SystemData.quitOnFinish)
        {
            SystemData.gNextScene = SceneType::EXIT_TRANS;
        }

        // protect
        if (PlayData.player[PLAYER_SLOT_MYBEST].ruleset &&
            !PlayData.player[PLAYER_SLOT_MYBEST].ruleset->isFailed() &&
            !PlayData.player[PLAYER_SLOT_MYBEST].ruleset->isFinished())
        {
            PlayData.player[PLAYER_SLOT_MYBEST].ruleset->fail();
        }

        if (!isBattle() &&
            PlayData.player[PLAYER_SLOT_TARGET].ruleset && 
            !PlayData.player[PLAYER_SLOT_TARGET].ruleset->isFailed() && 
            !PlayData.player[PLAYER_SLOT_TARGET].ruleset->isFinished())
        {
            PlayData.player[PLAYER_SLOT_TARGET].ruleset->fail();
        }
    }
}

void ScenePlay::updateFailed()
{
    auto t = Time();
    auto rt = t - PlayData.timers["play_start"];
    auto ft = t - PlayData.timers["fail"];

    if (PlayData.player[PLAYER_SLOT_PLAYER].chartObj != nullptr)
    {
        PlayData.player[PLAYER_SLOT_PLAYER].chartObj->update(rt);
    }
    if (PlayData.player[PLAYER_SLOT_TARGET].chartObj != nullptr)
    {
        PlayData.player[PLAYER_SLOT_TARGET].chartObj->update(rt);
    }
    if (PlayData.player[PLAYER_SLOT_MYBEST].chartObj != nullptr)
    {
        PlayData.player[PLAYER_SLOT_MYBEST].chartObj->update(rt);
    }

    if (PlayData.playStarted)
    {
        PlayData.bgaTexture->update(rt, false);
    }

    // play time / remain time
    updatePlayTime(rt);

    //
    spinTurntable(PlayData.playStarted);

    //failed play finished, move to next scene. No fadeout
    if (ft.norm() >= pSkin->info.timeFailed)
    {
        SystemData.timers["fadeout_start"] = t.norm();
        state = ePlayState::FADEOUT;
        LOG_DEBUG << "[Play] State changed to FADEOUT";
    }
}

void ScenePlay::updateWaitArena()
{
    Time t;
    auto rt = t - PlayData.timers["play_start"];

    PlayData.player[PLAYER_SLOT_PLAYER].chartObj->update(rt);
    PlayData.player[PLAYER_SLOT_PLAYER].ruleset->update(t);

    if (PlayData.playStarted)
    {
        PlayData.bgaTexture->update(rt, false);
    }

    // play time / remain time
    updatePlayTime(rt);

    // play bgm lanes
    procCommonNotes();

    if (!ArenaData.isOnline() || ArenaData.isPlayingFinished())
    {
        SystemData.timers["fadeout_start"] = t.norm();
        state = ePlayState::FADEOUT;
    }
}

void ScenePlay::updatePlayTime(const Time& rt)
{
}

void ScenePlay::procCommonNotes()
{
    assert(PlayData.player[PLAYER_SLOT_PLAYER].chartObj != nullptr);
    auto it = PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteBgmExpired.begin();
    size_t max = std::min(_bgmSampleIdxBuf.size(), PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteBgmExpired.size());
    size_t i = 0;
    for (; i < max && it != PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteBgmExpired.end(); ++i, ++it)
    {
        _bgmSampleIdxBuf[i] = (unsigned)it->dvalue;
    }
    SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_bgmSampleIdxBuf.data());

    // also play keysound in auto
    if (PlayData.isAuto)
    {
        i = 0;
        auto it = PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteExpired.size());
        while (i < max2 && it != PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteExpired.end())
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
    if (PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR)
    {
        i = 0;
        auto it = PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteExpired.size());
        while (i < max2 && it != PlayData.player[PLAYER_SLOT_PLAYER].chartObj->noteExpired.end())
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
    if (isBattle() && PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR)
    {
        i = 0;
        auto it = PlayData.player[PLAYER_SLOT_TARGET].chartObj->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + PlayData.player[PLAYER_SLOT_TARGET].chartObj->noteExpired.size());
        while (i < max2 && it != PlayData.player[PLAYER_SLOT_TARGET].chartObj->noteExpired.end())
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

        idx[0] = PlayData.player[slot].chartObj->getLaneFromKey(chart::NoteLaneCategory::Note, k);
        if (idx[0] != chart::NoteLaneIndex::_)
        {
            pNote[0] = &*PlayData.player[slot].chartObj->incomingNote(chart::NoteLaneCategory::Note, idx[0]);
            time[0] = pNote[0]->time.hres();
        }

        idx[1] = PlayData.player[slot].chartObj->getLaneFromKey(chart::NoteLaneCategory::LN, k);
        if (idx[1] != chart::NoteLaneIndex::_)
        {
            auto itLNNote = PlayData.player[slot].chartObj->incomingNote(chart::NoteLaneCategory::LN, idx[1]);
            while (!PlayData.player[slot].chartObj->isLastNote(chart::NoteLaneCategory::LN, idx[1], itLNNote))
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

        idx[2] = PlayData.player[slot].chartObj->getLaneFromKey(chart::NoteLaneCategory::Invs, k);
        if (idx[2] != chart::NoteLaneIndex::_)
        {
            pNote[2] = &*PlayData.player[slot].chartObj->incomingNote(chart::NoteLaneCategory::Invs, idx[2]);
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

    assert(PlayData.player[PLAYER_SLOT_PLAYER].chartObj != nullptr);
    for (size_t i = Input::K11; i <= Input::K19; ++i)
    {
        if (_inputAvailable[i])
            changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
    }
    if (!(PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
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
        if (!(PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
        {
            for (size_t i = Input::S2L; i <= Input::S2R; ++i)
            {
                if (_inputAvailable[i])
                    changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
            }
        }
    }
	if (isBattle())
	{
		assert(PlayData.player[PLAYER_SLOT_TARGET].chartObj != nullptr);
		for (size_t i = Input::K21; i <= Input::K29; ++i)
		{
			if (_inputAvailable[i])
				changeKeySample((Input::Pad)i, PLAYER_SLOT_TARGET);
		}
        if (!(PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
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
    auto rt = startedPlaying ? Time().norm() - PlayData.timers["play_start"] : 0;
    auto angle = rt * 360 / 2000;
    SystemData.scratchAxisValue[0] = (angle + (int)playerState[0].turntableAngleAdd) % 360;
    SystemData.scratchAxisValue[1] = (angle + (int)playerState[1].turntableAngleAdd) % 360;
}

void ScenePlay::requestExit()
{
    if (state == ePlayState::FADEOUT || state == ePlayState::WAIT_ARENA)
        return;

    Time t;

    if (PlayData.playStarted)
    {
        playInterrupted = true;

        if (!playerState[PLAYER_SLOT_PLAYER].finished)
        {
            PlayData.player[PLAYER_SLOT_PLAYER].ruleset->fail();
            LOG_INFO << "[Play] 1P finished";
        }
        if (!playerState[PLAYER_SLOT_TARGET].finished && isBattle() && PlayData.player[PLAYER_SLOT_TARGET].ruleset)
        {
            PlayData.player[PLAYER_SLOT_TARGET].ruleset->fail();
            LOG_INFO << "[Play] 2P finished";
        }

        if (ArenaData.isOnline())
        {
            if (ArenaData.isClient())
                g_pArenaClient->setPlayingFinished();
            else
                g_pArenaHost->setPlayingFinished();
        }

        if (PlayData.replayNew)
        {
            long long ms = t.norm() - PlayData.timers["play_start"];
            PlayData.replayNew->commands.push_back({ ms, ReplayChart::Commands::Type::ESC, 0 });
        }

        PlayData.pushGraphPoints();
    }

    if (ArenaData.isOnline())
    {
        ArenaData.timers["play_finish_wait"] = t.norm();
        state = ePlayState::WAIT_ARENA;
        LOG_DEBUG << "[Play] State changed to WAIT_ARENA";
    }
    else
    { 
        SoundMgr::setNoteVolume(0.0, 1000);
        SystemData.timers["fadeout_start"] = t.norm();
        state = ePlayState::FADEOUT;
        LOG_DEBUG << "[Play] State changed to FADEOUT";
    }
}

void ScenePlay::toggleLanecover(int slot, bool state)
{
    if (state)
    {
        switch (PlayData.player[slot].mods.laneEffect)
        {
        case PlayModifierLaneEffectType::OFF:
            PlayData.player[slot].mods.laneEffect = playerState[slot].origLanecoverType == PlayModifierLaneEffectType::OFF ?
                PlayModifierLaneEffectType::SUDDEN : playerState[slot].origLanecoverType;
            break;
        case PlayModifierLaneEffectType::LIFT:     PlayData.player[slot].mods.laneEffect = PlayModifierLaneEffectType::LIFTSUD; break;
        }
    }
    else
    {
        switch (PlayData.player[slot].mods.laneEffect)
        {
        case PlayModifierLaneEffectType::HIDDEN:   PlayData.player[slot].mods.laneEffect = PlayModifierLaneEffectType::OFF; break;
        case PlayModifierLaneEffectType::SUDDEN:   PlayData.player[slot].mods.laneEffect = PlayModifierLaneEffectType::OFF; break;
        case PlayModifierLaneEffectType::SUDHID:   PlayData.player[slot].mods.laneEffect = PlayModifierLaneEffectType::OFF; break;
        case PlayModifierLaneEffectType::LIFTSUD:  PlayData.player[slot].mods.laneEffect = PlayModifierLaneEffectType::LIFT; break;
        }
    }

    if (isPlayerEnabledLanecover(slot) && isPlayerLockSpeed(slot) && playerState[slot].lockspeedHispeedBuffered != 0.0)
    {
        // FIXME load P2 hispeedFix type
        PlayData.player[slot].hispeed = playerState[slot].lockspeedHispeedBuffered;
        double bpm = PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix == PlayModifierHispeedFixType::CONSTANT ?
            150.0 : PlayData.player[slot].chartObj->getCurrentBPM();
        const auto [green, val] = calcGreenNumber(bpm, slot, PlayData.player[slot].hispeed);
        playerState[slot].lockspeedValueInternal = val;
        playerState[slot].lockspeedGreenNumber = green;
        playerState[slot].hispeedHasChanged = true;
    }
}

void ScenePlay::toggleLockspeed(int slot, bool state)
{
    if (state)
    {
        PlayData.player[slot].mods.hispeedFix = playerState[slot].origLockspeedType != PlayModifierHispeedFixType::NONE ?
            playerState[slot].origLockspeedType : PlayModifierHispeedFixType::INITIAL;
    }
    else
    {
        PlayData.player[slot].mods.hispeedFix = PlayModifierHispeedFixType::NONE;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CALLBACK
void ScenePlay::inputGamePress(InputMask& m, const Time& t)
{
    using namespace Input;

    auto input = _inputAvailable & m;

    // individual keys
    if (!PlayData.isAuto && !PlayData.isReplay)
    {
        inputGamePressTimer(input, t);
        inputGamePressPlayKeysounds(input, t);
    }
    if (PlayData.playStarted && PlayData.replayNew)
    {
        long long ms = t.norm() - PlayData.timers["play_start"];
        ReplayChart::Commands cmd;
        cmd.ms = ms;
        for (size_t k = S1L; k < LANE_COUNT; ++k)
        {
            if (!input[k]) continue;

            if (PlayData.mode == SkinType::PLAY5 || PlayData.mode == SkinType::PLAY5_2)
            {
                if (REPLAY_INPUT_DOWN_CMD_MAP_5K[replayCmdMapIndex].find((Input::Pad)k) != REPLAY_INPUT_DOWN_CMD_MAP_5K[replayCmdMapIndex].end())
                {
                    cmd.type = REPLAY_INPUT_DOWN_CMD_MAP_5K[replayCmdMapIndex].at((Input::Pad)k);
                    PlayData.replayNew->commands.push_back(cmd);
                }
            }
            else
            {
                if (REPLAY_INPUT_DOWN_CMD_MAP.find((Input::Pad)k) != REPLAY_INPUT_DOWN_CMD_MAP.end())
                {
                    cmd.type = REPLAY_INPUT_DOWN_CMD_MAP.at((Input::Pad)k);
                    PlayData.replayNew->commands.push_back(cmd);
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
        if (t > playerState[slot].startPressedTime && (t - playerState[slot].startPressedTime).norm() < 200)
        {
            playerState[slot].startPressedTime = TIMER_NEVER;
            toggleLanecover(slot, !isPlayerEnabledLanecover(slot));
            playerState[slot].lanecoverStateHasChanged = true;
        }
        else
        {
            playerState[slot].startPressedTime = t;
        }
    };
    if (pressedStart[PLAYER_SLOT_PLAYER]) 
        toggleLanecoverSide(PLAYER_SLOT_PLAYER);
    if (isBattle() && pressedStart[PLAYER_SLOT_TARGET])
        toggleLanecoverSide(PLAYER_SLOT_TARGET);

    // double click SELECT when lanecover enabled: lock green number
    auto toggleLockspeedSide = [&](int slot)
    {
        if (t > playerState[slot].selectPressedTime && (t - playerState[slot].selectPressedTime).norm() < 200)
        {
            toggleLockspeed(slot, !isPlayerLockSpeed(slot));
            playerState[slot].selectPressedTime = TIMER_NEVER;

            if (isPlayerLockSpeed(slot))
            {
                double bpm = PlayData.player[slot].mods.hispeedFix == PlayModifierHispeedFixType::CONSTANT ?
                    150.0 : PlayData.player[slot].chartObj->getCurrentBPM();
                double hs = PlayData.player[slot].hispeed;
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
    if (isBattle() && pressedSelect[PLAYER_SLOT_TARGET])
        toggleLockspeedSide(PLAYER_SLOT_TARGET);

    // hs adjusted by key
    auto adjustHispeedSide = [&](int slot)
    {
        if (pressedSpeedUp[slot] ||
            ((isHoldingStart(slot) || isHoldingSelect(slot)) && black[slot]) ||
            (slot == PLAYER_SLOT_PLAYER && adjustHispeedWithUpDown && input[UP]))
        {
            if (PlayData.player[slot].hispeed < hiSpeedMax)
            {
                PlayData.player[slot].hispeed = std::min(PlayData.player[slot].hispeed + hiSpeedMargin, hiSpeedMax);
                playerState[slot].hispeedHasChanged = true;
            }
        }

        if (pressedSpeedDown[slot] ||
            ((isHoldingStart(slot) || isHoldingSelect(slot)) && white[slot]) ||
            (slot == PLAYER_SLOT_PLAYER && adjustHispeedWithUpDown && input[DOWN]))
        {
            if (PlayData.player[slot].hispeed > hiSpeedMinSoft)
            {
                PlayData.player[slot].hispeed = std::max(PlayData.player[slot].hispeed - hiSpeedMargin, hiSpeedMinSoft);
                playerState[slot].hispeedHasChanged = true;
            }
        }
    };
    adjustHispeedSide(PLAYER_SLOT_PLAYER);
    if (isBattle()) 
        adjustHispeedSide(PLAYER_SLOT_TARGET);

    // lanecover adjusted by key
    if (isPlayerEnabledLanecover(PLAYER_SLOT_PLAYER) || PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect == PlayModifierLaneEffectType::LIFT)
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
            if (isBattle())
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
        if (!ArenaData.isOnline() || state == ePlayState::PLAYING)
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
            playerState[PLAYER_SLOT_PLAYER].scratchLastUpdate = t.norm();
            playerState[PLAYER_SLOT_PLAYER].scratchDirection = input[S1L] ? AxisDir::AXIS_UP : AxisDir::AXIS_DOWN;
            SystemData.timers["key_on_s_1p"] = t.norm();
            SystemData.timers["key_off_s_1p"] = TIMER_NEVER;
        }
    }
    if (isBattle() || isPlaymodeDP())
    {
        if (input[S2L] || input[S2R])
        {
            playerState[PLAYER_SLOT_TARGET].scratchLastUpdate = t.norm();
            playerState[PLAYER_SLOT_TARGET].scratchDirection = input[S2L] ? AxisDir::AXIS_UP : AxisDir::AXIS_DOWN;
            SystemData.timers["key_on_s_2p"] = t.norm();
            SystemData.timers["key_off_s_2p"] = TIMER_NEVER;
        }
    }
}

void ScenePlay::inputGamePressPlayKeysounds(InputMask inputSample, const Time& t)
{
    using namespace Input;

    // do not play keysounds if player is failed
    if (isBattle())
    {
        if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset->isFailed()) inputSample &= ~INPUT_MASK_1P;
        if (PlayData.player[PLAYER_SLOT_TARGET].ruleset->isFailed()) inputSample &= ~INPUT_MASK_2P;
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
            SystemData.timers[InputGamePressMap.at(Input::Pad(i))] = t.norm();
            SystemData.timers[InputGameReleaseMap.at(Input::Pad(i))] = TIMER_NEVER;
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
    if (!PlayData.playStarted && (input[K1START] || input[K1SELECT] || input[K2START] || input[K2SELECT]))
    {
        delayedReadyTime = t;
    }

    auto ttUpdateSide = [&](int slot, Pad ttUp, Pad ttDn)
    {
        bool lanecover = PlayData.player[slot].mods.laneEffect != PlayModifierLaneEffectType::OFF;
        bool fnLanecover = isHoldingStart(slot) || !adjustHispeedWithSelect && isHoldingSelect(slot);
        bool fnHispeed = adjustHispeedWithSelect && isHoldingSelect(slot);

        int val = 0;
        if (input[ttUp]) val--;  // -1 per tick
        if (input[ttDn]) val++;  // +1 per tick

        // turntable spin
        playerState[slot].turntableAngleAdd += val * 0.25;
        playerState[slot].scratchLastUpdate = t.norm();

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
    else if (isBattle())
        ttUpdateSide(PLAYER_SLOT_TARGET, S2L, S2R);
}

// CALLBACK
void ScenePlay::inputGameRelease(InputMask& m, const Time& t)
{
    using namespace Input;
    auto input = _inputAvailable & m;

    if (!PlayData.isAuto && !PlayData.isReplay)
    {
        inputGameReleaseTimer(input, t);
    }
    
    if (PlayData.playStarted && PlayData.replayNew)
    {
        long long ms = t.norm() - PlayData.timers["play_start"];
        ReplayChart::Commands cmd;
        cmd.ms = ms;
        for (size_t k = S1L; k < LANE_COUNT; ++k)
        {
            if (!input[k]) continue;

            if (PlayData.mode == SkinType::PLAY5 || PlayData.mode == SkinType::PLAY5_2)
            {
                if (REPLAY_INPUT_UP_CMD_MAP_5K[replayCmdMapIndex].find((Input::Pad)k) != REPLAY_INPUT_UP_CMD_MAP_5K[replayCmdMapIndex].end())
                {
                    cmd.type = REPLAY_INPUT_UP_CMD_MAP_5K[replayCmdMapIndex].at((Input::Pad)k);
                    PlayData.replayNew->commands.push_back(cmd);
                }
            }
            else
            {
                if (REPLAY_INPUT_UP_CMD_MAP.find((Input::Pad)k) != REPLAY_INPUT_UP_CMD_MAP.end())
                {
                    cmd.type = REPLAY_INPUT_UP_CMD_MAP.at((Input::Pad)k);
                    PlayData.replayNew->commands.push_back(cmd);
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
            SystemData.timers[InputGamePressMap.at(Input::Pad(i))] = TIMER_NEVER;
            SystemData.timers[InputGameReleaseMap.at(Input::Pad(i))] = t.norm();

            // TODO stop sample playing while release in LN notes
        }

    if (true)
    {
        if (input[S1L] || input[S1R])
        {
            if ((input[S1L] && playerState[PLAYER_SLOT_PLAYER].scratchDirection == AxisDir::AXIS_UP) ||
                (input[S1R] && playerState[PLAYER_SLOT_PLAYER].scratchDirection == AxisDir::AXIS_DOWN))
            {
                SystemData.timers["key_on_s_1p"] = TIMER_NEVER;
                SystemData.timers["key_off_s_1p"] = t.norm();
                playerState[PLAYER_SLOT_PLAYER].scratchDirection = AxisDir::AXIS_NONE;
            }
        }
    }
    if (isBattle() || isPlaymodeDP())
    {
        if (input[S2L] || input[S2R])
        {
            if ((input[S2L] && playerState[PLAYER_SLOT_TARGET].scratchDirection == AxisDir::AXIS_UP) ||
                (input[S2R] && playerState[PLAYER_SLOT_TARGET].scratchDirection == AxisDir::AXIS_DOWN))
            {
                SystemData.timers["key_on_s_2p"] = TIMER_NEVER;
                SystemData.timers["key_off_s_2p"] = t.norm();
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

	if (!PlayData.isAuto && (!PlayData.isReplay || !PlayData.playStarted))
	{
		auto ttUpdateSide = [&](int slot, double S)
		{
			bool lanecover = PlayData.player[slot].mods.laneEffect != PlayModifierLaneEffectType::OFF;
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
		else if (isBattle())
			ttUpdateSide(PLAYER_SLOT_TARGET, S2);
		else
			ttUpdateSide(PLAYER_SLOT_PLAYER, S1 + S2);
	}
}

}
