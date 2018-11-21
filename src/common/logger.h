#pragma once
#include <iostream>
#include <fstream>

enum class severity_level
{
	L_DEBUG,
	L_INFO,
	L_WARNING,
	L_ERROR,
	L_FATAL
};

namespace logsink
{
    static std::ofstream nullSink;
    static std::ostream* sink = &std::cout;
}

void Log_Init();
void Log_Close();
std::ostream& getLogSink(severity_level);

std::ostream& LOG(severity_level);

#define LOG_DEBUG   LOG(severity_level::L_DEBUG) << "[" << __FILE__ << " (" << __LINE__ << ")] "

#if _DEBUG
#define LOG_INFO    LOG(severity_level::L_INFO) << "[" << __FILE__ << " (" << __LINE__ << ")] "
#define LOG_WARNING LOG(severity_level::L_WARNING) << "[" << __FILE__ << " (" << __LINE__ << ")] "
#define LOG_ERROR   LOG(severity_level::L_ERROR) << "[" << __FILE__ << " (" << __LINE__ << ")] "
#define LOG_FATAL   LOG(severity_level::L_FATAL) << "[" << __FILE__ << " (" << __LINE__ << ")] "
#else
#define LOG_INFO    LOG(severity_level::L_INFO)
#define LOG_WARNING LOG(severity_level::L_WARNING)
#define LOG_ERROR   LOG(severity_level::L_ERROR)
#define LOG_FATAL   LOG(severity_level::L_FATAL)
#endif
