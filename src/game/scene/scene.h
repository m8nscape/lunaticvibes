#pragma once
#include <bitset>
#include <string>
#include <memory>
#include <array>
#include "game/skin/skin.h"
#include "game/runtime/state.h"
#include "game/input/input_wrapper.h"
#include "common/types.h"

enum class eScene
{
    NOT_INIT,
    PRE_SELECT,
    SELECT,
    DECIDE,
    PLAY,
    RESULT,
    COURSE_TRANS,
    RETRY_TRANS,
    KEYCONFIG,
    CUSTOMIZE,
    EXIT_TRANS,
    EXIT
};
inline eScene getSceneFromMode(eMode m)
{
    static const std::map<eMode, eScene> modeSceneMap =
    {
        { eMode::MUSIC_SELECT,  eScene::SELECT},
        { eMode::DECIDE,        eScene::DECIDE},
        { eMode::THEME_SELECT,  eScene::CUSTOMIZE},
        { eMode::KEY_CONFIG,    eScene::KEYCONFIG},
        { eMode::PLAY5,         eScene::PLAY},
        { eMode::PLAY5_2,       eScene::PLAY},
        { eMode::PLAY7,         eScene::PLAY},
        { eMode::PLAY7_2,       eScene::PLAY},
        { eMode::PLAY9,         eScene::PLAY},
        { eMode::PLAY9_2,       eScene::PLAY},
        { eMode::PLAY10,        eScene::PLAY},
        { eMode::PLAY14,        eScene::PLAY},
        { eMode::RESULT,        eScene::RESULT},
        //{ eMode::COURSE_RESULT, eScene::},
    };
    return modeSceneMap.find(m) != modeSceneMap.end() ? modeSceneMap.at(m) : eScene::NOT_INIT;
}

// Parent class of scenes, defines how an object being stored and drawn.
// Every classes of scenes should inherit this class.
class vScene: public AsyncLooper
{
protected:
    eScene _scene;
    std::shared_ptr<vSkin> _skin;
    InputWrapper _input;
    AsyncLooper _looper;

    std::shared_ptr<TTFFont> _fNotifications;
    std::shared_ptr<Texture> _texNotificationsBG;
    std::shared_ptr<SpriteText> _sTopLeft;
    std::shared_ptr<SpriteText> _sTopLeft2;
    std::array<std::shared_ptr<SpriteText>, size_t(IndexText::_OVERLAY_NOTIFICATION_MAX) - size_t(IndexText::_OVERLAY_NOTIFICATION_0) + 1> _sNotifications;
    std::array<std::shared_ptr<SpriteStatic>, size_t(IndexText::_OVERLAY_NOTIFICATION_MAX) - size_t(IndexText::_OVERLAY_NOTIFICATION_0) + 1> _sNotificationsBG;

public:
	bool sceneEnding = false;
    bool inTextEdit = false;
    std::string textBeforeEdit;

public:
    vScene() = delete;
    vScene(eMode mode, unsigned rate = 240, bool backgroundInput = false);
    virtual ~vScene();
    void inputLoopStart() { _input.loopStart(); }
    void inputLoopEnd() { _input.loopEnd(); }
    void disableMouseInput() { _skin->setHandleMouseEvents(false); }

public:
    virtual void update();      // skin update
    void MouseClick(InputMask& m, const Time& t);
    void MouseDrag(InputMask& m, const Time& t);
    void MouseRelease(InputMask& m, const Time& t);
    virtual void draw() const;

    vSkin::skinInfo getSkinInfo() const { return _skin ? _skin->info : vSkin::skinInfo(); }

protected:
    virtual void _updateAsync() = 0;
    void _updateAsync1();

    virtual void _updateImgui();
    void DebugToggle(InputMask& m, const Time& t);

    bool isInTextEdit() const;
    IndexText textEditType() const;
    virtual bool checkAndStartTextEdit() { return false; }
    virtual void startTextEdit(bool clear);
    virtual void stopTextEdit(bool modify);
};


////////////////////////////////////////////////////////////////////////////////

struct InputTimerSwitchMap {
    IndexTimer tm;
    IndexSwitch sw;
};

