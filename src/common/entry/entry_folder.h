#pragma once
#include <vector>
#include <memory>
#include "entry.h"

namespace lunaticvibes
{

class EntryFolderBase : public EntryBase
{
public:
    EntryFolderBase() = delete;
    EntryFolderBase(HashMD5 md5, StringContentView name = "", StringContentView name2 = "");
    virtual ~EntryFolderBase() = default;

protected:
    std::vector<std::shared_ptr<EntryBase>> entries;
public:
    virtual std::shared_ptr<EntryBase> getEntry(size_t idx);
    virtual void pushEntry(std::shared_ptr<EntryBase> f);
    virtual size_t getContentsCount() { return entries.size(); }
    virtual bool empty() { return entries.empty(); }

};

}
