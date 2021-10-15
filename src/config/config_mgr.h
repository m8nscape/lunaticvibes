#pragma once
#include "cfg_general.h"
#include "cfg_profile.h"
#include "cfg_input.h"
#include "cfg_skin.h"
#include <string>
#include <memory>

inline const char* PROFILE_DEFAULT = "default";

class ConfigMgr
{
public:
    static std::shared_ptr<ConfigGeneral>     G;
    static std::shared_ptr<ConfigProfile>     P;
    static std::shared_ptr<ConfigInput>       I5;
    static std::shared_ptr<ConfigInput>       I7;
    static std::shared_ptr<ConfigInput>       I9;
    static std::shared_ptr<ConfigSkin>        S;
    static std::string profileName;

public:
    static inline void init()
    {
        profileName = PROFILE_DEFAULT;
        G = std::make_shared<ConfigGeneral>(PROFILE_DEFAULT);
        P = std::make_shared<ConfigProfile>(PROFILE_DEFAULT);
        I5 = std::make_shared<ConfigInput>(PROFILE_DEFAULT, 5);
        I7 = std::make_shared<ConfigInput>(PROFILE_DEFAULT, 7);
        I9 = std::make_shared<ConfigInput>(PROFILE_DEFAULT, 9);
        S = std::make_shared<ConfigSkin>(PROFILE_DEFAULT);

        G->setDefaults();
        P->setDefaults(); 
        I5->setDefaults(); 
        I7->setDefaults(); 
        I9->setDefaults(); 
        S->setDefaults();
    }

    static inline void load() { G->load(); P->load(); I5->load(); I7->load(); I9->load(); S->load(); }
    static inline void save() { G->save(); P->save(); I5->save(); I7->save(); I9->save(); S->save(); }
    static int selectProfile(const std::string& name);
    static void setGlobals();

public:
    template<class Ty_v>
    static Ty_v get(const std::string& type, const std::string& key, const Ty_v& fallback)
    {
        switch (type[0])
        {
        case 'A':                               // Audio
        case 'V': return G->get<Ty_v>(key, fallback);  // Video
        case 'P': return P->get<Ty_v>(key, fallback);  // Play
        case '5': return I5->get<Ty_v>(key, fallback);  // Input
        case '7': return I7->get<Ty_v>(key, fallback);  // Input
        case '9': return I9->get<Ty_v>(key, fallback);  // Input
        case 'S': return S->get<Ty_v>(key, fallback);  // Skin
        default:  return Ty_v(); break;
        }
    }
    static std::string get(const std::string& type, const std::string& key, const char* fallback)
    {
        return get<std::string>(type, key, fallback);
    }

    template<class Ty_v>
    static void set(const std::string& type, const std::string& key, const Ty_v& value) noexcept
    {
        switch (type[0])
        {
        case 'A':                               // Audio
        case 'V': return G->set<Ty_v>(key, value);     // Play
        case '5': return I5->set<Ty_v>(key, value);     // Input
        case '7': return I7->set<Ty_v>(key, value);     // Input
        case '9': return I9->set<Ty_v>(key, value);     // Input
        case 'S': return S->set<Ty_v>(key, value);     // Skin
        default:  return;
        }
    }
    static void set(const std::string& type, const std::string& key, const char* value) noexcept
    {
        set<std::string>(type, key, value);
    }

    static std::vector<Input::Keyboard> getKeyBindings(int mode, Input::Pad key)
    {
        switch (mode)
        {
        case 5: return I5->getBindings(key); break;
        case 7: return I7->getBindings(key); break;
        case 9: return I9->getBindings(key); break;
        default: return {};
        }
    }
    static std::string getDBPath()
    {
        auto path = "profile/"s + profileName + "/database";
        if (!fs::exists(path))
            fs::create_directories(path);
        return path;
    }
};