inline const InputTimerSwitchMap InputGamePressMap[] =
{
    { IndexTimer::S1L_DOWN, IndexSwitch::S1L_DOWN },
    { IndexTimer::S1R_DOWN, IndexSwitch::S1R_DOWN },
    { IndexTimer::K11_DOWN, IndexSwitch::K11_DOWN },
    { IndexTimer::K12_DOWN, IndexSwitch::K12_DOWN },
    { IndexTimer::K13_DOWN, IndexSwitch::K13_DOWN },
    { IndexTimer::K14_DOWN, IndexSwitch::K14_DOWN },
    { IndexTimer::K15_DOWN, IndexSwitch::K15_DOWN },
    { IndexTimer::K16_DOWN, IndexSwitch::K16_DOWN },
    { IndexTimer::K17_DOWN, IndexSwitch::K17_DOWN },
    { IndexTimer::K18_DOWN, IndexSwitch::K18_DOWN },
    { IndexTimer::K19_DOWN, IndexSwitch::K19_DOWN },
    { IndexTimer::K1START_DOWN, IndexSwitch::K1START_DOWN },
    { IndexTimer::K1SELECT_DOWN, IndexSwitch::K1SELECT_DOWN },
    { IndexTimer::K1SPDUP_DOWN, IndexSwitch::K1SPDUP_DOWN },
    { IndexTimer::K1SPDDN_DOWN, IndexSwitch::K1SPDDN_DOWN },
    { IndexTimer::S2L_DOWN, IndexSwitch::S2L_DOWN },
    { IndexTimer::S2R_DOWN, IndexSwitch::S2R_DOWN },
    { IndexTimer::K21_DOWN, IndexSwitch::K21_DOWN },
    { IndexTimer::K22_DOWN, IndexSwitch::K22_DOWN },
    { IndexTimer::K23_DOWN, IndexSwitch::K23_DOWN },
    { IndexTimer::K24_DOWN, IndexSwitch::K24_DOWN },
    { IndexTimer::K25_DOWN, IndexSwitch::K25_DOWN },
    { IndexTimer::K26_DOWN, IndexSwitch::K26_DOWN },
    { IndexTimer::K27_DOWN, IndexSwitch::K27_DOWN },
    { IndexTimer::K28_DOWN, IndexSwitch::K28_DOWN },
    { IndexTimer::K29_DOWN, IndexSwitch::K29_DOWN },
    { IndexTimer::K2START_DOWN, IndexSwitch::K2START_DOWN },
    { IndexTimer::K2SELECT_DOWN, IndexSwitch::K2SELECT_DOWN },
    { IndexTimer::K2SPDUP_DOWN, IndexSwitch::K2SPDUP_DOWN },
    { IndexTimer::K2SPDDN_DOWN, IndexSwitch::K2SPDDN_DOWN },
};

inline const InputTimerSwitchMap InputGameReleaseMap[] =
{
    { IndexTimer::S1L_UP, IndexSwitch::S1L_DOWN },
    { IndexTimer::S1R_UP, IndexSwitch::S1R_DOWN },
    { IndexTimer::K11_UP, IndexSwitch::K11_DOWN },
    { IndexTimer::K12_UP, IndexSwitch::K12_DOWN },
    { IndexTimer::K13_UP, IndexSwitch::K13_DOWN },
    { IndexTimer::K14_UP, IndexSwitch::K14_DOWN },
    { IndexTimer::K15_UP, IndexSwitch::K15_DOWN },
    { IndexTimer::K16_UP, IndexSwitch::K16_DOWN },
    { IndexTimer::K17_UP, IndexSwitch::K17_DOWN },
    { IndexTimer::K18_UP, IndexSwitch::K18_DOWN },
    { IndexTimer::K19_UP, IndexSwitch::K19_DOWN },
    { IndexTimer::K1START_UP, IndexSwitch::K1START_DOWN },
    { IndexTimer::K1SELECT_UP, IndexSwitch::K1SELECT_DOWN },
    { IndexTimer::K1SPDUP_UP, IndexSwitch::K1SPDUP_DOWN },
    { IndexTimer::K1SPDDN_UP, IndexSwitch::K1SPDDN_DOWN },
    { IndexTimer::S2L_UP, IndexSwitch::S2L_DOWN },
    { IndexTimer::S2R_UP, IndexSwitch::S2R_DOWN },
    { IndexTimer::K21_UP, IndexSwitch::K21_DOWN },
    { IndexTimer::K22_UP, IndexSwitch::K22_DOWN },
    { IndexTimer::K23_UP, IndexSwitch::K23_DOWN },
    { IndexTimer::K24_UP, IndexSwitch::K24_DOWN },
    { IndexTimer::K25_UP, IndexSwitch::K25_DOWN },
    { IndexTimer::K26_UP, IndexSwitch::K26_DOWN },
    { IndexTimer::K27_UP, IndexSwitch::K27_DOWN },
    { IndexTimer::K28_UP, IndexSwitch::K28_DOWN },
    { IndexTimer::K29_UP, IndexSwitch::K29_DOWN },
    { IndexTimer::K2START_UP, IndexSwitch::K2START_DOWN },
    { IndexTimer::K2SELECT_UP, IndexSwitch::K2SELECT_DOWN },
    { IndexTimer::K2SPDUP_UP, IndexSwitch::K2SPDUP_DOWN },
    { IndexTimer::K2SPDDN_UP, IndexSwitch::K2SPDDN_DOWN },
};

////////////////////////////////////////////////////////////////////////////////
