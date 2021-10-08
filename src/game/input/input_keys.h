#pragma once
#include <string>

namespace Input
{
    const std::size_t MAX_JOYSTICK_COUNT = 8;
    const std::size_t MAX_BINDINGS_PER_KEY = 10;

    enum Pad
    {
        S1L = 0,
        S1R,
        K11,
        K12,
        K13,
        K14,
        K15,
        K16,
        K17,
        K18,
        K19,
        K1START,
        K1SELECT,
        K1SPDUP,
        K1SPDDN,

        S2L,
        S2R,
        K21,
        K22,
        K23,
        K24,
        K25,
        K26,
        K27,
        K28,
        K29,
        K2START,
        K2SELECT,
        K2SPDUP,
        K2SPDDN,

        ESC,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        INSERT,
        DEL,
        HOME,
        END,
        PGUP,
        PGDN,
        RETURN,
        BACKSPACE,

		M1,
		M2,
		M3,
		M4,
		M5,

        KEY_COUNT
    };

    // Ingame key definitions, mostly based on scancodes.
    // I hate jp keyboard layout. Just mentioning.
    enum Keyboard: unsigned
    {
        K_ERROR = 0,

        K_ESC,

        // 0x02
        K_1,
        K_2,
        K_3,
        K_4,
        K_5,
        K_6,
        K_7,
        K_8,
        K_9,
        K_0,
        K_MINUS,
        K_EQUAL,            // US =+    JP ^ overbar
        K_BKSP,

        // 0x0f
        K_TAB,
        K_Q,
        K_W,
        K_E,
        K_R,
        K_T,
        K_Y,
        K_U,
        K_I,
        K_O,
        K_P,
        K_LBRACKET,         // US [{    JP @`
        K_RBRACKET,         // US ]}    JP [{

        K_ENTER,
        K_LCTRL,

        // 0x1e
        K_A,
        K_S,
        K_D,
        K_F,
        K_G,
        K_H,
        K_J,
        K_K,
        K_L,
        K_SEMICOLON,        // US ;:    JP ;+
        K_APOSTROPHE,       // US '"    JP :* 

        K_TYPEWRITER_APS,   // `~        JP hankaku zenkaku
        K_LSHIFT,
        K_BACKSLASH,        // US \|    JP ]}

        // 0x2c
        K_Z,
        K_X,
        K_C,
        K_V,
        K_B,
        K_N,
        K_M,
        K_COMMA,
        K_DOT,
        K_SLASH,
        K_RSHIFT,

        K_PRTSC,
        K_LALT,
        K_SPACE,
        K_CAPSLOCK,

        // 0x3b
        K_F1,
        K_F2,
        K_F3,
        K_F4,
        K_F5,
        K_F6,
        K_F7,
        K_F8,
        K_F9,
        K_F10,
        K_NUMLOCK,
        K_SCRLOCK,

        // 0x47
        K_NUM7,
        K_NUM8,
        K_NUM9,
        K_NUM_MINUS,
        K_NUM4,
        K_NUM5,
        K_NUM6,
        K_NUM_PLUS,
        K_NUM1,
        K_NUM2,
        K_NUM3,
        K_NUM0,
        K_NUM_DOT,
        K_SYSRQ,

        // Scancode of keys below varies, so I just place randomly
        K_F11,
        K_F12,
        K_F13,
        K_F14,
        K_F15,

        K_PAUSE,
        K_INS,
        K_DEL,
        K_HOME,
        K_END,
        K_PGUP,
        K_PGDN,

        K_RCTRL,

        K_LEFT,
        K_UP,
        K_RIGHT,
        K_DOWN,

        // Japanese
        K_JP_BACKSLASH,
        K_JP_NOCHANGE,
        K_JP_CHANGE,
        K_JP_HIRAGANA,

        K_COUNT
    };


    inline const char* keyboardNameMap[0xFF]
    {
        "",             // ERROR

        "ESC",

        // 0x02
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "0",
        "-",
        "=",            // US =+    JP ^ overbar
        "BackSpace",

        // 0x0f
        "TAB",
        "Q",
        "W",
        "E",
        "R",
        "T",
        "Y",
        "U",
        "I",
        "O",
        "P",
        "[",         // US [{    JP @`
        "]",         // US ]}    JP [{

        "Enter",
        "LCTRL",

        // 0x1e
        "A",
        "S",
        "D",
        "F",
        "G",
        "H",
        "J",
        "K",
        "L",
        ";",        // US ;:    JP ;+
        "'",        // US '"    JP :* 

        "`",        // `~        JP hankaku zenkaku
        "LSHIFT",
        "\\",       // US \|    JP ]}

        // 0x2c
        "Z",
        "X",
        "C",
        "V",
        "B",
        "N",
        "M",
        ",",
        ".",
        "/",
        "RSHIFT",

        "PRTSC",
        "LALT",
        "Space",
        "CapsLock",

        // 0x3b
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "NumLock",
        "ScrollLock",

        // 0x47
        "Numpad7",
        "Numpad8",
        "Numpad9",
        "Numpad-",
        "Numpad4",
        "Numpad5",
        "Numpad6",
        "Numpad+",
        "Numpad1",
        "Numpad2",
        "Numpad3",
        "Numpad0",
        "Numpad.",
        "SYSRQ",

        // Scancode of keys below varies, so I just place randomly
        "F11",
        "F12",
        "F13",
        "F14",
        "F15",

        "Pause",
        "INS",
        "DEL",
        "HOME",
        "END",
        "PGUP",
        "PGDN",

        "RCTRL"

        "LEFT",
        "UP",
        "RIGHT",
        "DOWN",

        // Japanese
        "JP_BACKSLASH",
        "JP_NOCHANGE",
        "JP_CHANGE",
        "JP_HIRAGANA",
    };

    inline Keyboard getByName(const std::string& name)
    {
        for (size_t i = 0; i < 0xFF; ++i)
            if (keyboardNameMap[i] != NULL && name == keyboardNameMap[i])
                return static_cast<Keyboard>(i);

        return Keyboard::K_ERROR;
    }
}