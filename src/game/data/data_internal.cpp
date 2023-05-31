#include "common/pch.h"
#include "data_internal.h"

namespace lunaticvibes
{

TimerStorage::ValueT& TimerStorage::operator[](const std::string& s)
{
    return operator[](std::string_view(s));
}
TimerStorage::ValueT& TimerStorage::operator[](const char* s)
{
    return operator[](std::string_view(s));
}
TimerStorage::ValueT& TimerStorage::operator[](std::string_view s)
{
    /*
    * Using std::hash<std::string_view> directly is too slow. We are gonna use the raw pointer instead.
    * This causes two problems:
    * 1. The pointer may be released.
    * 2. There will be duplicate strings.
    * The following procedure converts duplicate string pointers to one reusable universal key.
    */
    std::shared_lock l1(sm);
    bool found = keysMap.find((uintptr_t)s.data()) != keysMap.end();
    l1.unlock();
    if (!found)
    {
        size_t key = std::hash<std::string_view>{}(s);
        std::unique_lock l(sm);
        keysMap[(uintptr_t)s.data()] = key;
    }
    return operator[](keysMap[(uintptr_t)s.data()]);
}
TimerStorage::ValueT& TimerStorage::operator[](TimerStorage::KeyT key)
{
    if (timers.find(key) == timers.end())
    {
        std::unique_lock l(sm);
        timers[key] = TIMER_NEVER;
    }
    return timers[key];
}

void TimerStorage::clear()
{
    std::unique_lock l(sm);
    timers.clear();
}

}