#pragma once
#include "common/types.h"
#include "entry_folder.h"

class EntryFolderTable : public EntryFolderRegular
{
public:
    EntryFolderTable() = delete;
    EntryFolderTable(const StringContent& name = "", const StringContent& name2 = "") :
        EntryFolderRegular(HashMD5(""), "", name, name2)
    {
        _type = eEntryType::CUSTOM_FOLDER;
    }
};
