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

#define declare_member(converter, type, prefix, index)                                              \
class _##prefix##_##index                                                                           \
{                                                                                                   \
public:                                                                                             \
    template <typename U = converter, typename T = std::decay_t<decltype(U::##prefix##_##index)>>   \
        static constexpr type(*value(U*, T* = nullptr))() { return &U::##prefix##_##index; }        \
    template <typename U = converter>                                                               \
        static constexpr type(*value(...))() { return &U::##prefix##_0; }                           \
};

#define member(converter, type, prefix, index) _##prefix##_##index::value((converter*)0)

}
}