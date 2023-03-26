#include "common/log.h"
#include "input_mgr.h"
#include "common/sysutil.h"
#include "config/config_mgr.h"
#include "game/graphics/graphics.h"

#ifdef RENDER_SDL2
#include <game/graphics/SDL2/input.h>
#endif

InputMgr InputMgr::_inst;

using namespace Input;

void InputMgr::init()
{
    initInput();

    setDebounceTime(ConfigMgr::get('P', cfg::P_MIN_INPUT_INTERVAL, 16));
}

void InputMgr::updateDevices()
{
    // Check jostick connection status
    refreshInputDevices();
}

void InputMgr::updateBindings(GameModeKeys keys)
{
    // Clear current bindings
    for (auto& k : _inst.padBindings)
        k.reset();

    switch (keys)
    {
    case 10: keys = 5; break;
    case 14: keys = 7; break;
    }
    switch (keys)
    {
    case 5:
    case 7:
    case 9:
        for (Input::Pad key = Input::S1L; key < Input::ESC; ++(*(int*)&key))
        {
            _inst.padBindings[key] = ConfigMgr::Input(keys)->getBindings(key);
        }
        break;

    default: break;
    }

    updateDeadzones(keys);
    LOG_INFO << "Key bindings updated";
}

void InputMgr::updateBindings(GameModeKeys keys, Pad K)
{
    updateBindings(keys);
}

void InputMgr::updateDeadzones(GameModeKeys keys)
{
    using namespace cfg;
    _inst.padDeadzones[Input::S1L] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S1L, 0.2);
    _inst.padDeadzones[Input::S1R] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S1R, 0.2);
    _inst.padDeadzones[Input::K1START] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K1Start, 0.2);
    _inst.padDeadzones[Input::K1SELECT] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K1Select, 0.2);
    _inst.padDeadzones[Input::K11] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K11, 0.2);
    _inst.padDeadzones[Input::K12] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K12, 0.2);
    _inst.padDeadzones[Input::K13] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K13, 0.2);
    _inst.padDeadzones[Input::K14] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K14, 0.2);
    _inst.padDeadzones[Input::K15] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K15, 0.2);
    _inst.padDeadzones[Input::K16] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K16, 0.2);
    _inst.padDeadzones[Input::K17] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K17, 0.2);
    _inst.padDeadzones[Input::K18] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K18, 0.2);
    _inst.padDeadzones[Input::K19] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K19, 0.2);
    _inst.padDeadzones[Input::S2L] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S2L, 0.2);
    _inst.padDeadzones[Input::S2R] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S2R, 0.2);
    _inst.padDeadzones[Input::K2START] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K2Start, 0.2);
    _inst.padDeadzones[Input::K2SELECT] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K2Select, 0.2);
    _inst.padDeadzones[Input::K21] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K21, 0.2);
    _inst.padDeadzones[Input::K22] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K22, 0.2);
    _inst.padDeadzones[Input::K23] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K23, 0.2);
    _inst.padDeadzones[Input::K24] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K24, 0.2);
    _inst.padDeadzones[Input::K25] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K25, 0.2);
    _inst.padDeadzones[Input::K26] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K26, 0.2);
    _inst.padDeadzones[Input::K27] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K27, 0.2);
    _inst.padDeadzones[Input::K28] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K28, 0.2);
    _inst.padDeadzones[Input::K29] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K29, 0.2);
    _inst.padDeadzones[Input::S1A] = ConfigMgr::Input(keys)->get(I_INPUT_SPEED_S1A, 0.5);
    _inst.padDeadzones[Input::S2A] = ConfigMgr::Input(keys)->get(I_INPUT_SPEED_S2A, 0.5);
}

double InputMgr::getDeadzone(Input::Pad k)
{
    return _inst.padDeadzones[k];
}


#ifdef RENDER_SDL2
#include "SDL_mouse.h"
#endif

