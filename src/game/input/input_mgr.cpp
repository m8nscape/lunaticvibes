#include "logger.h"
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

//#define bindKey(_, kk, idx, sfk) _inst.bindings[kk][idx] = { KEYBOARD, 0, (int)sfk }
void InputMgr::updateBindings(k K)
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
	/*
	{
		bindKey(7, Input::S1L, 0, sf::Keyboard::LShift);
		bindKey(7, Input::S1R, 0, sf::Keyboard::LControl);
		bindKey(7, Input::S1R, 1, sf::Keyboard::Space);
		bindKey(7, Input::K11, 0, sf::Keyboard::Z);
		bindKey(7, Input::K12, 0, sf::Keyboard::S);
		bindKey(7, Input::K13, 0, sf::Keyboard::X);
		bindKey(7, Input::K14, 0, sf::Keyboard::D);
		bindKey(7, Input::K15, 0, sf::Keyboard::C);
		bindKey(7, Input::K16, 0, sf::Keyboard::F);
		bindKey(7, Input::K17, 0, sf::Keyboard::V);
		bindKey(7, Input::K1START, 0, sf::Keyboard::Q);
		bindKey(7, Input::K1SELECT, 0, sf::Keyboard::W);

		bindKey(7, Input::S2L, 0, sf::Keyboard::RShift);
		bindKey(7, Input::S2R, 0, sf::Keyboard::RControl);
		bindKey(7, Input::K21, 0, sf::Keyboard::M);
		bindKey(7, Input::K22, 0, sf::Keyboard::K);
		bindKey(7, Input::K23, 0, sf::Keyboard::Comma);
		bindKey(7, Input::K24, 0, sf::Keyboard::L);
		bindKey(7, Input::K25, 0, sf::Keyboard::Period);
		bindKey(7, Input::K26, 0, sf::Keyboard::SemiColon);
		bindKey(7, Input::K27, 0, sf::Keyboard::Slash);
		bindKey(7, Input::K2START, 0, sf::Keyboard::P);
		bindKey(7, Input::K2SELECT, 0, sf::Keyboard::RBracket);
	}
	*/

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
				//if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(b.scancode)))
				//	res[k] = true;
				break;
			case JOYSTICK:
				//if (sf::Joystick::isButtonPressed(b.device, b.scancode))
				//	res[k] = true;
				break;
			case CONTROLLER:
			case MOUSE:
				break;
			}
			if (res[k]) break;
        }
    }

    // FN input
    // TODO FN input

    return res;
}

