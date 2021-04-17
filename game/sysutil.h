#pragma once

void SetThreadName(const char* name);
void panic(const char* title, const char* msg);
void GetExecutablePath(char* output, size_t bufsize, size_t& len);

void setWindowHandle(void* handle);
void getMouseCursorPos(int& x, int& y);

#ifdef SetThreadName
#undef SetThreadName
#endif
#define SetThreadName SetThreadName

#ifdef panic
#undef panic
#endif
#define panic panic

#define GetExecutablePath GetExecutablePath