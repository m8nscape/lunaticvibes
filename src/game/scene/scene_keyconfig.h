#pragma once
#include "scene.h"
#include "game/input/input_keys.h"

class SceneKeyConfig : public vScene
{
private:
    bool _exiting = false;

public:
    SceneKeyConfig();
    virtual ~SceneKeyConfig() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    std::function<void()> _updateCallback;
    void updateStart();
    void updateMain();
    void updateFadeout();

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const Time&);
    void inputGamePressKeyboard(KeyboardMask&, const Time&);

public:
    static void setText(int keys, Input::Pad pad);
};