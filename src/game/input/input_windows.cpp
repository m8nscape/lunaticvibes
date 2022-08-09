#if _WIN32 || _WIN64
#include "input_mgr.h"
#include "input_dinput8.h"

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
    // refer to virtual key definition in MSDN
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

        DIK_RCONTROL,
        DIK_LEFT,
        DIK_UP,
        DIK_RIGHT,
        DIK_DOWN,

        DIK_YEN,
        DIK_NOCONVERT,
        DIK_CONVERT,
        DIK_KANA,
    };

    int vk = vkMap[static_cast<size_t>(key)];
    return InputDirectInput8::inst().getKeyboardState()[vk] & 0x80;
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

#endif