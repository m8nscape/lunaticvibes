#pragma once
#include "cfg_general.h"
#include "cfg_profile.h"
#include "cfg_input.h"
#include "cfg_skin.h"
#include <string>

inline const std::string PROFILE_DEFAULT = "default";

class ConfigMgr
{
public:
    static inline ConfigGeneral     G{PROFILE_DEFAULT};
    static inline ConfigProfile     P{PROFILE_DEFAULT};
    static inline ConfigInput       I5{PROFILE_DEFAULT, 5};
    static inline ConfigInput       I7{PROFILE_DEFAULT, 7};
    static inline ConfigInput       I9{PROFILE_DEFAULT, 9};
    static inline ConfigSkin        S{PROFILE_DEFAULT};

public:
    static inline void init() { G.setDefaults(); P.setDefaults(); I5.setDefaults(); I7.setDefaults(); I9.setDefaults(); S.setDefaults(); }
    static inline void load() { G.load(); P.load(); I5.load(); I7.load(); I9.load(); S.load(); }
    static inline void save() { G.save(); P.save(); I5.save(); I7.save(); I9.save(); S.save(); }
    static int selectProfile(const std::string& name);

public:
    template<class Ty_v>
    static Ty_v get(const std::string& type, const std::string& key, const Ty_v& fallback)
    {
        switch (type[0])
        {
        case 'A':                               // Audio
        case 'V': return G.get(key, fallback);  // Video
        case 'P': return P.get(key, fallback);  // Play
        case '5': return I5.get(key, fallback);  // Input
        case '7': return I7.get(key, fallback);  // Input
        case '9': return I9.get(key, fallback);  // Input
        case 'S': return S.get(key, fallback);  // Skin
        default:  return Ty_v(); break;
        }
    }
    template<class Ty_v>
    static void set(const std::string& type, const std::string& key, const Ty_v& value) noexcept
    {
        switch (type[0])
        {
        case 'A':                               // Audio
        case 'V': return G.set(key, value);     // Video
        case 'P': return P.set(key, value);     // Play
        case '5': return I5.set(key, value);     // Input
        case '7': return I7.set(key, value);     // Input
        case '9': return I9.set(key, value);     // Input
        case 'S': return S.set(key, value);     // Skin
        default:  return Ty_v(); break;
        }
    }
};
