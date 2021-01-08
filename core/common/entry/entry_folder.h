#pragma once
#include <vector>
#include <memory>
#include "entry.h"

enum class eFolderType
{
    UNKNOWN,

    FOLDER,
    SONG,
    CUSTOM_FOLDER
};

class vFolder: public vEntry
{
protected:
    eFolderType _ftype = eFolderType::UNKNOWN;
    Path _path;
public:
    constexpr eFolderType folderType() { return _ftype; }

public:
    vFolder() = delete;
    vFolder(eFolderType t, HashMD5 md5, const Path& path);
    virtual ~vFolder() = default;

// following fields are generic info, which are stored in db
public:
    HashMD5 pathmd5;

// following fields are filled during loading
public:
    virtual size_t getContentsCount() = 0;
    virtual bool empty() = 0;
    
};

class vChartFormat;
class FolderRegular : public vFolder
{
public:
    FolderRegular() = delete;
    FolderRegular(HashMD5 md5, const Path& path) : vFolder(eFolderType::FOLDER, md5, path) {}

protected:
    std::vector<std::shared_ptr<vFolder>> entries;
public:
    std::shared_ptr<vFolder> getEntry(int idx);
    void pushEntry(std::shared_ptr<vFolder> f);
    virtual size_t getContentsCount() { return entries.size(); }
    virtual bool empty() { return entries.empty(); }
};

class FolderSong : public vFolder
{
public:
    FolderSong() = delete;
    FolderSong(HashMD5 md5, const Path& path) : vFolder(eFolderType::SONG, md5, path) {}

protected:
    std::vector<std::shared_ptr<vChartFormat>> charts;

public:
    std::shared_ptr<vChartFormat> getChart(int idx);
    void pushChart(std::shared_ptr<vChartFormat> c);
    virtual size_t getContentsCount() { return charts.size(); }
    virtual bool empty() { return charts.empty(); }
};


