#ifndef WIN32
#ifdef RENDER_SDL2

#include "game/graphics/SDL2/input.h"

void initInput()
{
	// No-op.
}

void refreshInputDevices()
{
	// TODO?
}

void pollInput()
{
	// No-op.
	// We don't need to do anything, since all the updating is done by
	// window events.
}

bool isKeyPressed(Input::Keyboard key)
{
	return sdl::state::g_keyboard_scancodes[sdl_key_from_common_scancode(key)];
}

bool isButtonPressed(Input::Joystick c, double deadzone)
{
	// TODO
	return false;
}
double getJoystickAxis(size_t device, Input::Joystick::Type type, size_t index)
{
	// TODO
	return 0.;
}

bool isMouseButtonPressed(int idx)
{
	// Lunaticvibes expects middle and right mouse buttons to be swapped.
	switch (idx) {
	case 2: idx = 3; break;
	case 3: idx = 2; break;
	default: break;
	}

	return sdl::state::g_mouse_buttons[idx];
}

short getLastMouseWheelState()
{
	auto state = sdl::state::g_mouse_wheel_delta;
	sdl::state::g_mouse_wheel_delta = 0;
	return state;
}

#endif // RENDER_SDL2
#endif // WIN32
