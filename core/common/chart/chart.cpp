#include "chart.h"
#include "bms.h"
#include <fstream>
#include "plog/Log.h"

eChartType matchChartType(const Path& p)
{
    if (!fs::is_regular_file(p) || !p.has_extension())
        return eChartType::UNKNOWN;

    if (std::regex_match(p.extension().string(), regexFileExtBMS))
        return eChartType::BMS;

    if (std::regex_match(p.extension().string(), regexFileExtBMSON))
        return eChartType::BMSON;

    return eChartType::UNKNOWN;
}

std::shared_ptr<vChart> vChart::getFromFile(const Path& path)
{
    Path filePath = fs::absolute(path);
    std::ifstream fs(filePath);
    if (fs.fail())
    {
        LOG_WARNING << "[Chart] File invalid: " << filePath.generic_string();
        return nullptr;
    }

    // dispatch Chart object upon filename extension
    switch (matchChartType(path))
    {
    case eChartType::BMS:
        return std::make_shared<BMS>(filePath);

    case eChartType::UNKNOWN:
        LOG_WARNING << "[Chart] File type unknown: " << filePath.generic_string();
        return nullptr;

    default:
        LOG_WARNING << "[Chart] File type unsupported: " << filePath.generic_string();
        return nullptr;
    }

}

Path vChart::getDirectory() const
{
    return _absolutePath.parent_path().lexically_normal();
}