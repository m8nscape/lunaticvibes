#include "common/pch.h"
#include "config_mgr.h"

namespace fs = std::filesystem;

int ConfigMgr::_selectProfile(const std::string& name)
{
    Path folder = Path(GAMEDATA_PATH) / "profile" / name;
    if (!fs::exists(folder))
        fs::create_directories(folder);

    if (!fs::is_directory(folder))
    {
        LOG_WARNING << "[Config] Bad profile: " << name;
        return 2;
    }

    auto createFile = [](Path&& p) {
        if (fs::exists(p))
        {
            if (!fs::is_regular_file(p))
            {
                LOG_WARNING << "[Config] File is not regular file: " << p.u8string();
                return true;
            }
        }
        else
        {
            std::ofstream(p).close();
        }
        return false;
    };

    if (createFile(folder / CONFIG_FILE_INPUT_5) ||
        createFile(folder / CONFIG_FILE_INPUT_7) ||
        createFile(folder / CONFIG_FILE_INPUT_9) ||
        createFile(folder / CONFIG_FILE_SKIN) ||
        createFile(folder / CONFIG_FILE_PROFILE))
    {
        LOG_WARNING << "[Config] Bad profile: " << name;
        return 3;
    }

    {
        std::unique_lock l(getInst()._mutex);

        try
        {
            P = std::make_shared<ConfigProfile>(name);
            I5 = std::make_shared<ConfigInput>(name, 5);
            I7 = std::make_shared<ConfigInput>(name, 7);
            I9 = std::make_shared<ConfigInput>(name, 9);
            S = std::make_shared<ConfigSkin>(name);
        }
        catch (YAML::BadFile&)
        {
            LOG_WARNING << "[Config] Bad profile: " << name;
            return 1;
        }
    }

    load();

    profileName = name;
    G->set(cfg::E_PROFILE, profileName);

    return 0;
}

int ConfigMgr::_createProfile(const std::string& newProfile, const std::string& oldProfile)
{
    Path newFolder = Path(GAMEDATA_PATH) / "profile" / newProfile;
    if (fs::exists(newFolder))
    {
        LOG_WARNING << "[Config] Profile name duplicate: " << newProfile;
        return 2;
    }

    fs::create_directories(newFolder);

    Path oldFolder = Path(GAMEDATA_PATH) / "profile" / oldProfile;
    if (!oldProfile.empty() && fs::exists(oldFolder))
    {
        for (auto& f : fs::directory_iterator(oldFolder))
        {
            if (strEqual(f.path().extension().u8string(), ".yml", true) ||
                strEqual(f.path().filename().u8string(), "customize", true))
            {
                fs::copy(f, newFolder / f.path().lexically_relative(oldFolder));
            }
        }
    }

    ConfigProfile p(newProfile);
    p.setDefaults();
    p.set(cfg::P_PLAYERNAME, newProfile);
    p.save();

    return 0;
}
