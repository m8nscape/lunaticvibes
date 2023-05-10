#pragma once

#include "ruleset_bms.h"
#include "game/scene/scene_context.h"

class RulesetBMSAuto : public RulesetBMS
{
public:
    RulesetBMSAuto(
        std::shared_ptr<ChartFormatBase> format,
        std::shared_ptr<ChartObjectBase> chart,
        PlayModifierGaugeType gauge,
        GameModeKeys keys,
        JudgeDifficulty difficulty = JudgeDifficulty::NORMAL,
        double health = 1.0,
        PlaySide side = PlaySide::AUTO);

protected:
	double targetRate = 100.0;
    std::vector<JudgeArea> noteJudges;
    size_t judgeIndex = 0;

    std::map<JudgeArea, unsigned> totalJudgeCount;

    std::array<bool, Input::Pad::LANE_COUNT> isPressingLN;

public:
    void setTargetRate(double rate);
    double getTargetRate() const { return targetRate; }

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