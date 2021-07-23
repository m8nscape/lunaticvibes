#pragma once
#include <plog/Log.h>

#ifndef LOG_VERBOSE
#define LOG_VERBOSE                     PLOG(plog::verbose)
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG                       PLOG(plog::debug)
#endif

#ifndef LOG_INFO
#define LOG_INFO                        PLOG(plog::info)
#endif

#ifndef LOG_WARNING
#define LOG_WARNING                     PLOG(plog::warning)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR                       PLOG(plog::error)
#endif

#ifndef LOG_FATAL
#define LOG_FATAL                       PLOG(plog::fatal)
#endif

#ifndef LOG_NONE
#define LOG_NONE                        PLOG(plog::none)
#endif

int InitLogger();
int FreeLogger();