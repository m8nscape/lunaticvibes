#pragma once
#include "scene.h"
#include "common/keymap.h"

class SceneKeyConfig : public SceneBase
{
private:
    bool exiting = false;
    std::shared_mutex _mutex;

public:
    SceneKeyConfig();
    virtual ~SceneKeyConfig();

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    std::function<void()> _updateCallback;
    void updateStart();
    void updateMain();
    void updateFadeout();

protected:
    std::map<Input::Pad, long long> forceBargraphTriggerTimestamp;
    void updateForceBargraphs();

    void updateInfo(KeyMap k, int slot);
    void updateAllText();

    std::map<size_t, JoystickMask> joystickPrev;
    std::array<double, 2>   playerTurntableAngleAdd{ 0 };

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const Time&);
    void inputGameAxis(double s1, double s2, const Time&);
    void inputGamePressKeyboard(KeyboardMask&, const Time&);
    void inputGamePressJoystick(JoystickMask&, size_t device, const Time&);
    void inputGameAbsoluteAxis(JoystickAxis&, size_t device, const Time&);

public:
    static void setInputBindingText(GameModeKeys keys, Input::Pad pad);
};