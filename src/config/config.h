#pragma once
#include <yaml-cpp/yaml.h>	// I wonder how the YAML static lib was sized almost about 5MB...
#include "types.h"

class vCfg
{
protected:
	StringPath _path;
	YAML::Node _yaml;

public:
    vCfg();
	vCfg(StringPath file);
    ~vCfg();

    virtual void setDefaults() noexcept = 0;
	virtual void load();
	virtual void save();

    template<class Ty_v>
    inline Ty_v get(const std::string key, const Ty_v fallback) { return _yaml[key].as<Ty_v>(fallback); }
    template<class Ty_v>
	inline void set(const std::string key, const Ty_v value) noexcept { _yaml[key] = value; } // untested when type mismatch

protected:

};
