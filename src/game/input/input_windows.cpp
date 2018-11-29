#if _WIN32 || _WIN64
#include "input_mgr.h"
#include <Windows.h>

// refer to virtual key definition in MSDN
const std::array<int , Input::Key::K_COUNT> vkeyMap
{
    0,
    VK_ESCAPE,

    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    VK_OEM_MINUS,
    VK_OEM_PLUS,
    VK_BACK,

    VK_TAB,
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    VK_OEM_4,
    VK_OEM_6,

    VK_RETURN,
    VK_LCONTROL,

    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    VK_OEM_1,
    VK_OEM_7,

    VK_OEM_3,
    VK_LSHIFT,
    VK_OEM_5,

    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    VK_OEM_COMMA,
    VK_OEM_PERIOD,
    VK_OEM_2,
    VK_RSHIFT,

    VK_SNAPSHOT,
    VK_LMENU,
    VK_SPACE,
    VK_CAPITAL,

    VK_F1,
    VK_F2,
    VK_F3,
    VK_F4,
    VK_F5,
    VK_F6,
    VK_F7,
    VK_F8,
    VK_F9,
    VK_F10,
    VK_NUMLOCK,
    VK_SCROLL,

    VK_NUMPAD7,
    VK_NUMPAD8,
    VK_NUMPAD9,
    VK_SUBTRACT,
    VK_NUMPAD4,
    VK_NUMPAD5,
    VK_NUMPAD6,
    VK_ADD,
    VK_NUMPAD1,
    VK_NUMPAD2,
    VK_NUMPAD3,
    VK_NUMPAD0,
    VK_DECIMAL,
    0,

    VK_F11,
    VK_F12,
    VK_F13,
    VK_F14,
    VK_F15,

    VK_PAUSE,
    VK_INSERT,
    VK_DELETE,
    VK_HOME,
    VK_END,
    VK_PRIOR,
    VK_NEXT,

    VK_RCONTROL,

    VK_LEFT,
    VK_UP,
    VK_RIGHT,
    VK_DOWN,

    VK_OEM_102,
    VK_NONCONVERT,
    VK_CONVERT,
    VK_OEM_COPY,
};

bool isKeyPressed(Input::Key key)
{
    return GetAsyncKeyState(vkeyMap[key]);
}

#endif