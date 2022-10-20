#pragma once
#include <vector>
#include <memory>
#include "entry.h"

class EntryFolderBase: public EntryBase
{
protected:
    Path _path;

public:
    EntryFolderBase() = delete;
    EntryFolderBase(HashMD5 md5, const Path& path);
    virtual ~EntryFolderBase() = default;

// following fields are generic info, which are stored in db
public:
    Path getPath() { return _path; }

// following fields are filled during loading
public:
    virtual size_t getContentsCount() = 0;
    virtual bool empty() = 0;
    
};

class ChartFormatBase;
class EntryFolderRegular : public EntryFolderBase
{
public:
    EntryFolderRegular() = delete;
    EntryFolderRegular(HashMD5 md5, const Path& path, const StringContent& name = "", const StringContent& name2 = "") : 
        EntryFolderBase(md5, path)
    {
        _type = eEntryType::FOLDER;
        _name = name;
        _name2 = name2;
    }

protected:
    std::vector<std::shared_ptr<EntryFolderBase>> entries;
public:
    std::shared_ptr<EntryFolderBase> getEntry(size_t idx);
    void pushEntry(std::shared_ptr<EntryFolderBase> f);
    virtual size_t getContentsCount() { return entries.size(); }
    virtual bool empty() { return entries.empty(); }
};
