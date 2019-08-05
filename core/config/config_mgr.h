#pragma once
#include "cfg_general.h"
#include "cfg_profile.h"
#include "cfg_input.h"
#include "cfg_skin.h"
#include <string>

class ConfigMgr
{
public:
    static inline ConfigGeneral     G;
    static inline ConfigProfile     P;
    static inline ConfigInput       I;
    static inline ConfigSkin        S;

public:
    static void selectProfile(std::string name);
    static inline void init() { G.setDefaults(); P.setDefaults(); I.setDefaults(); S.setDefaults(); }
    static inline void load() { G.load(); P.load(); I.load(); S.setDefaults(); }
    static inline void save() { G.save(); P.save(); I.save(); S.setDefaults(); }

public:
    template<class Ty_v>
    static Ty_v get(const std::string key, const Ty_v fallback)
    {
        switch (key[0])
        {
        case 'A':
        case 'V': return G.get(key, fallback);
        case 'P': return P.get(key, fallback);
        case 'I': return I.get(key, fallback);
        case 'S': return S.get(key, fallback);
		default: break;
        }
    }
    template<class Ty_v>
    static void set(const std::string key, const Ty_v value) noexcept
    {
        switch (key[0])
        {
        case 'A':
        case 'V': return G.set(key, value);
        case 'P': return P.set(key, value);
        case 'I': return I.set(key, value);
        case 'S': return S.set(key, value);
		default: break;
        }
    }
};