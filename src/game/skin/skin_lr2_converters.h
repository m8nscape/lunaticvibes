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

#define DECLARE_MEMBER_CONCAT_(a, b) a##b
#define DECLARE_MEMBER_CONCAT(a, b) DECLARE_MEMBER_CONCAT_(a, b)

#define declare_member(converter, type, prefix, index)                                                                                           \
class DECLARE_MEMBER_CONCAT(_skin_conv_for_index_, index)                                                                                                       \
{                                                                                                                                                \
public:                                                                                                                                          \
    template <typename U = converter, typename T = std::decay_t<decltype(U:: DECLARE_MEMBER_CONCAT(DECLARE_MEMBER_CONCAT(prefix, _), index))>>   \
        static constexpr type(*value(U*, T* = nullptr))() { return &U:: DECLARE_MEMBER_CONCAT(DECLARE_MEMBER_CONCAT(prefix, _), index); }        \
    template <typename U = converter>                                                                                                            \
        static constexpr type(*value(...))() { return &U:: DECLARE_MEMBER_CONCAT(prefix, _0); }                                                  \
};

#define member(converter, type, prefix, index) DECLARE_MEMBER_CONCAT(_skin_conv_for_index_, index)::value((converter*)0)

}
}