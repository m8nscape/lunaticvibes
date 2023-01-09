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
    PlaySide side) : vRuleset(format, chart), RulesetBMS(format, chart, gauge, keys, difficulty, health, side)
{
    this->replay = replay;
    itReplayCommand = replay->commands.begin();
    showJudge = (_side == PlaySide::AUTO || _side == PlaySide::AUTO_DOUBLE || _side == PlaySide::AUTO_2P);

    doJudge = false;

    if (gPlayContext.mode == eMode::PLAY5 || gPlayContext.mode == eMode::PLAY5_2)
    {
        if (gPlayContext.shift1PNotes5KFor7KSkin)
        {
            replayCmdMapIndex = gPlayContext.shift2PNotes5KFor7KSkin ? 3 : 2;
        }
        else
        {
            replayCmdMapIndex = gPlayContext.shift2PNotes5KFor7KSkin ? 1 : 0;
        }
    }

    if (replay->pitchValue != 0)
    {
        double ps = (replay->pitchValue + 12) / 24.0;
        static const double tick = std::pow(2, 1.0 / 12);
        playbackSpeed = std::pow(tick, replay->pitchValue);
    }

    switch (side)
    {
    case RulesetBMS::PlaySide::AUTO:
    case RulesetBMS::PlaySide::AUTO_DOUBLE:
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;

    case RulesetBMS::PlaySide::AUTO_2P:
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;

    case RulesetBMS::PlaySide::MYBEST:
        _judgeScratch = !(gPlayContext.mods[PLAYER_SLOT_MYBEST].assist_mask & PLAY_MOD_ASSIST_AUTOSCR);
        break;
    }
}

void RulesetBMSReplay::update(const Time& t)
{
    if (!_hasStartTime)
        setStartTime(t);

    auto rt = t - _startTime.norm();
    using namespace chart;

    InputMask prev = keyPressing;
    while (itReplayCommand != replay->commands.end() && rt.norm() >= (long long)std::round(itReplayCommand->ms * playbackSpeed / gSelectContext.pitchSpeed))
    {
        auto cmd = itReplayCommand->type;
        if (_side == PlaySide::AUTO_2P)
        {
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

        if (gPlayContext.mode == eMode::PLAY5 || gPlayContext.mode == eMode::PLAY5_2)
        {
            if (replay_cmd_input_down_map_5k[replayCmdMapIndex].find(cmd) != replay_cmd_input_down_map_5k[replayCmdMapIndex].end())
            {
                keyPressing[replay_cmd_input_down_map_5k[replayCmdMapIndex].at(cmd)] = true;
            }
            else if (replay_cmd_input_up_map_5k[replayCmdMapIndex].find(cmd) != replay_cmd_input_up_map_5k[replayCmdMapIndex].end())
            {
                keyPressing[replay_cmd_input_up_map_5k[replayCmdMapIndex].at(cmd)] = false;
            }
        }
        else
        {
            if (replay_cmd_input_down_map.find(cmd) != replay_cmd_input_down_map.end())
            {
                keyPressing[replay_cmd_input_down_map.at(cmd)] = true;
            }
            else if (replay_cmd_input_up_map.find(cmd) != replay_cmd_input_up_map.end())
            {
                keyPressing[replay_cmd_input_up_map.at(cmd)] = false;
            }
        }

        switch (cmd)
        {
        case ReplayChart::Commands::Type::S1A_PLUS:  _scratchAccumulator[PLAYER_SLOT_PLAYER] = 0.0015; break;
        case ReplayChart::Commands::Type::S1A_MINUS: _scratchAccumulator[PLAYER_SLOT_PLAYER] = -0.0015; break;
        case ReplayChart::Commands::Type::S1A_STOP:  _scratchAccumulator[PLAYER_SLOT_PLAYER] = 0; break;
        case ReplayChart::Commands::Type::S2A_PLUS:  _scratchAccumulator[PLAYER_SLOT_TARGET] = 0.0015; break;
        case ReplayChart::Commands::Type::S2A_MINUS: _scratchAccumulator[PLAYER_SLOT_TARGET] = -0.0015; break;
        case ReplayChart::Commands::Type::S2A_STOP:  _scratchAccumulator[PLAYER_SLOT_TARGET] = 0; break;

        // extract judge from frames
        case ReplayChart::Commands::Type::JUDGE_LEFT_EXACT_0:   updateJudge(t, NoteLaneIndex::_, JudgeArea::EXACT_PERFECT, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_0:   updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_PERFECT, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_1:   updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_GREAT, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_2:   updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_GOOD, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_3:   updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_BAD, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_4:   updateJudge(t, NoteLaneIndex::_, JudgeArea::MISS, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_EARLY_5:   updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_KPOOR, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_0:    updateJudge(t, NoteLaneIndex::_, JudgeArea::LATE_PERFECT, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_1:    updateJudge(t, NoteLaneIndex::_, JudgeArea::LATE_GREAT, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_2:    updateJudge(t, NoteLaneIndex::_, JudgeArea::LATE_GOOD, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_3:    updateJudge(t, NoteLaneIndex::_, JudgeArea::LATE_BAD, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_4:    updateJudge(t, NoteLaneIndex::_, JudgeArea::MISS, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LATE_5:    updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_KPOOR, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EXACT_0:  updateJudge(t, NoteLaneIndex::_, JudgeArea::EXACT_PERFECT, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_0:  updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_PERFECT, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_1:  updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_GREAT, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_2:  updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_GOOD, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_3:  updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_BAD, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_4:  updateJudge(t, NoteLaneIndex::_, JudgeArea::MISS, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_EARLY_5:  updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_KPOOR, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_0:   updateJudge(t, NoteLaneIndex::_, JudgeArea::LATE_PERFECT, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_1:   updateJudge(t, NoteLaneIndex::_, JudgeArea::LATE_GREAT, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_2:   updateJudge(t, NoteLaneIndex::_, JudgeArea::LATE_GOOD, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_3:   updateJudge(t, NoteLaneIndex::_, JudgeArea::LATE_BAD, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_4:   updateJudge(t, NoteLaneIndex::_, JudgeArea::MISS, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LATE_5:   updateJudge(t, NoteLaneIndex::_, JudgeArea::EARLY_KPOOR, PLAYER_SLOT_TARGET, true); break;
        case ReplayChart::Commands::Type::JUDGE_LEFT_LANDMINE:  updateJudge(t, NoteLaneIndex::_, JudgeArea::MINE_KPOOR, PLAYER_SLOT_PLAYER, true); break;
        case ReplayChart::Commands::Type::JUDGE_RIGHT_LANDMINE: updateJudge(t, NoteLaneIndex::_, JudgeArea::MINE_KPOOR, PLAYER_SLOT_TARGET, true); break;
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
    RulesetBMS::fail();
}
