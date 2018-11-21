#pragma once
#include "config.h"
#include <array>
#include <utility>

class CfgInput : public vCfg
{
public:
    using keys = gamepadKeys;

public:
    int loadFile(const std::string path) noexcept;
    int saveFile(const std::string path) noexcept;

public:
    void clear() noexcept;
    virtual void setDefaults() noexcept override;
protected:
    virtual int copyValues(const json& j) noexcept override;
    virtual int checkValues() noexcept override;
    bool checkUnsigned(const json& j, const std::string& key, unsigned target, unsigned slot);

private:
    // For keyboard binds, the value is raw enum value defined in sf::Keyboard::Key.
    // For joystick binds, the value is a negate number, mixing all of identifiers:
    //  10000 * Joystick No

    // Usage: gamepadKeyBindings [target] [slot] {device, code}
    // Usage: gamepadKeyBindings [target] [slot] {device + code}
    //  Keyboard's inner device code is 0, joystick +10000
    //  Keyboard's outer device code is -1, joystick not adding 10000
    //  code +1000 if Axis, with hundreds digit representing direction (0 for -, 1 for +)
    std::array<std::array<std::pair<int, int>, KEYSLOTS>, Input::GAME_KEY_COUNT> gamepadKeyBindings = {};

    void _bindKey(unsigned K, keys target, unsigned slot, unsigned device, unsigned key);
public:
    void bindKey(unsigned K, keys target, unsigned slot, sf::Keyboard::Key key);
    void bindKey(unsigned K, keys target, unsigned slot, unsigned joyNo, unsigned button);
    void bindKey(unsigned K, keys target, unsigned slot, unsigned joyNo, sf::Joystick::Axis axis, int direction);
    std::array<std::pair<int, int>, KEYSLOTS> getBindings(unsigned K, keys button) const;
};
