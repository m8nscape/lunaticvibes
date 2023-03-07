#include "log.h"

#include "common/log.h"
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>

#include <plog/Appenders/ColorConsoleAppender.h>        // for command prompt log purpose
#include <plog/Appenders/RollingFileAppender.h>

std::shared_ptr<plog::ColorConsoleAppender<plog::TxtFormatterImpl<false>>> pConsoleAppender;
std::shared_ptr<plog::RollingFileAppender<plog::TxtFormatterImpl<false>>> pTxtAppender;
int InitLogger()
{
    pConsoleAppender = std::make_shared<plog::ColorConsoleAppender<plog::TxtFormatterImpl<false>>>();
    plog::init(plog::info, &*pConsoleAppender);

    /*
    char buf[128] = { 0 };
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    strftime(buf, sizeof(buf), "%F", std::gmtime(&t));
    std::stringstream logfile;
    logfile << PROJECT_NAME << "-" << buf << ".log";
    static auto txtAppender = plog::RollingFileAppender<plog::TxtFormatterImpl<false>>{ logfile.str().c_str(), 1000000, 5 };
    */
    std::stringstream logfile;
    logfile << "LunaticVibes" << ".log";
    pTxtAppender = std::make_shared<plog::RollingFileAppender<plog::TxtFormatterImpl<false>>>(logfile.str().c_str(), 1000000, 5);
    plog::get()->addAppender(&*pTxtAppender);

    return 0;
}

void SetLogLevel(int level)
{
    auto severity = plog::info;
    switch (level)
    {
    case 0: severity = plog::debug; break;
    case 1: severity = plog::info; break;
    case 2: severity = plog::warning; break;
    case 3: severity = plog::error; break;
    default: severity = plog::fatal; break;
    }
    plog::get()->setMaxSeverity(severity);
}

int FreeLogger()
{
    pTxtAppender.reset();
    pConsoleAppender.reset();
    return 0;
}