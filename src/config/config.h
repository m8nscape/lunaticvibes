#pragma once

#include <yaml-cpp/yaml.h>
#include "common/types.h"
//#include <unordered_map>

namespace cfg
{
    constexpr char OFF[] = "OFF";
    constexpr char ON[] = "ON";
}

class vConfig
{
protected:
	Path _path;
	YAML::Node _yaml;

public:
    vConfig();
	vConfig(const char* file);
	vConfig(const std::string& profile, const char* file);
    virtual ~vConfig() = default;

    virtual void setDefaults() noexcept = 0;

	void load();
	void save();
    
    template<class Ty_v>
    inline Ty_v get(const std::string& key, const Ty_v& fallback) const { return _yaml[key].as<Ty_v>(fallback); }
    inline std::string get(const std::string& key, const char* fallback) const { return get(key, std::string(fallback)); }

    template<class Ty_v>
    inline Ty_v get(const std::string& key, unsigned idx, const Ty_v& fallback) const { return _yaml[key][idx].as<Ty_v>(fallback); }
    inline std::string get(const std::string& key, unsigned idx, const char* fallback) const { return get(key, idx, std::string(fallback)); }

    template<class Ty_v>
	inline void set(const std::string& key, const Ty_v& value) noexcept { _yaml[key] = value; } // untested when type mismatch
    inline void set(const std::string& key, const char* value) noexcept { set(key, std::string(value)); }
    
    template<class Ty_v>
	inline void set(const std::string& key, unsigned idx, const Ty_v& value) noexcept { if (_yaml[key].IsSequence()) _yaml[key][idx] = value; } // untested when type mismatch
    inline void set(const std::string& key, unsigned idx, const char* value) noexcept { set(key, idx, std::string(value)); }
    
protected:

};