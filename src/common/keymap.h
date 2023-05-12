#pragma once

namespace lunaticvibes
{

namespace Input
{

enum Pad
{
    INVALID = -1,

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

    LANE_COUNT,

    S1A = LANE_COUNT,
    S2A,

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
    MWHEELUP,
    MWHEELDOWN,

    KEY_COUNT
};

// Ingame key definitions, mostly based on scancodes.
enum class Keyboard : unsigned
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

    K_RALT,
    K_RCTRL,

    K_LEFT,
    K_UP,
    K_RIGHT,
    K_DOWN,

    // Japanese
    K_JP_YEN,
    K_JP_NOCONVERT,
    K_JP_CONVERT,
    K_JP_KANA,

    // scancodes with mask (e0)
    K_NUM_SLASH,
    K_NUM_STAR,
    K_NUM_ENTER,

    K_COUNT
};
inline constexpr size_t keyboardKeyCount = static_cast<size_t>(Keyboard::K_COUNT);

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

    "RALT",
    "RCTRL",

    "LEFT",
    "UP",
    "RIGHT",
    "DOWN",

    // Japanese
    "JP_YEN",
    "JP_NOCONVERT",
    "JP_CONVERT",
    "JP_KANA",
};


enum class JoystickAxis
{
    X,
    Y,
    Z,
    Rx,
    Ry,
    Rz,
    Slider1,
    Slider2,

    COUNT
};
inline const char* joystickAxisName[(size_t)JoystickAxis::COUNT] =
{
    "X",
    "Y",
    "Z",
    "Rx",
    "Ry",
    "Rz",
    "Slider1",
    "Slider2",
};
struct Joystick
{
    size_t device;
    enum class Type
    {
        UNDEF,
        BUTTON,
        POV,    // bit31:left bit30:down bit29:up bit28:right
        AXIS_RELATIVE_POSITIVE,
        AXIS_RELATIVE_NEGATIVE,
        AXIS_ABSOLUTE,
    } type;
    size_t index;
};

}

class KeyMap
{
public:
    KeyMap() = default;
    KeyMap(const std::string_view& name) { loadFromString(name); }
    KeyMap(const Input::Keyboard& kb) { setKeyboard(kb); }
    KeyMap(size_t device, Input::Joystick::Type type, size_t index) { setJoystick(device, type, index); }
    ~KeyMap() = default;

    enum class DeviceType
    {
        UNDEF,
        KEYBOARD,
        JOYSTICK,
        MOUSE,          // ???
    };

    typedef int DeviceID;

protected:
    DeviceType type = DeviceType::UNDEF;
    union
    {
        Input::Keyboard keyboard;
        Input::Joystick joystick;
    };

public:
    void reset() { type = DeviceType::UNDEF; }
    DeviceType getType() const { return type; }
    std::string toString() const;

    Input::Keyboard getKeyboard() const { assert(type == DeviceType::KEYBOARD); return keyboard; }
    void setKeyboard(Input::Keyboard kb);

    Input::Joystick getJoystick() const { assert(type == DeviceType::JOYSTICK); return joystick; }
    void setJoystick(size_t device, Input::Joystick::Type jtype, size_t index);

private:
    void loadFromString(const std::string_view& name);
    void loadFromStringK(const std::string_view& name);
    void loadFromStringJ(const std::string_view& name);
    void loadFromStringM(const std::string_view& name);
    std::string toStringK() const;
    std::string toStringJ() const;
    std::string toStringM() const;
};

}
