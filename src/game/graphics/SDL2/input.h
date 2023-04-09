#include "common/keymap.h"

#include <SDL.h>

static constexpr size_t SDL_MOUSE_BUTTON_COUNT = 256;

namespace sdl::state {

// `true` for keys currently being pressed down.
inline bool g_keyboard_scancodes[SDL_NUM_SCANCODES];
// `true` for buttons currently being pressed down.
inline bool g_mouse_buttons[SDL_MOUSE_BUTTON_COUNT];
inline int g_mouse_x = 0;
inline int g_mouse_y = 0;
// Reset this to `0` after use.
inline short g_mouse_wheel_delta = 0;

} // namespace sdl::state

unsigned char sdl_key_from_common_scancode(Input::Keyboard key);
