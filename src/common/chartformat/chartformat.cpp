#include "common/pch.h"
#include "chartformat.h"
#include "chartformat_bms.h"

eChartFormat analyzeChartType(const Path& p)
{
    if (!p.has_extension())
        return eChartFormat::UNKNOWN;

    eChartFormat fmt = eChartFormat::UNKNOWN;

    auto extension = p.extension().u8string();
    if (extension.length() == 4)
    {
        if (strEqual(extension, ".bms", true) ||
            strEqual(extension, ".bme", true) ||
            strEqual(extension, ".bml", true) ||
            strEqual(extension, ".pms", true))
            fmt = eChartFormat::BMS;
    }
    else if (extension.length() == 6)
    {
        if (strEqual(extension, ".bmson", true))
            fmt = eChartFormat::BMSON;
    }

    if (!fs::is_regular_file(p))
        return eChartFormat::UNKNOWN;
    else
        return fmt;
}

std::shared_ptr<ChartFormatBase> ChartFormatBase::createFromFile(const Path& path, uint64_t randomSeed)
{
    Path filePath = fs::absolute(path);
    std::ifstream fs(filePath.c_str());
    if (fs.fail())
    {
        LOG_WARNING << "[Chart] File invalid: " << filePath.u8string();
        return nullptr;
    }

    // dispatch Chart object upon filename extension
    switch (analyzeChartType(path))
    {
    case eChartFormat::BMS:
        return std::static_pointer_cast<ChartFormatBase>(std::make_shared<ChartFormatBMS>(filePath, randomSeed));

    case eChartFormat::UNKNOWN:
        LOG_WARNING << "[Chart] File type unknown: " << filePath.u8string();
        return nullptr;

    default:
        LOG_WARNING << "[Chart] File type unsupported: " << filePath.u8string();
        return nullptr;
    }

}

Path ChartFormatBase::getDirectory() const
{
    return (absolutePath / "..").lexically_normal();
}