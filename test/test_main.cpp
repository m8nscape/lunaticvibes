#include "gmock/gmock.h"
#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>

int main(int argc, char** argv)
{
    std::filesystem::create_directories("test");
    std::filesystem::current_path("test");

    auto appender = plog::ColorConsoleAppender<plog::TxtFormatterImpl<false>>();
    plog::init(plog::debug, &appender);

    ::testing::InitGoogleMock(&argc, argv);
    
    bool bExit = false;
    std::thread t([&bExit]()
        {
            while (!bExit)
                doMainThreadTask();
        });

    int n = RUN_ALL_TESTS();

    StopHandleMainThreadTask();

    //std::this_thread::sleep_for(std::chrono::seconds(1));
    bExit = true;
    t.join();
    exit(n);
    return n;
}