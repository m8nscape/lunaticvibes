#pragma once
#include <bitset>
#include <string>
#include <memory>
#include <array>
#include "game/skin/skin.h"
#include "game/data/data.h"
#include "game/input/input_wrapper.h"
#include "common/types.h"

// Parent class of scenes, defines how an object being stored and drawn.
// Every classes of scenes should inherit this class.
class vScene: public AsyncLooper
{
protected:
    std::shared_ptr<vSkin> _skin;
    InputWrapper _input;

    std::shared_ptr<TTFFont> _fNotifications;
    std::shared_ptr<Texture> _texNotificationsBG;
    std::shared_ptr<SpriteText> _sTopLeft;
    std::array<std::shared_ptr<SpriteText>, size_t(eText::_OVERLAY_NOTIFICATION_MAX) - size_t(eText::_OVERLAY_NOTIFICATION_0) + 1> _sNotifications;
    std::array<std::shared_ptr<SpriteStatic>, size_t(eText::_OVERLAY_NOTIFICATION_MAX) - size_t(eText::_OVERLAY_NOTIFICATION_0) + 1> _sNotificationsBG;

public:
	bool sceneEnding = false;

public:
    vScene(eMode mode, unsigned rate = 240, bool backgroundInput = false);
    virtual ~vScene();

public:
    vScene() = delete;
    virtual void update();      // skin update
    void MouseClick(InputMask& m, const Time& t);
    virtual void draw() const;

protected:
    virtual void _updateAsync() = 0;
};


////////////////////////////////////////////////////////////////////////////////

struct InputTimerSwitchMap {
    eTimer tm;
    eSwitch sw;
};

inline const InputTimerSwitchMap InputGamePressMapSingle[] =
{
    { eTimer::S1L_DOWN, eSwitch::S1L_DOWN },
    { eTimer::S1R_DOWN, eSwitch::S1R_DOWN },
    { eTimer::K11_DOWN, eSwitch::K11_DOWN },
    { eTimer::K12_DOWN, eSwitch::K12_DOWN },
    { eTimer::K13_DOWN, eSwitch::K13_DOWN },
    { eTimer::K14_DOWN, eSwitch::K14_DOWN },
    { eTimer::K15_DOWN, eSwitch::K15_DOWN },
    { eTimer::K16_DOWN, eSwitch::K16_DOWN },
    { eTimer::K17_DOWN, eSwitch::K17_DOWN },
    { eTimer::K18_DOWN, eSwitch::K18_DOWN },
    { eTimer::K19_DOWN, eSwitch::K19_DOWN },
    { eTimer::K1START_DOWN, eSwitch::K1START_DOWN },
    { eTimer::K1SELECT_DOWN, eSwitch::K1SELECT_DOWN },
    { eTimer::K1SPDUP_DOWN, eSwitch::K1SPDUP_DOWN },
    { eTimer::K1SPDDN_DOWN, eSwitch::K1SPDDN_DOWN },
    { eTimer::S2L_DOWN, eSwitch::S1L_DOWN },
    { eTimer::S2R_DOWN, eSwitch::S1R_DOWN },
    { eTimer::K21_DOWN, eSwitch::K11_DOWN },
    { eTimer::K22_DOWN, eSwitch::K12_DOWN },
    { eTimer::K23_DOWN, eSwitch::K13_DOWN },
    { eTimer::K24_DOWN, eSwitch::K14_DOWN },
    { eTimer::K25_DOWN, eSwitch::K15_DOWN },
    { eTimer::K26_DOWN, eSwitch::K16_DOWN },
    { eTimer::K27_DOWN, eSwitch::K17_DOWN },
    { eTimer::K28_DOWN, eSwitch::K18_DOWN },
    { eTimer::K29_DOWN, eSwitch::K19_DOWN },
    { eTimer::K2START_DOWN, eSwitch::K1START_DOWN },
    { eTimer::K2SELECT_DOWN, eSwitch::K1SELECT_DOWN },
    { eTimer::K2SPDUP_DOWN, eSwitch::K1SPDUP_DOWN },
    { eTimer::K2SPDDN_DOWN, eSwitch::K1SPDDN_DOWN },
};

inline const InputTimerSwitchMap InputGameReleaseMapSingle[] =
{
    { eTimer::S1L_UP, eSwitch::S1L_DOWN },
    { eTimer::S1R_UP, eSwitch::S1R_DOWN },
    { eTimer::K11_UP, eSwitch::K11_DOWN },
    { eTimer::K12_UP, eSwitch::K12_DOWN },
    { eTimer::K13_UP, eSwitch::K13_DOWN },
    { eTimer::K14_UP, eSwitch::K14_DOWN },
    { eTimer::K15_UP, eSwitch::K15_DOWN },
    { eTimer::K16_UP, eSwitch::K16_DOWN },
    { eTimer::K17_UP, eSwitch::K17_DOWN },
    { eTimer::K18_UP, eSwitch::K18_DOWN },
    { eTimer::K19_UP, eSwitch::K19_DOWN },
    { eTimer::K1START_UP, eSwitch::K1START_DOWN },
    { eTimer::K1SELECT_UP, eSwitch::K1SELECT_DOWN },
    { eTimer::K1SPDUP_UP, eSwitch::K1SPDUP_DOWN },
    { eTimer::K1SPDDN_UP, eSwitch::K1SPDDN_DOWN },
    { eTimer::S2L_UP, eSwitch::S1L_DOWN },
    { eTimer::S2R_UP, eSwitch::S1R_DOWN },
    { eTimer::K21_UP, eSwitch::K11_DOWN },
    { eTimer::K22_UP, eSwitch::K12_DOWN },
    { eTimer::K23_UP, eSwitch::K13_DOWN },
    { eTimer::K24_UP, eSwitch::K14_DOWN },
    { eTimer::K25_UP, eSwitch::K15_DOWN },
    { eTimer::K26_UP, eSwitch::K16_DOWN },
    { eTimer::K27_UP, eSwitch::K17_DOWN },
    { eTimer::K28_UP, eSwitch::K18_DOWN },
    { eTimer::K29_UP, eSwitch::K19_DOWN },
    { eTimer::K2START_UP, eSwitch::K1START_DOWN },
    { eTimer::K2SELECT_UP, eSwitch::K1SELECT_DOWN },
    { eTimer::K2SPDUP_UP, eSwitch::K1SPDUP_DOWN },
    { eTimer::K2SPDDN_UP, eSwitch::K1SPDDN_DOWN },
};

