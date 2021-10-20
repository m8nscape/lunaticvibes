#pragma once
#include "cfg_general.h"
#include "cfg_profile.h"
#include "cfg_input.h"
#include "cfg_skin.h"
#include <string>
#include <memory>
#include <filesystem>

class ConfigMgr
{
private:
    ConfigMgr() = default;
    ~ConfigMgr() = default;
    static ConfigMgr& getInst()
    {
        static ConfigMgr inst;
        return inst;
    }

    void _init()
    {
        G = std::make_shared<ConfigGeneral>("./config.yml");
        profileName = G->get(cfg::E_PROFILE, cfg::PROFILE_DEFAULT);
        P = std::make_shared<ConfigProfile>(profileName);
        I5 = std::make_shared<ConfigInput>(profileName, 5);
        I7 = std::make_shared<ConfigInput>(profileName, 7);
        I9 = std::make_shared<ConfigInput>(profileName, 9);
        S = std::make_shared<ConfigSkin>(profileName);

        G->setDefaults();
        P->setDefaults();
        I5->setDefaults();
        I7->setDefaults();
        I9->setDefaults();
        S->setDefaults();
    }

    void _load() { G->load(); P->load(); I5->load(); I7->load(); I9->load(); S->load(); }
    void _save() { G->save(); P->save(); I5->save(); I7->save(); I9->save(); S->save(); }
    int _selectProfile(const std::string& name);
    std::string _getProfileName() { return P->getName(); }
    void _setGlobals();

    template<class Ty_v>
    Ty_v _get(char type, const std::string& key, const Ty_v& fallback)
    {
        switch (type)
        {
        case 'A':                               // Audio
        case 'E':                               // Audio
        case 'V': return G->get<Ty_v>(key, fallback);  // Video
        case 'P': return P->get<Ty_v>(key, fallback);  // Play
        case '5': return I5->get<Ty_v>(key, fallback);  // Input
        case '7': return I7->get<Ty_v>(key, fallback);  // Input
        case '9': return I9->get<Ty_v>(key, fallback);  // Input
        case 'S': return S->get<Ty_v>(key, fallback);  // Skin
        default:  return Ty_v(); break;
        }
    }
    template<class Ty_v>
    void _set(char type, const std::string& key, const Ty_v& value) noexcept
    {
        switch (type)
        {
        case 'A':                               // Audio
        case 'E':                               // Audio
        case 'V': return G->set<Ty_v>(key, value);     // Play
        case '5': return I5->set<Ty_v>(key, value);     // Input
        case '7': return I7->set<Ty_v>(key, value);     // Input
        case '9': return I9->set<Ty_v>(key, value);     // Input
        case 'S': return S->set<Ty_v>(key, value);     // Skin
        default:  return;
        }
    }

    std::vector<Input::Keyboard> _getKeyBindings(int mode, Input::Pad key)
    {
        switch (mode)
        {
        case 5: return I5->getBindings(key); break;
        case 7: return I7->getBindings(key); break;
        case 9: return I9->getBindings(key); break;
        default: return {};
        }
    }
    std::string _getDBPath()
    {
        auto path = "profile/"s + profileName;
        if (!std::filesystem::exists(path))
            std::filesystem::create_directories(path);
        return path;
    }

protected:
    std::shared_ptr<ConfigGeneral>     G;
    std::shared_ptr<ConfigProfile>     P;
    std::shared_ptr<ConfigInput>       I5;
    std::shared_ptr<ConfigInput>       I7;
    std::shared_ptr<ConfigInput>       I9;
    std::shared_ptr<ConfigSkin>        S;
    std::string profileName;

public:
    std::shared_ptr<ConfigGeneral> _General() { return G; }
    std::shared_ptr<ConfigProfile> _Profile() { return P; }
    std::shared_ptr<ConfigSkin>    _Skin() { return S; };
    std::shared_ptr<ConfigInput>   _Input(int mode)
    {
        switch (mode)
        {
        case 5: return I5;
        case 7: return I7;
        case 9: return I9;
        default: return nullptr;
        }
    }

public:
    static void init() { getInst()._init(); }
    static void load() { getInst()._load(); }
    static void save() { getInst()._save(); }
    static int selectProfile(const std::string& name) { return getInst()._selectProfile(name); }
    static void setGlobals() { getInst()._setGlobals(); }
    static std::shared_ptr<ConfigGeneral> General() { return getInst()._General(); }
    static std::shared_ptr<ConfigProfile> Profile() { return getInst()._Profile(); }
    static std::shared_ptr<ConfigInput>   Input(int mode)  { return getInst()._Input(mode) ; }
    static std::shared_ptr<ConfigSkin>    Skin()    { return getInst()._Skin()   ; }

    template<class Ty_v>
    static Ty_v get(char type, const std::string& key, const Ty_v& fallback) { return getInst()._get(type, key, fallback); }
    template<class Ty_v>
    static Ty_v get(const std::string& type, const std::string& key, const Ty_v& fallback) { return getInst()._get(type[0], key, fallback); }

    static std::string get(char type, const std::string& key, const std::string& fallback) { return get<std::string>(type, key, fallback); }
    static std::string get(const std::string& type, const std::string& key, const std::string& fallback) { return get<std::string>(type[0], key, fallback); }

    template<class Ty_v>
    static void set(char type, const std::string& key, const Ty_v& value) noexcept { return getInst()._set(type, key, value); }
    template<class Ty_v>
    static void set(const std::string& type, const std::string& key, const Ty_v& value) noexcept { return getInst()._set(type[0], key, value); }

    static void set(char type, const std::string& key, const std::string& value) noexcept { return set<std::string>(type, key, value); }
    static void set(const std::string& type, const std::string& key, const std::string& value) noexcept { return set<std::string>(type[0], key, value); }

    static std::string getDBPath() { return getInst()._getDBPath(); }


public:
};
