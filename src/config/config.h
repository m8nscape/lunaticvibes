#pragma once
// FIXME: current TOML lib cause internal errors
//#include <yaml-cpp/yaml.h>	// I wonder how the YAML static lib was sized almost about 5MB...
#include <cpptoml.h> // From cpptoml
#include "types.h"
#include <unordered_map>

namespace cfg
{
	inline const char* OFF = "OFF";
	inline const char* ON = "ON";
}

class key_error : std::logic_error {

public:

	key_error() : logic_error("Key not found!") {};
	key_error(const std::string key) : logic_error("Key \"" + key + "\" not found!") {};
};

/*
  TODO:
  1. Do not hard code all config files' name and path.
  2. TOML supports nested domain, consider redesign parse logic.
  3. May some convert utils needed.
*/
class vConfig
{
	typedef std::shared_ptr<cpptoml::table> TomlFile;

protected:
	StringPath _path;
	//YAML::Node _yaml;
	TomlFile _toml;

public:
	vConfig();
	vConfig(StringPath file);
	virtual ~vConfig();

	virtual void setDefaults() noexcept = 0;

	void load();
	void save();

	template<class Ty_v>
	inline Ty_v get(const std::string& key, const Ty_v value)
	{
		auto val = _toml->get_as<Ty_v>(key);
		if (val)
		{
			return *val;
		}
		else
		{
			throw new key_error(key);
			return value;
		}
	}

	template<class Ty_v>
	inline void set(const std::string& key, const Ty_v value) /*noexcept*/
	{
		_toml->insert(key, value);
	}

protected:

};