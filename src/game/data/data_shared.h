#pragma once
#include "data_internal.h"
#include "common/types.h"

namespace lunaticvibes
{

constexpr const long long TIMER_NEVER = TimerStorage::TIMER_NEVER;
void resetTimers();

void loadConfigs();

namespace cfg
{
void loadFilterDifficulty();
void loadFilterKeys();
}

long long getTimerValue(std::string_view key);

std::pair<bool, LampType> getMaxSaveScoreType();

void createNotification(StringContentView text);
}