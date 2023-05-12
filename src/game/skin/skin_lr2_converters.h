#pragma once

namespace lunaticvibes
{

namespace lr2skin
{

const char* timer(int n);

std::function<Ratio()> convertBargraphIndex(int n);
std::function<int()> convertButtonIndex(int n);
std::function<int()> convertNumberIndex(int n);
std::function<Ratio()> convertSliderIndex(int n);
std::function<std::string_view()> convertTextIndex(int n);

}
}