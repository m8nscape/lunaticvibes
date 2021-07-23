#include "sound_fmod_callback.h"
#include <queue>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////
// Open file

#pragma warning(push)
#pragma warning(disable: 4996)
FMOD_RESULT F_CALLBACK FmodCallbackFileOpen(const char* file, unsigned int* pSize, void **pHandle, void *pUserData)
{
    if (file)
    {
        FILE *pFile;

        pFile = fopen(file, "rb");
        if (!pFile)
            return FMOD_ERR_FILE_NOTFOUND;

        fseek(pFile, 0, SEEK_END);
        *pSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        *pHandle = pFile;
    }
    return FMOD_OK;
}
#pragma warning(pop)

////////////////////////////////////////////////////////////////////////////////
// Close file

FMOD_RESULT F_CALLBACK FmodCallbackFileClose(void *handle, void *userData)
{
    if (!handle)
        return FMOD_ERR_INVALID_HANDLE;
    fclose((FILE*)handle);
    return FMOD_OK;
}

////////////////////////////////////////////////////////////////////////////////
// Async Read: Push file to queue

std::mutex queueMutex;
SampleQueue asyncSampleLoadQueue;

FMOD_RESULT F_CALLBACK FmodCallbackAsyncRead(FMOD_ASYNCREADINFO *info, void *userData)
{
    try
    {
        LOCK_QUEUE;
        asyncSampleLoadQueue.push(info);
    }
    catch (std::exception&)
    {
        return FMOD_ERR_MEMORY;
    }
    return FMOD_OK;
};


////////////////////////////////////////////////////////////////////////////////
// Cancel Async Read: cancel reading(TODO) and pop file from queue

FMOD_RESULT F_CALLBACK FmodCallbackAsyncReadCancel(FMOD_ASYNCREADINFO *handle, void *userData)
{
    try
    {
        LOCK_QUEUE;
        if (!asyncSampleLoadQueue.empty())
        {
            auto &s = asyncSampleLoadQueue.front();
            if (s->handle == handle->handle)
                asyncSampleLoadQueue.pop();
        }
    }
    catch (std::exception&)
    {
        return FMOD_ERR_MEMORY;
    }
    return FMOD_OK;
}
