#pragma once
#include "game/skin/skin.h"
#include "game/input/input_wrapper.h"
#include "game/data/data_system.h"

namespace lunaticvibes
{

inline SceneType getSceneFromSkinType(SkinType m)
{
    static const std::map<SkinType, SceneType> modeSceneMap =
    {
        { SkinType::MUSIC_SELECT,  SceneType::SELECT},
        { SkinType::DECIDE,        SceneType::DECIDE},
        { SkinType::THEME_SELECT,  SceneType::CUSTOMIZE},
        { SkinType::KEY_CONFIG,    SceneType::KEYCONFIG},
        { SkinType::PLAY5,         SceneType::PLAY},
        { SkinType::PLAY5_2,       SceneType::PLAY},
        { SkinType::PLAY7,         SceneType::PLAY},
        { SkinType::PLAY7_2,       SceneType::PLAY},
        { SkinType::PLAY9,         SceneType::PLAY},
        { SkinType::PLAY9_2,       SceneType::PLAY},
        { SkinType::PLAY10,        SceneType::PLAY},
        { SkinType::PLAY14,        SceneType::PLAY},
        { SkinType::RESULT,        SceneType::RESULT},
        { SkinType::COURSE_RESULT, SceneType::COURSE_RESULT},
    };
    return modeSceneMap.find(m) != modeSceneMap.end() ? modeSceneMap.at(m) : SceneType::NOT_INIT;
}

// Parent class of scenes, defines how an object being stored and drawn.
// Every classes of scenes should inherit this class.
class SceneBase : public AsyncLooper
{
protected:
    SceneType _type;
    std::shared_ptr<SkinBase> pSkin;
    InputWrapper _input;

    std::shared_ptr<TTFFont> _fNotifications;
    std::shared_ptr<Texture> _texNotificationsBG;
    std::array<std::shared_ptr<SpriteText>, Struct_SystemData::NotificationTextManager::MAX_NOTIFICATIONS> _sNotifications;
    std::array<std::shared_ptr<SpriteStatic>, Struct_SystemData::NotificationTextManager::MAX_NOTIFICATIONS> _sNotificationsBG;

public:
    bool sceneEnding = false;
    bool inTextEdit = false;
    std::string textBeforeEdit;

protected:
    static bool queuedScreenshot;
    static bool queuedFPS;

    static bool showFPS;

public:
    SceneBase() = delete;
    SceneBase(SkinType skinType, unsigned rate = 240, bool backgroundInput = false);
    virtual ~SceneBase();
    void inputLoopStart() { _input.loopStart(); }
    void inputLoopEnd() { _input.loopEnd(); }
    void disableMouseInput() { pSkin->setHandleMouseEvents(false); }

public:
    virtual void update();      // skin update
    void MouseClick(InputMask& m, const Time& t);
    void MouseDrag(InputMask& m, const Time& t);
    void MouseRelease(InputMask& m, const Time& t);
    virtual void draw() const;

    SkinBase::skinInfo getSkinInfo() const { return pSkin ? pSkin->info : SkinBase::skinInfo(); }

protected:
    virtual void _updateAsync() = 0;
    void _updateAsync1();

    virtual void updateImgui();
    void DebugToggle(InputMask& m, const Time& t);

    bool isInTextEdit() const;
    virtual bool checkAndStartTextEdit() { return false; }
    virtual void startTextEdit(bool clear);
    virtual void stopTextEdit(bool modify);

    void GlobalFuncKeys(InputMask& m, const Time& t);
};


////////////////////////////////////////////////////////////////////////////////

inline const std::unordered_map<Input::Pad, std::string> InputGamePressMap =
{
    { Input::Pad::S1L, "key_on_sl_1p" },
    { Input::Pad::S1R, "key_on_sr_1p" },
    { Input::Pad::K11, "key_on_1_1p" },
    { Input::Pad::K12, "key_on_2_1p" },
    { Input::Pad::K13, "key_on_3_1p" },
    { Input::Pad::K14, "key_on_4_1p" },
    { Input::Pad::K15, "key_on_5_1p" },
    { Input::Pad::K16, "key_on_6_1p" },
    { Input::Pad::K17, "key_on_7_1p" },
    { Input::Pad::K18, "key_on_8_1p" },
    { Input::Pad::K19, "key_on_9_1p" },
    { Input::Pad::K1START, "key_on_start_1p" },
    { Input::Pad::K1SELECT, "key_on_select_1p" },
    { Input::Pad::K1SPDUP, "key_on_spdup_1p" },
    { Input::Pad::K1SPDDN, "key_on_spddn_1p" },
    { Input::Pad::S2L, "key_on_sl_2p" },
    { Input::Pad::S2R, "key_on_sr_2p" },
    { Input::Pad::K21, "key_on_1_2p" },
    { Input::Pad::K22, "key_on_2_2p" },
    { Input::Pad::K23, "key_on_3_2p" },
    { Input::Pad::K24, "key_on_4_2p" },
    { Input::Pad::K25, "key_on_5_2p" },
    { Input::Pad::K26, "key_on_6_2p" },
    { Input::Pad::K27, "key_on_7_2p" },
    { Input::Pad::K28, "key_on_8_2p" },
    { Input::Pad::K29, "key_on_9_2p" },
    { Input::Pad::K2START, "key_on_start_2p" },
    { Input::Pad::K2SELECT, "key_on_select_2p" },
    { Input::Pad::K2SPDUP, "key_on_spdup_2p" },
    { Input::Pad::K2SPDDN, "key_on_spddn_2p" },
};

inline const std::unordered_map<Input::Pad, std::string> InputGameReleaseMap =
{
    { Input::Pad::S1L, "key_off_sl_1p" },
    { Input::Pad::S1R, "key_off_sr_1p" },
    { Input::Pad::K11, "key_off_1_1p" },
    { Input::Pad::K12, "key_off_2_1p" },
    { Input::Pad::K13, "key_off_3_1p" },
    { Input::Pad::K14, "key_off_4_1p" },
    { Input::Pad::K15, "key_off_5_1p" },
    { Input::Pad::K16, "key_off_6_1p" },
    { Input::Pad::K17, "key_off_7_1p" },
    { Input::Pad::K18, "key_off_8_1p" },
    { Input::Pad::K19, "key_off_9_1p" },
    { Input::Pad::K1START, "key_off_start_1p" },
    { Input::Pad::K1SELECT, "key_off_select_1p" },
    { Input::Pad::K1SPDUP, "key_off_spdup_1p" },
    { Input::Pad::K1SPDDN, "key_off_spddn_1p" },
    { Input::Pad::S2L, "key_off_sl_2p" },
    { Input::Pad::S2R, "key_off_sr_2p" },
    { Input::Pad::K21, "key_off_1_2p" },
    { Input::Pad::K22, "key_off_2_2p" },
    { Input::Pad::K23, "key_off_3_2p" },
    { Input::Pad::K24, "key_off_4_2p" },
    { Input::Pad::K25, "key_off_5_2p" },
    { Input::Pad::K26, "key_off_6_2p" },
    { Input::Pad::K27, "key_off_7_2p" },
    { Input::Pad::K28, "key_off_8_2p" },
    { Input::Pad::K29, "key_off_9_2p" },
    { Input::Pad::K2START, "key_off_start_2p" },
    { Input::Pad::K2SELECT, "key_off_select_2p" },
    { Input::Pad::K2SPDUP, "key_off_spdup_2p" },
    { Input::Pad::K2SPDDN, "key_off_spddn_2p" },
};

////////////////////////////////////////////////////////////////////////////////

}
