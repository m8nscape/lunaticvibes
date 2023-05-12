#if _WIN32 || _WIN64
#include "common/pch.h"
#include "input_mgr.h"
#include "input_dinput8.h"

namespace lunaticvibes
{

void initInput()
{
    InputDirectInput8::inst();
}

void refreshInputDevices()
{
    InputDirectInput8::inst().refreshDevices();
}

void pollInput()
{
    InputDirectInput8::inst().poll();
}

bool isKeyPressed(Input::Keyboard key)
{
    // these are mappings toward enum Input::Keyboard
    // refer to virtual key definitions in MSDN
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    static const int vkMap[] =
    {
        0,
        DIK_ESCAPE,

        DIK_1,
        DIK_2,
        DIK_3,
        DIK_4,
        DIK_5,
        DIK_6,
        DIK_7,
        DIK_8,
        DIK_9,
        DIK_0,
        DIK_MINUS,
        DIK_EQUALS,
        DIK_BACK,

        DIK_TAB,
        DIK_Q,
        DIK_W,
        DIK_E,
        DIK_R,
        DIK_T,
        DIK_Y,
        DIK_U,
        DIK_I,
        DIK_O,
        DIK_P,
        DIK_LBRACKET,
        DIK_RBRACKET,

        DIK_RETURN,
        DIK_LCONTROL,

        DIK_A,
        DIK_S,
        DIK_D,
        DIK_F,
        DIK_G,
        DIK_H,
        DIK_J,
        DIK_K,
        DIK_L,
        DIK_SEMICOLON,
        DIK_APOSTROPHE,

        DIK_GRAVE,
        DIK_LSHIFT,
        DIK_BACKSLASH,

        DIK_Z,
        DIK_X,
        DIK_C,
        DIK_V,
        DIK_B,
        DIK_N,
        DIK_M,
        DIK_COMMA,
        DIK_PERIOD,
        DIK_SLASH,
        DIK_RSHIFT,

        0,  // PRTSC
        DIK_LMENU,
        DIK_SPACE,
        DIK_CAPITAL,

        DIK_F1,
        DIK_F2,
        DIK_F3,
        DIK_F4,
        DIK_F5,
        DIK_F6,
        DIK_F7,
        DIK_F8,
        DIK_F9,
        DIK_F10,
        DIK_NUMLOCK,
        DIK_SCROLL,

        DIK_NUMPAD7,
        DIK_NUMPAD8,
        DIK_NUMPAD9,
        DIK_SUBTRACT,
        DIK_NUMPAD4,
        DIK_NUMPAD5,
        DIK_NUMPAD6,
        DIK_ADD,
        DIK_NUMPAD1,
        DIK_NUMPAD2,
        DIK_NUMPAD3,
        DIK_NUMPAD0,
        DIK_DECIMAL,
        DIK_SYSRQ,

        DIK_F11,
        DIK_F12,
        DIK_F13,
        DIK_F14,
        DIK_F15,

        DIK_PAUSE,
        DIK_INSERT,
        DIK_DELETE,
        DIK_HOME,
        DIK_END,
        DIK_PRIOR,
        DIK_NEXT,

        DIK_RMENU,
        DIK_RCONTROL,

        DIK_LEFT,
        DIK_UP,
        DIK_RIGHT,
        DIK_DOWN,

        DIK_YEN,
        DIK_NOCONVERT,
        DIK_CONVERT,
        DIK_KANA,

        DIK_NUMPADSLASH,
        DIK_NUMPADSTAR,
        DIK_NUMPADENTER,
    };

    int vk = vkMap[static_cast<size_t>(key)];
    return InputDirectInput8::inst().getKeyboardState()[vk] & 0x80;
}

bool isButtonPressed(Input::Joystick c, double deadzone)
{
    if (deadzone < 0.01)
        deadzone = 0.01;

    if (c.device < InputDirectInput8::inst().getJoystickCount())
    {
        auto& stat = InputDirectInput8::inst().getJoystickState(c.device);
        switch (c.type)
        {
        case Input::Joystick::Type::BUTTON:
            return stat.rgbButtons[c.index];
        case Input::Joystick::Type::POV:
            if (LOWORD(stat.rgdwPOV[LOWORD(c.index)]) != 0xFFFF)
            {
                float x = std::sinf(stat.rgdwPOV[c.index & 0xFFFFFFF] / 100.0f * 0.0174532925f);
                float y = std::cosf(stat.rgdwPOV[c.index & 0xFFFFFFF] / 100.0f * 0.0174532925f);

                if (x < -0.01f && (c.index & (1ul << 31))) return true;
                else if (y < -0.01f && (c.index & (1ul << 30))) return true;
                else if (y > 0.01f && (c.index & (1ul << 29))) return true;
                else if (x > 0.01f && (c.index & (1ul << 28))) return true;
            }
            return false;
        case Input::Joystick::Type::AXIS_RELATIVE_POSITIVE:
            switch (c.index)
            {
            case 0: return stat.lX != 0 && (stat.lX - 32767) / 32767.0 >= deadzone;
            case 1: return stat.lY != 0 && (stat.lY - 32767) / 32767.0 >= deadzone;
            case 2: return stat.lZ != 0 && (stat.lZ - 32767) / 32767.0 >= deadzone;
            case 3: return stat.lRx != 0 && (stat.lRx - 32767) / 32767.0 >= deadzone;
            case 4: return stat.lRy != 0 && (stat.lRy - 32767) / 32767.0 >= deadzone;
            case 5: return stat.lRz != 0 && (stat.lRz - 32767) / 32767.0 >= deadzone;
            case 6: return stat.rglSlider[0] != 0 && (stat.rglSlider[0] - 32767) / 32767.0 >= deadzone;
            case 7: return stat.rglSlider[1] != 0 && (stat.rglSlider[1] - 32767) / 32767.0 >= deadzone;
            }
            break;
        case Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE:
            switch (c.index)
            {
            case 0: return stat.lX != 0 && (stat.lX - 32767) / -32767.0 >= deadzone;
            case 1: return stat.lY != 0 && (stat.lY - 32767) / -32767.0 >= deadzone;
            case 2: return stat.lZ != 0 && (stat.lZ - 32767) / -32767.0 >= deadzone;
            case 3: return stat.lRx != 0 && (stat.lRx - 32767) / -32767.0 >= deadzone;
            case 4: return stat.lRy != 0 && (stat.lRy - 32767) / -32767.0 >= deadzone;
            case 5: return stat.lRz != 0 && (stat.lRz - 32767) / -32767.0 >= deadzone;
            case 6: return stat.rglSlider[0] != 0 && (stat.rglSlider[0] - 32767) / -32767.0 >= deadzone;
            case 7: return stat.rglSlider[1] != 0 && (stat.rglSlider[1] - 32767) / -32767.0 >= deadzone;
            }
        }
    }
    return false;
}

double getJoystickAxis(size_t device, Input::Joystick::Type type, size_t index)
{
    if (device < InputDirectInput8::inst().getJoystickCount())
    {
        auto& stat = InputDirectInput8::inst().getJoystickState(device);
        switch (type)
        {
        case Input::Joystick::Type::AXIS_RELATIVE_POSITIVE:
            switch (index)
            {
            case 0: return stat.lX != 0 ? (stat.lX - 32767) / 32767.0 : -1.0;
            case 1: return stat.lY != 0 ? (stat.lY - 32767) / 32767.0 : -1.0;
            case 2: return stat.lZ != 0 ? (stat.lZ - 32767) / 32767.0 : -1.0;
            case 3: return stat.lRx != 0 ? (stat.lRx - 32767) / 32767.0 : -1.0;
            case 4: return stat.lRy != 0 ? (stat.lRy - 32767) / 32767.0 : -1.0;
            case 5: return stat.lRz != 0 ? (stat.lRz - 32767) / 32767.0 : -1.0;
            case 6: return stat.rglSlider[0] != 0 ? (stat.rglSlider[0] - 32767) / 32767.0 : -1.0;
            case 7: return stat.rglSlider[1] != 0 ? (stat.rglSlider[1] - 32767) / 32767.0 : -1.0;
            }
            break;
        case Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE:
            switch (index)
            {
            case 0: return stat.lX != 0 ? (stat.lX - 32767) / -32767.0 : -1.0;
            case 1: return stat.lY != 0 ? (stat.lY - 32767) / -32767.0 : -1.0;
            case 2: return stat.lZ != 0 ? (stat.lZ - 32767) / -32767.0 : -1.0;
            case 3: return stat.lRx != 0 ? (stat.lRx - 32767) / -32767.0 : -1.0;
            case 4: return stat.lRy != 0 ? (stat.lRy - 32767) / -32767.0 : -1.0;
            case 5: return stat.lRz != 0 ? (stat.lRz - 32767) / -32767.0 : -1.0;
            case 6: return stat.rglSlider[0] != 0 ? (stat.rglSlider[0] - 32767) / -32767.0 : -1.0;
            case 7: return stat.rglSlider[1] != 0 ? (stat.rglSlider[1] - 32767) / -32767.0 : -1.0;
            }
        case Input::Joystick::Type::AXIS_ABSOLUTE:
            switch (index)
            {
            case 0: return stat.lX != 0 ? stat.lX / 65535.0 : -1.0;
            case 1: return stat.lY != 0 ? stat.lY / 65535.0 : -1.0;
            case 2: return stat.lZ != 0 ? stat.lZ / 65535.0 : -1.0;
            case 3: return stat.lRx != 0 ? stat.lRx / 65535.0 : -1.0;
            case 4: return stat.lRy != 0 ? stat.lRy / 65535.0 : -1.0;
            case 5: return stat.lRz != 0 ? stat.lRz / 65535.0 : -1.0;
            case 6: return stat.rglSlider[0] != 0 ? stat.rglSlider[0] / 65535.0 : -1.0;
            case 7: return stat.rglSlider[1] != 0 ? stat.rglSlider[1] / 65535.0 : -1.0;
            }
        }
    }
    return -1.0;
}

bool isMouseButtonPressed(int idx)
{
    switch (idx)
    {
    case 1:
    case 2:
    case 3:
    case 4: return InputDirectInput8::inst().getMouseState().rgbButtons[idx - 1] & 0x80;
    default: return false;
    }
}

short getLastMouseWheelState()
{
    int z = InputDirectInput8::inst().getMouseState().lZ;
    return z == 0 ? 0 : z / WHEEL_DELTA;
}

}

#endif
