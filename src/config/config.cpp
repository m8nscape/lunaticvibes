#include "config.h"
#include <fstream>

vCfg::vCfg()
{
    // placeholder, do nothing
}

vCfg::vCfg(std::string json) : _json(json) {}

vCfg::~vCfg() {}

bool vCfg::checkBool(const json& j, const std::string& key) {
    if (j[key].is_boolean())
    {
        _json[key] = j[key];
        return false;
    }
    LOG_WARNING << "Value Type(Bool) Check Error ( " << key << " : " << j[key] << " )";
    return true;
}
bool vCfg::checkStr(const json& j, const std::string& key) {
    if (j[key].is_string())
    {
        _json[key] = j[key];
        return false;
    }
    LOG_WARNING << "Value Type(String) Check Error ( " << key << " : " << j[key] << " )";
    return true;
}
bool vCfg::checkInt(const json& j, const std::string& key) {
    if (j[key].is_number_integer())
    {
        _json[key] = j[key];
        return false;
    }
    LOG_WARNING << "Value Type(Int) Check Error ( " << key << " : " << j[key] << " )";
    return true;
}
bool vCfg::checkUnsigned(const json& j, const std::string& key) {
    if (j[key].is_number_unsigned())
    {
        _json[key] = j[key];
        return false;
    }
    LOG_WARNING << "Value Type(Unsigned) Check Error ( " << key << " : " << j[key] << " )";
    return true;
}

void to_json(json& j, const vCfg& o)
{
    j = o._json;
}

void from_json(const json& j, vCfg& o)
{
    o.setDefaults();
    o.copyValues(j);
    o.checkValues();
}
