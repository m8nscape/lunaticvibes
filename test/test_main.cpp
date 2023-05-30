#include "gmock/gmock.h"
#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include "config/config_mgr.h"
using namespace lunaticvibes;

bool gEventQuit;

int main(int argc, char** argv)
{
    std::filesystem::create_directories("test");
    std::filesystem::current_path("test");

    SetThreadAsMainThread();

    auto appender = plog::ColorConsoleAppender<plog::TxtFormatterImpl<false>>();
    plog::init(plog::debug, &appender);

    ConfigMgr::init();
    ConfigMgr::load();

    ::testing::InitGoogleMock(&argc, argv);
    
    int n = RUN_ALL_TESTS();

    return n;
}