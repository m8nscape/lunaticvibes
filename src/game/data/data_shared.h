#pragma once

namespace lunaticvibes::data
{

constexpr const long long TIMER_NEVER = LLONG_MIN;

void loadConfigs();

long long getTimerValue(const std::string& key);

}