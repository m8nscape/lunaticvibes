#pragma once
#include <mutex>
#include "scene.h"

enum class eDecideState
{
    START,
    SKIP,
    CANCEL,
};

class SceneDecide : public vScene
{
private:
    eDecideState _state;
    InputMask _inputAvailable;

public:
    SceneDecide();
    virtual ~SceneDecide() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    std::function<void()> _updateCallback;
    void updateStart();
    void updateSkip();
    void updateCancel();

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, Time);
    void inputGameHold(InputMask&, Time);
    void inputGameRelease(InputMask&, Time);
};