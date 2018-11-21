#include "logger.h"
#include "defs/defs_general.h"
#include <ctime>
#include <chrono>
#include <string>
#include <sstream>

bool isFirstLog = true;

void Log_Init()
{
    char timebuf[100];
    std::time_t t = std::time(NULL);

    if (strftime(timebuf, sizeof(timebuf), "_%F_%T", std::localtime(&t)))
        logsink::sink = new std::ofstream(std::string() + "log" + timebuf);

    std::stringstream linebuf;
    linebuf << MAIN_NAME;
    linebuf << " " << SUB_NAME;
    linebuf << " " << VER_MAJOR << "." << VER_MINOR;
    if (VER_PATCH) linebuf << "." << VER_PATCH;
    linebuf.flush();

    *logsink::sink << linebuf.str();
}

void Log_Close()
{
    if (logsink::sink != &std::cout)
        ((std::ofstream*)logsink::sink)->close();
}

std::ostream& getLogSink(severity_level s)
{
#if !_DEBUG
    if (s == severity_level::LOG_DEBUG)
        return log::nullSink;
#endif
    auto& sink = *logsink::sink;
    sink << std::endl;
    switch (s)
    {
    case severity_level::L_DEBUG:		sink << " D: "; break;
    case severity_level::L_INFO:		sink << " I: "; break;
    case severity_level::L_WARNING:	    sink << " W: "; break;
    case severity_level::L_ERROR:		sink << " E: "; break;
    case severity_level::L_FATAL:		sink << " F: "; break;
    }
    return sink;
}

std::ostream& LOG(severity_level s)
{
    char timebuf[100];
    std::time_t t = std::time(NULL);

    auto& sink = getLogSink(s);
    if (strftime(timebuf, sizeof(timebuf), "%F %T ", std::localtime(&t)))
        return sink << timebuf;
    else
        return sink;
}