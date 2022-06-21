#include "chartformat.h"
#include "chartformat_bms.h"
#include <fstream>
#include "common/log.h"

eChartFormat matchChartType(const Path& p)
{
    if (!fs::is_regular_file(p) || !p.has_extension())
        return eChartFormat::UNKNOWN;

    auto extension = p.extension().u8string();
    if (extension.length() == 4)
    {
        if (strEqual(extension, ".bms", true) ||
            strEqual(extension, ".bme", true) ||
            strEqual(extension, ".bml", true) ||
            strEqual(extension, ".pms", true))
            return eChartFormat::BMS;
    }
    else if (extension.length() == 6)
    {
        if (strEqual(extension, ".bmson", true))
            return eChartFormat::BMSON;
    }

    return eChartFormat::UNKNOWN;
}

std::shared_ptr<vChartFormat> vChartFormat::getFromFile(const Path& path)
{
    Path filePath = fs::absolute(path);
    std::ifstream fs(filePath.c_str());
    if (fs.fail())
    {
        LOG_WARNING << "[Chart] File invalid: " << filePath.u8string();
        return nullptr;
    }

    // dispatch Chart object upon filename extension
    switch (matchChartType(path))
    {
    case eChartFormat::BMS:
        return std::make_shared<BMS>(filePath);

    case eChartFormat::UNKNOWN:
        LOG_WARNING << "[Chart] File type unknown: " << filePath.u8string();
        return nullptr;

    default:
        LOG_WARNING << "[Chart] File type unsupported: " << filePath.u8string();
        return nullptr;
    }

}

Path vChartFormat::getDirectory() const
{
    return (absolutePath / "..").lexically_normal();
}