#pragma once
#include "common/types.h"
#include "entry_folder.h"

class EntryFolderArena : public EntryFolderBase
{
public:
    EntryFolderArena() :
        EntryFolderBase(HashMD5(""), "ARENA", "")
    {
        _type = eEntryType::ARENA_FOLDER;
    }
};

class EntryArenaCommand : public EntryBase
{
public:
    enum class Type
    {
        HOST_LOBBY,
        JOIN_LOBBY,
        LEAVE_LOBBY,

    };
protected:
    Type cmdType = Type::HOST_LOBBY;

public:
    EntryArenaCommand() = default;
    EntryArenaCommand(Type type, const std::string& name): cmdType(type)
    {
        _name = name;
        _type = eEntryType::ARENA_COMMAND;
    }

    Type getCmdType() const { return cmdType; }
};

class EntryArenaLobby : public EntryBase
{
protected:
    std::string address;

public:
    EntryArenaLobby() = default;
    EntryArenaLobby(const std::string& name, const std::string& address) : address(address)
    {
        _name = name;
        _type = eEntryType::ARENA_LOBBY;
    }

    const std::string& getAddress() const { return address; }
};
