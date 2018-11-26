#pragma once
#include "cfg_general.h"
#include "cfg_profile.h"
#include "cfg_input.h"
#include <string>

class ConfigMgr
{
public:
    static ConfigGeneral G;
    static ConfigProfile P;
    static ConfigInput   I;

public:
    static void selectProfile(std::string name);
    static inline void init() { G.setDefaults(); P.setDefaults(); I.setDefaults(); }
    static inline void load() { G.load(); P.load(); I.load(); }
    static inline void save() { G.save(); P.save(); I.save(); }

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
        }
    }
};