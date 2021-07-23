#include "log.h"

#include "common/log.h"
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>

#if _DEBUG
#include <plog/Appenders/ColorConsoleAppender.h>        // for command prompt log purpose
#else
#include <plog/Appenders/RollingFileAppender.h>
#endif

int InitLogger()
{
#if _DEBUG
    static auto appender = plog::ColorConsoleAppender<plog::TxtFormatterImpl<false>>();
    plog::init(plog::debug, &appender);
#else
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto fmt = std::put_time(localtime_(&t), "%F");
    std::stringstream logfile;
    logfile << MAIN_NAME << "-" << fmt << ".log";
    static auto appender = plog::RollingFileAppender<plog::TxtFormatterImpl<false>>{ logfile.str().c_str(), 1000000, 5 };
    plog::init(plog::info, &appender);
#endif
    return 0;
}

int FreeLogger()
{
    return 0;
}