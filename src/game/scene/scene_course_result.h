#pragma once
#include "game/ruleset/ruleset.h"
#include "scene.h"

namespace lunaticvibes
{

class ScoreBase;
enum class eCourseResultState
{
    DRAW,
    STOP,
    RECORD,
    FADEOUT,
};

class SceneCourseResult : public SceneBase
{
private:
    eCourseResultState state;
    InputMask _inputAvailable;

protected:
    bool _scoreSyncFinished = false;
    bool _retryRequested = false;
    std::shared_ptr<ScoreBase> _pScoreOld;

    enum SummaryArgs
    {
        ARG_TOTAL_NOTES,
        ARG_MAX_SCORE,
        ARG_SCORE,
        ARG_EXSCORE,
        ARG_MAXCOMBO,
        ARG_FAST,
        ARG_SLOW,
        ARG_BP,
        ARG_CB,

        ARG_JUDGE_0,
        ARG_JUDGE_1,
        ARG_JUDGE_2,
        ARG_JUDGE_3,
        ARG_JUDGE_4,
        ARG_JUDGE_5,
    };
    std::map<SummaryArgs, unsigned> summary;
    double acc = 0.;

public:
    SceneCourseResult();
    virtual ~SceneCourseResult();

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    void updateDraw();
    void updateStop();
    void updateRecord();
    void updateFadeout();

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const Time&);
    void inputGameHold(InputMask&, const Time&);
    void inputGameRelease(InputMask&, const Time&);
};

}
