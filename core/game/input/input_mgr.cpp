#include <plog/Log.h>
#include "input_mgr.h"
#include "config/config_mgr.h"

InputMgr InputMgr::_inst;

using namespace Input;

void InputMgr::updateDevices()
{
    // Check jostick connection status
    _inst.haveJoystick = false;
    //for (unsigned i = 0; i < sf::Joystick::Count; i++)
    //    if (sf::Joystick::isConnected(i))
    //    {
    //        _inst.joysticksConnected[i] = true;
    //        _inst.haveJoystick = true;
    //    }
}

#define bindKey(kk, idx, igk) _inst.bindings[kk][idx] = { KEYBOARD, 0, igk }
void InputMgr::updateBindings(unsigned keys, Ingame K)
{
    // Clear current bindings
    for (auto& k : _inst.bindings)
        k = {};

    switch (keys)
    {
    case 5:
        for (Input::Ingame key = Input::S1L; key < Input::ESC; ++(*(int*)&key))
        {
            auto bindings = ConfigMgr::I5.getBindings(key);
            for (unsigned slot = 0; slot < bindings.size(); ++slot)
                bindKey(key, slot, bindings[slot]);
        }
        break;

    case 7:
        for (Input::Ingame key = Input::S1L; key < Input::ESC; ++(*(int*)&key))
        {
            auto bindings = ConfigMgr::I7.getBindings(key);
            for (unsigned slot = 0; slot < bindings.size(); ++slot)
                bindKey(key, slot, bindings[slot]);
        }
        break;

    case 9:
        for (Input::Ingame key = Input::S1L; key < Input::ESC; ++(*(int*)&key))
        {
            auto bindings = ConfigMgr::I9.getBindings(key);
            for (unsigned slot = 0; slot < bindings.size(); ++slot)
                bindKey(key, slot, bindings[slot]);
        }
        break;

    default: break;
    }

    LOG_INFO << "Key bindings updated";
}

std::bitset<KEY_COUNT> InputMgr::detect()
{
    std::bitset<KEY_COUNT> res{};

    // game input
    for (int k = S1L; k < ESC; k++)
    {
        for (auto b : _inst.bindings[k])
        {
			switch (b.type)
			{
			case KEYBOARD:
                res[k] = isKeyPressed(b.key);
				break;
			case JOYSTICK:
                // TODO joystick
				break;
			case CONTROLLER:
			case MOUSE:
				break;
			}
			if (res[k]) break;
        }
    }

    // FN input
    res[ESC] = isKeyPressed(K_ESC);
    res[F1] = isKeyPressed(K_F1);
    res[F2] = isKeyPressed(K_F2);
    res[F3] = isKeyPressed(K_F3);
    res[F4] = isKeyPressed(K_F4);
    res[F5] = isKeyPressed(K_F5);
    res[F6] = isKeyPressed(K_F6);
    res[F7] = isKeyPressed(K_F7);
    res[F8] = isKeyPressed(K_F8);
    res[F9] = isKeyPressed(K_F9);
    res[F10] = isKeyPressed(K_F10);
    res[F11] = isKeyPressed(K_F11);
    res[F12] = isKeyPressed(K_F12);
    res[F13] = isKeyPressed(K_F13);
    res[F14] = isKeyPressed(K_F14);
    res[F15] = isKeyPressed(K_F15);
    res[UP] = isKeyPressed(K_UP);
    res[DOWN] = isKeyPressed(K_DOWN);
    res[LEFT] = isKeyPressed(K_LEFT);
    res[RIGHT] = isKeyPressed(K_RIGHT);
    res[INSERT] = isKeyPressed(K_INS);
    res[DELETE] = isKeyPressed(K_DEL);
    res[HOME] = isKeyPressed(K_HOME);
    res[END] = isKeyPressed(K_END);
    res[PGUP] = isKeyPressed(K_PGUP);
    res[PGDN] = isKeyPressed(K_PGDN);
    res[RETURN] = isKeyPressed(K_ENTER);
    res[BACKSPACE] = isKeyPressed(K_BKSP);

    return res;
}

