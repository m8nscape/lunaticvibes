#pragma once
#include "scene.h"
#include "common/keymap.h"

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
#ifdef RAWINPUT_AVAILABLE
    void inputGamePressRawinput(int deviceID, RawinputKeyMap& button, RawinputAxisDiffMap& axisDiff, const Time&);
#endif

public:
    static void setInputBindingText(GameModeKeys keys, Input::Pad pad);
};