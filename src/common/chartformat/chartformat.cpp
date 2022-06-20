#include "chartformat.h"
#include "chartformat_bms.h"
#include <fstream>
#include "common/log.h"

eChartFormat matchChartType(const Path& p)
{
    static const std::regex regexFileExtBMS{ R"(\.(bms|bme|bml|pms)$)", std::regex::icase | std::regex::optimize };
    static const std::regex regexFileExtBMSON{ R"(\.(bmson)$)", std::regex::icase | std::regex::optimize };

    if (!fs::is_regular_file(p) || !p.has_extension())
        return eChartFormat::UNKNOWN;

    if (std::regex_match(p.extension().u8string(), regexFileExtBMS))
        return eChartFormat::BMS;

    if (std::regex_match(p.extension().u8string(), regexFileExtBMSON))
        return eChartFormat::BMSON;

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