std::bitset<KEY_COUNT> InputMgr::_detect()
{
    pollInput();

    std::bitset<KEY_COUNT> res{};
    scratch1 = 0.0;
    scratch2 = 0.0;

    Time t;

    // game input
    for (int k = S1L; k < LANE_COUNT; k++)
    {
        KeyMap& b = padBindings[k];
        {
			switch (b.getType())
			{
            case KeyMap::DeviceType::KEYBOARD:
                if (isKeyPressed(b.getKeyboard()))
                {
                    res[k] = true;
                    pressedTime[k] = t;
                }
				break;
			case KeyMap::DeviceType::JOYSTICK:
                if (isButtonPressed(b.getJoystick(), padDeadzones[k]))
                {
                    res[k] = true;
                    pressedTime[k] = t;
                }
				break;
			case KeyMap::DeviceType::MOUSE:
				break;
			}
			//if (res[k]) break;
        }
    }

    for (int k = S1L; k < LANE_COUNT; k++)
    {
        if (!res[k] && (t - pressedTime[k]).norm() <= debounceTime)
            res[k] = true;
    }

    if (padBindings[S1A].getType() == KeyMap::DeviceType::JOYSTICK)
    {
        const auto j = padBindings[S1A].getJoystick();
        if (j.type == Input::Joystick::Type::AXIS_ABSOLUTE)
            scratch1 = getJoystickAxis(j.device, j.type, j.index);
    }
    if (padBindings[S2A].getType() == KeyMap::DeviceType::JOYSTICK)
    {
        const auto j = padBindings[S2A].getJoystick();
        if (j.type == Input::Joystick::Type::AXIS_ABSOLUTE)
            scratch2 = getJoystickAxis(j.device, j.type, j.index);
    }

    // FN input
    res[ESC] = isKeyPressed(Keyboard::K_ESC);
    res[F1] = isKeyPressed(Keyboard::K_F1);
    res[F2] = isKeyPressed(Keyboard::K_F2);
    res[F3] = isKeyPressed(Keyboard::K_F3);
    res[F4] = isKeyPressed(Keyboard::K_F4);
    res[F5] = isKeyPressed(Keyboard::K_F5);
    res[F6] = isKeyPressed(Keyboard::K_F6);
    res[F7] = isKeyPressed(Keyboard::K_F7);
    res[F8] = isKeyPressed(Keyboard::K_F8);
    res[F9] = isKeyPressed(Keyboard::K_F9);
    res[F10] = isKeyPressed(Keyboard::K_F10);
    res[F11] = isKeyPressed(Keyboard::K_F11);
    res[F12] = isKeyPressed(Keyboard::K_F12);
    res[F13] = isKeyPressed(Keyboard::K_F13);
    res[F14] = isKeyPressed(Keyboard::K_F14);
    res[F15] = isKeyPressed(Keyboard::K_F15);
    res[UP] = isKeyPressed(Keyboard::K_UP);
    res[DOWN] = isKeyPressed(Keyboard::K_DOWN);
    res[LEFT] = isKeyPressed(Keyboard::K_LEFT);
    res[RIGHT] = isKeyPressed(Keyboard::K_RIGHT);
    res[INSERT] = isKeyPressed(Keyboard::K_INS);
    res[DEL] = isKeyPressed(Keyboard::K_DEL);
    res[HOME] = isKeyPressed(Keyboard::K_HOME);
    res[END] = isKeyPressed(Keyboard::K_END);
    res[PGUP] = isKeyPressed(Keyboard::K_PGUP);
    res[PGDN] = isKeyPressed(Keyboard::K_PGDN);
    res[RETURN] = isKeyPressed(Keyboard::K_ENTER) || isKeyPressed(Keyboard::K_NUM_ENTER);
    res[BACKSPACE] = isKeyPressed(Keyboard::K_BKSP);

    res[M1] = isMouseButtonPressed(1);
    res[M2] = isMouseButtonPressed(2);
    res[M3] = isMouseButtonPressed(3);
    res[M4] = isMouseButtonPressed(4);
    res[M5] = isMouseButtonPressed(5);

    auto mouseWheelState = getLastMouseWheelState();
    res[MWHEELUP] = mouseWheelState > 0;
    res[MWHEELDOWN] = mouseWheelState < 0;

    return res;
}
std::bitset<KEY_COUNT> InputMgr::detect()
{
    return _inst._detect();
}


bool InputMgr::getMousePos(int& x, int& y)
{
#ifdef WIN32
    bool ret = getMouseCursorPos(x, y);
#elif defined(RENDER_SDL2)
    x = sdl::state::g_mouse_x;
    y = sdl::state::g_mouse_y;
    bool ret = true;
#else
#error "No mouse pos getting implementation"
#endif
    if (ret)
    {
        double canvasScaleX = graphics_get_canvas_scale_x();
        double canvasScaleY = graphics_get_canvas_scale_y();
        if (canvasScaleX != 1.0) x = (int)std::floor(x / canvasScaleX);
        if (canvasScaleY != 1.0) y = (int)std::floor(y / canvasScaleY);
    }
    return ret;
}

bool InputMgr::getScratchPos(double& x, double& y)
{
    x = _inst.scratch1;
    y = _inst.scratch2;
    return true;
}

void InputMgr::setDebounceTime(int ms)
{
    _inst.debounceTime = ms;
}