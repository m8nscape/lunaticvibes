#pragma once

#include <yaml-cpp/yaml.h>
#include "common/types.h"
//#include <unordered_map>

namespace cfg
{
    inline const char* OFF = "OFF";
    inline const char* ON = "ON";
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
    inline std::string get(const std::string& key, const char* fallback) const { return _yaml[key].as<std::string>(std::string(fallback)); }

    template<class Ty_v>
	inline void set(const std::string& key, const Ty_v& value) noexcept { _yaml[key] = value; } // untested when type mismatch
    inline void set(const std::string& key, const char* value) noexcept { _yaml[key] = std::string(value); }
    
protected:

};