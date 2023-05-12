#pragma once

namespace lunaticvibes
{

struct DXArchiveSegment
{
    size_t size;
    std::shared_ptr<uint8_t> data;
};

using DXArchive = std::map<std::string, DXArchiveSegment>;

DXArchive extractDxaToMem(const StringPath& path);
int extractDxaToFile(const StringPath& path);

}