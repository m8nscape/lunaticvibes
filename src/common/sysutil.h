#pragma once

void SetThreadAsMainThread();
int64_t GetCurrentThreadID();
bool IsMainThread();
void SetThreadName(const char* name);
void panic(const char* title, const char* msg);
std::string GetExecutablePath();

void setWindowHandle(void* handle);
void getWindowHandle(void* handle);

bool IsWindowForeground();
void SetWindowForeground(bool foreground);

#ifdef _DEBUG
#define SetDebugThreadName(x) SetThreadName(x)
#else
#define SetDebugThreadName(x) do {} while(0)
#endif

void pushMainThreadTask(std::function<void()> f);
void doMainThreadTask();
void StopHandleMainThreadTask();
bool CanHandleMainThreadTask();

template<typename T>
inline T pushAndWaitMainThreadTask(std::function<T()> f)
{
	if (CanHandleMainThreadTask())
	{
		std::promise<T> taskPromise;
		pushMainThreadTask(std::bind([&]() { taskPromise.set_value(f()); }));
		std::future<T> taskFuture = taskPromise.get_future();
		taskFuture.wait();
		return taskFuture.get();
	}
	else
	{
		return f();
	}
}
template<>
inline void pushAndWaitMainThreadTask(std::function<void()> f)
{
	if (CanHandleMainThreadTask())
	{
		std::promise<void> taskPromise;
		pushMainThreadTask(std::bind([&]() { f(); taskPromise.set_value(); }));
		std::future<void> taskFuture = taskPromise.get_future();
		taskFuture.wait();
		return taskFuture.get();
	}
}
template<typename T, typename... Arg>
inline T pushAndWaitMainThreadTask(std::function<T(Arg...)> f, Arg... arg)
{
	if (CanHandleMainThreadTask())
	{
		return pushAndWaitMainThreadTask<T>(std::bind(f, arg...));
	}
	else
	{
		return T();
	}
}

void addWMEventHandler(void* f);
void callWMEventHandler(void* arg1 = 0, void* arg2 = 0, void* arg3 = 0, void* arg4 = 0);

// Unix epoch time.
long long getFileTimeNow();
// Unix epoch time.
long long getFileLastWriteTime(const Path& p);

enum class Languages
{
	EN,
	JP,
	KR,
	ZHCN,
	ZHTW,
	ZHHK,
	// tbd
};
Path getSysFontPath(std::string* faceName = NULL, int* faceIndex = NULL, Languages lang = Languages::EN);
Path getSysMonoFontPath(std::string* faceName = NULL, int* faceIndex = NULL, Languages lang = Languages::EN);

// Thread-safe strerror().
//
// This uses whatever safe implementation of strerror() target system
// has.
const char* safe_strerror(int errnum, char* buffer, size_t buffer_length);
// Wrapper over the other safe_strerror() that returns an owned string.
std::string safe_strerror(int errnum);
