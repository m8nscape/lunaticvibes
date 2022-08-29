#include "entry_folder.h"
#include "common/chartformat/chartformat.h"

EntryFolderBase::EntryFolderBase(HashMD5 md5, const Path& path): _path(path)
{
    this->md5 = md5;
}

std::shared_ptr<EntryFolderBase> EntryFolderRegular::getEntry(size_t idx)
{
    idx %= getContentsCount();
    return entries[idx];
}

void EntryFolderRegular::pushEntry(std::shared_ptr<EntryFolderBase> f)
{
    entries.push_back(f);
}
