#pragma once

void SetThreadAsMainThread();
bool IsMainThread();
void SetThreadName(const char* name);
void panic(const char* title, const char* msg);
void GetExecutablePath(char* output, size_t bufsize, size_t& len);

void setWindowHandle(void* handle);
bool getMouseCursorPos(int& x, int& y);

bool IsWindowForeground();
void SetWindowForeground(bool foreground);

#ifdef _MSC_VER
#ifdef SetThreadName
#undef SetThreadName
#endif
#define SetThreadName SetThreadName

#ifdef panic
#undef panic
#endif
#define panic panic

#define GetExecutablePath GetExecutablePath
#endif

#include <functional>
void pushMainThreadTask(std::function<void()> f);
void doMainThreadTask();

#include <future>
template<typename T>
inline T pushAndWaitMainThreadTask(std::function<T()> f)
{
	std::promise<T> taskPromise;
	pushMainThreadTask(std::bind([&]() { taskPromise.set_value(f()); }));
	std::future<T> taskFuture = taskPromise.get_future();
	taskFuture.wait();
	return taskFuture.get();
}
template<>
inline void pushAndWaitMainThreadTask(std::function<void()> f)
{
	std::promise<void> taskPromise;
	pushMainThreadTask(std::bind([&]() { f(); taskPromise.set_value(); }));
	std::future<void> taskFuture = taskPromise.get_future();
	taskFuture.wait();
	return taskFuture.get();
}
template<typename T, typename... Arg>
inline T pushAndWaitMainThreadTask(std::function<T(Arg...)> f, Arg... arg)
{
	return pushAndWaitMainThreadTask<T>(std::bind(f, arg...));
}

void addWMEventHandler(std::function<void(void*, void*, void*, void*)> f);
void callWMEventHandler(void* arg1, void* arg2 = 0, void* arg3 = 0, void* arg4 = 0);