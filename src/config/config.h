#pragma once
// FIXME: current TOML lib cause internal errors
#include <yaml-cpp/yaml.h>	// I wonder how the YAML static lib was sized almost about 5MB...
//#include <cpptoml.h> // From cpptoml
#include "types.h"
//#include <unordered_map>

namespace cfg
{
    inline const char* OFF = "OFF";
    inline const char* ON = "ON";
}

class vConfig
{
protected:
	StringPath _path;
	YAML::Node _yaml;
	//TomlFile _toml;

public:
    vConfig();
	vConfig(StringPath file);
    virtual ~vConfig();

    virtual void setDefaults() noexcept = 0;

	void load();
	void save();

    /* TOML get/set
	template<class Ty_v>
	inline Ty_v get(const std::string key, const Ty_v value)
	{
		auto val = _toml->get_as<Ty_v>(key);
		if (val)
		{
			return *val;
		}
		else
		{
			throw new key_error(key);
		}
	}

	template<class Ty_v>
	inline void set(const std::string key, const Ty_v value) noexcept
	{
		_toml->insert(key, value);
	}
    */

protected:

};