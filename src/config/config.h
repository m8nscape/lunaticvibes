#pragma once
#include <yaml-cpp/yaml.h>	// I wonder how the YAML static lib was sized almost about 5MB...
#include "types.h"

namespace cfg
{
    const char* OFF = "OFF";
    const char* ON = "ON";
}

class vConfig
{
protected:
	StringPath _path;
	YAML::Node _yaml;

public:
    vConfig();
	vConfig(StringPath file);
    virtual ~vConfig();

    virtual void setDefaults() noexcept = 0;

	void load();
	void save();

    template<class Ty_v>
    inline Ty_v get(const std::string key, const Ty_v fallback) { return _yaml[key].as<Ty_v>(fallback); }
    template<class Ty_v>
	inline void set(const std::string key, const Ty_v value) noexcept { _yaml[key] = value; } // untested when type mismatch

protected:

};
