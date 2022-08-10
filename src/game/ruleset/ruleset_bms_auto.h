#pragma once

#include "ruleset_bms.h"
#include "game/scene/scene_context.h"

class RulesetBMSAuto : public RulesetBMS
{
public:
    RulesetBMSAuto(
        std::shared_ptr<vChartFormat> format,
        std::shared_ptr<chart::vChart> chart,
        eModGauge gauge,
        GameModeKeys keys,
        JudgeDifficulty difficulty = JudgeDifficulty::NORMAL,
        double health = 1.0,
        PlaySide side = PlaySide::AUTO);

protected:
	double targetRate = 100.0;
    std::vector<JudgeType> noteJudges;
    size_t judgeIndex = 0;
    bool showJudge = true;

    std::map<JudgeType, unsigned> totalJudgeCount;

    std::array<bool, Input::Pad::KEY_COUNT> isPressingLN;

public:
    void setTargetRate(double rate);

    virtual bool isFailed() const override { return gPlayContext.isAuto ? RulesetBMS::isFailed() : true; }
    virtual bool isFinished() const override { return gPlayContext.isAuto ? RulesetBMS::isFinished() : true; }

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
    virtual void reset();
};