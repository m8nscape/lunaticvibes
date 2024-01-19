#pragma once
#include "common/types.h"
#include "entry_folder.h"
#include "entry_song.h"

class EntryFolderTable : public EntryFolderRegular
{
public:
    EntryFolderTable() = delete;
    EntryFolderTable(StringContentView name, size_t index)
        : EntryFolderRegular(HashMD5(""), "", name, "")
        , _index(index)
    {
        _type = eEntryType::CUSTOM_FOLDER;
    }

    [[nodiscard]] size_t getIndex() const {
        return _index;
    }

private:
    size_t _index;
};
