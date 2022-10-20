#pragma once
#include <mutex>
#include "game/ruleset/ruleset.h"
#include "scene.h"

class vScore;
enum class eCourseResultState
{
    DRAW,
    STOP,
    RECORD,
    FADEOUT,
};

class SceneCourseResult : public vScene
{
private:
    eCourseResultState _state;
    InputMask _inputAvailable;

protected:
    bool _scoreSyncFinished = false;
    bool _retryRequested = false;
    std::shared_ptr<vScore> _pScoreOld;

    vRuleset::BasicData d1p;
    std::map<unsigned, unsigned> judgeCount;
    unsigned totalNotes = 0;

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