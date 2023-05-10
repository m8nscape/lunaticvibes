#pragma once
namespace lr2skin
{
std::function<long long()> convertTimerIndex(int n);

std::function<Ratio()> convertBargraphIndex(int n);
std::function<int()> convertButtonIndex(int n);
std::function<int()> convertNumberIndex(int n);
std::function<Ratio()> convertSliderIndex(int n);
std::function<std::string_view()> convertTextIndex(int n);

}