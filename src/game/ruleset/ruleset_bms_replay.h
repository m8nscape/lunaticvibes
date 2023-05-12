#pragma once

#include "ruleset_bms.h"
#include "game/scene/scene_context.h"

namespace lunaticvibes
{

class RulesetBMSReplay : public RulesetBMS
{
public:
    RulesetBMSReplay(
        std::shared_ptr<ChartFormatBase> format,
        std::shared_ptr<ChartObjectBase> chart,
        std::shared_ptr<ReplayChart> replay,
        PlayModifierGaugeType gauge,
        GameModeKeys keys,
        JudgeDifficulty difficulty = JudgeDifficulty::NORMAL,
        double health = 1.0,
        PlaySide side = PlaySide::AUTO);

public:
    double playbackSpeed = 1.0;

protected:
    std::shared_ptr<ReplayChart> replay;
    std::vector<ReplayChart::Commands>::iterator itReplayCommand;
    InputMask keyPressing;
    unsigned replayCmdMapIndex = 0;

public:

    // Register to InputWrapper
    virtual void updatePress(InputMask& pg, const Time& t) override {}
    // Register to InputWrapper
    virtual void updateHold(InputMask& hg, const Time& t) override {}
    // Register to InputWrapper
    virtual void updateRelease(InputMask& rg, const Time& t) override {}
    // Register to InputWrapper
    virtual void updateAxis(double s1, double s2, const Time& t) override {}
    // Called by ScenePlay
    virtual void update(const Time& t) override;

    virtual void fail();
};

}
