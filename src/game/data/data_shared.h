#pragma once
#include "common/types.h"

namespace lunaticvibes
{

constexpr const long long TIMER_NEVER = LLONG_MIN;
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