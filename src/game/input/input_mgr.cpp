#include "common/log.h"
#include "input_mgr.h"
#include "common/sysutil.h"
#include "config/config_mgr.h"
#include "game/graphics/graphics.h"

#ifdef _WIN32
#include "input_dinput8.h"
#endif

InputMgr InputMgr::_inst;

using namespace Input;

void InputMgr::init()
{
    initInput();
}

void InputMgr::updateDevices()
{
    // Check jostick connection status
    refreshInputDevices();
}

void InputMgr::updateBindings(GameModeKeys keys, Pad K)
{
    // Clear current bindings
    for (auto& k : _inst.padBindings)
        k.reset();

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

    LOG_INFO << "Key bindings updated";
}

#ifdef RENDER_SDL2
#include "SDL_mouse.h"
#endif

std::bitset<KEY_COUNT> InputMgr::detect()
{
    pollInput();

    std::bitset<KEY_COUNT> res{};

    // game input
    for (int k = S1L; k < ESC; k++)
    {
        KeyMap& b = _inst.padBindings[k];
        {
			switch (b.getType())
			{
            case KeyMap::DeviceType::KEYBOARD:
                if (isKeyPressed(b.getKeyboard()))
                    res[k] = true;
				break;
			case KeyMap::DeviceType::JOYSTICK:
                if (isButtonPressed(b.getJoystick(), 0.2))
                    res[k] = true;
				break;
			case KeyMap::DeviceType::MOUSE:
				break;
			}
			//if (res[k]) break;
        }
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
    res[RETURN] = isKeyPressed(Keyboard::K_ENTER);
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


bool InputMgr::getMousePos(int& x, int& y)
{
    bool ret = getMouseCursorPos(x, y);
    if (ret)
    {
        double canvasScaleX = graphics_get_canvas_scale_x();
        double canvasScaleY = graphics_get_canvas_scale_y();
        if (canvasScaleX != 1.0) x = (int)std::floor(x / canvasScaleX);
        if (canvasScaleY != 1.0) y = (int)std::floor(y / canvasScaleY);
    }
    return ret;
}
