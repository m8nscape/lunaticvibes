#include "entry_folder.h"
#include "common/chartformat/chartformat.h"

namespace lunaticvibes
{

EntryFolderBase::EntryFolderBase(HashMD5 md5, StringContentView name, StringContentView name2)
{
    this->md5 = md5;
    _name = name;
    _name2 = name2;
}

std::shared_ptr<EntryBase> EntryFolderBase::getEntry(size_t idx)
{
    idx %= getContentsCount();
    return entries[idx];
}

void EntryFolderBase::pushEntry(std::shared_ptr<EntryBase> f)
{
    entries.push_back(f);
}

}
