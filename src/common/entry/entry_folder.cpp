#include "entry_folder.h"
#include "common/chartformat/chartformat.h"

vFolder::vFolder(eFolderType t, HashMD5 md5, const Path& path): _path(path)
{
    this->md5 = md5;
    switch (t)
    {
    case eFolderType::FOLDER: 
        _type = eEntryType::FOLDER; 
        break;
    case eFolderType::SONG: 
        _type = eEntryType::SONG; 
        break;
    case eFolderType::CUSTOM_FOLDER: 
        _type = eEntryType::CUSTOM_FOLDER; 
        break;
    default:
        break;
    }
}

std::shared_ptr<vFolder> FolderRegular::getEntry(size_t idx)
{
    idx %= getContentsCount();
    return entries[idx];
}

void FolderRegular::pushEntry(std::shared_ptr<vFolder> f)
{
    entries.push_back(f);
}
