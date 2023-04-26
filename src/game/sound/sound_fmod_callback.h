#pragma once
#include "fmod.hpp"

typedef std::lock_guard<std::mutex> guard;
typedef std::queue<FMOD_ASYNCREADINFO*> SampleQueue;
extern std::mutex queueMutex;
extern SampleQueue asyncSampleLoadQueue;

#define LOCK_QUEUE guard _queueLock_(queueMutex)

FMOD_RESULT F_CALLBACK FmodCallbackFileOpen(const char* file, unsigned int* pSize, void **pHandle, void *pUserData);
FMOD_RESULT F_CALLBACK FmodCallbackFileClose(void *handle, void *userData);
FMOD_RESULT F_CALLBACK FmodCallbackAsyncRead(FMOD_ASYNCREADINFO *info, void *userData);
FMOD_RESULT F_CALLBACK FmodCallbackAsyncReadCancel(FMOD_ASYNCREADINFO *handle, void *userData);
