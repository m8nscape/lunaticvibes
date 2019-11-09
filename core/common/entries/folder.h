#pragma once
#include "types.h"
#include <vector>
#include <memory>

enum class eFolderType
{
    UNKNOWN,

    FOLDER,
    SONG,
    CUSTOM_FOLDER
};

class vFolder
{
protected:
    eFolderType _type = eFolderType::UNKNOWN;
    Path _absolutePath;
public:
    constexpr eFolderType type() { return _type; }

public:
    vFolder() = delete;
    virtual ~vFolder() = default;
protected:
    vFolder(eFolderType t, Path absPath) : _type(t), _absolutePath(absPath) {}

// following fields are generic info, which are stored in db
public:
    HashMD5 hash;
    StringContent name;
    StringContent name2;

// following fields are filled during loading
public:
    virtual size_t getContentsCount() = 0;
    virtual bool empty() = 0;
    
};

class vChart;
class FolderRegular : public vFolder
{
public:
    FolderRegular() = delete;
    FolderRegular(Path absPath) : vFolder(eFolderType::FOLDER, absPath) {}

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
    FolderSong(Path absPath) : vFolder(eFolderType::SONG, absPath) {}

protected:
    std::vector<std::shared_ptr<vChart>> charts;

public:
    std::shared_ptr<vChart> getChart(int idx);
    void pushChart(std::shared_ptr<vChart> c);
    virtual size_t getContentsCount() { return charts.size(); }
    virtual bool empty() { return charts.empty(); }
};