inline const InputTimerSwitchMap InputGamePressMapBattle[] =
{
    { eTimer::S1L_DOWN, eSwitch::S1L_DOWN },
    { eTimer::S1R_DOWN, eSwitch::S1R_DOWN },
    { eTimer::K11_DOWN, eSwitch::K11_DOWN },
    { eTimer::K12_DOWN, eSwitch::K12_DOWN },
    { eTimer::K13_DOWN, eSwitch::K13_DOWN },
    { eTimer::K14_DOWN, eSwitch::K14_DOWN },
    { eTimer::K15_DOWN, eSwitch::K15_DOWN },
    { eTimer::K16_DOWN, eSwitch::K16_DOWN },
    { eTimer::K17_DOWN, eSwitch::K17_DOWN },
    { eTimer::K18_DOWN, eSwitch::K18_DOWN },
    { eTimer::K19_DOWN, eSwitch::K19_DOWN },
    { eTimer::K1START_DOWN, eSwitch::K1START_DOWN },
    { eTimer::K1SELECT_DOWN, eSwitch::K1SELECT_DOWN },
    { eTimer::K1SPDUP_DOWN, eSwitch::K1SPDUP_DOWN },
    { eTimer::K1SPDDN_DOWN, eSwitch::K1SPDDN_DOWN },
    { eTimer::S2L_DOWN, eSwitch::S2L_DOWN },
    { eTimer::S2R_DOWN, eSwitch::S2R_DOWN },
    { eTimer::K21_DOWN, eSwitch::K21_DOWN },
    { eTimer::K22_DOWN, eSwitch::K22_DOWN },
    { eTimer::K23_DOWN, eSwitch::K23_DOWN },
    { eTimer::K24_DOWN, eSwitch::K24_DOWN },
    { eTimer::K25_DOWN, eSwitch::K25_DOWN },
    { eTimer::K26_DOWN, eSwitch::K26_DOWN },
    { eTimer::K27_DOWN, eSwitch::K27_DOWN },
    { eTimer::K28_DOWN, eSwitch::K28_DOWN },
    { eTimer::K29_DOWN, eSwitch::K29_DOWN },
    { eTimer::K2START_DOWN, eSwitch::K2START_DOWN },
    { eTimer::K2SELECT_DOWN, eSwitch::K2SELECT_DOWN },
    { eTimer::K2SPDUP_DOWN, eSwitch::K2SPDUP_DOWN },
    { eTimer::K2SPDDN_DOWN, eSwitch::K2SPDDN_DOWN },
};

inline const InputTimerSwitchMap InputGameReleaseMapBattle[] =
{
    { eTimer::S1L_UP, eSwitch::S1L_DOWN },
    { eTimer::S1R_UP, eSwitch::S1R_DOWN },
    { eTimer::K11_UP, eSwitch::K11_DOWN },
    { eTimer::K12_UP, eSwitch::K12_DOWN },
    { eTimer::K13_UP, eSwitch::K13_DOWN },
    { eTimer::K14_UP, eSwitch::K14_DOWN },
    { eTimer::K15_UP, eSwitch::K15_DOWN },
    { eTimer::K16_UP, eSwitch::K16_DOWN },
    { eTimer::K17_UP, eSwitch::K17_DOWN },
    { eTimer::K18_UP, eSwitch::K18_DOWN },
    { eTimer::K19_UP, eSwitch::K19_DOWN },
    { eTimer::K1START_UP, eSwitch::K1START_DOWN },
    { eTimer::K1SELECT_UP, eSwitch::K1SELECT_DOWN },
    { eTimer::K1SPDUP_UP, eSwitch::K1SPDUP_DOWN },
    { eTimer::K1SPDDN_UP, eSwitch::K1SPDDN_DOWN },
    { eTimer::S2L_UP, eSwitch::S2L_DOWN },
    { eTimer::S2R_UP, eSwitch::S2R_DOWN },
    { eTimer::K21_UP, eSwitch::K21_DOWN },
    { eTimer::K22_UP, eSwitch::K22_DOWN },
    { eTimer::K23_UP, eSwitch::K23_DOWN },
    { eTimer::K24_UP, eSwitch::K24_DOWN },
    { eTimer::K25_UP, eSwitch::K25_DOWN },
    { eTimer::K26_UP, eSwitch::K26_DOWN },
    { eTimer::K27_UP, eSwitch::K27_DOWN },
    { eTimer::K28_UP, eSwitch::K28_DOWN },
    { eTimer::K29_UP, eSwitch::K29_DOWN },
    { eTimer::K2START_UP, eSwitch::K2START_DOWN },
    { eTimer::K2SELECT_UP, eSwitch::K2SELECT_DOWN },
    { eTimer::K2SPDUP_UP, eSwitch::K2SPDUP_DOWN },
    { eTimer::K2SPDDN_UP, eSwitch::K2SPDDN_DOWN },
};

////////////////////////////////////////////////////////////////////////////////
