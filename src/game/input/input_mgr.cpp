#include <plog/Log.h>
#include "input_mgr.h"

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

#define bindKey(_, kk, idx, igk) _inst.bindings[kk][idx] = { KEYBOARD, 0, igk }
void InputMgr::updateBindings(Ingame K)
{
    // Clear current bindings
    for (auto& k : _inst.bindings)
        k.clear();

    // Update key binding
	// TODO get bindings from config
    //for (size_t k = S1L; k < ESC; k++)
    //{
    //    bindings[k].push_back(...);
    //}

	// for debug purpose
	// TODO to be removed
	{
		bindKey(7, Input::S1L, 0, Input::Key::K_LSHIFT);
		bindKey(7, Input::S1R, 0, Input::Key::K_LCTRL);
		bindKey(7, Input::S1R, 1, Input::Key::K_SPACE);
		bindKey(7, Input::K11, 0, Input::Key::K_Z);
		bindKey(7, Input::K12, 0, Input::Key::K_S);
		bindKey(7, Input::K13, 0, Input::Key::K_X);
		bindKey(7, Input::K14, 0, Input::Key::K_D);
		bindKey(7, Input::K15, 0, Input::Key::K_C);
		bindKey(7, Input::K16, 0, Input::Key::K_F);
		bindKey(7, Input::K17, 0, Input::Key::K_V);
        bindKey(7, Input::K1START, 0, Input::Key::K_TAB);
		bindKey(7, Input::K1SELECT, 0, Input::Key::K_Q);

		bindKey(7, Input::S2L, 0, Input::Key::K_RCTRL);
		bindKey(7, Input::S2R, 0, Input::Key::K_RSHIFT);
		bindKey(7, Input::K21, 0, Input::Key::K_M);
		bindKey(7, Input::K22, 0, Input::Key::K_K);
		bindKey(7, Input::K23, 0, Input::Key::K_COMMA);
		bindKey(7, Input::K24, 0, Input::Key::K_L);
		bindKey(7, Input::K25, 0, Input::Key::K_DOT);
		bindKey(7, Input::K26, 0, Input::Key::K_SEMICOLON);
		bindKey(7, Input::K27, 0, Input::Key::K_SLASH);
		bindKey(7, Input::K2START, 0, Input::Key::K_BACKSLASH);
		bindKey(7, Input::K2SELECT, 0, Input::Key::K_RBRACKET);
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

