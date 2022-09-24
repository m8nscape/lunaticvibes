#include "ruleset_bms_replay.h"
#include "game/scene/scene.h"
#include "game/scene/scene_context.h"

RulesetBMSReplay::RulesetBMSReplay(
    std::shared_ptr<ChartFormatBase> format,
    std::shared_ptr<ChartObjectBase> chart,
    std::shared_ptr<ReplayChart> replay,
    eModGauge gauge,
    GameModeKeys keys,
    JudgeDifficulty difficulty,
    double health,
    PlaySide side) : RulesetBMS(format, chart, gauge, keys, difficulty, health, side)
{
    this->replay = replay;
    itReplayCommand = replay->commands.begin();
    showJudge = (_side == PlaySide::AUTO || _side == PlaySide::AUTO_2P);

    doJudge = false;

    if (replay->pitchValue != 0)
    {
        double ps = (replay->pitchValue + 12) / 24.0;
        static const double tick = std::pow(2, 1.0 / 12);
        playbackSpeed = std::pow(tick, replay->pitchValue);
    }
}

void RulesetBMSReplay::update(const Time& t)
{
    auto rt = t - gTimers.get(eTimer::PLAY_START);
    using namespace chart;

    InputMask prev = keyPressing;
    while (itReplayCommand != replay->commands.end() && rt.norm() >= (long long)std::round(itReplayCommand->ms * playbackSpeed / gSelectContext.pitchSpeed))
    {
        switch (itReplayCommand->type)
        {
        case ReplayChart::Commands::Type::S1L_DOWN: keyPressing[Input::Pad::S1L] = true; break;
        case ReplayChart::Commands::Type::S1R_DOWN: keyPressing[Input::Pad::S1R] = true; break;
        case ReplayChart::Commands::Type::K11_DOWN: keyPressing[Input::Pad::K11] = true; break;
        case ReplayChart::Commands::Type::K12_DOWN: keyPressing[Input::Pad::K12] = true; break;
        case ReplayChart::Commands::Type::K13_DOWN: keyPressing[Input::Pad::K13] = true; break;
        case ReplayChart::Commands::Type::K14_DOWN: keyPressing[Input::Pad::K14] = true; break;
        case ReplayChart::Commands::Type::K15_DOWN: keyPressing[Input::Pad::K15] = true; break;
        case ReplayChart::Commands::Type::K16_DOWN: keyPressing[Input::Pad::K16] = true; break;
        case ReplayChart::Commands::Type::K17_DOWN: keyPressing[Input::Pad::K17] = true; break;
        case ReplayChart::Commands::Type::K18_DOWN: keyPressing[Input::Pad::K18] = true; break;
        case ReplayChart::Commands::Type::K19_DOWN: keyPressing[Input::Pad::K19] = true; break;
        case ReplayChart::Commands::Type::S2L_DOWN: keyPressing[Input::Pad::S2L] = true; break;
        case ReplayChart::Commands::Type::S2R_DOWN: keyPressing[Input::Pad::S2R] = true; break;
        case ReplayChart::Commands::Type::K21_DOWN: keyPressing[Input::Pad::K21] = true; break;
        case ReplayChart::Commands::Type::K22_DOWN: keyPressing[Input::Pad::K22] = true; break;
        case ReplayChart::Commands::Type::K23_DOWN: keyPressing[Input::Pad::K23] = true; break;
        case ReplayChart::Commands::Type::K24_DOWN: keyPressing[Input::Pad::K24] = true; break;
        case ReplayChart::Commands::Type::K25_DOWN: keyPressing[Input::Pad::K25] = true; break;
        case ReplayChart::Commands::Type::K26_DOWN: keyPressing[Input::Pad::K26] = true; break;
        case ReplayChart::Commands::Type::K27_DOWN: keyPressing[Input::Pad::K27] = true; break;
        case ReplayChart::Commands::Type::K28_DOWN: keyPressing[Input::Pad::K28] = true; break;
        case ReplayChart::Commands::Type::K29_DOWN: keyPressing[Input::Pad::K29] = true; break;
        case ReplayChart::Commands::Type::S1L_UP: keyPressing[Input::Pad::S1L] = false; break;
        case ReplayChart::Commands::Type::S1R_UP: keyPressing[Input::Pad::S1R] = false; break;
        case ReplayChart::Commands::Type::K11_UP: keyPressing[Input::Pad::K11] = false; break;
        case ReplayChart::Commands::Type::K12_UP: keyPressing[Input::Pad::K12] = false; break;
        case ReplayChart::Commands::Type::K13_UP: keyPressing[Input::Pad::K13] = false; break;
        case ReplayChart::Commands::Type::K14_UP: keyPressing[Input::Pad::K14] = false; break;
        case ReplayChart::Commands::Type::K15_UP: keyPressing[Input::Pad::K15] = false; break;
        case ReplayChart::Commands::Type::K16_UP: keyPressing[Input::Pad::K16] = false; break;
        case ReplayChart::Commands::Type::K17_UP: keyPressing[Input::Pad::K17] = false; break;
        case ReplayChart::Commands::Type::K18_UP: keyPressing[Input::Pad::K18] = false; break;
        case ReplayChart::Commands::Type::K19_UP: keyPressing[Input::Pad::K19] = false; break;
        case ReplayChart::Commands::Type::S2L_UP: keyPressing[Input::Pad::S2L] = false; break;
        case ReplayChart::Commands::Type::S2R_UP: keyPressing[Input::Pad::S2R] = false; break;
        case ReplayChart::Commands::Type::K21_UP: keyPressing[Input::Pad::K21] = false; break;
        case ReplayChart::Commands::Type::K22_UP: keyPressing[Input::Pad::K22] = false; break;
        case ReplayChart::Commands::Type::K23_UP: keyPressing[Input::Pad::K23] = false; break;
        case ReplayChart::Commands::Type::K24_UP: keyPressing[Input::Pad::K24] = false; break;
        case ReplayChart::Commands::Type::K25_UP: keyPressing[Input::Pad::K25] = false; break;
        case ReplayChart::Commands::Type::K26_UP: keyPressing[Input::Pad::K26] = false; break;
        case ReplayChart::Commands::Type::K27_UP: keyPressing[Input::Pad::K27] = false; break;
        case ReplayChart::Commands::Type::K28_UP: keyPressing[Input::Pad::K28] = false; break;
        case ReplayChart::Commands::Type::K29_UP: keyPressing[Input::Pad::K29] = false; break;
        case ReplayChart::Commands::Type::S1A_PLUS: _scratchAccumulator[PLAYER_SLOT_PLAYER] = 0.0015; break;
        case ReplayChart::Commands::Type::S1A_MINUS: _scratchAccumulator[PLAYER_SLOT_PLAYER] = -0.0015; break;
        case ReplayChart::Commands::Type::S1A_STOP: _scratchAccumulator[PLAYER_SLOT_PLAYER] = 0; break;
        case ReplayChart::Commands::Type::S2A_PLUS: _scratchAccumulator[PLAYER_SLOT_TARGET] = 0.0015; break;
        case ReplayChart::Commands::Type::S2A_MINUS: _scratchAccumulator[PLAYER_SLOT_TARGET] = -0.0015; break;
        case ReplayChart::Commands::Type::S2A_STOP: _scratchAccumulator[PLAYER_SLOT_TARGET] = 0; break;

        // extract judge from frames
        case ReplayChart::Commands::Type::JUDGE_LEFT_EXACT_0: updateHit(t, NoteLaneIndex::_, JudgeType::PERFECT, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_0: updateHit(t, NoteLaneIndex::_, JudgeType::PERFECT, PLAYER_SLOT_PLAYER, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_1: updateHit(t, NoteLaneIndex::_, JudgeType::GREAT, PLAYER_SLOT_PLAYER, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_2: updateHit(t, NoteLaneIndex::_, JudgeType::GOOD, PLAYER_SLOT_PLAYER, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_3: updateMiss(t, NoteLaneIndex::_, JudgeType::BAD, PLAYER_SLOT_PLAYER, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_4: updateMiss(t, NoteLaneIndex::_, JudgeType::MISS, PLAYER_SLOT_PLAYER, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_5: updateMiss(t, NoteLaneIndex::_, JudgeType::BPOOR, PLAYER_SLOT_PLAYER, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_0: updateHit(t, NoteLaneIndex::_, JudgeType::PERFECT, PLAYER_SLOT_PLAYER, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_1: updateHit(t, NoteLaneIndex::_, JudgeType::GREAT, PLAYER_SLOT_PLAYER, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_2: updateHit(t, NoteLaneIndex::_, JudgeType::GOOD, PLAYER_SLOT_PLAYER, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_3: updateMiss(t, NoteLaneIndex::_, JudgeType::BAD, PLAYER_SLOT_PLAYER, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4: updateMiss(t, NoteLaneIndex::_, JudgeType::MISS, PLAYER_SLOT_PLAYER, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_5: updateMiss(t, NoteLaneIndex::_, JudgeType::BPOOR, PLAYER_SLOT_PLAYER, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EXACT_0: updateHit(t, NoteLaneIndex::_, JudgeType::PERFECT, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_0: updateHit(t, NoteLaneIndex::_, JudgeType::PERFECT, PLAYER_SLOT_TARGET, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_1: updateHit(t, NoteLaneIndex::_, JudgeType::GREAT, PLAYER_SLOT_TARGET, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_2: updateHit(t, NoteLaneIndex::_, JudgeType::GOOD, PLAYER_SLOT_TARGET, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_3: updateMiss(t, NoteLaneIndex::_, JudgeType::BAD, PLAYER_SLOT_TARGET, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_4: updateMiss(t, NoteLaneIndex::_, JudgeType::MISS, PLAYER_SLOT_TARGET, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_5: updateMiss(t, NoteLaneIndex::_, JudgeType::BPOOR, PLAYER_SLOT_TARGET, true); _basic.fast++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_0: updateHit(t, NoteLaneIndex::_, JudgeType::PERFECT, PLAYER_SLOT_TARGET, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_1: updateHit(t, NoteLaneIndex::_, JudgeType::GREAT, PLAYER_SLOT_TARGET, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_2: updateHit(t, NoteLaneIndex::_, JudgeType::GOOD, PLAYER_SLOT_TARGET, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_3: updateMiss(t, NoteLaneIndex::_, JudgeType::BAD, PLAYER_SLOT_TARGET, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4: updateMiss(t, NoteLaneIndex::_, JudgeType::MISS, PLAYER_SLOT_TARGET, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_5: updateMiss(t, NoteLaneIndex::_, JudgeType::BPOOR, PLAYER_SLOT_TARGET, true); _basic.slow++; break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LANDMINE: updateMiss(t, NoteLaneIndex::_, JudgeType::BPOOR, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LANDMINE: updateMiss(t, NoteLaneIndex::_, JudgeType::BPOOR, PLAYER_SLOT_TARGET, true); break;
        }
        itReplayCommand++;
    }
    InputMask pressed = keyPressing & ~prev;
    InputMask released = ~keyPressing & prev;
    if (pressed.any())
        RulesetBMS::updatePress(pressed, t);
    if (keyPressing.any())
        RulesetBMS::updateHold(keyPressing, t);
    if (released.any())
        RulesetBMS::updateRelease(released, t);

    RulesetBMS::update(t);
}

void RulesetBMSReplay::fail()
{
    _isFailed = true;
}

void RulesetBMSReplay::reset()
{
    vRuleset::reset();

    updateGlobals();
}