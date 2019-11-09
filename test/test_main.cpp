#include "gmock/gmock.h"
#include "plog/Log.h"
#include <filesystem>
#include <plog/Appenders/ColorConsoleAppender.h>        // for command prompt log purpose
int main(int argc, char** argv)
{
    auto appender = plog::ColorConsoleAppender<plog::TxtFormatterImpl<false>>();
    plog::init(plog::debug, &appender);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}