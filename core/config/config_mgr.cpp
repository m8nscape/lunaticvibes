#include "config_mgr.h"
#include <filesystem>
#include <fstream>
#include <plog/Log.h>
namespace fs = std::filesystem;

int ConfigMgr::selectProfile(const std::string& name)
{
    Path folder = Path(".") / "profile" / name;
    if (!fs::exists(folder))
        fs::create_directory(folder);

    if (!fs::is_directory(folder))
    {
        LOG_WARNING << "[Config] Bad profile: " << name;
        return 2;
    }

    auto createFile = [](Path& p) {
        if (fs::exists(p))
        {
            if (!fs::is_regular_file(p))
            {
                LOG_WARNING << "[Config] File is not regular file: " << p.generic_string();
                return true;
            }
        }
        else
        {
            std::ofstream(p).close();
        }
        return false;
    };

    if (createFile(folder / CONFIG_FILE_GENERAL) ||
        createFile(folder / CONFIG_FILE_INPUT_5) ||
        createFile(folder / CONFIG_FILE_INPUT_7) ||
        createFile(folder / CONFIG_FILE_INPUT_9) ||
        createFile(folder / CONFIG_FILE_SKIN) ||
        createFile(folder / CONFIG_FILE_PROFILE))
    {
        LOG_WARNING << "[Config] Bad profile: " << name;
        return 3;
    }

    try
    {
        G = {name};
        P = {name};
        I5 = {name, 5};
        I7 = {name, 7};
        I9 = {name, 9};
        S = {name};
    }
    catch (YAML::BadFile&)
    {
        LOG_WARNING << "[Config] Bad profile: " << name;
        return 1;
    }

    load();
    return 0;
}
