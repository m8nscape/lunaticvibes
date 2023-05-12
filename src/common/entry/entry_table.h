#pragma once
#include "common/types.h"
#include "entry_folder.h"

namespace lunaticvibes
{

class EntryFolderTable : public EntryFolderRegular
{
public:
    EntryFolderTable() = delete;
    EntryFolderTable(StringContentView name, StringContentView name2 = "") :
        EntryFolderRegular(HashMD5(""), "", name, name2)
    {
        _type = eEntryType::CUSTOM_FOLDER;
    }
};

}
