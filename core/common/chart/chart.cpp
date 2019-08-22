#include "chart.h"
#include "bms.h"
#include <regex>
#include <fstream>
#include "plog/Log.h"

std::shared_ptr<vChart> vChart::getFromFile(const Path& path)
{
    StringPath filePath = fs::absolute(path).string();
    std::ifstream fs(filePath);
    if (fs.fail())
    {
        LOG_WARNING << "[Chart] File invalid: " << filePath;
        return nullptr;
    }

    // dispatch Chart object upon filename extension
    if (std::regex_match(filePath, std::regex(R"(.*\.(bms|bme|bml|pms)$)", std::regex::icase)))
    {
        return std::make_shared<BMS>(filePath);
    }

    LOG_WARNING << "[Chart] File type unknown: " << filePath;
    return nullptr;
}

Path vChart::getDirectory() const
{
    return _filePath.parent_path();
}